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

    /**
     * @class SparseGraph
     * @brief 构建和存储矩阵图的类
     * @details 用于存储矩阵图，旨在快速构建其他稀疏矩阵格式
     */
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
            /**
             * @brief 构造函数
             */
            SparseGraph(IndexType N){
                mpComm = &ParallelEnvironment::GetDataCommunicator("Serial");
            }

            /**
             * @brief 构造函数
             */
            SparseGraph(){
                mpComm = &ParallelEnvironment::GetDataCommunicator("Serial");
            }

            /**
             * @brief 构造函数
             */
            SparseGraph(DataCommunicator& rComm){
                if(rComm.IsDistributed()){
                    QUEST_ERROR <<"Attempting to construct a serial CsrMatrix with a distributed communicator" << std::endl;
                }

                mpComm = &rComm;
            }

            /**
             * @brief 析构函数
             */
            ~SparseGraph(){}

            /**
             * @brief 拷贝构造函数
             */
            SparseGraph(const SparseGraph& rOther):
                mpComm(rOther.mpComm),
                mGraph(rOther.mGraph)
            {
            }

            /**
             * @brief 构造函数
             */
            SparseGraph(const std::vector<IndexType>& rSingleVectorRepresentation){
                AddFromSingleVectorRepresentation(rSingleVectorRepresentation);
            }

            /**
             * @brief 获取数据通信器
             */
            const DataCommunicator& GetComm() const{
                return *mpComm;
            }

            /**
             * @brief 获取数据通信器指针
             */
            const DataCommunicator& pGetComm() const{
                return mpComm;
            }

            /**
             * @brief 返回矩阵行数
             */
            IndexType Size() const{
                if(!this->GetGraph().empty()){
                    return (--(this->GetGraph().end()))->first + 1;
                }
                return 0;
            }

            /**
             * @brief 判断矩阵图是否为空
             */
            bool IsEmpty() const{
                return mGraph.empty();
            }

            /**
             * @brief 判断（i,j）是否存在
             */
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

            /**
             * @brief 下标访问，返回第i行的非零元素的列索引集合
             */
            const typename GraphType::mapped_type& operator[](const IndexType& Key) const{
                return (mGraph.find(Key))->second;
            }

            /**
             * @brief 清空矩阵图
             */
            void Clear(){
                mGraph.clear();
            }

            /**
             * @brief 添加矩阵元素(i,j)
             */
            void AddEntry(const IndexType I, const IndexType J){
                mGraph[I].insert(J);
            }

            /**
             * @brief 添加元素集合
             */
            template<typename TContainerType>
            void AddEntries(const IndexType RowIndex, const TContainerType& rColumnIndices){
                mGraph[RowIndex].insert(rColumnIndices.begin(), rColumnIndices.end());
            }

            /**
             * @brief 添加元素集合
             */
            template<typename TIteratorType>
            void AddEntries(
                const IndexType RowIndex,
                const TIteratorType& rColBegin,
                const TIteratorType& rColEnd
            ){
                mGraph[RowIndex].insert(rColBegin, rColEnd);
            }

            /**
             * @brief 添加元素集合
             */
            template<typename TContainerType>
            void AddEntries(const TContainerType& rIndices){
                for(auto I : rIndices){
                    mGraph[I].insert(rIndices.begin(), rIndices.end());
                }
            }

            /**
             * @brief 添加元素集合
             */
            template<typename TContainerType>
            void AddEntries(const TContainerType& rRowIndices, const TContainerType& rColumnIndices){
                for(auto I : rRowIndices){
                    AddEntries(I, rColumnIndices);
                }
            }

            /**
             * @brief 添加元素集合
             */
            void AddEntries(SparseGraph& rOtherGraph){
                for(const auto& item : rOtherGraph.GetGraph()){
                    AddEntries(item.first, item.second);
                }
            }

            /**
             * @brief 终止函数
             */
            void Finalize(){}

            /**
             * @brief 获取图的存储结构(邻接表)
             */
            const GraphType& GetGraph() const{
                return mGraph;
            }

            /**
             * @brief 导出CSR数组格式
             */
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

            /**
             * @brief 导出CSR数组格式
             */
            IndexType ExportCSRArrays(
                Quest::span<IndexType>& rRowIndices,
                Quest::span<IndexType>& rColIndices
            ) const = delete;

            /**
             * @brief 导出CSR数组格式
             */
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

            /**
             * @brief 将图作为单个向量导出
             * @details 格式为：行索引 行中条目数量 行中所有索引的列表
             */
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

            /**
             * @brief 从单个向量导入图
             */
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

            /**
             * @brief 返回指向map中首行的迭代器
             */
            const_iterator_adaptor begin() const{
                return const_iterator_adaptor(mGraph.begin());
            }

            /**
             * @brief 返回指向map中尾行的迭代器
             */
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
            /**
             * @brief 数据通信器
             */
            DataCommunicator* mpComm;

            /**
             * @brief 图的存储结构(邻接表)
             * @details std::map<IndexType, std::unordered_set<IndexType>>
             *  IndexType: 存在非零元素的行索引
             *  std::unordered_set<IndexType>: 该行的非零元素的列索引集合
             */
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