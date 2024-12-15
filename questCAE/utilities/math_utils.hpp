#ifndef QUEST_MATH_UTILS_HPP
#define QUEST_MATH_UTILS_HPP

// 系统头文件
#include <cmath>
#include <type_traits>

// 项目头文件
#include "IO/logger.hpp"
#include "includes/ublas_interface.hpp"
#include "includes/global_variables.hpp"

namespace Quest{
    
    /**
     * @class MathUtils
     * @brief 数学工具类,提供常用数学计算功能
     */
    template<typename TDataType = double>
    class QUEST_API(QUEST_API) MathUtils{
        public:
            using MatrixType = Matrix;
            using VectorType = Vector;
            using SizeType = std::size_t;
            using IndexType = std::size_t;
            using IndirectArrayType = boost::numeric::ublas::indirect_array<DenseVector<std::size_t>>;

            static constexpr double ZeroTolerance = std::numeric_limits<double>::epsilon();

        public:
            /**
             * @brief 获取容差
             */
            static inline double GetZeroTolerance() {
                return ZeroTolerance;
            }

            /**
             * @brief Heron 三角形面积计算
             * @param a 第一边长
             * @param b 第二边长
             * @param c 第三边长
             */
            template<bool TCheck>
            static inline double Heron(double a, double b, double c){
                const double s = (a + b + c) / 2.0;
                const double A2 = s * (s - a) * (s - b) * (s - c);
                if constexpr (TCheck){
                    if (A2 < 0.0){
                        QUEST_ERROR << "The square of area if negative, probably the triangle is in bad shape:" << A2 << std::endl;
                    } else {
                        return std::sqrt(A2);
                    }
                } else {
                    return std::sqrt(std::abs(A2));
                }
            }

            /**
             * @brief 计算矩阵的代数余子式
             * @param rMat 矩阵
             * @param i 行索引
             * @param j 列索引
             */
            template<typename TMatrixType>
            static double Cofactor(const TMatrixType& rMat, IndexType i, IndexType j){
                static_assert(std::is_same<typename TMatrixType::value_type, double>::value, "Bad value type.");

                QUEST_ERROR_IF(rMat.size1() != rMat.size2() || rMat.size1() == 0) << "Bad matrix dimensions." << std::endl;

                if(rMat.size1() == 1){
                    return 1;
                }

                IndirectArrayType ia1(rMat.size1() - 1), ia2(rMat.size2() - 1);

                unsigned i_sub = 0;
                for(unsigned k = 0; k < rMat.size1(); ++k){
                    if(k!= i){
                        ia1(i_sub++) = k;
                    }
                }

                unsigned j_sub = 0;
                for(unsigned k = 0; k < rMat.size2(); ++k){
                    if(k!= j){
                        ia2(j_sub++) = k;
                    }
                }

                boost::numeric::ublas::matrix_indirect<const TMatrixType, IndirectArrayType> sub_mat(rMat, ia1, ia2);
                const double first_minor = Det(sub_mat);
                return ((i + j) % 2)? -first_minor : first_minor;
            }

            /**
             * @brief 计算矩阵的伴随矩阵（即矩阵的代数余子式矩阵）
             * @param rMat 矩阵
             */
            template<typename TMatrixType>
            static MatrixType CofactorMatrix(const TMatrixType& rMat){
                static_assert(std::is_same<typename TMatrixType::value_type, double>::value, "Bad value type.");

                MatrixType cofactor_mat(rMat.size1(), rMat.size2());

                for(IndexType i = 0; i < rMat.size1(); ++i){
                    for(IndexType j = 0; j < rMat.size2(); ++j){
                        cofactor_mat(i, j) = Cofactor(rMat, i, j);
                    }
                }

                return cofactor_mat;
            }

            /**
             * @brief 用于检查矩阵的条件数（判断矩阵是否接近奇异）
             * @param rInputMatrix 输入矩阵（不会被修改）
             * @param rInvertedMatrix 逆矩阵
             * @param Tolerance 容差
             * @param ThrowError 条件数不满足要求时为false
             */
            template<typename TMatrix1, typename TMatrix2>
            static inline bool CheckConditionNumber(
                const TMatrix1& rInputMatrix,
                TMatrix2& rInvertedMatrix,
                const double Tolerance = std::numeric_limits<double>::epsilon(),
                const bool ThrowError = true)
            {
                const double max_condition_number = (1.0/Tolerance)*1.0e-4;

                const double input_matrix_norm = norm_frobenius(rInputMatrix);
                const double inverted_matrix_norm = norm_frobenius(rInvertedMatrix);

                const double cond_number = input_matrix_norm * inverted_matrix_norm;

                if(cond_number > max_condition_number){
                    if(ThrowError){
                        QUEST_WATCH(rInputMatrix);
                        QUEST_ERROR << " Condition number of the matrix is too high!, cond_number = " << cond_number << std::endl;
                    }
                    return false;
                }

                return false;
            }

            /**
             * @brief 计算非方阵的逆矩阵
             * @param rInputMatrix 输入矩阵（不会被修改）
             * @param rInvertedMatrix 输出矩阵，即输入矩阵的逆矩阵
             * @param rInputMatrixDet 输入矩阵的行列式
             * @param Tolerance 容差
             * @tparam TMatrix1 输入矩阵类型
             * @tparam TMatrix2 输出矩阵类型
             */
            template<typename TMatrix1, typename TMatrix2>
            static void GeneralizedInvertMatrix(
                const TMatrix1& rInputMatrix,
                TMatrix2& rInvertedMatrix,
                double& rInputMatrixDet,
                const double Tolerance = ZeroTolerance
            ){
                const SizeType size_1 = rInputMatrix.size1();
                const SizeType size_2 = rInputMatrix.size2();

                if(size_1 != size_2){
                    InverMatrix(rInputMatrix, rInvertedMatrix, rInputMatrixDet, Tolerance);
                } else if (size_1 < size_2){
                    if (rInvertedMatrix.size1() != size_2 || rInvertedMatrix.size2() != size_1){
                        rInvertedMatrix.resize(size_2, size_1, false);
                    }
                    const Matrix aux = prod(rInputMatrixDet, trans(rInputMatrix));
                    Matrix auxInv;
                    InverMatrix(aux, auxInv, rInputMatrixDet, Tolerance);
                    rInputMatrixDet = std::sqrt(rInputMatrixDet);
                    noalias(rInvertedMatrix) = prod(trans(rInputMatrix), auxInv);
                } else {
                    if(rInvertedMatrix.size1() != size_2 || rInvertedMatrix.size2() != size_1){
                        rInvertedMatrix.resize(size_2, size_1, false);
                    }
                    const Matrix aux = prod(trans(rInputMatrix), rInputMatrix);
                    Matrix auxInv;
                    InverMatrix(aux, auxInv, rInputMatrixDet, Tolerance);
                    rInputMatrixDet = std::sqrt(rInputMatrixDet);
                    noalias(rInvertedMatrix) = prod(auxInv, trans(rInputMatrix));
                }
            }

            /**
             * @brief 求解稠密线性方程组
             * @param A 系数矩阵
             * @param rX 解向量，也可为迭代求解器的初始猜测值
             * @param rB 右端常数
             */
            static void Solve(
                MatrixType A,
                VectorType& rX,
                const VectorType& rB
            );

            /**
             * @brief 用于求解2阶、3阶、4阶矩阵的逆矩阵
             * @param rInputMatrix 输入矩阵（不会被修改）
             * @param rInvertedMatrix 输出矩阵，即输入矩阵的逆矩阵
             * @param rInputMatrixDet 输入矩阵的行列式
             * @param Tolerance 容差
             * @tparam TMatrix1 输入矩阵类型
             * @tparam TMatrix2 输出矩阵类型
             */
            template<typename TMatrix1, typename TMatrix2>
            static void InvertMatrix(
                const TMatrix1& rInputMatrix,
                TMatrix2& rInvertedMatrix,
                double& rInputMatrixDet,
                const double Tolerance = ZeroTolerance
            ){
                QUEST_DEBUG_ERROR_IF_NOT(rInputMatrix.size1() == rInputMatrix.size2()) << "Matrix provided is non-square" << std::endl;

                const SizeType size = rInputMatrix.size2();

                if(size == 1){
                    if(rInvertedMatrix.size1() != 1 || rInvertedMatrix.size2() != 1){
                        rInvertedMatrix.resize(1, 1, false);
                    }
                    rInvertedMatrix(0, 0) = 1.0 / rInputMatrix(0, 0);
                    rInputMatrixDet = rInputMatrix(0, 0);
                } else if(size == 2){
                    InvertMatrix2(rInputMatrix, rInvertedMatrix, rInputMatrixDet);
                } else if(size == 3){
                    InvertMatrix3(rInputMatrix, rInvertedMatrix, rInputMatrixDet);
                } else if(size == 4){
                    InvertMatrix4(rInputMatrix, rInvertedMatrix, rInputMatrixDet);
                } else if (std::is_same<TMatrix1, Matrix>::value) {
                    const SizeType size1 = rInputMatrix.size1();
                    const SizeType size2 = rInputMatrix.size2();
                    if(rInvertedMatrix.size1() != size1 || rInvertedMatrix.size2() != size2){
                        rInvertedMatrix.resize(size1, size2, false);
                    }

                    Matrix A(rInputMatrix);
                    typedef permutation_matrix<SizeType> pmatrix;
                    pmatrix pm(A.size1());
                    const int singulaar = lu_factorize(A, pm);
                    rInvertedMatrix.assign(IndentityMatrix(A.size1()));
                    QUEST_ERROR_IF(singular == 1) << "Matrix is singular: " << rInputMatrix << std::endl;
                    lu_substitute(A, pm, rInvertedMatrix);

                    rInputMatrixDet = 1.0;

                    for(IndexType i = 0; i < size1; ++i){
                        IndexType ki = pm[i] == i? 0:1;
                        rInputMatrixDet *= (ki == 0)? A(i, i) : -A(i, i);
                    }
                } else {
                    const SizeType size1 = rInputMatrix.size1();
                    const SizeType size2 = rInputMatrix.size2();

                    Matrix A(rInputMatrix);
                    Matrix invA(rInvertedMatrix);

                    typedef permutation_matrix<SizeType> pmatrix;
                    pmatrix pm(size1);
                    const int singular = lu_factorize(A, pm);
                    invA.assign(IndentityMatrix(size1));
                    QUEST_ERROR_IF(singular == 1) << "Matrix is singular: " << rInputMatrix << std::endl;
                    lu_substitute(A, pm, invA);

                    rInputMatrixDet = 1.0;

                    for(IndexType i = 0; i < size1; ++i){
                        IndexType ki = pm[i] == i? 0:1;
                        rInputMatrixDet *= (ki == 0)? A(i, i) : -A(i, i);
                    }

                    for(IndexType i = 0; i < size1; ++i){
                        for(IndexType j = 0; j < size2; ++j){
                            rInvertedMatrix(i, j) = invA(i, j);
                        }
                    }
                }

                if(Tolerance > 0.0){
                    CheckConditionNumber(rInputMatrix, rInvertedMatrix, Tolerance);
                }
            }

