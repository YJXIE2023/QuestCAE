/*--------------------------------------------------------
“串行”稀疏压缩行存储（CSR, Compressed Sparse Row）格式的矩阵
--------------------------------------------------------*/

#ifndef QUEST_CSR_MATRIX_HPP
#define QUEST_CSR_MATRIX_HPP

// 系统头文件
#include <iostream>
#include <limits>

// 第三方头文件
#include "span/span.hpp"

// 项目头文件
#include "includes/define.hpp"
#include "container/sparse_contiguous_row_graph.hpp"
#include "container/system_vector.hpp"
#include "utilities/parallel_utilities.hpp"
#include "utilities/atomic_utilities.hpp"
#include "utilities/reduction_utilities.hpp"
#include "includes/key_hash.hpp"
#include "includes/parallel_enviroment.hpp"

namespace Quest{

    template<typename TDataType = double, typename TIndexType = std::size_t>
    class CsrMatrix final{
        public:
            using IndexType = TIndexType;
            using MatrixMapType = std::unordered_map<
                std::pair<IndexType, IndexType>,
                double,
                PairHasher<IndexType, IndexType>,
                PairComparor<IndexType, IndexType>
            >;

            QUEST_CLASS_POINTER_DEFINITION(CsrMatrix);

        public:
            CsrMatrix(){
                mpComm = &ParallelEnvironment::GetDataCommunicator("Serial");
            }


            CsrMatrix(const DataCommunicator& rComm){
                if(rComm.IsDistributed()){
                    QUEST_ERROR << "Attempting to construct a serial CsrMatrix with a distributed communicator" << std::endl;
                }
                mpComm = &rComm;
            }


            template<typename TGraphType>
            CsrMatrix(const TGraaphType& rSparseGraph){
                mpComm = rSparseGraph.pGetComm();
                TIndexType row_data_size = 0;
                TIndexType col_data_size = 0;
                rSparseGraph.ExportCSRArrays(mpRowIndicesData, row_data_size, mpColIndicesData, col_data_size);
                mRowIndices = Quest::span<IndexType>(mpRowIndicesData, row_data_size);
                mColIndices = Quest::span<IndexType>(mpColIndicesData, col_data_size);

                mNrows = size1();

                ComputeSolSize();

                ResizeValueData(mColIndices.size());
                SetValue(0.0);
            }


            CsrMatrix(const MatrixMapType& matrix_map){
                SparseGraph<TIndexType> Agraph;
                for(const auto& item : matrix_map){
                    IndexType I = item.first.first;
                    IndexType J = item.first.second;
                    Agraph.AddEntry(I,J);
                }
                Agraph.Finalize();

                this->BeginAssemble();
                for(const auto& item : matrix_map){
                    IndexType I = item.first.first;
                    IndexType J = item.first.second;
                    TDataType value = item.second;
                    this->AssembleEntry(value, I, J);
                }
                this->FinalizeAssemble();
            }


            explicit CsrMatrix(const CsrMatrix<TDataType, TIndexType>& rOtherMatrix){
                mpComm = rOtherMatrix.mpComm;
                ResizeIndex1Data(rOtherMatrix.mRowIndices.size());
                ResizeIndex2Data(rOtherMatrix.mColIndices.size());
                ResizeValueData(rOtherMatrix.mValuesVector.size());

                mNrows = rOtherMatrix.mNrows;
                mNcols = rOtherMatrix.mNcols;

                IndexPartition<IndexType>(mRowIndices.size()).for_each([&](IndexType i){
                    mRowIndices[i] = rOtherMatrix.mRowIndices[i];
                });

                IndexPartition<IndexType>(mColIndices.size()).for_each([&](IndexType i){
                    mColIndices[i] = rOtherMatrix.mColIndices[i];
                });

                IndexPartition<IndexType>(mValuesVector.size()).for_each([&](IndexType i){
                    mValuesVector[i] = rOtherMatrix.mValuesVector[i];
                });
            }


            CsrMatrix(CsrMatrix<TDataType, TIndexType>&& rOtherMatrix){
                mIsOwnerOfData = rOtherMatrix.mIsOwnerOfData;
                rOtherMatrix.mIsOwnerOfData = false;

                mpRowIndicesData = rOtherMatrix.mpRowIndicesData;
                mpColIndicesData = rOtherMatrix.mpColIndicesData;
                mpValuesVectorData = rOtherMatrix.mpValuesVectorData;

                mRowIndices = rOtherMatrix.mRowIndices;
                mColIndices = rOtherMatrix.mColIndices;
                mValuesVector = rOtherMatrix.mValuesVector;

                mNrows = rOtherMatrix.mNrows;
                mNcols = rOtherMatrix.mNcols;
            }


