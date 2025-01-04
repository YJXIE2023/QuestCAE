#ifndef QUEST_UBLAS_SPACE_HPP
#define QUEST_UBLAS_SPACE_HPP

// 系统头文件
#include <numeric>

// 项目头文件
#include "includes/define.hpp"
#include "includes/process_info.hpp"
#include "includes/ublas_interface.hpp"
#include "includes/matrix_market_interface.hpp"
#include "utilities/dof_updater.hpp"
#include "utilities/parallel_utilities.hpp"
#include "utilities/reduction_utilities.hpp"

namespace Quest{

    #ifdef _OPENMP
    template<typenamme Type>
    class MultValueNoAdd{
        private:
            Type Factor;
        public:
            MultValueNoAdd(const Type& _Val): Factor(_Val){};

            inline Type operator()(const Type& elem) const{
                return elem*Factor;
            }
    };

    template<typename Type>
    class MultValueAdd{
        private:
            Type Factor;
        public:
            MultValueAdd(const Type& _Val): Factor(_Val){};

            inline Type operator()(const Type& elem1, const Type& elem2) const{
                return elem1*Factor+elem2;
            }
    };
    #endif

    template<typename TDataType, typename TMatrixType, typename TVectorType>
    class UblasSpace;


    template<typename TDataType>
    using TUblasSparseSpace = UblasSpace<TDataType, boost::numeric::ublas::compressed_matrix<TDataType>, boost::numeric::ublas::vector<TDataType>>;

    template<typename TDataType>
    using TUblasDenseSpace = UblasSpace<TDataType, DenseMatrix<TDataType>, DenseVector<TDataType>>;


    /**
     * @brief 是否进行缩放
     * @details NO_SCALING: 不进行缩放
     *          CONSIDER_NORM_DIAGONAL: 考虑对角线的范数进行缩放
     *          CONSIDER_MAX_DIAGONAL: 考虑对角线的最大值进行缩放
     *          CONSIDER_PRESCRIBED_DIAGONAL: 使用预定义的对角线值进行缩放
     */
    enum class SCALING_DIAGONAL{
        NO_SCALING = 0,
        CONSIDER_NORM_DIAGONAL = 1,
        CONSIDER_MAX_DIAGONAL = 2,
        CONSIDER_PRESCRIBED_DIAGONAL = 3
    };


    /**
     * @class UblasSpace
     * @brief 用于在 Ublas 空间中处理数据类型、矩阵和向量的模板类
     * @details 用于在 Ublas 空间中处理不同的数据类型、矩阵类型和向量类型。它提供了类型定义和工具，可以有效地管理这些类型
     * @tparam TDataType Ublas 空间中使用的数据类型
     * @tparam TMatrixType Ublas 空间中使用的矩阵类型
     * @tparam TVectorType Ublas 空间中使用的向量类型
     */
    template<typename TDataType, typename TMatrixType, typename TVectorType>
    class UblasSpace{
        public:
            QUEST_CLASS_POINTER_DEFINITION(UblasSpace);

            using DataType = TDataType;
            using MatrixType = TMatrixType;
            using VectorType = TVectorType;
            using IndexType = size_t;
            using SizeType = size_t;
            using MatrixPointerType = typename Quest::shared_ptr<TMatrixType>;
            using VectorPointerType = typename Quest::shared_ptr<TVectorType>;
            using DofUpdaterType = DofUpdater<UblasSpace<TDataType, TMatrixType, TVectorType>>;
            using DofUpdaterPointerType = typename DofUpdaterType::UniquePointer;

        public:
            /**
             * @brief 默认构造函数
             */
            UblasSpace(){}


            /**
             * @brief 析构函数
             */
            virtual ~UblasSpace(){}


            /**
             * @brief 创建一个空的矩阵指针对象
             */
            static MatrixPointerType CreateEmptyMatrixPointer(){
                return MatrixPointerType(new TMatrixType(0, 0));
            }


            /**
             * @brief 创建一个空的向量指针对象
             */
            static VectorPointerType CreateEmptyVectorPointer(){
                return VectorPointerType(new TVectorType(0));
            }


            /**
             * @brief 获取向量的维度
             */
            static IndexType Size(const VectorType& rV){
                return rV.size();
            }