            /**
             * @brief 用于求解2阶矩阵的逆矩阵
             * @param rInputMatrix 输入矩阵（不会被修改）
             * @param rInvertedMatrix 输出矩阵，即输入矩阵的逆矩阵
             * @param rInputMatrixDet 输入矩阵的行列式
             * @tparam TMatrix1 输入矩阵类型
             * @tparam TMatrix2 输出矩阵类型
             */
            template<typename TMatrix1, typename TMatrix2>
            static void InvertMatrix2(
                const TMatrix1& rInputMatrix,
                TMatrix2& rInvertedMatrix,
                double& rInputMatrixDet
            ){
                QUEST_TRY;

                QUEST_DEBUG_ERROR_IF_NOT(rInputMatrix.size1() == rInputMatrix.size2()) << "Matrix provided is non-square" << std::endl;

                if(rInvertedMatrix.size1() != 2 || rInvertedMatrix.size2() != 2){
                    rInvertedMatrix.resize(2, 2, false);
                }

                rInputMatrixDet = rInputMatrix(0, 0) * rInputMatrix(1, 1) - rInputMatrix(0, 1) * rInputMatrix(1, 0);

                rInvertedMatrix(0, 0) = rInputMatrix(1, 1);
                rInvertedMatrix(0, 1) = -rInputMatrix(0, 1);
                rInvertedMatrix(1, 0) = -rInputMatrix(1, 0);
                rInvertedMatrix(1, 1) = rInputMatrix(0, 0);

                rInvertedMatrix /= rInputMatrixDet;

                QUEST_CATCH("")
            }

            /**
             * @brief 用于求解 3 阶矩阵的逆矩阵
             * @param rInputMatrix 输入矩阵（在输出时保持不变）
             * @param rInvertedMatrix 计算得到的逆矩阵
             * @param rInputMatrixDet 输入矩阵的行列式
             */
            template<typename TMatrix1, typename TMatrix2>
            static void InvertMatrix3(
                const TMatrix1& rInputMatrix,   
                TMatrix2& rInvertedMatrix,
                double& rInputMatrixDet
            ){
                QUEST_TRY;

                QUEST_DEBUG_ERROR_IF_NOT(rInputMatrix.size1() == rInputMatrix.size2()) << "Matrix provided is non-square" << std::endl;

                if(rInvertedMatrix.size1() != 3 || rInvertedMatrix.size2() != 3){
                    rInvertedMatrix.resize(3, 3, false);
                }

                rInvertedMatrix(0, 0) = rInputMatrix(1, 1) * rInputMatrix(2, 2) - rInputMatrix(1, 2) * rInputMatrix(2, 1);
                rInvertedMatrix(1, 0) = -rInputMatrix(1, 0) * rInputMatrix(2, 2) + rInputMatrix(1, 2) * rInputMatrix(2, 0);
                rInvertedMatrix(2, 0) = rInputMatrix(1, 0) * rInputMatrix(2, 1) - rInputMatrix(1, 1) * rInputMatrix(2, 0);

                rInvertedMatrix(0, 1) = -rInputMatrix(0, 1) * rInputMatrix(2, 2) + rInputMatrix(0, 2) * rInputMatrix(2, 1);
                rInvertedMatrix(1, 1) = rInputMatrix(0, 0) * rInputMatrix(2, 2) - rInputMatrix(0, 2) * rInputMatrix(2, 0);
                rInvertedMatrix(2, 1) = -rInputMatrix(0, 0) * rInputMatrix(2, 1) + rInputMatrix(0, 1) * rInputMatrix(2, 0);

                rInvertedMatrix(0, 2) = rInputMatrix(0, 1) * rInputMatrix(1, 2) - rInputMatrix(0, 2) * rInputMatrix(1, 1);
                rInvertedMatrix(1, 2) = -rInputMatrix(0, 0) * rInputMatrix(1, 2) + rInputMatrix(0, 2) * rInputMatrix(1, 0);
                rInvertedMatrix(2, 2) = rInputMatrix(0, 0) * rInputMatrix(1, 1) - rInputMatrix(0, 1) * rInputMatrix(1, 0);

                rInputMatrixDet = rInputMatrix(0, 0) * rInvertedMatrix(0, 0) + rInputMatrix(0, 1) * rInvertedMatrix(1, 0) + rInputMatrix(0, 2) * rInvertedMatrix(2, 0);

                rInvertedMatrix /= rInputMatrixDet;

                QUEST_CATCH("")
            }

