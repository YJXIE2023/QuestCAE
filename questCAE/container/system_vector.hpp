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

    /**
     * @class SystemVector
     * @brief 用于存储和操作系统向量，并提供 FEM 特定的组装功能
     * @tparam TDataType 向量元素的数据类型
     * @tparam TIndexType 向量元素的索引类型
     */
    template<typename TDataType = double, typename TIndexType = std::size_t>
    class SystemVector final{
        public:
            using IndexType = TIndexType;

            QUEST_CLASS_POINTER_DEFINITION(SystemVector);

        public:
            /**
             * @brief 构造函数
             */
            SystemVector(const SparseGraph<IndexType>& rGraph){
                mpComm = rGraph.pGetComm();
                mData.resize(rGraph.Size(),false);
            }

            /**
             * @brief 构造函数
             */
            SystemVector(const SparseContiguousRowGraph<IndexType>& rGraph){
                mpComm = rGraph.pGetComm();
                mData.resize(rGraph.Size(),false);
            }

            /**
             * @brief 构造函数
             */
            SystemVector(IndexType size, DataCommunicator& rComm = ParallelEnvironment::GetDataCommunicator("Serial")){
                if(rComm.IsDistributed()){
                    QUEST_ERROR << "Attempting to construct a serial system_vector with a distributed communicator" << std::endl;
                }
                mpComm = &rComm;
                mData.resize(size,false);
            }

            /**
             * @brief 构造函数
             */
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

            /**
             * @brief 拷贝构造函数
             */
            explicit SystemVector(const SystemVector<TDataType, TIndexType>& rOther){
                mpComm = rOther.mpComm;
                mData.resize(rOther.size(),false);

                IndexPartition<IndexType>(size()).for_each([&](IndexType i){
                    (*this)[i] = rOther[i];
                });
            }

            /**
             * @brief 析构函数
             */
            ~SystemVector(){}

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
                mData.clear();
            }

            /**
             * @brief 设置向量所有元素的值为 value
             */
            void SetValue(const TDataType value){
                IndexPartition<IndexType>(mData.size()).for_each([&](IndexType i){
                    mData[i] = value;
                });
            }

            /**
             * @brief 返回向量的大小
             */
            IndexType size() const{
                return mData.size();
            }

            /**
             * @brief 解引用操作符，返回向量的第 i 个元素的引用
             */
            TDataType& operator()(IndexType i){
                return mData[i];
            }

            /**
             * @brief 解引用操作符，返回向量的第 i 个元素的引用
             */
            const TDataType& operator()(IndexType i) const{
                return mData[i];
            }

            /**
             * @brief 重载 [] 操作符，返回向量的第 i 个元素的引用
             */
            TDataType& operator[](IndexType i){
                return mData[i];
            }

            /**
             * @brief 重载 [] 操作符，返回向量的第 i 个元素的引用
             */
            const TDataType& operator[](IndexType i) const{
                return mData[i];
            }

            /**
             * @brief 返回底层数据容器
             */
            DenseVector<TDataType>& data(){
                return mData;
            }

            /**
             * @brief 返回底层数据容器
             */
            const DenseVector<TDataType>& data() const{
                return mData;
            }

            /**
             * @brief 加上 factor * rOtherVector
             */
            void Add(
                const TDataType factor,
                const SystemVector& rOtherVector
            ){
                IndexPartition<IndexType>(size()).for_each([&](IndexType i){
                    (*this)[i] += factor*rOtherVector[i];
                });
            }

            /**
             * @brief 赋值操作符
             */
            SystemVector& operator=(const SystemVector& rOther){
                IndexPartition<IndexType>(size()).for_each([&](IndexType i){
                    (*this)[i] = rOther[i];
                });
                return *this;
            }

            /**
             * @brief +=操作符
             */
            SystemVector& operator+=(const SystemVector& rOther){
                IndexPartition<IndexType>(size()).for_each([&](IndexType i){
                    (*this)[i] += rOther[i];
                });
                return *this;
            }

            /**
             * @brief -=操作符
             */
            SystemVector& operator-=(const SystemVector& rOther){
                IndexPartition<IndexType>(size()).for_each([&](IndexType i){
                    (*this)[i] -= rOther[i];
                });
                return *this;
            }

            /**
             * @brief *=操作符
             */
            SystemVector& operator*=(const TDataType multiplier_factor){
                IndexPartition<IndexType>(size()).for_each([&](IndexType i){
                    (*this)[i] *= multiplier_factor;
                });
                return *this;
            }

            /**
             * @brief /=操作符
             */
            SystemVector& operator/=(const TDataType divisor_factor){
                IndexPartition<IndexType>(size()).for_each([&](IndexType i){
                    (*this)[i] /= divisor_factor;
                });
                return *this;
            }

            /**
             * @brief 点乘操作符
             */
            TDataType Dot(const SystemVector& rOtherVector, IndexType gather_on_rank=0){
                QUEST_WARNING_IF("SystemVector",gather_on_rank!=0) 
                    << "the parameter gather_on_rank essentially does nothing for a non-distribued vector. It is added to have the same interface as for the distributed_system_vector" << std::endl;

                auto partition = IndexPartition<IndexType>(size());
                TDataType dot_value = partition.template for_each<SumReduction<TDataType>>([&](IndexType i){
                    return (*this)[i] * rOtherVector[i];
                });

                return dot_value;
            }

            /**
             * brief 并行计算中初始化
             */
            void BeginAssemble(){}

            /**
             * @brief 结束并行化计算
             */
            void FinalizeAssemble(){}

            /**
             * @brief 将输入向量的数据（rVectorInput）根据给定的方程 ID （EquationId）组装到全局向量 mData 中
             */
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
            /**
             * @brief 数据通信器指针
             */
            const DataCommunicator& mpComm;
            
            /**
             * @brief 系统向量数据
             */
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