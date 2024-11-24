#ifndef QUEST_LINEAR_SOLVER_HPP
#define QUEST_LINEAR_SOLVER_HPP

// 项目头文件
#include "includes/model_part.hpp"
#include "linear_solvers/reorderer/reorderer.hpp"

namespace Quest{

    /**
     * class LinearSolver
     * @brief 线性求解器基类
     * @details 提供线性求解器的通用接口
     * @tparam TSparseSpaceType 稀疏空间的类型
     * @tparam TDenseSpaceType 稠密空间的类型
     * @tparam TReordererType 重排序器的类型，默认为 Reorderer<TSparseSpaceType, TDenseSpaceType>
     */
    template<class TSparseSpaceType, typename TDenseSpaceType, typename TReordererType = Reorderer<TSparseSpaceType, TDenseSpaceType>>
    class LinearSolver{
        public:
            QUEST_CLASS_POINTER_DEFINITION(LinearSolver);

            using SparseMatrixType = typename TSparseSpaceType::MatrixType;
            using SparseMatrixPointerType = typename TSparseSpaceType::MatrixPointerType;
            using VectorType = typename TSparseSpaceType::VectorType;
            using VectorPointerType = typename TSparseSpaceType::VectorPointerType;
            using DenseMatrixType = TDenseSpaceType::MatrixType;
            using DenseVectorType = TDenseSpaceType::VectorType;
            using SizeType = std::size_t;
            using IndexType = typename TSparseSpaceType::IndexType;

        public:
            /**
             * @brief 默认构造函数
             */
            LinearSolver() : mpReorderer(new TReordererType()) {}


            /**
             * @brief 构造函数
             */
            LinearSolver(TReordererType::Pointer NewReorderer) : mpReorderer(NewReorderer) {}


            /**
             * @brief 复制构造函数
             */
            LinearSolver(const LinearSolver& Other) : mpReorderer(Other.mpReorderer) {}


            /**
             * @brief 析构函数
             */
            virtual ~LinearSolver() {}


            /**
             * @brief 复制运算符重载
             */
            LinearSolver& operator = (const LinearSolver& Other){
                mpReorderer = Other.mpReorderer;
                
                return *this;
            }


            /**
             * @brief 初始化求解过程中所需的内存资源
             * @details 创建依赖矩阵链接结构的数据结构，为后续的计算分配内存并确保在执行实际的计算时不重复进行昂贵的内存分配操作
             * @param rA 系统矩阵，该矩阵的稀疏不在此步骤中使用，仅依赖矩阵的连接性（矩阵的非零元素的结构）
             * @param rX 求解向量，不仅作为目标输出，还作为迭代求解器的初始猜测
             * @param rB 常数项
             */
            virtual void Initialize(SparseMatrixType& rA, VectorType& rX, VectorType& rB){
                mpReorderer->Initialize(rA, rX, rB);
            }


            /**
             * @brief 矩阵求解前的预处理操作
             * @details 该函数在求解器开始求解之前调用，可以用于执行一些必要的预处理操作，如直接求解之前进行矩阵分解等
             */
            virtual void InitializeSolutionStep(SparseMatrixType& rA, VectorType& rX, VectorType& rB){

            }


            /**
             * @brief 实际执行矩阵求解的函数
             * @details 利用InitializeSolutionStep()函数完成的预处理操作（如矩阵分解、预处理等）进行计算
             * @param rA 系数矩阵
             * @param rX 解向量，不仅作为输出，还作为迭代求解器的初始猜测
             * @param rB 常数项
             */
            virtual void PerformSolutionStep(SparseMatrixType& rA, VectorType& rX, VectorType& rB){
                QUEST_ERROR << "Calling linear solver base class" << std::endl;
            }


            /**
             * @brief 求解结束后移出不再需要的数据资源
             * @param rA 系数矩阵
             * @param rX 求解向量
             * @param rB 常数项
             */
            virtual void FinalizeSolutionStep(SparseMatrixType& rA, VectorType& rX, VectorType& rB){
            }


            /**
             * @brief 清除求解器的所有内部数据，将求解器对象恢复到初始创建时的状态
             */
            virtual void Clear(){
            }


            /**
             * @brief 直接法求解一个线性系统
             * @details 该函数用于求解线性方程组 𝐴×𝑋=𝐵，并将解存储在 rX 中。rX 也用作迭代法的初始猜测
             * @param rA 系数矩阵
             * @param rX 解向量
             * @param rB 常数项
             */
            virtual bool Solve(SparseMatrixType& rA, VectorType& rX, VectorType& rB){
                QUEST_ERROR << "Calling linear solver base class" << std::endl;
                return false;
            }


            /**
             * @brief 多重求解方法解一组线性系统（在相同稀疏矩阵下，求解多个线性系统）
             * @details 该函数用于求解线性方程组 𝐴×𝑋=𝐵，并将解存储在 rX 中。rX 也用作迭代法的初始猜测
             * @param rA 系数矩阵
             * @param rX 解向量
             * @param rB 常数项
             */
            virtual bool Solve(SparseMatrixType& rA, DenseMatrixType& rX, DenseMatrixType& rB){
                QUEST_ERROR << "Calling linear solver base class" << std::endl;
                return false;
            }