            /**
             * @brief 用于求解 4 阶矩阵的逆矩阵
             * @param rInputMatrix 输入矩阵（在输出时保持不变）
             * @param rInvertedMatrix 计算得到的逆矩阵
             * @param rInputMatrixDet 输入矩阵的行列式
             */
            template<typename TMatrix1, typename TMatrix2>
            static void InvertMatrix4(
                const TMatrix1& rInputMatrix,
                TMatrix2& rInvertedMatrix,
                double& rInputMatrixDet
            ){
                QUEST_TRY;

                QUEST_DEBUG_ERROR_IF_NOT(rInputMatrix.size1() == rInputMatrix.size2()) << "Matrix provided is non-square" << std::endl;

                if(rInvertedMatrix.size1() != 4 || rInvertedMatrix.size2() != 4){
                        rInvertedMatrix.resize(4, 4, false);
                }

                rInvertedMatrix(0, 0) = -(rInputMatrix(1, 3) * rInputMatrix(2, 2) * rInputMatrix(3, 1)) + (rInputMatrix(1, 2) * rInputMatrix(2, 3) * rInputMatrix(3, 1)) + (rInputMatrix(1, 3) * rInputMatrix(2, 1) * rInputMatrix(3, 2)) - (rInputMatrix(1, 1) * rInputMatrix(2, 3) * rInputMatrix(3, 2)) - (rInputMatrix(1, 2) * rInputMatrix(2, 1) * rInputMatrix(3, 3)) + (rInputMatrix(1, 1) * rInputMatrix(2, 2) * rInputMatrix(3, 3));
                rInvertedMatrix(1, 0) = (rInputMatrix(0, 3) * rInputMatrix(2, 2) * rInputMatrix(3, 1)) - (rInputMatrix(0, 2) * rInputMatrix(2, 3) * rInputMatrix(3, 1)) - (rInputMatrix(0, 3) * rInputMatrix(2, 1) * rInputMatrix(3, 2)) + (rInputMatrix(0, 1) * rInputMatrix(2, 3) * rInputMatrix(3, 2)) + (rInputMatrix(0, 2) * rInputMatrix(2, 1) * rInputMatrix(3, 3)) - (rInputMatrix(0, 1) * rInputMatrix(2, 2) * rInputMatrix(3, 3));
                rInvertedMatrix(2, 0) = -(rInputMatrix(0, 3) * rInputMatrix(1, 2) * rInputMatrix(3, 1)) + (rInputMatrix(0, 2) * rInputMatrix(1, 3) * rInputMatrix(3, 1)) + (rInputMatrix(0, 3) * rInputMatrix(1, 1) * rInputMatrix(3, 2)) - (rInputMatrix(0, 1) * rInputMatrix(1, 3) * rInputMatrix(3, 2)) - (rInputMatrix(0, 2) * rInputMatrix(1, 1) * rInputMatrix(3, 3)) + (rInputMatrix(0, 1) * rInputMatrix(1, 2) * rInputMatrix(3, 3));
                rInvertedMatrix(3, 0) = (rInputMatrix(0, 3) * rInputMatrix(1, 2) * rInputMatrix(2, 1)) - (rInputMatrix(0, 2) * rInputMatrix(1, 3) * rInputMatrix(2, 1)) - (rInputMatrix(0, 3) * rInputMatrix(1, 1) * rInputMatrix(2, 2)) + (rInputMatrix(0, 1) * rInputMatrix(1, 3) * rInputMatrix(2, 2)) + (rInputMatrix(0, 2) * rInputMatrix(1, 1) * rInputMatrix(2, 3)) - (rInputMatrix(0, 1) * rInputMatrix(1, 2) * rInputMatrix(2, 3));

                rInvertedMatrix(0, 1) = (rInputMatrix(1, 3) * rInputMatrix(2, 2) * rInputMatrix(3, 0)) - (rInputMatrix(1, 2) * rInputMatrix(2, 3) * rInputMatrix(3, 0)) - (rInputMatrix(1, 3) * rInputMatrix(2, 0) * rInputMatrix(3, 2)) + (rInputMatrix(1, 0) * rInputMatrix(2, 3) * rInputMatrix(3, 2)) + (rInputMatrix(1, 2) * rInputMatrix(2, 0) * rInputMatrix(3, 3)) - (rInputMatrix(1, 0) * rInputMatrix(2, 2) * rInputMatrix(3, 3));
                rInvertedMatrix(1, 1) = -(rInputMatrix(0, 3) * rInputMatrix(2, 2) * rInputMatrix(3, 0)) + (rInputMatrix(0, 2) * rInputMatrix(2, 3) * rInputMatrix(3, 0)) + (rInputMatrix(0, 3) * rInputMatrix(2, 0) * rInputMatrix(3, 2)) - (rInputMatrix(0, 0) * rInputMatrix(2, 3) * rInputMatrix(3, 2)) - (rInputMatrix(0, 2) * rInputMatrix(2, 0) * rInputMatrix(3, 3)) + (rInputMatrix(0, 0) * rInputMatrix(2, 2) * rInputMatrix(3, 3));
                rInvertedMatrix(2, 1) = (rInputMatrix(0, 3) * rInputMatrix(1, 2) * rInputMatrix(3, 0)) - (rInputMatrix(0, 2) * rInputMatrix(1, 3) * rInputMatrix(3, 0)) - (rInputMatrix(0, 3) * rInputMatrix(1, 0) * rInputMatrix(3, 2)) + (rInputMatrix(0, 0) * rInputMatrix(1, 3) * rInputMatrix(3, 2)) + (rInputMatrix(0, 2) * rInputMatrix(1, 0) * rInputMatrix(3, 3)) - (rInputMatrix(0, 0) * rInputMatrix(1, 2) * rInputMatrix(3, 3));
                rInvertedMatrix(3, 1) = -(rInputMatrix(0, 3) * rInputMatrix(1, 2) * rInputMatrix(2, 0)) + (rInputMatrix(0, 2) * rInputMatrix(1, 3) * rInputMatrix(2, 0)) + (rInputMatrix(0, 3) * rInputMatrix(1, 0) * rInputMatrix(2, 2)) - (rInputMatrix(0, 0) * rInputMatrix(1, 3) * rInputMatrix(2, 2)) - (rInputMatrix(0, 2) * rInputMatrix(1, 0) * rInputMatrix(2, 3)) + (rInputMatrix(0, 0) * rInputMatrix(1, 2) * rInputMatrix(2, 3));

                rInvertedMatrix(0, 2) = -(rInputMatrix(1, 3) * rInputMatrix(2, 1) * rInputMatrix(3, 0)) + (rInputMatrix(1, 1) * rInputMatrix(2, 3) * rInputMatrix(3, 0)) + (rInputMatrix(1, 3) * rInputMatrix(2, 0) * rInputMatrix(3, 1)) - (rInputMatrix(1, 0) * rInputMatrix(2, 3) * rInputMatrix(3, 1)) - (rInputMatrix(1, 1) * rInputMatrix(2, 0) * rInputMatrix(3, 3)) + (rInputMatrix(1, 0) * rInputMatrix(2, 1) * rInputMatrix(3, 3));
                rInvertedMatrix(1, 2) = (rInputMatrix(0, 3) * rInputMatrix(2, 1) * rInputMatrix(3, 0)) - (rInputMatrix(0, 1) * rInputMatrix(2, 3) * rInputMatrix(3, 0)) - (rInputMatrix(0, 3) * rInputMatrix(2, 0) * rInputMatrix(3, 1)) + (rInputMatrix(0, 0) * rInputMatrix(2, 3) * rInputMatrix(3, 1)) + (rInputMatrix(0, 1) * rInputMatrix(2, 0) * rInputMatrix(3, 3)) - (rInputMatrix(0, 0) * rInputMatrix(2, 1) * rInputMatrix(3, 3));
                rInvertedMatrix(2, 2) = -(rInputMatrix(0, 3) * rInputMatrix(1, 1) * rInputMatrix(3, 0)) + (rInputMatrix(0, 1) * rInputMatrix(1, 3) * rInputMatrix(3, 0)) + (rInputMatrix(0, 3) * rInputMatrix(1, 0) * rInputMatrix(3, 1)) - (rInputMatrix(0, 0) * rInputMatrix(1, 3) * rInputMatrix(3, 1)) - (rInputMatrix(0, 1) * rInputMatrix(1, 0) * rInputMatrix(3, 3)) + (rInputMatrix(0, 0) * rInputMatrix(1, 1) * rInputMatrix(3, 3));
                rInvertedMatrix(3, 2) = (rInputMatrix(0, 3) * rInputMatrix(1, 1) * rInputMatrix(2, 0)) - (rInputMatrix(0, 1) * rInputMatrix(1, 3) * rInputMatrix(2, 0)) - (rInputMatrix(0, 3) * rInputMatrix(1, 0) * rInputMatrix(2, 1)) + (rInputMatrix(0, 0) * rInputMatrix(1, 3) * rInputMatrix(2, 1)) + (rInputMatrix(0, 1) * rInputMatrix(1, 0) * rInputMatrix(2, 3)) - (rInputMatrix(0, 0) * rInputMatrix(1, 1) * rInputMatrix(2, 3));

                rInvertedMatrix(0, 3) = (rInputMatrix(1, 2) * rInputMatrix(2, 1) * rInputMatrix(3, 0)) - (rInputMatrix(1, 1) * rInputMatrix(2, 2) * rInputMatrix(3, 0)) - (rInputMatrix(1, 2) * rInputMatrix(2, 0) * rInputMatrix(3, 1)) + (rInputMatrix(1, 0) * rInputMatrix(2, 2) * rInputMatrix(3, 1)) + (rInputMatrix(1, 1) * rInputMatrix(2, 0) * rInputMatrix(3, 2)) - (rInputMatrix(1, 0) * rInputMatrix(2, 1) * rInputMatrix(3, 2));
                rInvertedMatrix(1, 3) = -(rInputMatrix(0, 2) * rInputMatrix(2, 1) * rInputMatrix(3, 0)) + (rInputMatrix(0, 1) * rInputMatrix(2, 2) * rInputMatrix(3, 0)) + (rInputMatrix(0, 2) * rInputMatrix(2, 0) * rInputMatrix(3, 1)) - (rInputMatrix(0, 0) * rInputMatrix(2, 2) * rInputMatrix(3, 1)) - (rInputMatrix(0, 1) * rInputMatrix(2, 0) * rInputMatrix(3, 2)) + (rInputMatrix(0, 0) * rInputMatrix(2, 1) * rInputMatrix(3, 2));
                rInvertedMatrix(2, 3) = (rInputMatrix(0, 2) * rInputMatrix(1, 1) * rInputMatrix(3, 0)) - (rInputMatrix(0, 1) * rInputMatrix(1, 2) * rInputMatrix(3, 0)) - (rInputMatrix(0, 2) * rInputMatrix(1, 0) * rInputMatrix(3, 1)) + (rInputMatrix(0, 0) * rInputMatrix(1, 2) * rInputMatrix(3, 1)) + (rInputMatrix(0, 1) * rInputMatrix(1, 0) * rInputMatrix(3, 2)) - (rInputMatrix(0, 0) * rInputMatrix(1, 1) * rInputMatrix(3, 2));
                rInvertedMatrix(3, 3) = -(rInputMatrix(0, 2) * rInputMatrix(1, 1) * rInputMatrix(2, 0)) + (rInputMatrix(0, 1) * rInputMatrix(1, 2) * rInputMatrix(2, 0)) + (rInputMatrix(0, 2) * rInputMatrix(1, 0) * rInputMatrix(2, 1)) - (rInputMatrix(0, 0) * rInputMatrix(1, 2) * rInputMatrix(2, 1)) - (rInputMatrix(0, 1) * rInputMatrix(1, 0) * rInputMatrix(2, 2)) + (rInputMatrix(0, 0) * rInputMatrix(1, 1) * rInputMatrix(2, 2));

                rInputMatrixDet = (rInputMatrix(0, 1) * rInputMatrix(1, 3) * rInputMatrix(2, 2) * rInputMatrix(3, 0) 
                                    - rInputMatrix(0, 1) * rInputMatrix(1, 2) * rInputMatrix(2, 3) * rInputMatrix(3, 0) 
                                    - rInputMatrix(0, 0) * rInputMatrix(1, 3) * rInputMatrix(2, 2) * rInputMatrix(3, 1) 
                                    + rInputMatrix(0, 0) * rInputMatrix(1, 2) * rInputMatrix(2, 3) * rInputMatrix(3, 1) 
                                    - rInputMatrix(0, 1) * rInputMatrix(1, 3) * rInputMatrix(2, 0) * rInputMatrix(3, 2) 
                                    + rInputMatrix(0, 0) * rInputMatrix(1, 3) * rInputMatrix(2, 1) * rInputMatrix(3, 2) 
                                    + rInputMatrix(0, 1) * rInputMatrix(1, 0) * rInputMatrix(2, 3) * rInputMatrix(3, 2) 
                                    - rInputMatrix(0, 0) * rInputMatrix(1, 1) * rInputMatrix(2, 3) * rInputMatrix(3, 2) 
                                    + rInputMatrix(0, 3) * (rInputMatrix(1, 2) * rInputMatrix(2, 1) * rInputMatrix(3, 0) 
                                    - rInputMatrix(1, 1) * rInputMatrix(2, 2) * rInputMatrix(3, 0) 
                                    - rInputMatrix(1, 2) * rInputMatrix(2, 0) * rInputMatrix(3, 1) 
                                    + rInputMatrix(1, 0) * rInputMatrix(2, 2) * rInputMatrix(3, 1) 
                                    + rInputMatrix(1, 1) * rInputMatrix(2, 0) * rInputMatrix(3, 2) 
                                    - rInputMatrix(1, 0) * rInputMatrix(2, 1) * rInputMatrix(3, 2)) 
                                    + (rInputMatrix(0, 1) * rInputMatrix(1, 2) * rInputMatrix(2, 0) 
                                    - rInputMatrix(0, 0) * rInputMatrix(1, 2) * rInputMatrix(2, 1) 
                                    - rInputMatrix(0, 1) * rInputMatrix(1, 0) * rInputMatrix(2, 2) 
                                    + rInputMatrix(0, 0) * rInputMatrix(1, 1) * rInputMatrix(2, 2)) * rInputMatrix(3, 3) 
                                    + rInputMatrix(0, 2) * (-(rInputMatrix(1, 3) * rInputMatrix(2, 1) * rInputMatrix(3, 0)) 
                                    + rInputMatrix(1, 1) * rInputMatrix(2, 3) * rInputMatrix(3, 0) 
                                    + rInputMatrix(1, 3) * rInputMatrix(2, 0) * rInputMatrix(3, 1) 
                                    - rInputMatrix(1, 0) * rInputMatrix(2, 3) * rInputMatrix(3, 1) 
                                    - rInputMatrix(1, 1) * rInputMatrix(2, 0) * rInputMatrix(3, 3) 
                                    + rInputMatrix(1, 0) * rInputMatrix(2, 1) * rInputMatrix(3, 3)));
                
                rInvertedMatrix /= rInputMatrixDet;

                QUEST_CATCH("")
            }

