#ifndef QUEST_DIAGONAL_PRECONDITIONER_HPP
#define QUEST_DIAGONAL_PRECONDITIONER_HPP

// 第三方库文件
#include <boost/numeric/ublas/vector.hpp>

// 项目头文件
#include "includes/define.hpp"
#include "linear_solvers/preconditioner/preconditioner.hpp"

namespace Quest{

    template<typename TSpareSpaceType, typename TDenseSpaceType>
    class DiagonalPreconditioner : public Preconditioner<TSpareSpaceType, TDenseSpaceType>{
        public:
            QUEST_CLASS_POINTER_DEFINITION(DiagonalPreconditioner);

            using BaseType = Preconditioner<TSpareSpaceType, TDenseSpaceType>;
            using DataType = typename TSparseSpaceType::DataType;
            using SparseMatrixType = typename TSparseSpaceType::MatrixType;
            using VectorType = typename TSparseSpaceType::VectorType;
            using DenseMatrixType = typename TDenseSpaceType::MatrixType;

        public:
            /**
             * @brief 构造函数
             */
            DiagonalPreconditioner(){}

            /**
             * @brief 复制构造函数
             */
            DiagonalPreconditioner(const DiagonalPreconditioner& rOther):
                BaseType(rOther),
                mDiagonal(rOther.mDiagonal),
                mTemp(rOther.mTemp)
            {}


            /**
             * @brief 解构函数
             */
            virtual ~DiagonalPreconditioner() override {} 


            /**
             * @brief 赋值运算符
             */
            DiagonalPreconditioner& operator = (const DiagonalPreconditioner& rOther){
                BaseType::operator=(rOther);
                mDiagonal = rOther.mDiagonal;
                mTemp = rOther.mTemp;
                return *this;
            }


            /**
             * @brief 为线性系统 rA*rX = rB 初始化预处理器
             * @param rA 稀疏矩阵
             * @param rX 解向量
             * @param rB 右端常数向量
             */
            void Initialize(SparseMatrixType& rA, VectorType& rX, VectorType& rB) override{
                mDiagonal.resize(TSpareSpaceType::Size(rX));
                mTemp.resize(TSpareSpaceType::Size(rX));

                const DataType zero = DataType();

                IndexPartition<std::size_t>(rA.size1()).for_each([&](std::size_t Index){
                    double diag_Aii = rA(Index, Index);
                    if(diag_Aii != zero){
                        mDiagonal[Index] = 1.00/std::sqrt(std::abs(diag_Aii));
                    } else {
                        QUEST_THROW_ERROR(std::logic_error, "zero found in the diagonal. Diagonal preconditioner can not be used","");
                    }
                });
            }


            void Initialize(SparseMatrixType& rA, DenseMatrixType& rX, DenseMatrixType& rB) override{
                BaseType::Initialize(rA, rX, rB);
            }


            /**
             * @brief 计算稀疏矩阵 rA 与向量 rX 的乘积，支持通过 ApplyRight 和 ApplyLeft 对向量进行自定义处理
             */
            virtual void Mult(SparseMatrixType& rA, VectorType& rX, VectorType& rY) override {
                IndexPartition<std::size_t>(TSpareSpaceType::Size(rX)).for_each([&](std::size_t Index){
                    mTemp[Index] = rX[Index] * mDiagonal[Index];
                });

                TSpareSpaceType::Mult(rA, mTemp, rY);
                ApplyLeft(rY);
            }


            /**
             * @brief 计算稀疏矩阵 rA 的转置与向量 rX 的乘积，支持通过 ApplyTransposeLeft 和 ApplyTransposeRight 对向量进行自定义处理
             */
            virtual void TransposeMult(SparseMatrixType& rA, VectorType& rX, VectorType& rY) override {
                IndexPartition<std::size_t>(TSparseSpaceType::Size(rX)).for_each([&](std::size_t Index){
                    mTemp[Index] = rX[Index] * mDiagonal[Index];
                });

                TSparseSpaceType::TransposeMult(rA,mTemp, rY);
                ApplyRight(rY);
            }


            /**
             * @brief 对向量进行左预处理
             */
            virtual VectorType& ApplyLeft(VectorType& rX){
                IndexPartition<std::size_t>(TSparseSpaceType::Size(rX)).for_each([&](std::size_t Index){
                    rX[Index] *= mDiagonal[Index];
                });

                return rX;
            }


            /**
             * @brief 对向量进行右预处理
             */
            virtual VectorType& ApplyRight(VectorType& rX){
                IndexPartition<std::size_t>(TSparseSpaceType::Size(rX)).for_each([&](std::size_t Index){
                    rX[Index] *= mDiagonal[Index];
                });

                return rX;
            }


            /**
             * @brief 转置预处理系统（MT * X = y）的左预处理
             */
            virtual VectorType& ApplyTransposeLeft(VectorType& rX){
                IndexPartition<std::size_t>(TSparseSpaceType::Size(rX)).for_each([&](std::size_t Index){
                    rX[Index] *= mDiagonal[Index];
                });

                return rX;
            }


            /**
             * @brief 转置预处理系统（MT * X = y）的右预处理
             */
            virtual VectorType& ApplyTransposeRight(VectorType& rX){
                IndexPartition<std::size_t>(TSparseSpaceType::Size(rX)).for_each([&](std::size_t Index){
                    rX[Index] *= mDiagonal[Index];
                });

                return rX;
            }


            /**
             * @brief 右侧逆操作预处理
             */
            virtual VectorType& ApplyInverseRight(VectorType& rX){
                IndexPartition<std::size_t>(TSparseSpaceType::Size(rX)).for_each([&](std::size_t Index){
                    rX[Index] /= mDiagonal[Index];
                });

                return rX;
            }


            /**
             * @brief 用于恢复rX的值
             */
            virtual VectorType& Finalize(VectorType& rX){
                IndexPartition<std::size_t>(TSparseSpaceType::Size(rX)).for_each([&](std::size_t Index){
                    rX[Index] *= mDiagonal[Index];
                });

                return rX;
            }


            std::string Info() const override{
                return "Diagonal Preconditioner";
            }

            
            void PrintInfo(std::ostream& rOstream) const override{
                rOstream << "Diagonal Preconditioner";
            }

        protected:

        private:

        private:
            /**
             * @brief 存储矩阵的对角线元素
             */
            VectorType mDiagonal;

            /**
             * @brief 存储临时计算结果的向量
             */
            VectorType mTemp;

    };

    template<typename TSpareSpaceType, typename TDenseSpaceType>
    inline std::istream& operator >> (std::istream& rIstream, const DiagonalPreconditioner<TSpareSpaceType, TDenseSpaceType>& rThis){
        return rIstream;
    }


    template<typename TSpareSpaceType, typename TDenseSpaceType>
    inline std::ostream& operator << (std::ostream& rOstream, const DiagonalPreconditioner<TSpareSpaceType, TDenseSpaceType>& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }

}


#endif //QUEST_DIAGONAL_PRECONDITIONER_HPP