            /**
             * @brief 获取矩阵行数
             */
            static IndexType Size1(const MatrixType& rM){
                return rM.size1();
            }


            /**
             * @brief 获取矩阵列数
             */
            static IndexType Size2(const MatrixType& rM){
                return rM.size2();
            }


            /**
             * @brief  rXi = rMij
             * @details 此版本用于从 AMatrix 矩阵的多列解中提取一列，并将其传递给 ublas 向量
             */
            template<typename TColumnType>
            static void GetColumn(unsigned int j, Matrix& rM, TColumnType& rX){
                if (rX.size() != rM.size1())
                    rX.resize(rM.size1(), false);

                for (std::size_t i = 0; i < rM.size1(); ++i){
                    rX[i] = rM(i, j);
                }
            }


            /**
             * @brief 从 AMatrix 矩阵的多列解中提取一列，并将其传递给 ublas 向量
             */
            template<typename TColumnType>
            static void SetColumn(unsigned int j, Matrix& rM, TColumnType& rX){
                for (std::size_t i = 0; i < rM.size1(); ++i){
                    rM(i, j) = rX[i];
                }
            }


            /**
             * @brief rY = rX
             */
            static void Copy(const MatrixType& rX, MatrixType& rY){
                rY.assign(rX);
            }


            static void Copy(const VectorType& rX, VectorType& rY){
                #ifndef _OPENMP

                rY.assign(rX);
                
                #else
                
                const int size = rX.size();
                if(rY.size() != static_cast<unsigned int>(size)){
                    rY.resize(size, false);
                }

                #pragma omp parallel for
                for(int i = 0; i < size; i++){
                    rY[i] = rX[i];
                }

                #endif
            }


            /**
             * @brief rX * rY
             */
            static TDataType Dot(const VectorType& rX, const VectorType& rY){
                #ifndef _OPENMP

                return inner_pord(rX, rY);

                #else 

                const int size = static_cast<int>(rX.size());

                TDataType total = TDataType();
                #pragma omp parallel for reduction(+:total), firstprivate(size)
                for(int i = 0; i < size; i++){
                    total += rX[i] * rY[i];
                }

                return total;

                #endif
            }


            /**
             * @brief ||rX||2
             */
            static TDataType TwoNorm(const VectorType& rX){
                return std::sqrt(Dot(rX, rX));
            }


            static TDataType TwoNorm(const Matrix& rA){
                TDataType aux_norm = TDataType();
                #pragma omp parallel for reduction(+:aux_norm)
                for(int i = 0; i < static_cast<int>(rA.size1()); i++){
                    for(int j = 0; j < static_cast<int>(rA.size2()); j++){
                        aux_norm += std::pow(rA(i, j), 2);
                    }
                }
                return std::sqrt(aux_norm);
            }


            static TDataType TwoNorm(const compressed_matrix<TDataType>& rA){
                TDataType aux_norm = TDataType();

                const auto& r_values = rA.value_data();

                #pragma omp parallel for reduction(+:aux_norm)
                for(int i = 0; i < static_cast<int>(r_values.size()); ++i){
                    aux_norm += std::pow(r_values[i], 2);
                }
                return std::sqrt(aux_norm);
            }


            /**
             * 此方法计算雅可比范数
             * @param rA 要计算雅可比范数的矩阵
             * @return aux_sum: 雅可比范数
             */
            static TDataType JacobiNorm(const Matrix& rA){
                TDataType aux_sum = TDataType();
                #pragma omp parallel for reduction(+:aux_sum)
                for(int i = 0; i < static_cast<int>(rA.size1()); i++){
                    for(int j = 0; j < static_cast<int>(rA.size2()); j++){
                        if(i != j){
                            aux_sum += std::abs(rA(i, j));
                        }
                    }
                }
                return aux_sum;
            }


            static TDataType JacobiNorm(const compressed_matrix<TDataType>& rA){
                TDataType aux_sum = TDataType();

                typedef typename compressed_matrix<TDataType>::const_iterator1 t_it_1;
                typedef typename compressed_matrix<TDataType>::const_iterator2 t_it_2;

                for(t_it_1 it_1 = rA.begin1(); it_1 != rA.end1(); ++it_1){
                    for(t_it_2 it_2 = it_1.begin(); it_2 != it_1.end(); ++it_2){
                        if(it_1.index1() != it_2.index2()){
                            aux_sum += std::abs(*it_2);
                        }
                    }
                }
                return aux_sum;
            }