            /**
             * @brief 计算2阶方阵的行列式（未检查矩阵大小）
             * @param rInputMatrix 输入矩阵
             */
            template<typename TMatrixType>
            static inline double Det2(const TMatrixType& rA){
                QUEST_DEBUG_ERROR_IF_NOT(rA.size1() == rA.size2()) << "Matrix provived is non-squaare" << std::endl;

                return (rA(0,0)*rA(1,1)-rA(0,1)*rA(1,0));
            }

            /**
             * @brief 计算3阶方阵的行列式（未检查矩阵大小）
             * @param rInputMatrix 输入矩阵
             */
            template<typename TMatrixType>
            static inline double Det3(const TMatrixType& rA){
                QUEST_DEBUG_ERROR_IF_NOT(rA.size1() == rA.size2()) << "Matrix provived is non-squaare" << std::endl;

                const double a = rA(1,1)*rA(2,2) - rA(1,2)*rA(2,1);
                const double b = rA(1,0)*rA(2,2) - rA(1,2)*rA(2,0);
                const double c = rA(1,0)*rA(2,1) - rA(1,1)*rA(2,0);

                return rA(0,0)*a - rA(0,1)*b + rA(0,2)*c;
            }

            /**
             * @brief 计算4阶方阵的行列式（未检查矩阵大小）
             * @param rInputMatrix 输入矩阵
             */
            template<class TMatrixType>
            static inline double Det4(const TMatrixType& rA)
            {
                KRATOS_DEBUG_ERROR_IF_NOT(rA.size1() == rA.size2()) << "Matrix provided is non-square" << std::endl;

                const double det = rA(0,1)*rA(1,3)*rA(2,2)*rA(3,0)-rA(0,1)*rA(1,2)*rA(2,3)*rA(3,0)-rA(0,0)*rA(1,3)*rA(2,2)*rA(3,1)+rA(0,0)*rA(1,2)*rA(2,3)*rA(3,1)
                                -rA(0,1)*rA(1,3)*rA(2,0)*rA(3,2)+rA(0,0)*rA(1,3)*rA(2,1)*rA(3,2)+rA(0,1)*rA(1,0)*rA(2,3)*rA(3,2)-rA(0,0)*rA(1,1)*rA(2,3)*rA(3,2)+rA(0,3)*(rA(1,2)*rA(2,1)*rA(3,0)-rA(1,1)*rA(2,2)*rA(3,0)-rA(1,2)*rA(2,0)*rA(3,1)+rA(1,0)*rA(2,2)*rA(3,1)+rA(1,1)*rA(2,0)*rA(3,2)
                                -rA(1,0)*rA(2,1)*rA(3,2))+(rA(0,1)*rA(1,2)*rA(2,0)-rA(0,0)*rA(1,2)*rA(2,1)-rA(0,1)*rA(1,0)*rA(2,2)+rA(0,0)*rA(1,1)*rA(2,2))*rA(3,3)+rA(0,2)*(-(rA(1,3)*rA(2,1)*rA(3,0))+rA(1,1)*rA(2,3)*rA(3,0)+rA(1,3)*rA(2,0)*rA(3,1)-rA(1,0)*rA(2,3)*rA(3,1)-rA(1,1)*rA(2,0)*rA(3,3)+rA(1,0)*rA(2,1)*rA(3,3));
                return det;
            }

            /**
             * @brief 计算任意阶方阵的行列式（未检查矩阵大小）
             * @param rInputMatrix 输入矩阵
             */
            template<typename TMatrixType>
            static inline double Det(const TMatrixType& rA){
                QUEST_DEBUG_ERROR_IF_NOT(rA.size1() == rA.size2()) << "Matrix provided is non-square" << std::endl;

                switch(rA.size1()){
                    case 2:
                        return Det2(rA);
                    case 3:
                        return Det3(rA);
                    case 4:
                        return Det4(rA);
                    default:
                        double det = 1.0;
                        using namespace boost::numeric::ublas;
                        Matrix Aux(rA);
                        pmatrix pm(Aux.size1());
                        bool singular = lu_factorize(Aux,pm);

                        if(singular){
                            return 0.0;
                        }

                        for(int i=0; i<Aux.size1(); i++){
                            IndexType ki = pm[i] == i ? 0 : 1;
                            det *= std::pow(-1.0,ki)*Aux(i,i);
                        }

                        return det;
                }
            }

            /**
             * @brief 计算任意矩阵的行列式（未检查矩阵大小）
             * @param rInputMatrix 输入矩阵
             */
            template<typename TMatrixType>
            static inline double GeneralizedDet(const TMatrixType& rA){
                if (rA.size1() == rA.size2()){
                    return Det(rA);
                } else if (rA.size1() < rA.size2()){
                    const Matrix AAT = prod(rA, trans(rA));
                    return std::sqrt(Det(AAT));
                } else {
                    const Matrix ATA = prod(trans(rA), rA);
                    return std::sqrt(Det(ATA));
                }
            }

            /**
             * @brief 计算两个三维向量的点积（未检查向量大小）
             * @param a 向量a
             * @param b 向量b
             */
            static inline double Dot3(const Vector& a, const Vector& b){
                return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
            }

            /**
             * @brief 计算任意两个大小相同的向量的点积（未检查向量大小）
             * @param rFirstVector 向量a
             * @param rSecondVector 向量b
             */
            static inline double Dot(const Vector& rFirstVector, const Vector& rSecondVector){
                Vector::const_iterator i = rFirstVector.begin();
                Vector::const_iterator j = rSecondVector.begin();
                double temp = 0.0;
                while(i != rFirstVector.end()){
                    temp += *(i++) * *(j++);
                }
                return temp;
            }

            /**
             * @brief 计算三维向量的2范数（未检查向量大小）
             */
            template<typename TVectorType>
            static inline double Norm3(const TVectorType& a){
                double temp = std::pow(a[0],2) + std::pow(a[1],2) + std::pow(a[2],2);
                temp = std::sqrt(temp);
                return temp;
            }

            /**
             * @brief 计算任意大小的向量的2范数（未检查向量大小）
             * @param a 向量
             */
            static inline double Norm(const Vector& a){
                Vector::const_iterator i = a.begin();
                double temp = 0.0;
                while(i != a.end()){
                    temp += (*i) * (*i);
                    ++i;
                }
                return std::sqrt(temp);
            }

            /**
             * @brief 计算向量"a"的范数，同时避免下溢和上溢
             * @param a 向量
             */
            static inline double StableNorm(const Vector& a){
                if (a.size() == 0.0){
                    return 0.0;
                } 

                if (a.size() == 1){
                    return a[0];
                }

                double scale {0};

                double sqr_sum_scaled {1};

                for (auto it = a.begin(); it!= a.end(); ++it){
                    double x = *it;

                    if(x != 0){
                        const double abs_x = std::abs(x);

                        if(abs_x > scale){
                            const double f = scale / abs_x;
                            sqr_sum_scaled = 1 + sqr_sum_scaled * f * f;
                            scale = abs_x;
                        } else {
                            x = abs_x / scale;
                            sqr_sum_scaled += x * x;
                        }
                    }
                }

                return scale * std::sqrt(sqr_sum_scaled);
            }

            /**
             * @brief 计算输入向量a和b的向量积（不检查向量大小）
             * @param a 向量a
             * @param b 向量b
             */
            template<typename T>
            static inline T CrossProduct(const T& a, const T& b){
                T c(a);

                c[0] = a[1]*b[2] - a[2]*b[1];
                c[1] = a[2]*b[0] - a[0]*b[2];
                c[2] = a[0]*b[1] - a[1]*b[0];

                return c;
            }

            /**
             * @brief 检查是否存在别名
             * @param value1 第一个值
             * @param value2 第二个值
             */
            template<typename T1, typename T2>
            static inline typename std::enable_if<std::is_same<T1,T2>::value, bool>::type CheckIsAlias(T1& value1, T2& value2){
                return value1 == value2;
            }