            ~CsrMatrix(){
                AssignIndex1Data(nullptr,0);
                AssignIndex2Data(nullptr,0);
                AssignValueData(nullptr,0);
            }


            CsrMatrix& operator = (const CsrMatrix& rOtherMatrix) = delete;


            CsrMatrix& operator = (CsrMatrix&& rOtherMatrix){
                mpComm = rOtherMatrix.mpComm;
                mIsOwnerOfData = rOtherMatrix.mIsOwnerOfData;
                rOtherMatrix.mIsOwnerOfData = false;

                mpRowIndicesData = rOtherMatrix.mpRowIndicesData;
                mpColIndicesData = rOtherMatrix.mpColIndicesData;
                mpValuesVectorData = rOtherMatrix.mpValuesVectorData;

                mRowIndices = rOtherMatrix.mRowIndices;
                mColIndices = rOtherMatrix.mColIndices;
                mValuesVector = rOtherMatrix.mValuesVector;

                mNrows = rOtherMatrix.mNrows;
                mNcols = rOtherMatrix.mNcols;

                return *this;
            }


            void Clear(){
                AssignIndex1Data(nullptr,0);
                AssignIndex2Data(nullptr,0);
                AssignValueData(nullptr,0);
                mNrows = 0;
                mNcols = 0;
            }


            const DataCommunicator& GetComm() const{
                return *mpComm;
            }


            const DataCommunicator* pGetComm() const{
                return mpComm;
            }


            void SetValue(const TDataType value){
                IndexPartition<IndexType>(mValuesVector.size()).for_each([&](IndexType i){
                    mValuesVector[i] = value;
                });
            }


            IndexType size1() const{
                return mRowIndices.size() - 1;
            }


            IndexType size2() const{
                return mNcols;
            }


            inline IndexType nnz() const{
                return index2_data().size();
            }


            bool IsOwnerOfData() const{
                return mIsOwnerOfData;
            }


            void SetIsOwnerOfData(bool IsOwner){
                mIsOwnerOfData = IsOwner;
            }


            inline Quest::span<IndexType>& index1_data(){
                return mRowIndices;
            }


            inline Quest::span<IndexType>& index2_data(){
                return mColIndices;
            }


            inline Quest::span<TDataType>& value_data(){
                return mValuesVector;
            }


            inline const Quest::span<IndexType>& index1_data() const{
                return mRowIndices;
            }


            inline const Quest::span<IndexType>& index2_data() const{
                return mColIndices;
            }


            inline const Quest::span<TDataType>& value_data() const{
                return mValuesVector;
            }


            void SetColSize(IndexType Ncols){
                mNcols = Ncols;
            }


            void SetRowSize(IndexType Nrows){
                mNrows = Nrows;
            }


            void ComputeColSize(){
                IndexType max_col = IndexPartition<IndexType>(mColIndices.size()).template for_each<MaxReduction<double>>([&](IndexType i){
                    return mColIndices[i];
                });

                mNcols = max_col + 1;
            }


            void AssignIndex1Data(TIndexType* pExternalData, TIndexType DataSize){
                if(IsOwnerOfData() && mpRowIndicesData != nullptr){
                    delete [] mpRowIndicesData;
                }
                mpRowIndicesData = pExternalData;
                if(DataSize != 0){
                    mRowIndices = Quest::span<IndexType>(mpRowIndicesData, DataSize);
                } else {
                    mRowIndices = Quest::span<IndexType>();
                }
            }


            void AssignIndex2Data(TIndexType* pExternalData, TIndexType DataSize){
                if(IsOwnerOfData() && mpColIndicesData != nullptr){
                    delete [] mpColIndicesData;
                }
                mpColIndicesData = pExternalData;
                if(DataSize != 0){
                    mColIndices = Quest::span<IndexType>(mpColIndicesData, DataSize);
                } else {
                    mColIndices = Quest::span<IndexType>();
                }
            }


            void AssignValueData(TDataType* pExternalData, TIndexType DataSize){
                if(IsOwnerOfData() && mpValuesVectorData != nullptr){
                    delete [] mpValuesVectorData;
                }
                mpValuesVectorData = pExternalData;
                if(DataSize != 0){
                    mValuesVector = Quest::span<TDataType>(mpValuesVectorData, DataSize);
                } else {
                    mValuesVector = Quest::span<TDataType>();
                }
            }


