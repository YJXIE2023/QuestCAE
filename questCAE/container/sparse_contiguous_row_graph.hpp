#ifndef QUEST_SPARSE_CONTIGUOUS_ROW_GRAPH_HPP
#define QUEST_SPARSE_CONTIGUOUS_ROW_GRAPH_HPP

// 系统头文件
#include <iostream>
#include <unordered_map>
#include <unordered_set>

// 第三方头文件
#include "span/span.hpp"

// 项目头文件
#include "includes/define.hpp"
#include "includes/ublas_interface.hpp"
#include "includes/serializer.hpp"
#include "includes/lock_object.hpp"
#include "includes/parallel_enviroment.hpp"
#include "utilities/parallel_utilities.hpp"

namespace Quest{

    /**
     * @class SparseContiguousRowGraph
     * @brief 稀疏图结构，用于存储矩阵的稀疏表示，特别适合构造压缩行存储（CSR）格式的矩阵或其他类型的稀疏矩阵
     */
    template<typename TIndexType = std::size_t>
    class SparseContiguousRowGraph final{
        public:
            using IndexType = TIndexType;
            using GraphType = DenseVector<std::unordered_set<IndexType>>;
            using const_row_iterator = typename GraphType::const_iterator;

            QUEST_CLASS_POINTER_DEFINITION(SparseContiguousRowGraph);

        public:
            class const_iterator_adaptor{
                private:
                    const_row_iterator map_iterator;
                    const row_iterator mbegin;

                public:
                    using iterator_category = std::forward_iterator_tag;
                    using difference_type   = std::ptrdiff_t;
                    using value_type        = typename GraphType::value_type;
                    using pointer           = typename GraphType::value_type*;
                    using reference         = typename GraphType::value_type&;

                public:
                    const_iterator_adaptor(const_row_iterator it): map_iterator(it), mbegin(it){}
                    const_iterator_adaptor(const const_iterator_adaptor& it): map_iterator(it.map_iterator), mbegin(it.mbegin){}
                    const_iterator_adaptor& operator ++ () {mpa_iteartor++; return *this;}
                    const_iterator_adaptor operator ++ (int) {const_iterator_adaptor tmp(*this); operator ++(); return tmp;}
                    bool operator == (const const_iterator_adaptor& rhs) const {return map_iterator == rhs.map_iterator;}
                    bool operator!= (const const_iterator_adaptor& rhs) const {return map_iterator!= rhs.map_iterator;}
                    const typename GraphType::value_type& operator * () const {return *map_iterator;}
                    const typename GraphType::value_type& operator -> () const {return *map_iterator;}
                    const_row_iterator& base() {return map_iterator;}
                    const const_row_iterator& base() const {return map_iterator;}
                    IndexType GetRowIndex() const{return map_iterator-mbegin;}
            };

        public:
            /**
             * @brief 构造函数
             */
            SparseContiguousRowGraph(){
                mpComm = &ParallelEnvironment::GetDataCommunicator("Serial");
            }

            /**
             * @brief 构造函数
             */
            SparseContiguousRowGraph(IndexType GraphSize){
                mpComm = &ParallelEnvironment::GetDataCommunicator("Serial");
                mGraph.resize(GraphSize, false);
                mLocks = decltype(mLocks)(GraphSize);

                IndexPartition<IndexType>(GraphSize).for_each([&](IndexType i){
                    mGraph[i] = std::unordered_set<IndexType>();
                });
            }

            /**
             * @brief 析构函数
             */
            ~SparseContiguousRowGraph(){}

            /**
             * @brief 赋值运算符
             */
            SparseContiguousRowGraph& operator = (const SparseContiguousRowGraph& rOther) = delete;