            /**
             * @brief 检查是否存在别名
             * @param value1 第一个值
             * @param value2 第二个值
             */
            template<typename T1, typename T2>
            static inline typename std::enable_if<!std::is_same<T1,T2>::value, bool>::type CheckIsAlias(T1& value1, T2& value2){
                return false;
            }

            /**
             * @brief 计算两个输入向量a和b的向量积（仅在调试模式下检查向量大小）
             * @param a 向量a
             * @param b 向量b
             * @param c 结果向量
             */
            template<typename T1, typename T2, typename T3>
            static inline void CrossProduct(T1& c, const T2& a, const T3& b){
                if(c.size() != 3)
                    c.resize(3);

                QUEST_DEBUG_ERROR_IF(a.size() != 3 || b.size() != 3 || c.size() != 3)
                    << "The size of the vectors is different of 3: "
                    << a << ", " << b << " and " << c << std::endl;
                QUEST_DEBUG_ERROR_IF(CheckIsAlias(c,a))
                    << "Aliasing between the output parameter and the first input parameter" << std::endl;
                QUEST_DEBUG_ERROR_IF(CheckIsAlias(c,b))
                    << "Aliasing between the output parameter and the second input parameter" << std::endl;

                c[0] = a[1]*b[2] - a[2]*b[1];
                c[1] = a[2]*b[0] - a[0]*b[2];
                c[2] = a[0]*b[1] - a[1]*b[0];
            }

            /**
             * @brief 计算两个输入向量a和b的单位向量积
             * @param a 向量a
             * @param b 向量b
             * @param c 结果向量
             */
            template<typename T1, typename T2, typename T3>
            static inline void UnitCrossProduct(T1& c, const T2& a, const T3& b){
                CrossProduct(c, a, b);
                const double norm = norm_2(c);
                QUEST_DEBUG_ERROR_IF(norm < 1000.0*ZeroTolerance)
                    << "norm is 0 when making the UnitCrossProduct of the vectors "
                    << a << " and " << b << std::endl;
                c /= norm;
            }

            /**
             * @brief 计算给定向量的正交归一基
             * @param c 输入向量
             * @param a 第一个结果向量
             * @param b 第二个结果向量
             * @param Type 算法类型（0：Hughes-Moeller，1：Frisvad，2：Naive）
             */
            template<typename T1, typename T2, typename T3>
            static inline void OrthonormalBasis(const T1& c, T2& a, T3& b, const IndexType = 0){
                if(Type == 0){
                    OrthonormalBasisHughesMoeller(c, a, b);
                } else if (Type == 1){
                    OrthonormalBasisFrisvad(c, a, b);
                } else {
                    OrthonormalBasisNaive(c, a, b);
                }
            }

            /**
             * @brief 计算给定向量的正交归一基（Hughes-Moeller算法）
             * @param c 输入向量
             * @param a 第一个结果向量
             * @param b 第二个结果向量
             */
            template<typename T1, typename T2, typename T3>
            static inline void OrthonormalBasisHughesMoeller(const T1& c, T2& a, T3& b){
                QUEST_DEBUG_ERROR_IF(norm_2(c) < (1.0-1.0e-6) || norm_2(c) > (1.0+1.0e-6)) << "Input should be a normal vector" << std::endl;

                if(std::abs(c[0]) > std::abs(c[2])){
                    b[0] = c[1];
                    b[1] = -c[0];
                    b[2] = 0.0;
                } else {
                    b[0] = 0.0;
                    b[1] = c[2];
                    b[2] = -c[1];
                }
                b /= norm_2(b);
                UnitCrossProduct(a, b, c);
            }

            /**
             * @brief 计算给定向量的正交归一基（Frisvad算法）
             * @param c 输入向量
             * @param a 第一个结果向量
             * @param b 第二个结果向量
             */
            template<typename T1, typename T2, typename T3>
            static inline void OrthonomalBasisFrisvad(const T1& c, T2& a, T3& b){
                QUEST_DEBUG_ERROR_IF(norm_2(c) < (1.0-1.0e-3) || norm_2(c) > (1.0+1.0e-3)) << "Input should be a normal vector" << std::endl;
                if ((c[2] + 1.0) > 1.0e4 * ZeroTolerance){
                    a[0] = 1.0 - std::pow(c[0],2)/(1.0 + c[2]);
                    a[1] = -(c[0]*c[1])/(1.0 + c[2]);
                    a[2] = -c[0];
                    const double norm_a = norm_2(a);
                    a /= norm_a;
                    b[0] = -(c[0] * c[1])/(1.0 + c[2]);
                    b[1] = 1.0 - std::pow(c[1],2)/(1.0 + c[2]);
                    b[2] = -c[1];
                    const double norm_b = norm_2(b);
                    b /= norm_b;
                } else {
                    a[0] = 1.0;
                    a[1] = 0.0;
                    a[2] = 0.0;
                    b[0] = 0.0;
                    b[1] = -1.0;
                    b[2] = 0.0;
                }
            }

            /**
             * @brief 计算给定向量的正交归一基（Naive算法）
             * @param c 输入向量
             * @param a 第一个结果向量
             * @param b 第二个结果向量
             */
            template<typename T1, typename T2, typename T3>
            static inline void OrthonormalBasisNaive(const T1& c, T2& a, T3& b){
                QUEST_DEBUG_ERROR_IF(norm_2(c) < (1.0-1.0e-3) || norm_2(c) > (1.0+1.0e-3)) << "Input should be a normal vector" << std::endl;
                if(c[0] > 0.0f){
                    a[0] = 0.0;
                    a[1] = 1.0;
                    a[2] = 0.0;
                } else {
                    a[0] = 1.0;
                    a[1] = 0.0;
                    a[2] = 0.0;
                }
                a -= c*inner_prod(a,c);
                a /= norm_2(a);
                UnitCrossProduct(b,c,a);
            }

            /**
             * @brief 计算三维空间中两个向量的夹角（弧度制）
             * @param rV1 向量1
             * @param rV2 向量2
             */
            template<typename T1,typename T2>
            static inline double VectorsAngle(const T1& rV1, const T2& rV2){
                const T1 aux_1 = rV1*norm_2(rV2);
                const T2 aux_2 = norm_2(rV1)*rV2;
                const double num = norm_2(aux_1 - aux_2);
                const double denom = norm_2(aux_1 + aux_2);
                return 2.0*std::atan2(num, denom);
            }

            /**
             * @brief 计算两个三阶张量的张量积（不检查向量大小）
             * @param a 向量a
             * @param b 向量b
             */
            static inline MatrixType TensorProduct3(const Vector& a, const Vector& b){
                MatrixType A(3,3);

                A(0,0) = a[0]*b[0];
                A(0,1) = a[0]*b[1];
                A(0,2) = a[0]*b[2];
                A(1,0) = a[1]*b[0];
                A(1,1) = a[1]*b[1];
                A(1,2) = a[1]*b[2];
                A(2,0) = a[2]*b[0];
                A(2,1) = a[2]*b[1];
                A(2,2) = a[2]*b[2];

                return A;
            }

            /**
             * @brief 将 "rInputMatrix" 添加到 "Destination" 矩阵，从目标矩阵的初始行和初始列开始
             * @param rDestination 目标矩阵
             * @param rInputMatrix 输入矩阵
             * @param InitialRow 目标矩阵的初始行
             * @param InitialCol 目标矩阵的初始列
             */
            template<typename TMatrixType1, typename TMatrixType2>
            static inline void AddMatrix(
                TMatrixType1& rDestination,
                const TMatrixType2& rInputMatrix,
                const IndexType InitialRow,
                const IndexType InitialCol
            ){
                QUEST_TRY

                for(IndexType i=0; i<rInputMatrix.size1(); i++){
                    for(IndexType j=0; j<rInputMatrix.size2(); j++){
                        rDestination(InitialRow+i, InitialCol+j) += rInputMatrix(i,j);
                    }
                }
                QUEST_CATCH("")
            }

            /**
             * @brief 将 "rInputVector" 添加到 "Destination" 向量，从目标向量的初始索引开始
             * @param rDestination 目标向量
             * @param rInputVector 输入向量
             * @param InitialIndex 目标向量的初始索引
             */
            template<typename TVectorType1, typename TVectorType2>
            static inline void AddVector(
                TVectorType1& rDestination,
                const TVectorType2& rInputVector,
                const IndexType InitialIndex
            ){
                QUEST_TRY

                for(IndexType i=0; i<rInputVector.size(); i++){
                    rDestination(InitialIndex+i) += rInputVector(i);
                }
                QUEST_CATCH("")
            }

            /**
             * @brief 将 "rInputMatrix" 减去 "Destination" 矩阵，从目标矩阵的初始行和初始列开始
             * @param rDestination 目标矩阵
             * @param rInputMatrix 输入矩阵
             * @param InitialRow 目标矩阵的初始行
             * @param InitialCol 目标矩阵的初始列
             */
            static inline void SubtractMatrix(
                MatrixType& rDestination,
                const MatrixType& rInputMatrix,
                const IndexType InitialRow, 
                const IndexType InitialCol
            ){
                QUEST_TRY

                for(IndexType i=0; i<rInputMatrix.size1(); i++){
                    for(IndexType j=0; j<rInputMatrix.size2(); j++){
                        rDestination(InitialRow+i, InitialCol+j) -= rInputMatrix(i,j);
                    }
                }

                QUEST_CATCH("")
            }