            void ResizeIndex1Data(TIndexType DataSize){
                QUEST_ERROR_IF_NOT(IsOwnerOfData()) << "ResizeIndex1Data is only allowed if the data are locally owned" << std::endl;
                if(mpRowIndicesData != nullptr){
                    delete [] mpRowIndicesData;
                }
                mpRowIndicesData = new IndexType[DataSize];
                mRowIndices = Quest::span<IndexType>(mpRowIndicesData, DataSize);
            }


            void ResizeIndex2Data(TIndexType DataSize){
                QUEST_ERROR_IF_NOT(IsOwnerOfData()) << "ResizeIndex2Data is only allowed if the data are locally owned" << std::endl;
                if(mpColIndicesData != nullptr){
                    delete [] mpColIndicesData;
                }
                mpColIndicesData = new IndexType[DataSize];
                mColIndices = Quest::span<IndexType>(mpColIndicesData, DataSize);
            }


            void ResizeValueData(TIndexType DataSize){
                QUEST_ERROR_IF_NOT(IsOwnerOfData()) << "ResizeValueData is only allowed if the data are locally owned" << std::endl;
                if(mpValuesVectorData != nullptr){
                    delete [] mpValuesVectorData;
                }
                mpValuesVectorData = new TDataType[DataSize];
                mValuesVector = Quest::span<TDataType>(mpValuesVectorData, DataSize);
            }


            void CheckColSize(){
                IndexType max_col = 0;
                for(IndexType i = 0; i < mColIndices.size(); ++i){
                    max_col = std::max(max_col, mColIndices[i]);
                }
                if(max_col > mNcols){
                    QUEST_ERROR << " max column index : " << max_col << " exceeds mNcols :" << mNcols << std::endl;
                }
            }


            TDataType& opeartor()(IndexType i, IndexType j){
                const IndexType row_begin = index1_data()[i];
                const IndexType row_end = index1_data()[i+1];
                IndexType k = BinarySearch(index2_data(), row_begin, row_end, j);
                QUEST_DEBUG_ERROR_IF(k == std::numeric_limits<IndexType>::max()) << "local indices I,J : " << I << " " << J << " not found in matrix" << std::endl;
                return value_data()[k];
            }


            const TDataType& operator()(IndexType i, IndexType j) const{
                const IndexType row_begin = index1_data()[i];
                const IndexType row_end = index1_data()[i+1];
                IndexType k = BinarySearch(index2_data(), row_begin, row_end, j);
                QUEST_DEBUG_ERROR_IF(k == std::numeric_limits<IndexType>::max()) << "local indices I,J : " << I << " " << J << " not found in matrix" << std::endl;
                return value_data()[k];
            }


            bool Has(IndexType I, IndexType J) const{
                const IndexType row_begin = index1_data()[I];
                const IndexType row_end = index1_data()[I+1];
                IndexType k = BinarySearch(index2_data(), row_begin, row_end, J);
                return k != std::numeric_limits<IndexType>::max();
            }


            template<typename TInputVectorType, typename TOutputVectorType>
            void SpMV(const TInputVectorType& x, TOutputVectorType& y) const{
                QUEST_ERROR_IF(size1() != y.size()) << "SpMV: mismatch between row sizes : " << size1()  << " and destination vector size " << y.size() << std::endl;
                QUEST_ERROR_IF(size2() != x.size()) << "SpMV: mismatch between col sizes : " << size2()  << " and input vector size " << x.size() << std::endl;
                if(nnz() != 0){
                    IndexPartition<IndexType>(size1()).for_each([&](IndexType i){
                        IndexType row_begin = index1_data()[i];
                        IndexType row_end = index1_data()[i+1];
                        for(IndexType k = row_begin; k < row_end; ++k){
                            IndexType col = index2_data()[k];
                            y(i) += value_data()[k] * x(col);
                        }
                    });
                }
            }


            template<typename TInputVectorType, typename TOutputVectorType>
            void SpMV(
                const TDataType alpha,
                const TInputVectorType& x,
                const TDataType beta,
                TOutputVectorType& y
            ) const {
                QUEST_ERROR_IF(size1() != y.size()) << "SpMV: mismatch between matrix sizes : " << size1() << " " <<size2() << " and destination vector size " << y.size() << std::endl;
                QUEST_ERROR_IF(size2() != x.size()) << "SpMV: mismatch between matrix sizes : " << size1() << " " <<size2() << " and input vector size " << x.size() << std::endl;
                IndexPartition<IndexType>(y.size()).for_each([&](IndexType i){
                    IndexType row_begin = index1_data()[i];
                    IndexType row_end = index1_data()[i+1];
                    TDataType aux = TDataType();
                    for(IndexType k = row_begin; k < row_end; ++k){
                        IndexType col = index2_data()[k];
                        aux += value_data()[k] * x(col);
                    }
                    y(i) = alpha * aux + beta * y(i);
                });
            }


