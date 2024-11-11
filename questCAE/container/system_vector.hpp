/*------------------------------------------
用于存储和操作系统向量，并提供 FEM 特定的组装功能*
------------------------------------------*/

#ifndef QUEST_SYSTEM_VECTOR_HPP
#define QUEST_SYSTEM_VECTOR_HPP

// 系统头文件
#include <string>
#include <iostream>

// 项目头文件
#include "includes/define.hpp"
#include "utilities/parallel_utilities.hpp"
#include "utilities/atomic_utilities.hpp"
#include "container/sparse_graph.hpp"
#include "container/sparse_contiguous_row_graph.hpp"
#include "utilities/reduction_utilities.hpp"
#include "utilities/parallel_utilities.hpp"

namespace Quest{

    template<typename TDataType = double, typename TIndexType = std::size_t>
    class SystemVector final{
        public:
            using IndexType = TIndexType;

            QUEST_CLASS_POINTER_DEFINITION(SystemVector);

        public:
            SystemVector(const SparseGraph<IndexType>& rGraph){
                mpComm = rGraph.pGetComm();
                mData.resize(rGraph.Size(),false);
            }


            SyatemVector(const SparseContiguousRowGraph<IndexType>& rGraph){
                mpComm = rGraph.pGetComm();
                mData.resize(rGraph.Size(),false);
            }


            SystemVector(IndexType size, DataCommunicator& rComm = ParallelEnvironment::GetDataCommunicator("Serial")){
                if(rComm.IsDistributed()){
                    QUEST_ERROR << "Attempting to construct a serial system_vector with a distributed communicator" << std::endl;
                }
                mpComm = &rComm;
                mData.resize(size,false);
            }


            SystemVector(
                const Vector& data,
                DataCommunicator& rComm = ParallelEnvironment::GetDataCommunicator("Serial")
            ){
                if(rComm.IsDistributed()){
                    QUEST_ERROR << "Attempting to construct a serial system_vector with a distributed communicator" << std::endl;
                }
                mpComm = &rComm;
                mData.resize(data.size(),false);
                noalias(mData) = data;
            }


            explicit SystemVector(const SystemVector<TDataType, TIndexType>& rOther){
                mpComm = rOther.mpComm;
                mData.resize(rOther.size(),false);

                IndexPartition<IndexType>(size()).for_each([&](IndexType i){
                    (*this)[i] = rOther[i];
                });
            }


            ~SystemVector(){}


            const DataCommunicator& GetComm() const{
                return *mpComm;
            }


            const DataCommunicator* pGetComm() const{
                return mpComm;
            }


            void Clear(){
                mData.clear();
            }


            void SetValue(const TDataType value){
                IndexPartition<IndexType>(mData.size()).for_each([&](IndexType i){
                    mData[i] = value;
                });
            }


            IndexType size() const{
                return mData.size();
            }


            TDataType& operator()(IndexType i){
                return mData[i];
            }


            const TDataType& operator()(IndexType i) const{
                return mData[i];
            }


            TDataType& operator[](IndexType i){
                return mData[i];
            }


            const TDataType& operator[](IndexType i) const{
                return mData[i];
            }


            DenseVector<TDataType>& data(){
                return mData;
            }


            const DenseVector<TDataType>& data() const{
                return mData;
            }


            void Add(
                const TDataType factor,
                const SystemVector& rOtherVector
            ){
                IndexPartition<IndexType>(size()).for_each([&](IndexType i){
                    (*this)[i] += factor*rOtherVector[i];
                });
            }


            SystemVector& operator=(const SystemVector& rOther){
                IndexPartition<IndexType>(size()).for_each([&](IndexType i){
                    (*this)[i] = rOther[i];
                });
                return *this;
            }


            SystemVector& operator+=(const SystemVector& rOther){
                IndexPartition<IndexType>(size()).for_each([&](IndexType i){
                    (*this)[i] += rOther[i];
                });
                return *this;
            }


            SystemVector& operator-=(const SystemVector& rOther){
                IndexPartition<IndexType>(size()).for_each([&](IndexType i){
                    (*this)[i] -= rOther[i];
                });
                return *this;
            }


            SystemVector& operator*=(const TDataType multiplier_factor){
                IndexPartition<IndexType>(size()).for_each([&](IndexType i){
                    (*this)[i] *= multiplier_factor;
                });
                return *this;
            }


            SystemVector& operator/=(const TDataType divisor_factor){
                IndexPartition<IndexType>(size()).for_each([&](IndexType i){
                    (*this)[i] /= divisor_factor;
                });
                return *this;
            }


            TDataType Dot(const SystemVector& rOtherVector, IndexType gather_on_rank=0){
                QUEST_WARNING_IF("SystemVector",gather_on_rank!=0) 
                    << "the parameter gather_on_rank essentially does nothing for a non-distribued vector. It is added to have the same interface as for the distributed_system_vector" << std::endl;

                auto partition = IndexPartition<IndexType>(size());
                TDataType dot_value = partition.template for_each<SumReduction<TDataType>>([&](IndexType i){
                    return (*this)[i] * rOtherVector[i];
                });

                return dot_value;
            }

            
            void BeginAssemble(){}


            void FinalizeAssemble(){}


            template<typename TVectorType, typename TIndexVectorType>
            void Assemble(
                const TVectorType& rVectorInput,
                const TIndexVectorType& EquationId
            ){
                QUEST_DEBUG_ERROR_IF(rVectorInput.size()!= EquationId.size());

                for(unsigned int i=0; i<EquationId.size(); ++i){
                    IndexType global_i = EquationId[i];
                    QUEST_DEBUG_ERROR_IF(global_i > mData.size());
                    AtomicAdd(mData[global_i], rVectorInput[i]);
                }
            }


            std::string Info() const{
                std::stringstream buffer;
                buffer << "SystemVector";
                return buffer.str();
            }


            void PrintInfo(std::ostream& rOstream) const{
                rOstream << "SystemVector" << std::endl;
                PrintData(rOstream);
            }


            void PrintData(std::ostream& rOstream) const{
                std::cout << mData << std::endl;
            }

        protected:

        private:

        private:
            const DataCommunicator& mpComm;
            DenseVector<TDataType> mData;

    };


    template<typename TDataType, typename TIndexType>
    inline std::istream& operator >> (std::istream& rIstream, SystemVector<TDataType, TIndexType>& rThis){
        return rIstream;
    }


    template<typename TDataType, typename TIndexType>
    inline std::ostream& operator << (std::ostream& rOstream, const SystemVector<TDataType, TIndexType>& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }

} // namespace Quest

#endif //QUEST_SYSTEM_VECTOR_HPP