            /**
             * @brief 将 "rInputMatrix" 写入 "rDestination" 矩阵，从目标矩阵的初始行和初始列开始
             * @warning 目标矩阵将被覆盖!!
             * @param rDestination 目标矩阵
             * @param rInputMatrix 输入矩阵
             * @param InitialRow 目标矩阵的初始行
             * @param InitialCol 目标矩阵的初始列
             */
            static inline void WriteMatrix(
                MatrixType& rDestination,
                const MatrixType& rInputMatrix,
                const IndexType InitialRow,
                const IndexType InitialCol
            ){
                QUEST_TRY

                for(IndexType i=0; i<rInputMatrix.size1(); i++){
                    for(IndexType j=0; j<rInputMatrix.size2(); j++){
                        rDestination(InitialRow+i, InitialCol+j) = rInputMatrix(i,j);
                    }
                }

                QUEST_CATCH("")
            }

            /**
             * @brief 执行缩减矩阵与大小为 "dimension" 的单位矩阵的 Kronecker 乘积
             * @param rDestination 目标矩阵
             * @param rReducedMatrix 要进行计算的缩减矩阵
             * @param Dimension 计算时使用的维度
             */
            static inline void ExpandReducedMatrix(
                MatrixType& rDestination,
                const MatrixType& rReducedMatrix,
                const SizeType Dimension
            ){
                QUEST_TRY

                const SizeType size = rReducedMatrix.size2();
                IndexType rowindex = 0;
                IndexType colindex = 0;

                for(IndexType i=0; i<size; ++i){
                    rowindex = i*Dimension;
                    for(IndexType j=0; j<size; ++j){
                        colindex = j*Dimension;
                        for(IndexType k=0; k<Dimension; ++k){
                            rDestination(rowindex+k, colindex+k) = rReducedMatrix(i,j);
                        }
                    }
                }
                QUEST_CATCH("")
            }

            /**
             * @brief 执行缩减矩阵与大小为 "dimension" 的单位矩阵的 Kronecker 乘积，并将结果加到目标矩阵中
             * @param rDestination 目标矩阵
             * @param rReducedMatrix 要加到目标矩阵中的缩减矩阵
             * @param Dimension 计算时使用的维度
             */
            static inline void ExpandAndAddReducedMatrix(
                MatrixType& rDestination,
                const MatrixType& rReducedMatrix,
                const SizeType Dimension
            ){
                QUEST_TRY

                const SizeType size = rReducedMatrix.size2();
                IndexType rowindex = 0;
                IndexType colindex = 0;

                for(IndexType i=0; i<size; ++i){
                    rowindex = i*Dimension;
                    for(IndexType j=0; j<size; ++j){
                        colindex = j*Dimension;
                        for(IndexType k=0; k<Dimension; ++k){
                            rDestination(rowindex+k, colindex+k) += rReducedMatrix(i,j);
                        }
                    }
                }
                QUEST_CATCH("")
            }

            /**
             * @brief 执行 rX += coeff*rY, 没有进行边界检查
             * @param rX 目标向量
             * @param rY 要加的向量
             * @param coeff 系数
             */
            static inline void VecAdd(Vector& rX, const double coeff, Vector& rY){
                QUEST_TRY
                SizeType size = rX.size();

                for(SizeType i=0; i<size; ++i){
                    rX[i] += coeff*rY[i];
                }
                QUEST_CATCH("")
            }

            /**
             * @brief 将应力向量转换为矩阵。假设应力以以下方式存储：
             * \f$ [ s11, s22, s33, s12, s23, s13 ] \f$ 对于三维情况，
             * \f$ [ s11, s22, s33, s12 ] \f$ 对于二维情况，
             * \f$ [ s11, s22, s12 ] \f$ 对于二维情况。
             * @param rStressVector 给定的应力向量
             * @return 对应的应力张量矩阵形式
             * @tparam TVector 考虑的向量类型
             * @tparam TMatrixType 返回的矩阵类型
             */
            template<typename TVector, typename TMatrixType = MatrixType>
            static inline TMatrixType StressVectorToTensor(const TVector& rStressVector){
                QUEST_TRY

                const SizeType matrix_size = rStressVector.size() == 3 ? 2 : 3;
                TMatrixType stress_tensor(matrix_size, matrix_size);

                if (rStressVector.size() == 3){
                    stress_tensor(0,0) = rStressVector[0];
                    stress_tensor(0,1) = rStressVector[2];
                    stress_tensor(1,0) = rStressVector[2];
                    stress_tensor(1,1) = rStressVector[1];
                } else if (rStressVector.size()==4){
                    stress_tensor(0,0) = rStressVector[0];
                    stress_tensor(0,1) = rStressVector[3];
                    stress_tensor(0,2) = 0.0;
                    stress_tensor(1,0) = rStressVector[3];
                    stress_tensor(1,1) = rStressVector[1];
                    stress_tensor(1,2) = 0.0;
                    stress_tensor(2,0) = 0.0;
                    stress_tensor(2,1) = 0.0;
                    stress_tensor(2,2) = rStressVector[2];
                } else if (rStressVector.size()==6){
                    stress_tensor(0,0) = rStressVector[0];
                    stress_tensor(0,1) = rStressVector[3];
                    stress_tensor(0,2) = rStressVector[5];
                    stress_tensor(1,0) = rStressVector[3];
                    stress_tensor(1,1) = rStressVector[1];
                    stress_tensor(1,2) = rStressVector[4];
                    stress_tensor(2,0) = rStressVector[5];
                    stress_tensor(2,1) = rStressVector[4];
                    stress_tensor(2,2) = rStressVector[2];
                }

                return stress_tensor;

                QUEST_CATCH("")
            }

            /**
             * @brief 将向量转换为对称矩阵。
             * @details 假设分量以以下方式存储：
             * \f$ [ s11, s22, s33, s12, s23, s13 ] \f$ 对于三维情况，
             * \f$ [ s11, s22, s33, s12 ] \f$ 对于二维情况，
             * \f$ [ s11, s22, s12 ] \f$ 对于二维情况。
             * @param rVector 给定的应力向量
             * @return 对应的张量矩阵形式
             * @tparam TVector 考虑的向量类型
             * @tparam TMatrixType 返回的矩阵类型
             */
            template<typename TVector, typename TMatrixType = MatrixType>
            static inline TMatrixType VectorToSymmetricTensor(const TVector& rVector){
                QUEST_TRY

                const SizeType matrix_size = rVector.size() == 3 ? 2 : 3;
                TMatrixType tensor(matrix_size, matrix_size);

                if (rVector.size() == 3){
                    tensor(0,0) = rVector[0];
                    tensor(0,1) = rVector[2];
                    tensor(1,0) = rVector[2];
                    tensor(1,1) = rVector[1];
                } else if (rVector.size()==4){
                    tensor(0,0) = rVector[0];
                    tensor(0,1) = rVector[3];
                    tensor(0,2) = 0.0;
                    tensor(1,0) = rVector[3];
                    tensor(1,1) = rVector[1];
                    tensor(1,2) = 0.0;
                    tensor(2,0) = 0.0;
                    tensor(2,1) = 0.0;
                    tensor(2,2) = rVector[2];
                } else if (rVector.size()==6){
                    tensor(0,0) = rVector[0];
                    tensor(0,1) = rVector[3];
                    tensor(0,2) = rVector[5];
                    tensor(1,0) = rVector[3];
                    tensor(1,1) = rVector[1];
                    tensor(1,2) = rVector[4];
                    tensor(2,0) = rVector[5];
                    tensor(2,1) = rVector[4];
                    tensor(2,2) = rVector[2];
                }

                return tensor;

                QUEST_CATCH("")
            }

            /**
             * @brief 提取一个值的符号
             * @param ThisDataType 给定的数值
             */
            static inline int Sign(const double& ThisDataType){
                QUEST_TRY

                const double& x = ThisDataType;
                return (x>0) ? 1 : ((x<0) ? -1 : 0);
                QUEST_CATCH("")
            }

            /**
             * @brief 将应变向量转换为矩阵。假设应变按照以下方式存储：
             * \f$ [ e11, e22, e33, 2*e12, 2*e23, 2*e13 ] \f$ 对于 3D 情况，和
             * \f$ [ e11, e22, e33, 2*e12 ] \f$ 对于 2D 情况。
             * \f$ [ e11, e22, 2*e12 ] \f$ 对于 2D 情况。
             * @details 因此，应变向量的偏差分量在存储到矩阵时会除以 2。
             * @param rStrainVector 给定的应变向量
             * @return 对应的应变张量矩阵形式
             * @tparam TVector 所考虑的向量类型
             * @tparam TMatrixType 返回的矩阵类型
             */
            template<typename TVector, typename TMatrixType = MatrixType>
            static inline TMatrixType StrainVectorToTensor(const TVector& rStrainVector){
                QUEST_TRY

                const SizeType matrix_size = rStrainVector.size() == 3 ? 2 : 3;
                TMatrixType strain_tensor(matrix_size, matrix_size);

                if (rStrainVector.size() == 3){
                    strain_tensor(0,0) = rStrainVector[0];
                    strain_tensor(0,1) = 0.5*rStrainVector[2];
                    strain_tensor(1,0) = 0.5*rStrainVector[2];
                    strain_tensor(1,1) = rStrainVector[1];
                } else if (rStrainVector.size()==4){
                    strain_tensor(0,0) = rStrainVector[0];
                    strain_tensor(0,1) = 0.5*rStrainVector[3];
                    strain_tensor(0,2) = 0.0;
                    strain_tensor(1,0) = 0.5*rStrainVector[3];
                    strain_tensor(1,1) = rStrainVector[1];
                    strain_tensor(1,2) = 0.0;
                    strain_tensor(2,0) = 0.0;
                    strain_tensor(2,1) = 0.0;
                    strain_tensor(2,2) = rStrainVector[2];
                } else if (rStrainVector.size()==6){
                    strain_tensor(0,0) = rStrainVector[0];
                    strain_tensor(0,1) = 0.5*rStrainVector[3];
                    strain_tensor(0,2) = 0.5*rStrainVector[5];
                    strain_tensor(1,0) = 0.5*rStrainVector[3];
                    strain_tensor(1,1) = rStrainVector[1];
                    strain_tensor(1,2) = 0.5*rStrainVector[4];
                    strain_tensor(2,0) = 0.5*rStrainVector[5];
                    strain_tensor(2,1) = 0.5*rStrainVector[4];
                    strain_tensor(2,2) = rStrainVector[2];
                }

                return strain_tensor;

                QUEST_CATCH("")
            }