            /**
             * @brief 矩阵向量相乘
             */
            static void Mult(const Matrix& rA, VectorType& rX, VectorType& rY){
                axpy_pord(rA, rX, rY, true);
            }


            static void Mult(const compressed_matrix<TDataType>& rA, VectorType& rX, VectorType& rY){
                #ifndef _OPENMP
                axpy_pord(rA, rX, rY, true);
                #else 
                ParallelProductNoAdd(rA, rX, rY);
                #endif
            }


            /**
             * @brief rY = rAT * rX
             */
            template<typename TOtherMatrixType>
            static void TransposeMult(TOtherMatrixType& rA, VectorType& rX, VectorType& rY){
                boost::numeric::ublas::axpy_prod(rX, rA, rY, true);
            }


            /**
             * @brief 计算稀疏矩阵中某一行（对应于图的某个节点）的图的度数（即与该节点相连的边的数量）
             */
            static inline SizeType GraphDegree(IndexType i, TMatrixType& A){
                typename MatrixType::iterator1 a_iterator = A.begin1();
                std::advance(a_iterator, i);
                #ifndef BOOST_UBLAS_NO_NESTED_CLASS_RELATION
                    return (std::diatance(a_iterator.begin(), a_iterator.end()));
                #else
                    return (std::distance(beign(a_iterator, boost::numeric::ublas::iterator1_tag()), end(a_iterator, boost::numeric::ublas::iterator1_tag())));
                #endif
            }


