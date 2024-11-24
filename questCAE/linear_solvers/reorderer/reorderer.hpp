#ifndef QUEST_REORDERER_HPP
#define QUEST_REORDERER_HPP

// 系统头文件
#include <string>
#include <iostream>
#include <vector>

// 项目头文件
#include "includes/define.hpp"

namespace Quest{

    /**
     * @class Reorderer
     * @brief 重排器类基类，用于对矩阵进行重排
     * @tparam TSparseSpaceType 矩阵稀疏存储类型
     * @tparam TDenseSpaceType 矩阵密集存储类型
     */
    template<typename TSparseSpaceType, typename TDenseSpaceType>
    class Reorderer{
        public:
            QUEST_CLASS_POINTER_DEFINITION(Reorderer);

            using SparseMatrixType = typename TSparseSpaceType::MatrixType;
            using VectorType = typename TSparseSpaceType::VectorType;
            using DenseMatrixType = typename TDenseSpaceType::MatrixType;
            using IndexType = unsigned int;
            using SizeType = unsigned int;
            using IndexVectorType = std::vector<IndexType>;

        public:
            /**
             * @brief 默认构造函数
             */
            Reorderer() {}


            /**
             * @brief 复制构造函数
             */
            Reorderer(const Reorderer& rOther) {}


            /**
             * @brief 析构函数
             */
            virtual ~Reorderer() {}


            /**
             * @brief 赋值运算符重载
             */
            Reorderer& operator = (const Reorderer& rOther) { return *this; }


            /**
             * @brief 初始化矩阵行索引
             * @details 调用CalculateIndexPermutation()方法计算矩阵的索引
             */
            virtual void Initialize(SparseMatrixType& rA, VectorType& rX, VectorType& rB){
                CalculateIndexPermutation(rA);
            }


            /**
             * @brief 对矩阵进行重排
             */
            virtual void Reorder(SparseMatrixType& rA, VectorType& rX, VectorType& rB){}


            /**
             * @brief 恢复矩阵的原始顺序
             */
            virtual void InverseReorder(SparseMatrixType& rA, VectorType& rX, VectorType& rB){}


            /**
             * @brief 实际初始化矩阵行索引的函数
             * @return std::vector<IndexType>& 矩阵的行索引
             */
            virtual IndexVectorType& CalculateIndexPermutation(SparseMatrixType& rA, IndexType InitialiIndex = IndexType()){
                SizeType size = TSparseSpaceType::Size1(rA);

                if(mIndexPermutation.size()!= size){
                    mIndexPermutation.resize(size);
                }

                for(SizeType i = 0; i < size; i++){
                    mIndexPermutation[i] = i;
                }

                return mIndexPermutation;
            }


            /**
             * @brief 获取矩阵的行索引
             */
            virtual IndexVectorType& GetIndexPermutation(){
                return mIndexPermutation;
            }


            virtual std::string Info() const{
                return "Reorderer";
            }


            virtual void PrintInfo(std::ostream& rOstream) const{
                rOstream << this->Info();
            }


            virtual void PrintData(std::ostream& rOstream)const {}

        protected:

        private:
            /**
             * @brief 存储重排后的矩阵的行索引
             */
            IndexVectorType mIndexPermutation;

    };


    template<typename TSparseSpaceType, typename TDenseSpaceType>
    inline std::istream& operator >> (std::istream& rIstream, Reorderer<TSparseSpaceType, TDenseSpaceType>& rThis);


    template<typename TSparseSpaceType, typename TDenseSpaceType>
    inline std::ostream& operator << (std::ostream& rOstream, const Reorderer<TSparseSpaceType, TDenseSpaceType>& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }

} // namespace Quest

#endif //QUEST_REORDERER_HPP