            /**
             * @brief 将给定的对称应变张量转换为 Voigt 符号表示法：
             * @details 以下情况：
             *  - 在 3D 情况下：从一个二阶张量 (3*3) 矩阵转换为对应的 (6*1) 向量
             *    \f$ [ e11, e22, e33, 2*e12, 2*e23, 2*e13 ] \f$ 对于 3D 情况
             *  - 在 2D 情况下：从一个二阶张量 (3*3) 矩阵转换为对应的 (4*1) 向量
             *    \f$ [ e11, e22, e33, 2*e12 ] \f$ 对于 2D 情况
             *  - 在 2D 情况下：从一个二阶张量 (2*2) 矩阵转换为对应的 (3*1) 向量
             *    \f$ [ e11, e22, 2*e12 ] \f$ 对于 2D 情况
             * @param rStrainTensor 给定的对称二阶应变张量
             * @return 对应的应变张量向量形式
             * @tparam TMatrixType 所考虑的矩阵类型
             * @tparam TVector 返回的向量类型
             */
            template<typename TMatrixType, typename TVector = Vector>
            static inline Vector StrainTensorToVector(
                const TMatrixType& rstrainTensor,
                SizeType rSize = 0
            ){
                QUEST_TRY

                if (rSize == 0){
                    if(rstrainTensor.size1() == 2){
                        rSize = 3;
                    } else if (rstrainTensor.size1() == 3){
                        rSize = 6;
                    }
                }

                Vector strain_vector(rSize);

                if(rSize == 3){
                    strain_vector[0] = rstrainTensor(0,0);
                    strain_vector[1] = rstrainTensor(1,1);
                    strain_vector[2] = 2.0*rstrainTensor(0,1);
                } else if (rSize == 4){
                    strain_vector[0] = rstrainTensor(0,0);
                    strain_vector[1] = rstrainTensor(1,1);
                    strain_vector[2] = rstrainTensor(2,2);
                    strain_vector[3] = 2.0*rstrainTensor(0,1);
                } else if (rSize == 6){
                    strain_vector[0] = rstrainTensor(0,0);
                    strain_vector[1] = rstrainTensor(1,1);
                    strain_vector[2] = rstrainTensor(2,2);
                    strain_vector[3] = 2.0*rstrainTensor(0,1);
                    strain_vector[4] = 2.0*rstrainTensor(0,2);
                    strain_vector[5] = 2.0*rstrainTensor(1,2);
                }

                return strain_vector;

                QUEST_CATCH("")
            }

            /**
             * @brief 将给定的对称应力张量转换为 Voigt 符号表示法：
             * @details 组件假设按以下方式存储：
             * \f$ [ s11, s22, s33, s12, s23, s13 ] \f$ 对于 3D 情况，
             * \f$ [ s11, s22, s33, s12 ] \f$ 对于 2D 情况，
             * \f$ [ s11, s22, s12 ] \f$ 对于 2D 情况。
             * 在 3D 情况下：从一个二阶张量 (3*3) 矩阵转换为对应的 (6*1) 向量，
             * 在 2D 情况下：从一个二阶张量 (3*3) 矩阵转换为对应的 (4*1) 向量，
             * 在 2D 情况下：从一个二阶张量 (2*2) 矩阵转换为对应的 (3*1) 向量。
             * @param rStressTensor 给定的对称二阶应力张量
             * @return 对应的应力张量向量形式
             * @tparam TMatrixType 所考虑的矩阵类型
             * @tparam TVector 返回的向量类型
             */
            template<typename TMatrixType, typename TVector = Vector>
            static inline TVector StressTensorToVector(
                const TMatrixType& rStressTensor,
                SizeType rSize = 0
            ){
                QUEST_TRY

                if (rSize == 0){
                    if(rStressTensor.size1() == 2){
                        rSize = 3;
                    } else if (rStressTensor.size1() == 3){
                        rSize = 6;
                    }
                }

                TVector stress_vector(rSize);

                if(rSize == 3){
                    stress_vector[0] = rStressTensor(0,0);
                    stress_vector[1] = rStressTensor(1,1);
                    stress_vector[2] = rStressTensor(0,1);
                } else if (rSize == 4){
                    stress_vector[0] = rStressTensor(0,0);
                    stress_vector[1] = rStressTensor(1,1);
                    stress_vector[2] = rStressTensor(2,2);
                    stress_vector[3] = rStressTensor(0,1);
                } else if (rSize == 6){
                    stress_vector[0] = rStressTensor(0,0);
                    stress_vector[1] = rStressTensor(1,1);
                    stress_vector[2] = rStressTensor(2,2);
                    stress_vector[3] = rStressTensor(0,1);
                    stress_vector[4] = rStressTensor(1,2);
                    stress_vector[5] = rStressTensor(0,2);
                }

                return stress_vector;

                QUEST_CATCH("")
            }

            /**
             * @brief 将给定的对称张量转换为 Voigt 符号表示法：
             * @details 以下是不同的情况：
             *  - 在 3D 情况下：从一个二阶张量 (3*3) 矩阵转换为对应的 (6*1) 向量
             *  - 在 3D 情况下：从一个二阶张量 (3*3) 矩阵转换为对应的 (4*1) 向量
             *  - 在 2D 情况下：从一个二阶张量 (2*2) 矩阵转换为对应的 (3*1) 向量
             * @param rTensor 给定的对称二阶张量
             * @return 对应的张量向量形式
             * @tparam TMatrixType 所考虑的矩阵类型
             * @tparam TVector 返回的向量类型
             */
            template<typename TMatrixType, typename TVector = Vector>
            static inline TVector SymmetrixTensorToVector(
                const TMatrixType& rTensor,
                SizeType rSize = 0
            ){
                QUEST_TRY

                if (rSize == 0){
                    if(rTensor.size1() == 2){
                        rSize = 3;
                    } else if (rTensor.size1() == 3){
                        rSize = 6;
                    }
                }

                TVector vector(rSize);

                if(rSize == 3){
                    vector[0] = rTensor(0,0);
                    vector[1] = rTensor(1,1);
                    vector[2] = rTensor(0,1);
                } else if (rSize == 4){
                    vector[0] = rTensor(0,0);
                    vector[1] = rTensor(1,1);
                    vector[2] = rTensor(2,2);
                    vector[3] = rTensor(0,1);
                } else if (rSize == 6){
                    vector[0] = rTensor(0,0);
                    vector[1] = rTensor(1,1);
                    vector[2] = rTensor(2,2);
                    vector[3] = rTensor(0,1);
                    vector[4] = rTensor(1,2);
                    vector[5] = rTensor(0,2);
                }

                return vector;

                QUEST_CATCH("")
            }

            /**
             * @brief 计算乘积操作 BT*D*B
             * @param rA 结果矩阵
             * @param rD "中心"矩阵
             * @param rB 需要转置的矩阵
             * @tparam TMatrixType1 所考虑的矩阵类型（1）
             * @tparam TMatrixType2 所考虑的矩阵类型（2）
             * @tparam TMatrixType3 所考虑的矩阵类型（3）
             */
            template<typename TMatrixType1, typename TMatrixType2, typename TMatrixType3>
            static inline void BtDBProductOperator(
                TMatrixType1& rA,
                const TMatrixType2& rD,
                const TMatrixType3& rB
            ){
                const SizeType size1 = rB.size2();
                const SizeType size2 = rB.size2();

                if(rA.size1() != size1 || rA.size2() != size2){
                    rA.resize(size1, size2, false);
                }

                rA.clear();
                for(IndexType k=0;k<rD.size1();++k){
                    for(IndexType l=0;l<rD.size2();++l){
                        const double Dkl = rD(k,l);
                        for(IndexType j=0;j<rB.size2();++j){
                            const double DklBlj = Dkl*rB(l,j);
                            for(IndexType i=0;i<rB.size2();++i){
                                rA(i,j) += rB(k,i)*DklBlj;
                            }
                        }
                    }
                }
            }

            /**
             * @brief 计算乘积操作 B*D*BT
             * @param rA 结果矩阵
             * @param rD "中心"矩阵
             * @param rB 需要转置的矩阵
             * @tparam TMatrixType1 所考虑的矩阵类型（1）
             * @tparam TMatrixType2 所考虑的矩阵类型（2）
             * @tparam TMatrixType3 所考虑的矩阵类型（3）
             */
            template<typename TMatrixType1, typename TMatrixType2, typename TMatrixType3>
            static inline void BDBtProductOperation(
                TMatrixType1& rA,
                const TMatrixType2& rD,
                const TMatrixType3& rB
            ){
                const SizeType size1 = rB.size1();
                const SizeType size2 = rB.size1();

                if(rA.size1() != size1 || rA.size2() != size2){
                    rA.resize(size1, size2, false);
                }

                rA.clear();
                for(IndexType k=0;k<rD.size1();++k){
                    for(IndexType l=0;l<rD.size2();++l){
                        const double Dkl = rD(k,l);
                        for(IndexType j=0;j<rB.size1();++j){    
                            const double DklBjl = Dkl*rB(j,l);
                            for(IndexType i=0;i<rB.size1();++i){
                                rA(i,j) += rB(i,k)*DklBjl;
                            }
                        }
                    }
                }
            }

