/*---------------------------------------
构建和存储矩阵图的类
稀疏关系构建为图结构，再根据该图生成稀疏矩阵
---------------------------------------*/

#ifndef QUEST_SPARSE_GRAPH_HPP
#define QUEST_SPARSE_GRAPH_HPP

// 系统头文件
#include <iostream>
#include <unordered_map>
#include <unordered_set>

// 第三方头文件
#include "span/span.hpp"

// 项目头文件
#include "includes/define.hpp"
#include "utilities/parallel_utilities.hpp"
#include "includes/ublas_interface.hpp"
#include "includes/serializer.hpp"
#include "includes/parallel_enviroment.hpp"

namespace Quest{

    template<typename TIndexType = std::size_t>
    class SparseGraph final{
        public:
            using IndexType = TIndexType;
            using GraphType = std::map<IndexType, std::unordered_set<IndexType>>;
            using const_row_iterator = typename GraphType::const_iterator;

            QUEST_CLASS_POINTER_DEFINITION(SparseGraph);

        public:
            class const_iterator_adaptor{
                private:
                    const_row_iterator map_iterator;
                public:
                    using iterator_category = std::forward_iterator_tag;
                    using difference_type   = std::ptrdiff_t;
                    using value_type        = typename GraphType::value_type;
                    using pointer           = typename GraphType::value_type*;
                    using reference         = typename GraphType::value_type&;

                public:
                    const_iterator_adaptor(const_row_iterator it): map_iterator(it){}
                    const_iterator_adaptor(const const_iterator_adaptor& it): map_iterator(it.map_iterator){}
                    const_iterator_adaptor& operator ++ () {mpa_iteartor++; return *this;}
                    const_iterator_adaptor operator ++ (int) {const_iterator_adaptor tmp(*this); operator ++(); return tmp;}
                    bool operator == (const const_iterator_adaptor& rhs) const {return map_iterator == rhs.map_iterator;}
                    bool operator!= (const const_iterator_adaptor& rhs) const {return map_iterator!= rhs.map_iterator;}
                    const typename GraphType::value_type& operator * () const {return map_iterator->second;}
                    const typename GraphType::value_type& operator -> () const {return map_iterator->second;}
                    const_row_iterator& base() {return map_iterator;}
                    const const_row_iterator& base() const {return map_iterator;}
                    IndexType GetRowIndex() const{return map_iterator->first;}
            };

        public:
            SparseGraph(IndexType N){
                mpComm = &ParallelEnvironment::GetDataCommunicator("Serial");
            }


            SparseGraph(){
                mpComm = &ParallelEnvironment::GetDataCommunicator("Serial");
            }


            SparseGraph(DataCommunicator& rComm){
                if(rComm.IsDistributed()){
                    QUEST_ERROR <<"Attempting to construct a serial CsrMatrix with a distributed communicator" << std::endl;
                }

                mpComm = &rComm;
            }


            ~SparseGraph(){}


            SparseGraph(const SparseGraph& rOther):
                mpComm(rOther.mpComm),
                mGraph(rOther.mGraph)
            {
            }


            SparseGraph(const std::vector<IndexType>& rSingleVectorRepresentation){
                AddFromSingleVectorRepresentation(rSingleVectorRepresentation);
            }


            const DataCommunicator& GetComm() const{
                return *mpComm;
            }


            const DataCommunicator& pGetComm() const{
                return mpComm;
            }


            IndexType Size() const{
                if(!this->GetGraph().empty()){
                    return (--(this->GetGraph().end()))->first + 1;
                }
                return 0;
            }


            bool IsEmpty() const{
                return mGraph.empty();
            }


            bool Has(const IndexType I, const IndexType J) const{
                const auto& row_it = mGraph.find(I);
                if(row_it == mGraph.end()){
                    if((roww_it->second).find(J) != (row_it->second).end()){
                        return true;
                    }
                    return false;
                }
                return false;
            }


            const typename GraphType::mapped_type& operator[](const IndexType& Key) const{
                return (mGraph.find(Key))->second;
            }


            void Clear(){
                mGraph.clear();
            }


            void AddEntry(const IndexType I, const IndexType J){
                mGraph[I].insert(J);
            }


            template<typename TContainerType>
            void AddEntries(const IndexType RowIndex, const TContainerType& rColumnIndices){
                mGraph[RowIndex].insert(rColumnIndices.begin(), rColumnIndices.end());
            }


            template<typename TIteratorType>
            void AddEntries(
                const IndexType RowIndex,
                const TIteratorType& rColBegin,
                const TIteratorType& rColEnd
            ){
                mGraph[RowIndex].insert(rColBegin, rColEnd);
            }


            template<typename TContainerType>
            void AddEntries(const TContainerType& rIndices){
                for(auto I : rIndices){
                    mGraph[I].insert(rIndices.begin(), rIndices.end());
                }
            }


            template<typename TContainerType>
            void AddEntries(const TContainerType& rRowIndices, const TContainerType& rColumnIndices){
                for(auto I : rRowIndices){
                    AddEntries(I, rColumnIndices);
                }
            }


            void AddEntries(SparseGraph& rOtherGraph){
                for(const auto& item : rOtherGraph.GetGraph()){
                    AddEntries(item.first, item.second);
                }
            }


            void Finalize(){}


            const GraphType& GetGraph() const{
                return mGraph;
            }