            template<typename TInputVectorType, typename TOutputVectorType>
            void TransposeSpMV(const TInputVectorType& x, TOutputVectorType& y) const{
                QUEST_ERROR_IF(size2() != y.size()) << "TransposeSpMV: mismatch between transpose matrix sizes : " << size2() << " " <<size1() << " and destination vector size " << y.size() << std::endl;
                QUEST_ERROR_IF(size1() != x.size()) << "TransposeSpMV: mismatch between transpose matrix sizes : " << size2() << " " <<size1() << " and input vector size " << x.size() << std::endl;
                IndexPartition<IndexType>(size1()).for_each([&](IndexType i){
                    IndexType row_begin = index1_data()[i];
                    IndexType row_end = index1_data()[i+1];
                    for(IndexType k = row_begin; k < row_end; ++k){
                        IndexType j = index2_data()[k];
                        AtomicAdd(y(j), value_data()[k] * x(i));
                    }
                });
            }


            template<typename TInputVectorType, typename TOutputVectorType>
            void TransposeSpMV(
                const TDataType alpha,
                const TInputVectorType& x,
                const TDataType beta,
                TOutputVectorType& y
            ) const {
                QUEST_ERROR_IF(size2() != y.size()) << "TransposeSpMV: mismatch between transpose matrix sizes : " << size2() << " " <<size1() << " and destination vector size " << y.size() << std::endl;
                QUEST_ERROR_IF(size1() != x.size()) << "TransposeSpMV: mismatch between transpose matrix sizes : " << size2() << " " <<size1() << " and input vector size " << x.size() << std::endl;
                y *= beta;
                IndexPartition<IndexType>(size1()).for_each([&](IndexType i){
                    IndexType row_begin = index1_data()[i];
                    IndexType row_end = index1_data()[i+1];
                    TDataType aux = alpha * x(i);
                    for(IndexType k = row_begin; k < row_end; ++k){
                        IndexType j = index2_data()[k];
                        AtomicAdd(y(j), value_data()[k] * x(i));
                    }
                });
            }


            TDataType NormFrobenius() const{
                auto sum2 = IndexPartition<IndexType>(this->value_data().size()).template for_each<SumReduction<TDataType>>([this](IndexType i){    
                    return std::pow(this->value_data()[i], 2);  
                });
                return std::sqrt(sum2);
            }


            void reserve(IndexType NRows, IndexType nnz){
                ResizeIndex1Data(NRows+1);

                if(nnz > 0){
                    index1_data()[0] = 0;
                }

                ResizeIndex2Data(nnz);
                ResizeValueData(nnz);
                mNrows = NRows;
            }


            MatrixMapType ToMap() const{
                MatrixMapType value_map;
                for(unsigned int i = 0; i < size1(); ++i){
                    IndexType row_begin = index1_data()[i];
                    IndexType row_end = index1_data()[i+1];
                    for(unsigned int k = row_begin; k < row_end; ++k){
                        IndexType j = index2_data()[k];
                        IndexType v = value_data()[k];
                        value_map[{i,j}] = v;
                    }
                }
                return value_map;
            }


            void BeginAssemble(){}


            void FinalizeAssemble(){}


            template<typename TMatrixType, typename TIndexVectorType>
            void Assemble(
                const TMatrixType& rMatrixInput,
                const TIndexVectorType& EquationId
            ){
                QUEST_DEBUG_ERROR_IF(rMatrixInput.size1() != EquationId.size()) << "sizes of matrix and equation id do not match in Assemble" << std::endl;
                QUEST_DEBUG_ERROR_IF(rMatrixInput.size2() != EquationId.size()) << "sizes of matrix and equation id do not match in Assemble" << std::endl;

                const unsigned int local_size = rMatrixInput.size1();

                for(unsigned int i_local = 0; i_local < local_size; ++i_local){
                    const IndexType I = EquationId[i_local];
                    const IndexType row_begin = index1_data()[I];
                    const IndexType row_end = index1_data()[I+1];

                    IndexTYpe J = EquationId[0];
                    IndexType k = BinarySearch(index2_data(), row_begin, row_end, EquationId[0]);
                    IndexType lastJ = J;

                    AtomicAdd(value_data()[k], rMatrixInput(i_local, 0));

                    for(unsigned int j_local = 1; j_local < local_size; ++j_local){
                        J = EquationId[j_local];

                        if(k+1<row_end && index2_data()[k+1] == J){
                            k = k+1;
                        } else if (J > lastJ){
                            k = BinarySearch(index2_data(), k+2, row_end, J);
                        } else if (J < lastJ){
                            k = BinarySearch(index2_data(), row_begin, k-1, J);
                        }

                        AtomicAdd(value_data()[k], rMatrixInput(i_local, j_local));

                        lastJ = J;
                    }
                }
            }