            /**
             * @brief 提取给定节点的所有邻居节点
             */
            static inline void GraphNeighbors(IndexType i, TMatrixType& A, std::vector<IndexType>& neighbors){
                neighbors.clear();
                typename MatrixType::iterator1 a_iterator = A.begin1();
                std::advance(a_iterator, i);
                #ifndef BOOST_UBLAS_NO_NESTED_CLASS_RELATION
                    for(typename MatrixType::iterator2 row_iterator = a_iterator.begin(); row_iterator != a_iterator.end(); ++row_iterator){
                #else
                    for(typename MatrixType::iterator2 row_iterator = begin(a_iterator, boost::numeric::ublas::iterator1_tag()); row_iterator != end(a_iterator, boost::numeric::ublas::iterator1_tag()); ++row_iterator){
                #endif
                    neighbors.push_back(row_iterator.index2());
                }
            }


            /**
             * @brief 检查是否需要进行乘法操作，并尝试避免进行乘法
             */
            static void InplaceMult(VectorType& rX, const double A){
                if(A == 1.0){}
                else if(A == -1.00){
                    #ifndef _OPENMP

                    typename VectorType::iterator x_iterator = rX.begin();
                    typename VectorType::iterator end_iterator = rX.end();
                    while(x_iterator != end_iterator){
                        *x_iterator = -(*x_iterator);
                        ++x_iterator;
                    }

                    #else 

                    cosnt int size = rX.size();
                    #pragma omp parallel for firstprivate(size)
                    for(int i = 0; i < size; i++)
                        rX[i] = -rX[i];
                    
                    #endif
                } else{
                    #ifndef _OPENMP

                    rX *= A;

                    #else

                    const int size = rX.size();
                    #pragma omp parallel for firstprivate(size)
                    for(int i = 0; i < size; i++)
                        rX[i] *= A;

                    #endif
                }
            }


            /**
             * @brief 检查是否需要进行乘法操作，并尝试避免进行乘法
             * @details X = A * y
             */
            static void Assign(VectorType& rX, const double A, const VectorType& rY){
                #ifndef _OPENMP
                    if(A == 1.0)
                        noalias(rX) = rY;
                    else if(A == -1.00)
                        noalias(rX) = -rY;
                    else
                        noalias(rX) = A * rY;
                #else
                    const int size = rY.size();
                    if(rX.size() != static_cast<unsigned int>(size))
                        rX.resize(size, false);
                    
                    if(A == 1.0){
                        #pragma omp parallel for
                        for(int i = 0; i < size; i++)
                            rX[i] = rY[i];
                    } else if(A == -1.00){
                        #pragma omp parallel for
                        for(int i = 0; i < size; i++)
                            rX[i] = -rY[i];
                    } else{
                        #pragma omp parallel for
                        for(int i = 0; i < size; i++)
                            rX[i] = A * rY[i];
                    }
                #endif
            }


            /**
             * @brief X += A*y
             */
            static void UnaliasedAdd(VectorType& rX, const double A, const VectorType& rY){
                #ifndef _OPENMP
                    if(A == 1.0)
                        noalias(rX) += rY;
                    else if(A == -1.00)
                        noalias(rX) -= rY;
                    else
                        noalias(rX) += A * rY;
                #else
                    const int size = rY.size();
                    if(rX.size() != static_cast<unsigned int>(size))
                        rX.resize(size, false);
                    
                    if(A == 1.0){
                        #pragma omp parallel for
                        for(int i = 0; i < size; i++)
                            rX[i] += rY[i];
                    } else if(A == -1.00){
                        #pragma omp parallel for
                        for(int i = 0; i < size; i++)
                            rX[i] -= rY[i];
                    } else{
                        #pragma omp parallel for
                        for(int i = 0; i < size; i++)
                            rX[i] += A * rY[i];
                    }
                #endif
            }


            /**
             * @brief rZ = (A * rX) + (B * rY)
             */
            static void ScaleAndAdd(const double A, const VectorType& rX, const double B, const VectorType& rY, VectorType& rZ){
                Assign(rZ, A, rX);
                UnaliasedAdd(rZ, B, rY);
            }


            /**
             * @brief rY = (A * rX) + (B * rY)
             */
            static void ScaleAndAdd(const double A, const VectorType& rX, const double B, VectorType& rY){
                InplaceMult(rY, B);
                UnaliasedAdd(rY, A, rX);
            }


            /**
             * @brief rA[i] * rX
             */
            static double RowDot(unsigned int i, MatrixType& rA, VectorType& rX){
                return inner_prod(row(rA, i), rX);
            }


            /**
             * @brief 设置向量某一元素值
             */
            static void SetValue(VectorType& rX, IndexType i, TDataType value){
                rX[i] = value;
            }


            /**
             * @brief rX = A
             */
            static void Set(VectorType& rX, TDataType A){
                std::fill(rX.begin(), rX.end(), A);
            }


            /**
             * @brief 重新设置矩阵的维度
             */
            static void Resize(MatrixType& rA, SizeType m, SizeType n){
                rA.resize(m, n, false);
            }


            static void Resize(MatrixPointerType& pA, SizeType m, SizeType n){
                pA->resize(m, n, false);
            }


            static void Resize(VectorType& rX, SizeType n){
                rX.resize(n, false);
            }


            static void Resize(VectorPointerType& pX, SizeType n){
                pX->resize(n, false);
            }


            static void Clear(MatrixPointerType& pA){
                pA->clear();
                pA->resize(0, 0, false);
            }


            static void Clear(VectorPointerType& pX){
                pX->clear();
                pX->resize(0, false);
            }


            /**
             * @brief 重置所有元素为零
             */
            template<typename TOtherMatrixType>
            inline static void ResizeData(TOtherMatrixType& rA, SizeType m){
                rA.resize(m, false);
                #ifndef _OPENMP
                    std::fill(rA.begin(), rA.end(), TDataType());
                #else
                    DataType* vals = rA.value_data().begin();
                    #pragma omp parallel for firstprivate(m)
                    for(int i=0; i<static_cast<int>(m); ++i)
                        vals[i] = TDataType();
                #endif
            }


            inline static void ResizeData(compressed_matrix<TDataType>& rA, SizeType m){
                rA.value_data().resize(m);
                #ifndef _OPENMP
                    std::fill(rA.value_data().begin(), rA.value_data().end(), TDataType());
                #else
                    TDataType* vals = rA.value_data().begin();
                    #pragma omp parallel for firstprivate(m)
                    for(int i=0; i<static_cast<int>(m); ++i)
                        vals[i] = TDataType();
                #endif
            }


            inline static void ResizeData(VectorType& rX, SizeType m){
                rX.resize(m, false);
                #ifndef _OPENMP
                    std::fill(rX.begin(), rX.end(), TDataType());
                #else
                    const int size = rX.size();
                    #pragma omp parallel for firstprivate(size)
                    for(int i=0; i<size; ++i)
                        rX[i] = TDataType();
                #endif
            }


            /**
             * @brief 将矩阵 rA 的所有元素设置为零
             */
            template<typename TOtherMatrixType>
            inline static void SetToZero(TOtherMatrixType& rA){
                #ifndef _OPENMP
                    std::fill(rA.begin(), rA.end(), TDataType());
                #else
                    TDataType* vals = rA.value_data().begin();
                    const int size = rA.value_data().end() - rA.value_data().begin();
                    #pragma omp parallel for firstprivate(size)
                    for(int i=0; i<size; ++i)
                        vals[i] = TDataType();
                #endif
            }


            inline static void SetToZero(compressed_matrix<TDataType>& rA){
                #ifndef _OPENMP
                    std::fill(rA.value_data().begin(), rA.value_data().end(), TDataType());
                #else
                    TDataType* vals = rA.value_data().begin();
                    const int size = rA.value_data().end() - rA.value_data().begin();
                    #pragma omp parallel for firstprivate(size)
                    for(int i=0; i<size; ++i)
                        vals[i] = TDataType();
                #endif
            }


            inline static void SetToZero(VectorType& rX){
                #ifndef _OPENMP
                    std::fill(rX.begin(), rX.end(), TDataType());
                #else
                    const int size = rX.size();
                    #pragma omp parallel for firstprivate(size)
                    for(int i=0; i<size; ++i)
                        rX[i] = TDataType();
                #endif
            }


            /**
             * @brief 将局部矩阵 LHS_Contribution 的贡献组装到全局矩阵 A 中
             */
            template<typename TOtherMatrixType, typename TEquationIdVectorType>
            inline static void AssembleLHS(
                MatrixType& A,
                TOtherMatrixType& LHS_Contribution,
                TEquationIdVectorType& EquationId
            ){
                unsigned int system_size = A.size1();
                unsigned int local_size = LHS_Contribution.size1();

                for(unsigned int i_local = 0; i_local < local_size; i_local++){
                    unsigned i_global = EquationId[i_local];
                    if(i_global < system_size){
                        for(unsigned int j_local = 0; j_local < local_size; j_local++){
                            unsigned j_global = EquationId[j_local];
                            if(j_global < system_size){
                                A(i_global, j_global) += LHS_Contribution(i_local, j_local);
                            }
                        }
                    }
                }
            }


            /**
             * @brief 检查并修正零对角值
             * @details 返回考虑对角线缩放的缩放范数
             * @param rProcessInfo 问题处理信息
             * @param rA 左侧矩阵
             * @param rX 右侧向量
             * @param ScalingDiagonal 考虑的对角线缩放类型
             * @return 考虑对角线缩放的缩放范数
             */
            static double CheckAndCorrectZeroDiagonalValues(
                const ProcessInfo& rProcessInfo,
                MatrixType& rA,
                VectorType& rb,
                const SCALING_DIAGONAL ScalingDiagonal = SCALING_DIAGONAL::NO_SCALING
            ){
                const std::size_t system_size = rA.size1();

                const double* Avalues = rA.value_data().begin();
                const std::size_t* Arow_indices = rA.index1_data().begin();

                const double zero_tolerance = std::numeric_limits<double>::epsilon();
                const double scale_factor = GetScaleNorm(rProcessInfo, rA, ScalingDiagonal);

                IndexPartition(system_size).for_each([&](std::size_t Index){
                    bool empty = true;

                    const std::size_t col_begin = Arow_indices[Index];
                    const std::size_t col_end = Arow_indices[Index+1];

                    for(std::size_t j = col_begin; j < col_end; +=j){
                        if(std::abs(Avalues[j]) > zero_tolerance){
                            empty = false;
                            break;
                        }
                    }

                    if(empty){
                        rA[Index, Index] = scale_factor;
                        rb[Index] = 0.0;
                    }
                });
                return scale_factor;
            }


            /**
             * @brief 返回考虑对角线缩放的缩放范数
             * @param rProcessInfo 问题处理信息
             * @param rA 左侧矩阵
             * @param ScalingDiagonal 考虑的对角线缩放类型
             * @return 缩放范数
             */
            static double GetScaleNorm(
                const ProcessInfo& rProcessInfo,
                const MatrixType& rA,
                const SCALING_DIAGONAL ScalingDiagonal = SCALING_DIAGONAL::NO_SCALING
            ){
                switch(ScalingDiagonal){
                    case SCALING_DIAGONAL::NO_SCALING:
                        return 1.0;
                    case SCALING_DIAGONAL::CONSIDER_PRESCRIBED_DIAGONAL:
                        QUEST_ERROR_IF_NOT(rProcessInfo.Has(BUILD_SCALE_FACTOR)) << "Scale factor not defined at process info" << std::endl;
                        return rProcessInfo.GetValue(BUILD_SCALE_FACTOR);
                    case SCALING_DIAGONAL::CONSIDER_NORM_DIAGONAL:
                        return GetDiagonalNorm(rA)/static_cast<double>(rA.size1());
                    case SCALING_DIAGONAL::CONSIDER_MAX_DIAGONAL:
                        return GetMaxDiagonal(rA);
                    default:
                        return GetMaxDiagonal(rA);
                }
            }


            /**
             * @brief 返回考虑对角线缩放的对角线范数
             * @param rA 左侧矩阵
             */
            static double GetDiagonalNorm(const MatrixType& rA){
                const double* Avalues = rA.value_data().begin();
                const std::size_t* Arow_indices = rA.index1_data().begin();
                const std::size_t* Acol_indices = rA.index2_data().begin();

                const double diagonal_norm = IndexPartition<std::size_t>(Size1(rA)).for_each<SumReduction<double>>([&](std::size_t Index){
                    const std::size_t col_begin = Arow_indices[Index];
                    const std::size_t col_end = Arow_indices[Index+1];

                    for(std::size_t j = col_begin; j < col_end; ++j){
                        if(Acol_indices[j] == Index){
                            return std::pow(Avalues[j], 2);
                        }
                    }
                    return 0.0;
                });
                return std::sqrt(diagonal_norm);
            }


            /**
             * @brief 返回对角线的最大值和最小值的平均值
             * @param rA 左侧矩阵
             */
            static double GetAveragevalueDiagonal(const MatrixType& rA){
                return 0.5*(GetMaxDiagonal(rA) + GetMinDiagonal(rA));
            }


            /**
             * @brief 返回对角线的最大值
             * @param rA 左侧矩阵
             */
            static double GetMaxDiagonal(const MatrixType& rA){
                const double* Avalues = rA.value_data().begin();
                const std::size_t* Arow_indices = rA.index1_data().begin();
                const std::size_t* Acol_indices = rA.index2_data().begin();

                return IndexPartition<std::size_t>(Size1(rA)).for_each<MaxReduction<double>>([&](std::size_t Index){
                    const std::size_t col_begin = Arow_indices[Index];
                    const std::size_t col_end = Arow_indices[Index+1];

                    for(std::size_t j = col_begin; j < col_end; ++j){
                        if(Acol_indices[j] == Index){
                            return std::abs(Avalues[j]);
                        }
                    }
                    return std::numeric_limits<double>::lowest();
                });
            }


            /**
             * @brief 返回对角线的最小值
             * @param rA 左侧矩阵
             */
            static double GetMinDiagonal(const MatrixType& rA){
                const double* Avalues = rA.value_data().begin();
                const std::size_t* Arow_indices = rA.index1_data().begin();
                const std::size_t* Acol_indices = rA.index2_data().begin();

                return IndexPartition<std::size_t>(Size1(rA)).for_each<MinReduction<double>>([&](std::size_t Index){
                    const std::size_t col_begin = Arow_indices[Index];
                    const std::size_t col_end = Arow_indices[Index+1];

                    for(std::size_t j = col_begin; j < col_end; ++j){
                        if(Acol_indices[j] == Index){
                            return std::abs(Avalues[j]);
                        }
                    }
                    return std::numeric_limits<double>::max();
                });
            }


            virtual std::string Info() const{
                return "UblasSpace";
            }


            virtual void PrintInfo(std::ostream& rOStream) const{
                rOStream << Info();
            }


            virtual void PrintData(std::ostream& rOStream) const{}


            inline static constexpr bool IsDistributed() { return false; }


            inline static TDataType GetValue(const VectorType& x, std::size_t i){
                return x[i];
            }


            static void GatherValues(const VectorType& x, const std::vector<std::size_t>& IndexArray, TDataType* pValue){
                QUEST_TRY

                for(std::size_t i = 0; i < IndexArray.size(); ++i)
                    pValue[i] = x[IndexArray[i]];

                QUEST_CATCH("")
            }


            template<typename TOtherMatrixType>
            static bool WriteMatrixMarket(const char* pFileName, TOtherMatrixType& rM, const bool Symmetrix){
                return Quest::WriteMatrixMarket(pFileName, rM, Symmetrix);
            }


            template<typename TOtherVectorType>
            static bool WriteVectorMarket(const char* pFileName, const TOtherVectorType& rV){
                return Quest::WriteVectorMarket(pFileName, rV);
            }


            static DofUpdaterPointerType CreateDofUpdater(){
                DofUpdaterType tmp;
                return rmp.Create();
            }


            static constexpr bool IsDistributedSpace(){
                return false;
            }

        protected:

        private:
        #ifdef _OPENMP
            /**
             * @brief y += A*x（并行环境中）
             */
            static void ParallelProductNoAdd(const MatrixType& A, const VectorType& in, VectorType& out){
                DenseVector<unsigned int> partition;
                unsigned int number_of_threads = omp_get_max_threads();
                unsigned int number_of_initialized_rows = A.filled1()-1;
                CreatePartition(number_of_threads, number_of_initialized_rows, partition);

                #pragma omp parallel
                {
                    int thread_id = omp_get_thread_num();
                    int number_of_rows = partition[thread_id+1] - partition[thread_id];
                    typename compressed_matrix<TDataType>::index_array_type::const_iterator row_iter_begin = A.index1_data().begin() + partition[thread_id];
                    typename compressed_matrix<TDataType>::index_array_type::const_iterator index_2_begin = A.index2_data().begin()+*row_iter_begin;
                    typename compressed_matrix<TDataType>::value_array_type::const_iterator value_begin = A.value_data().begin()+*row_iter_begin;

                    partial_product_no_add(
                        number_of_rows,
                        row_iter_begin,
                        index_2_begin,
                        value_begin,
                        in,
                        partition[thread_id],
                        out
                    );
                }
            }


            static void CreatePartition(unsigned int number_of_threads, const int number_of_rows, DenseVector<unsigned int>& partitions){
                partitions.resize(number_of_threads+1);
                int partition_size = number_of_rows/number_of_threads;
                partitions[0] = 0;
                partitions[number_of_threads] = number_of_rows;
                for(unsigned int i = 1; i < number_of_threads; ++i)
                    partitions[i] = partitions[i-1] + partition_size;
            }


            static void partial_product_no_add(
                int number_of_rows,
                typename compressed_matrix<TDataType>::index_array_type::const_iterator row_begin,
                typename compressed_matrix<TDataType>::index_array_type::const_iterator index_2_begin,
                typename compressed_matrix<TDataType>::value_array_type::const_iterator value_begin,
                const VectorType& input_vec,
                unsigned int output_begin_index,
                VectorType& output_vec
            ){
                int row_size;
                int kkk = output_begin_index;
                typename MatrixType::index_array_type::const_iterator row_iter = row_begin;
                for(int k = 0; k < number_of_rows; ++k){
                    row_size = *(row_iter+1) - *row_iter;
                    ++row_iter;
                    TDataType t = TDataType();

                    for(int i = 0; i < row_size; ++i)
                        t += *value_begin++ * (input_vec[*index_2_begin++]);

                    output_vec[kkk++] = t;
                }
            }
        #endif

        private:
            UblasSpace& operator=(const UblasSpace& rOther);

            UblasSpace(const UblasSpace& rOther);

    };


} // namespace Quest

#endif //QUEST_UBLAS_SPACE_HPP