            template<typename TVectorType = DenseVector<IndexType>>
            IndexType ExportCSRArrays(
                TVectorType& rRowIndices,
                TVectorType& rColIndices
            ) const {
                IndexType* pRowIndicesDataSize = nullptr;
                IndexType RowIndicesDataSize = 0;
                IndexType* pColIndicesDataSize = nullptr;
                IndexType ColIndicesDataSize = 0;
                ExportCSRArrays(pRowIndicesDataSize, RowIndicesDataSize, pColIndicesDataSize, ColIndicesDataSize);
                if(rRowIndices.size() != RowIndicesDataSize){
                    rRowIndices.resize(RowIndicesDataSize);
                }
                IndexPartition<IndexType>(RowIndicesDataSize).for_each([&](IndexType i){
                    rRowIndices[i] = pRowIndicesData[i];
                });

                delete[] pRowIndicesData;
                if(rColIndices.size() != ColIndicesDataSize){
                    rColIndices.resize(ColIndicesDataSize);
                }
                IndexPartition<IndexType>(ColIndicesDataSize).for_each([&](IndexType i){
                    rColIndices[i] = pColIndicesData[i];
                });
                delete[] pColIndicesData;

                return rRowIndices.size();
            }


            IndexType ExportCSRArrays(
                Quest::span<IndexType>& rRowIndices,
                Quest::span<IndexType>& rColIndices
            ) const = delete;


            IndexType ExportCSRArrays(
                IndexType*& pRowIndicesData,
                IndexType& rRowDataSize,
                IndexType*& pColIndicesData,
                IndexType& rColDataSize
            ) const {
                IndexType nrows = this->Size();

                pRowIndicesData = new IndexType[nrows+1];
                rRowDataSize = nrows+1;
                Quest::span<IndexType> row_indices(pRowIndicesData, nrows+1);

                IndexPartition<IndexType>(row_indices.size()).for_each([&](IndexType i){
                    row_indices[i] = 0;
                });

                for(const auto& item : this->GetGraph()){
                    row_indices[item.first+1] = item.second.size();
                }

                for(int i = 1; i < static_cast<int>(row_indices.size()); ++i){
                    row_indices[i] += row_indices[i-1];
                }


                IndexType nnz = row_indices[nrows];
                rColDataSize = nnz;
                pColIndicesData = new IndexType[nnz];
                Quest::span<IndexType> col_indices(pColIndicesData, nnz);

                IndexPartition<IndexType>(col_indices.size()).for_each([&](IndexType i){
                    col_indices[i] = 0;
                });

                for(const auto& item : this->GetGraph()){
                    IndexType start = row_indices[item.first];

                    IndexType counter = 0;
                    for(auto j : item.second){
                        col_indices[start+counter] = j;
                        ++counter;
                    }
                }

                IndexPartition<IndexType>(row_indices.size()-1).for_each([&](IndexType i){
                    std::sort(col_indices.begin()+row_indices[i], col_indices.begin()+row_indices[i+1]);
                });
                return nrows;
            }


            std::vector<IndexType> ExportSingleVectorRepresentation() const {
                std::vector<IndexType> single_vector_representation;

                IndexType nrows = this->Size();
                single_vector_representation.push_back(nrows);

                for(const auto& item : this->GetGraph()){
                    IndexType I = item.first;
                    single_vector_representation.push_back(I);
                    single_vector_representation.push_back(item.second.size());
                    for(auto J : item.second){
                        single_vector_representation.push_back(J);
                    }
                }
                return single_vector_representation;
            }


            void AddFromSingleVectorRepresentation(const std::vector<IndexType>& rSingleVectorRepresentation){
                IndexType counter = 1;
                while(counter < rSingleVectorRepresentation.size()){
                    auto I = rSingleVectorRepresentation[counter++];
                    auto nrow = rSingleVectorRepresentation[counter++];
                    auto begin = &rSingleVectorRepresentation[counter];
                    AddEntries(I, begin, begin+nrow);
                    counter += nrow;
                }
            }


            const_iterator_adaptor begin() const{
                return const_iterator_adaptor(mGraph.begin());
            }


            const_iterator_adaptor end() const{
                return const_iterator_adaptor(mGraph.end());
            }


            std::string Info() const{
                std::stringstream buffer;
                buffer << "SparseGraph";
                return buffer.str();
            }


            void PrintInfo(std::ostream& rOstream) const{
                rOstream << "SparseGraph" << std::endl;
            }


            void PrintData(std::ostream& rOstream) const{}

        protected:

        private:
            friend class Serializer;


            void save(Serializer& rSerializer) const{
                std::vector<IndexType> IJ = ExportSingleVectorRepresentation();
                rSerializer.save("IJ", IJ);
            }


            void load(Serializer& rSerializer){
                std::vector<IndexType> IJ;
                rSerializer.load("IJ", IJ);
                AddFromSingleVectorRepresentation(IJ);
            }


            SparseGraph& operator = (const SparseGraph& rOther) = delete;

        private:
            DataCommunicator* mpComm;
            GraphType mGraph;

    };


    template<typename TIndexType = std::size_t>
    inline std::istream& operator >> (std::istream& rIstream, SparseGraph<TIndexType>& rThis){
        return rIstream;
    }


    template<typename TIndexType = std::size_t>
    inline std::ostream& operator << (std::ostream& rOstream, const SparseGraph<TIndexType>& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }

} // namespace Quest

#endif //QUEST_SPARSE_GRAPH_HPP