            /**
             * @brief 拷贝构造函数
             */
            SparseContiguousRowGraph(const SparseContiguousRowGraph& rOther){
                mpComm = rOther.mpComm;
                mGraph.resize(rOther.mGraph.size());
                IndexPartition<IndexType>(rOther.mGraph.size()).for_each([&](IndexType i){
                    mGraph[i] = rOther.unordered_set<IndexType>();
                });
                mLocks = decltype(mLocks)(rOther.mLocks);
                this->AddEntries(rOther);
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
            const DataCommunicator* pGetComm() const{
                return mpComm;
            }

            /**
             * @brief 清空容器
             */
            void Clear(){
                mGraph.clear();
                mLocks.clear();
            }

            /**
             * @brief 获取矩阵行数
             */
            inline IndexType Size() const{
                return mGraph.size();
            }

            /**
             * @brief 检查（i,j）是否存在
             */
            bool Has(const IndexType i, const IndexType j) const{
                const auto& row = mGraph[i];
                return (ro.find(j) != row.end());
            }

            /**
             * @brief 下标访问，获取第i行的元素
             */
            const typename GraphType::value_type& operator [] (const IndexType& i) const{
                return mGraph[i];
            }

            /**
             * @brief 返回本地索引
             */
            inline IndexType LocalIndex(const IndexType GlobalIndex) const{
                return GlobalIndex;
            }

            /**
             * @brief 返回全局索引
             */
            inline IndexType GlobalIndex(const IndexType LocalIndex) const{
                return LocalIndex;
            }

            /**
             * @brief 添加一个元素
             */
            void AddEntry(const IndexType RowIndex, const IndexType ColIndex){
                mLocks[RowIndex].lock();
                mGraph[RowIndex].insert(ColIndex);
                mLocks[RowIndex].unlock();
            }

            /**
             * @brief 添加一组元素
             */
            template<typename TContainerType>
            void AddEntries(const IndexType RowIndex, const TContainerType& rColIndices){
                mLocks[RowIndex].lock();
                mGraph[RowIndex].insert(rColIndices.begin(), rColIndices.end());
                mLocks[RowIndex].unlock();
            }

            /**
             * @brief 添加一组元素
             */
            template<typename TIteratorType>
            void AddEntries(
                const IndexType RowIndex,
                const TIteratorType& rColBegin,
                const TIteratorType& rColEnd
            ){
                mLocks[RowIndex].lock();
                mGraph[RowIndex].insert(rColBegin, rColEnd);
                mLocks[RowIndex].unlock();
            }

            /**
             * @brief 添加一组元素
             */
            template<typename TContainerType>
            void AddEntries(const TContainerType& rIndices){
                for(auto i : rIndices){
                    QUEST_DEBUG_ERROR_IF(i > this->Size()) << "Index : " << I
                        << " exceeds the graph size : " << Size() << std::endl;
                    mLocks[i].lock();
                    mGraph[i].insert(rIndices.begin(), rIndices.end());
                    mLocks[i].unlock();
                }
            }

            /**
             * @brief 添加一组元素
             */
            template<typename TContainerType>
            void AddEntries(const TContainerType& rRowIndices, const TContainerType& rColIndices){
                for(auto i : rRowIndices){
                    AddEntries(i, rColIndices);
                }
            }

            /**
             * @brief 添加一组元素
             */
            void AddEntries(const SparseContiguousRowGraph& rOther){
                for(IndexType i=0;i<rOther.Size();i++){
                    AddEntries(i, rOther.GetGraph()[i]);
                }
            }

            /**
             * @brief 结束并行化
             */
            void Finalize(){}

            /**
             * @brief 获取图的邻接表表示
             */
            const GraphType& GetGraph() const{
                return mGraph;
            }

            /**
             * @brief 导出CSR格式的数组
             * 
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
             * @brief 导出CSR格式的数组
             * 
             */
            IndexType ExportCSRArrays(
                Quest::span<IndexType>& rRowIndices,
                Quest::span<IndexType>& rColIndices
            ) const = delete;

            /**
             * @brief 导出CSR格式的数组
             * 
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

                if(nrows == 0){
                    row_indices[0] = 0;
                } else {
                    IndexPartition<IndexType>(nrows+1).for_each([&](IndexType i){
                        row_indices[i] = 0;
                    });

                    IndexPartition<IndexType>(nrows).for_each([&](IndexType i){ 
                        row_indices[i+1] = mGraph[i].size();
                    });

                    for(IndexType i = 1; i < static_cast<IndexType>(row_indices.size()); ++i){
                        row_indices[i] += row_indices[i-1];
                    }
                }

                IndexType nnz = row_indices[nrows];
                rColDataSize = nnz;
                pColIndicesData = new IndexType[nnz];
                Quest::span<IndexType> col_indices(pColIndicesData, nnz);

                IndexPartition<IndexType>(col_indices.size()).for_each([&](IndexType i){
                    col_indices[i] = 0;
                });

                IndexPartition<IndexType>(nrows).for_each([&](IndexType i){
                    IndexType start = row_indices[i];

                    IndexType counter = 0;
                    for(auto j : mGraph[i]){
                        col_indices[start+counter] = j;
                        counter++;
                    }
                });


                IndexPartition<IndexType>(row_indices.size()-1).for_each([&](IndexType i){
                    std::sort(col_indices.begin()+row_indices[i], col_indices.begin()+row_indices[i+1]);
                });

                return nrows;
            }

            /**
             * @brief 将图作为单个向量导出
             * @details 格式为：行索引 行中条目数量 行中所有索引的列表
             */
            std::vector<IndexType> ExportSingleVectorRepresentation(){
                std::vector<IndexType> IJ;
                IJ.push_back(GetGraph().size());
                for(unsigned int I=0; I<GetGraph().size(); ++I){
                    IJ.push_back(I);
                    IJ.push_back(mGraph[I].size());
                    for(auto j : mGraph[I]){
                        IJ.push_back(j);
                    }
                }
                return IJ;
            }

            /**
             * @brief 从单个向量导入图
             */
            void AddFromSingleVectorRepresentation(const std::vector<IndexType>& rSingleVectorRepresentation){
                auto graph_size = rSingleVectorRepresentation[0];
                QUEST_ERROR_IF(graph_size > GetGraph().size()) 
                    << "mismatching size - attempting to add a graph with more rows than the ones allowed in graph" << std::endl;
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
             * @brief 返回DenseVector的首元素的迭代器
             */
            const_iterator_adaptor begin() const{
                return const_iterator_adaptor(mGraph.begin());
            }

            /**
             * @brief 返回DenseVector的尾元素的迭代器
             */
            const_iterator_adaptor end() const{
                return const_iterator_adaptor(mGraph.end());
            }


            std::string Info() const{
                std::stringstream buffer;
                buffer << "SparseContiguousRowGraph";
                return buffer.str();
            }


            void PrintInfo(std::ostream& rOstream) const{
                rOstream << "SparseContiguousRowGraph";
            }


            void PrintData(std::ostream& rOstream) const{}

        protected:

        private:
            friend class Serializer;

            void save(Serializer& rSerializer) const{
                const IndexType N = this->Size();
                rSerializer.save("GraphSize",N);
                for(IndexType i=0;i<N;i++){
                    IndexType row_size = mGraph[i].size();
                    rSerializer.save("row_size",row_size);
                    for(auto j : mGraph[i]){
                        rSerializer.save("J",j);
                    }
                }
            }


            void load(Serializer& rSerializer){
                IndexType size;
                rSerializer.load("GraphSize",size);

                mLocks = decltype(mLocks)(size);
                mGraph.resize(size);

                for(IndexType i=0;i<size;i++){
                    IndexType row_size;
                    rSerializer.load("row_size",row_size);
                    for(IndexType j=0;j<row_size;j++){
                        IndexType J;
                        rSerializer.load("J",J);
                        AddEntry(i,J);
                    }
                }
            }

        private:
            /**
             * @brief 数据通讯器指针
             */
            DataCommunicator* mpComm;

            /**
             * @brief 图的邻接表表示
             * @details DenseVector<std::unordered_set<IndexType>>
             *  存储每行的非零元素集合
             */
            GraphType mGraph;

            /**
             * @brief 锁对象数组
             */
            std::vector<LockObject> mLocks;

    };


    template<typename TIndexType=std::size_t>
    inline std::istream& operator >> (std::istream& rIstream, SparseContiguousRowGraph<TIndexType>& rThis){
        return rIstream;
    }


    template<typename TIndexType=std::size_t>
    inline std::ostream& operator << (std::ostream& rOstream, const SparseContiguousRowGraph<TIndexType>& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }

} // namespace Quest

#endif //QUEST_SPARSE_CONTIGUOUS_ROW_GRAPH_HPP