            template<typename TMatrixType, typename TIndexVectorType>
            void Assemble(
                const TMatrixType& rMatrixInput,
                const TIndexVectorType& RowEquationId,
                const TIndexVectorType& ColEquationId
            ){
                QUEST_DEBUG_ERROR_IF(rMatrixInput.size1() != RowEquationId.size()) << "sizes of matrix and equation id do not match in Assemble" << std::endl;
                QUEST_DEBUG_ERROR_IF(rMatrixInput.size2() != ColEquationId.size()) << "sizes of matrix and equation id do not match in Assemble" << std::endl;

                const unsigned int local_size = rMatrixInput.size1();
                const unsigned int col_size = rMatrixInput.size2();

                for(unsigned int i_local = 0; i_local < local_size; ++i_local){
                    const IndexType I = RowEquationId[i_local];
                    const IndexType row_begin = index1_data()[I];
                    const IndexType row_end = index1_data()[I+1];

                    IndexType J = ColEquationId[0];
                    IndexType k = BinarySearch(index2_data(), row_begin, row_end, J);
                    IndexType lastJ = J;

                    AtomicAdd(value_data()[k], rMatrixInput(i_local, 0));

                    for(unsigned int j_local = 1; j_local < col_size; ++j_local){
                        J = ColEquationId[j_local];

                        if(k+1<row_end && index2_data()[k+1] == J){
                            k = k+1;
                        } else if (J > lastJ){
                            k = BinarySearch(index2_data(), k+2, row_end, J);
                        } else if (J < lastJ){
                            k = BinarySearch(index2_data(), row_begin, k-1, J);
                        }

                        AtomicAdd(value_data()[k], rMatrixInput(i_local, j_local));

                        lastJ = J;
                    }
                }
            }


            void AssembleEntry(const TDataType& Value, const IndexType GlobalI, const IndexType GlobalJ){
                IndexType k = BinarySearch(index2_data(), index1_data()[GlobalI], index1_data()[GlobalI+1], GlobalJ);
                AtomicAdd(value_data()[k], Value);
            }


            template<typename TVectorType1, typename TVectorType2=TVectorType1>
            void ApplyHomogeneousDirichlet(
                const TVectorType1& rFreeDofsVector,
                const TDataType& DiagonalValue,
                TVectorType2& rRHS
            ){
                QUEST_ERROR_IF(size1() != rFreeDofsVector.size()) << "ApplyDirichlet: mismatch between row sizes : " << size1()
                    << " and free_dofs_vector size " << rFreeDofsVector.size() << std::endl;
                QUEST_ERROR_IF(size2() != rFreeDofsVector.size()) << "ApplyDirichlet: mismatch between col sizes : " << size2()
                    << " and free_dofs_vector size " << rFreeDofsVector.size() << std::endl;

                if(DiagonalValue != 0){
                    IndexPartition<IndexType>(size1()).for_each([&](IndexType i){
                        rRHS[i] *= rFreeDofsVector[i];

                        const IndexType row_begin = index1_data()[i];
                        const IndexType row_end = index1_data()[i+1];
                        if(std::abs(rFreeDofsVector[i]-1.0) < 1e-14){
                            for(IndexType k = row_begin; k < row_end; ++k){
                                IndexType col = index2_data()[k];
                                value_data()[k] *= rFreeDofsVector[col];
                            }
                        } else {
                            for(IndexType k = row_begin; k < row_end; ++k){
                                IndexType col = index2_data()[k];
                                if(col!=i){
                                    value_data()[k] = TDataType();
                                } else {
                                    value_data()[k] = DiagonalValue;
                                }
                            }
                        }
                    });
                }
            }


            std::string Info() const {
                std::stringstream buffer;
                buffer << "CsrMatrix";
                return buffer.str();
            }