            /**
             * @brief 求解特征值问题
             * @details 用于求解特征值问题，即求解方程 Kv=λM，K为刚度阵，M为质量阵，λ为特征值，v为对应的特征向量
             * @param K 刚度矩阵
             * @param M 质量矩阵
             * @param Eigenvalues 特征值向量
             * @param Eigenvectors 特征向量矩阵
             */
            virtual void Solve(
                SparseMatrixType& K,
                SparseMatrixType& M,
                DenseVectorType& Eigenvalues,
                DenseMatrixType& Eigenvectors
            ){
                QUEST_ERROR << "Calling linear solver base class" << std::endl;
            }


            /**
             * @brief 检查是否需要额外的物理数据
             * @details 该函数用于检查是否需要额外的物理数据，如：混合u-p问题，平滑聚合求解器等
             */
            virtual bool AdditionalPhysicalDataIsNeeded(){
                return false;
            }


            /**
             * @brief 提供额外的物理数据
             * @details 该函数用于提供额外的物理数据，如：混合u-p问题，平滑聚合求解器等
             * @param rA 系数矩阵
             * @param rX 求解向量
             * @param rB 常数项
             * @param rDofSet 自由度集
             * @param rModelPart 模型部分
             */
            virtual void ProvideAdditionalData(
                SparseMatrixType& rA,
                VectorType& rX,
                VectorType& rB,
                typename ModelPart::DofsArrayType& rDofSet,
                ModelPart& rModelPart
            ){}


            /**
             * @brief 获取重排序器
             */
            virtual typename TReordererType::Pointer GetReorderer(){
                return mpReorderer;
            }


            /**
             * @brief 设置重排序器
             */
            virtual void SetReorderer(typename TReordererType::Pointer pNewReorderer){
                mpReorderer = pNewReorderer;
            }


            /**
             * @brief 设置求解精度
             */
            virtual void SetTolerance(double NewTolerance){
                QUEST_WARNING("LinearSolver") << "Accessed base function \"SetTolerance\". This does nothing !" << std::endl;
            }


            /**
             * @brief 获取求解精度
             */
            virtual double GetTolerance(){
                QUEST_WARNING("LinearSolver") << "Accessed base function \"GetTolerance\". No tolerance defined, returning 0 !" << std::endl;
                return 0;
            }


            /**
             * @brief 获取迭代次数
             */
            virtual IndexType GetIterationsNumber(){
                QUEST_WARNING("LinearSolver") << "Accessed base function \"GetIterationsNumber\", returning 0 !" << std::endl;
                return 0;
            }


            /**
             * @brief 检查矩阵的维度是否一致
             */
            virtual bool IsConsistent(SparseMatrixType& rA, VectorType& rX, VectorType& rB){
                const SizeType size = TSparseSpaceType::Size1(rA);
                const SizeType size_a = TSparseSpaceType::Size2(rA);
                const SizeType size_x = TSparseSpaceType::Size(rX);
                const SizeType size_b = TSparseSpaceType::Size(rB);

                return (size == size_a) && (size == size_x) && (size == size_b);
            }


            /**
             * @brief 检查多重求解方法的输入矩阵维度是否一致
             */
            virtual bool IsConsistent(SparseMatrixType& rA, DenseMatrixType& rX, DenseMatrixType& rB){
                const SizeType size = TSparseSpaceType::Size1(rA);
                const SizeType size_a = TSparseSpaceType::Size2(rA);
                const SizeType size_1_x = TDenseSpaceType::Size1(rX);
                const SizeType size_2_x = TDenseSpaceType::Size2(rX);
                const SizeType size_1_b = TDenseSpaceType::Size1(rB);
                const SizeType size_2_b = TDenseSpaceType::Size2(rB);

                return (size == size_a) && (size == size_1_x) && (size == size_1_b) && (size_2_x == size_2_b);
            }


            /**
             * @brief 检查矩阵维度是否不一致
             */
            virtual bool IsNotConsistent(SparseMatrixType& rA, VectorType& rX, VectorType& rB){
                return !IsConsistent(rA, rX, rB);
            }


            /**
             * @brief 检查多重求解方法的输入矩阵维度是否不一致
             */
            virtual bool IsNotConsistent(SparseMatrixType& rA, DenseMatrixType& rX, DenseMatrixType& rB){
                return !IsConsistent(rA, rX, rB);
            }


            virtual std::string Info() const{
                return "Linear Solver";
            }


            virtual void PrintInfo(std::ostream& rOstream) const{
                rOstream << "Linear Solver";
            }


            virtual void PrintData(std::ostream& rOstream) const{
            }

        protected:

        private:
        /**
         * @brief 重排序器
         * @details 重排器基类指针，应用桥接模式，指向具体的重排器实例
         */
        typename TReordererType::Pointer mpReorderer;
        
    };


    template<class TSparseSpaceType, typename TDenseSpaceType, typename TReordererType>
    inline std::istream& operator >> (std::istream& rIstream, LinearSolver<TSparseSpaceType, TDenseSpaceType, TReordererType>& rThis){
        return rIstream;
    }


    template<class TSparseSpaceType, typename TDenseSpaceType, typename TReordererType>
    inline std::ostream& operator << (std::ostream& rOstream, const LinearSolver<TSparseSpaceType, TDenseSpaceType, TReordererType>& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);
        
        return rOstream;
    }

} // namespace Quest

#endif //QUEST_LINEAR_SOLVER_HPP