            /**
             * @brief 计算给定对称矩阵的特征向量和特征值
             * @details 使用迭代的高斯-赛德尔方法计算特征向量和特征值。结果分解为 LDLT
             * @param rA 给定的对称矩阵，计算特征向量
             * @param rEigenVectorsMatrix 结果矩阵（将被覆盖为特征向量）
             * @param rEigenValuesMatrix 结果对角矩阵，包含特征值
             * @param Tolerance 认为是零的最大值
             * @param MaxIterations 最大迭代次数
             * @tparam TMatrixType1 所考虑的矩阵类型（1）
             * @tparam TMatrixType2 所考虑的矩阵类型（2）
             */
            template<typename TMatrixType1, typename TMatrixType2>
            static inline bool GuassSeideEigenSystem(
                const TMatrixType1& rA,
                TMatrixType2& rEigenVectorsMatrix,
                TMatrixType2& rEigenValuesMatrix,
                const double Tolerance = 1e-18,
                const SizeType MaxIterations = 20
            ){
                bool is_converged = false;

                const SizeType size = rA.size1();

                if(rEigenVectorsMatrix.size1() != size || rEigenVectorsMatrix.size2() != size){
                    rEigenVectorsMatrix.resize(size, size, false);
                }

                if(rEigenValuesMatrix.size1() != size || rEigenValuesMatrix.size2() != 1){
                    rEigenValuesMatrix.resize(size, 1, false);
                }

                const TMatrixType2 identity_matrix = IdentityMatrix(size);
                noalias(rEigenVectorsMatrix) = identity_matrix;
                noalias(rEigenValuesMatrix) = rA;

                TMatrixType2 aux_A, aux_V_matrix, rotation_matrix;
                double a,u,c,s,gamma,teta;
                SizeType index1,index2;

                aux_A.resize(size, size, false);
                aux_V_matrix.resize(size, size, false);
                rotation_matrix.resize(size, size, false);

                for(IndexType iterations = 0; iterations < MaxIterations; ++iterations){
                    is_converged = true;

                    a = 0.0;
                    index1 = 0;
                    index2 = 1;

                    for(IndexType i = 0; i < size; ++i){
                        for(IndexType j = i+1; j < size; ++j){
                            if((std::abs(rEigenValuesMatrix(i,j)) > a) && (std::abs(rEigenValuesMatrix(i,j)) > Tolerance)){
                                a = std::abs(rEigenValuesMatrix(i,j));
                                index1 = i;
                                index2 = j;
                                is_converged = false;
                            }
                        }
                    }

                    if(is_converged){
                        break;
                    }

                    gamma = (rEigenValuesMatrix(index2,index2) - rEigenValuesMatrix(index1,index1)) / (2.0*rEigenValuesMatrix(index1,index2));
                    u = 1.0;

                    if(std::abs(gamma) > Tolerance && std::abs(gamma) < (1.0/Tolerance)){
                        u = gamma/std::abs(gamma) * 1.0/(std::abs(gamma) + std::sqrt(1.0 + gamma*gamma));
                    } else {
                        if (std::abs(gamma) >= (1.0/Tolerance)){
                            u = 0.5/gamma;
                        }
                    }

                    c = 1.0/std::sqrt(1.0 + u*u);
                    s = u*c;
                    teta = s/(1.0 + c);

                    noalias(aux_A) = rEigenValuesMatrix;
                    aux_A(index2,index2) = rEigenValuesMatrix(index2,index2) + u*rEigenValuesMatrix(index1,index2);
                    aux_A(index1,index1) = rEigenValuesMatrix(index1,index1) - u*rEigenValuesMatrix(index1,index2);
                    aux_A(index1,index2) = 0.0;
                    aux_A(index2,index1) = 0.0;

                    for(IndexType i = 0; i < size; ++i){
                        if(i != index1 && i != index2){
                            aux_A(index2,i) = rEigenValuesMatrix(index2,i) + s*(rEigenValuesMatrix(index1,i)-teta*rEigenValuesMatrix(index2,i));
                            aux_A(i,index2) = rEigenValuesMatrix(index2,i) + s*(rEigenValuesMatrix(index1,i)-teta*rEigenValuesMatrix(index2,i));
                            aux_A(index1,i) = rEigenValuesMatrix(index1,i) - s*(rEigenValuesMatrix(index2,i)+teta*rEigenValuesMatrix(index1,i));
                            aux_A(i,index1) = rEigenValuesMatrix(index1,i) - s*(rEigenValuesMatrix(index2,i)+teta*rEigenValuesMatrix(index1,i));
                        }
                    }

                    noalias(rEigenValuesMatrix) = aux_A;

                    noalias(rotation_matrix) = identity_matrix;
                    rotation_matrix(index2,index1) = -s;
                    rotation_matrix(index1,index2) = s;
                    rotation_matrix(index1,index1) = c;
                    rotation_matrix(index2,index2) = c;

                    noalias(aux_V_matrix) = ZeroMatrix(size, size);

                    for(IndexType i = 0; i < size; ++i){
                        for(IndexType j = 0; j < size; ++j){
                            for(IndexType k = 0; k < size; ++k){
                                aux_V_matrix(i,j) += rEigenVectorsMatrix(i,k) * rotation_matrix(k,j);
                            }
                        }
                    }
                    noalias(rEigenVectorsMatrix) = aux_V_matrix;
                }

                QUEST_WARNING_IF("MathUtils::EigenSystem", !is_converged) << "Spectral decomposition not converged " << std::endl;

                return is_converged;
            }

            /**
             * @brief 计算矩阵的平方根
             * @details 此函数通过进行特征值分解来计算矩阵的平方根
             * 矩阵 A 的平方根定义为 A = V*S*inv(V)，其中 A 是特征向量矩阵，
             * S 是包含特征值平方根的对角矩阵。注意，前述表达式可以重写为 A = V*S*trans(V)，
             * 因为 V 是正交矩阵。
             * @tparam TMatrixType1 输入矩阵类型
             * @tparam TMatrixType2 输出矩阵类型
             * @param rA 输入矩阵
             * @param rMatrixSquareRoot 输出矩阵的平方根
             * @param Tolerance 特征值分解的容差
             * @param MaxIterations 特征值分解的最大迭代次数
             * @return true 特征值分解问题收敛
             * @return false 特征值分解问题未收敛
             */
            template<typename TMatrixType1, typename TMatrixType2>
            static inline bool MatrixSquareRoot(
                const TMatrixType1& rA,
                TMatrixType2& rMatrixSquareRoot,
                const double Tolerance = 1e-18,
                const SizeType MaxIterations = 20
            ){
                TMatrixType2 eigenvectors_matrix, eigenvalues_matrix;
                const bool is_converged = GuassSeideEigenSystem(rA, eigenvectors_matrix, eigenvalues_matrix, Tolerance, MaxIterations);
                QUEST_WARNING_IF("MatrixSquareRoot", !is_converged) << "GuassSeidelEigenSystem did not converge.\n";

                SizeType size = eigenvalues_matrix.size1();
                for(IndexType i = 0; i < size; ++i){
                    QUEST_ERROR_IF(eigenvalues_matrix(i,0) < 0.0) << "Eigenvalue " << i << " is negative. Square root matrix cannot be computed" << std::endl;
                    eigenvalues_matrix(i,i) = std::sqrt(eigenvalues_matrix(i,i));
                }

                BDBtProductOperation(rMatrixSquareRoot, eigenvalues_matrix, eigenvectors_matrix);

                return is_converged;
            }

            /**
             * @brief 计算一个数字 k 的阶乘，阶乘 = k!
             * @tparam Number 需要计算阶乘的数字
             */
            template<typename TIntegerType>
            static inline TIntegerType Factorial(const TIntegerType Number){
                if(Number == 0){
                    return 1;
                } 
                TIntegerType k = Number;
                for(TIntegerType i = Number-1; i > 0; --i){
                    k *= i;
                }
                return k;
            }

            /**
             * @brief 计算矩阵的指数
             * @tparam rMatrix：要计算指数的矩阵 A
             * @tparam rExponentialMatrix：矩阵 A 的指数 exp(A)
             */
            template<typename TMatrixType>
            static inline void CalculateExponentialOfMatrix(
                const TMatrixType& rMatrix,
                TMatrixType& rExponentialMatrix,
                const double Tolerance = 1000*ZeroTolerance,
                const SizeType MaxTerms = 200
            ){
                SizeType series_term = 2;
                SizeType factorial = 1;
                const SizeType dimension = rMatrix.size1();

                noalias(rExponentialMatrix) = IdentityMatrix(dimension)+rMatrix;
                TMatrixType exponent_matrix = rMatrix;
                TMatrixType aux_matrix;

                while(series_term < MaxTerms){
                    noalias(aux_matrix) = prod(exponent_matrix, rMatrix);
                    noalias(exponent_matrix) = aux_matrix;
                    factorial = Factorial(series_term);
                    noalias(rExponentialMatrix) += exponent_matrix/factorial;
                    const double norm_series_term = std::abs(norm_frobenius(exponent_matrix)/factorial);
                    if(norm_series_term < Tolerance){
                        break;
                    }
                    ++series_term;
                }
            }

            /**
             * @brief 计算角度的弧度值
             * @param AngleInDegrees 角度值（单位：度）
             */
            static double DegreesToRadians(double AngleInDegrees){
                return (AngleInDegrees*Globals::Pi)/180.0;
            }


        protected:

        private:
            MathUtils(void);

            MathUtils(const MathUtils& rhs);

        private:

    };

} // namespace Quest

#endif //QUEST_MATH_UTILS_HPP