            void PrintInfo(std::ostream& rOstream) const {
                rOstream << "CsrMatrix" << std::endl;
                PrintData(rOstream);
            }


            void PrintData(std::ostream& rOstream) const {
                rOstream << "size1 : " << size1() << std::endl;
                rOstream << "size2 : " << size2() << std::endl;
                rOstream << "nnz   : " << nnz() << std::endl;
                rOstream << "index1_data : " << std::endl;
                for(auto item : index1_data()){
                    rOstream << item << ",";
                }
                rOstream << std::endl;

                rOstream << "index2_data : " << std::endl;
                for(auto item : index2_data()){
                    rOstream << item << ",";
                }
                rOstream << std::endl;

                rOstream << "value_data : " << std::endl;
                for(auto item : value_data()){
                    rOstream << item << ",";
                }
                rOstream << std::endl;
            }

        protected:
            template<typename TVectorType>
            inline IndexType BinarySearch(const TVectorType& arr, IndexType l, IndexType r, IndexType x) const{
                while(l <= r){
                    int m = l + (r - l) / 2;

                    if(arr[m] == x){
                        return m;
                    }

                    if(arr[m] < x){
                        l = m + 1;
                    }else{
                        r = m - 1;
                    }
                }
                return std::numeric_limits<IndexType>::max();
            }

        private:
            friend class Serializer;


            void save(Serializer& rSerializer) const{
                rSerializer.save("IsOwnerOfData", mIsOwnerOfData);

                rSerializer.save("nrows",mRowIndices.size());
                for(IndexType i = 0; i < mRowIndices.size(); ++i){
                    rSerializer.save("i", mRowIndices[i]);
                }

                rSerializer.save("cols_size", mColIndices.size());
                for(IndexType i = 0; i < mColIndices.size(); ++i){
                    rSerializer.save("i", mColIndices[i]);
                }

                rSerializer.save("val_size", mValuesVector.size());
                for(IndexType i = 0; i < mValuesVector.size(); ++i){
                    rSerializer.save("d", mValuesVector[i]);
                }

                rSerializer.save("Nrow", mNrows);
                rSerializer.save("Ncol", mNcols);
            }


            void load(Serializer& rSerializer){
                rSerializer.load("IsOwnerOfData", mIsOwnerOfData);
                if(mIsOwnerOfData == false){
                    mIsOwnerOfData = true;
                    QUEST_WARNING("csr_matrix becomes owner of a copy of data after serialization");
                } 

                IndexType rows_size;
                rSerializer.load("nrows", rows_size);
                mpRowIndicesData = new IndexType[rows_size];
                mRowIndices = Quest::span<IndexType>(mpRowIndicesData, rows_size);
                for(IndexType i = 0; i < rows_size; ++i){
                    rSerializer.load("i", mRowIndices[i]);
                }

                IndexType cols_size;
                rSerializer.load("cols_size", cols_size);
                mpColIndicesData = new IndexType[cols_size];    
                mColIndices = Quest::span<IndexType>(mpColIndicesData, cols_size);
                for(IndexType i = 0; i < cols_size; ++i){
                    rSerializer.load("i", mColIndices[i]);
                }

                IndexType val_size;
                rSerializer.load("val_size", val_size);
                mpValuesVectorData = new TDataType[val_size];
                mValuesVector = Quest::span<TDataType>(mpValuesVectorData, val_size);
                for(IndexType i = 0; i < val_size; ++i){
                    rSerializer.load("d", mValuesVector[i]);
                }

                rSerializer.load("Nrow", mNrows);
                rSerializer.load("Ncol", mNcols);
            }

        private:
            const DataCommunicator* mpComm;
            bool mIsOwnerOfData = true;
            IndexType *mpRowIndicesData = nullptr;
            IndexType *mpColIndicesData = nullptr;
            TDataType *mpValuesVectorData = nullptr;
            Quest::span<IndexType> mRowIndices;
            QUest::span<IndexType> mColIndices;
            Quest::span<TDataType> mValuesVector;
            IndexType mNrows = 0;
            IndexType mNcols = 0;

    };


    template<typename TDataType>
    inline std::istream& operator >> (std::istream& rIstream, CsrMatrix<TDataType>& rThis){
        return rIstream;
    }


    template<typename TDataType>
    inline std::ostream& operator << (std::ostream& rOstream, const CsrMatrix<TDataType>& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }

} // namespace Quest

#endif //QUEST_CSR_MATRIX_HPP