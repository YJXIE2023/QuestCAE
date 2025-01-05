#ifndef QUEST_SCALING_SOLVER_HPP
#define QUEST_SCALING_SOLVER_HPP

// 系统头文件
#include <cmath>
#include <complex>

// 项目头文件
#include "includes/define.hpp"
#include "factories/linear_solver_factory.hpp"
#include "linear_solvers/linear_solver.hpp"
#include "utilities/openmp_utils.hpp"


namespace Quest{

    /**
     * @brief 此求解器通过重新缩放来改善系统的条件数
     * @details 对矩阵进行重新缩放，并使用指定的线性求解器
     * @param TSparseSpaceType 稀疏空间
     * @param TDenseSpaceType 稠密空间
     * @param TReordererType 重排器类型
     */
    template<typename TSparseSpaceType, typename TDenseSpaceType, typename TReordererType = Reorderer<TSparseSpaceType, TDenseSpaceType>>
    class ScalingSolver : public LinearSolver<TSparseSpaceType, TDenseSpaceType, TReordererType>{
        public:
            QUEST_CLASS_POINTER_DEFINITION(ScalingSolver);

            using BaseType = LinearSolver<TSparseSpaceType, TDenseSpaceType, TReordererType>;
            using SparseMatrixType = typename TSparseSpaceType::MatrixType;
            using VectorType = typename TDenseSpaceType::VectorType;
            using DenseMatrixType = typename TDenseSpaceType::MatrixType;
            using LinearSolverFactoryType = LinearSolverFactory<TSparseSpaceType, TDenseSpaceType>;
            using IndexType = typename TSparseSpaceType::IndexType;

        public:
            /**
             * @brief 默认构造函数
             */
            ScalingSolver(){}


            /**
             * @brief 构造函数
             */
            ScalingSolver(
                typename BaseType::Pointer pLinearSolver,
                const bool symmetricScaling = true
            ) : BaseType(), mpLinearSolver(pLinearSolver), mSymmetricScaling(symmetricScaling){}


            /**
             * @brief 构造函数
             */
            ScalingSolver(Parameters ThisParameters) : BaseType(){
                QUEST_TRY

                QUEST_ERROR_IR_NOT(ThisParameters.Has("solver_type")) << "Solver_type must be specified to construct the ScalingSolver" << std::endl;

                mpLinearSolver = LinearSolverFactoryType().Create(ThisParameters);

                mSymmetricScaling = ThisParameters.Has("symmetric_scaling") ? ThisParameters["symmetric_scaling"].GetBool() : true;

                QUEST_CATCH("")
            }


            /**
             * @brief 复制构造函数
             */
            ScalingSolver(const ScalingSolver& Other) : BaseType(Other){}


            /**
             * @brief 析构函数
             */
            ~ScalingSolver() override {}


            /**
             * @brief 重载赋值运算符
             */
            ScalingSolver& operator = (const ScalingSolver& Other){
                BaseType::operator=(Other);
                return *this;
            }


            /**
             * @brief 用于指示求解器是否需要了解矩阵结构
             * @details 例如，在求解混合 u-p 问题时，识别与 u 和 p 相关的行
             */
            bool AdditionalPhysicalDataIsNeeded() override{
                return mpLinearSolver->AdditionalPhysicalDataIsNeeded();
            }


            /**
             * @brief 为求解器提供额外的矩阵结构信息
             * @details 例如，在求解混合 u-p 问题时，识别与 u 和 p 相关的行
             */
            void ProvideAdditionalData(
                SparseMatrixType& rA,
                VectorType& rX,
                VectorType& rB,
                typename ModelPart::DofsArrayType& rdof_set,
                ModelPart& r_model_part
            ) override {
                mpLinearSolver->ProvideAdditionalData(rA, rX, rB, rdof_set, r_model_part);
            }


            /**
             * @brief 初始化求解步信息
             */
            void InitializeSolutionStep(
                SparseMatrixType& rA,
                VectorType& rX,
                VectorType& rB
            ) override {
                mpLinearSolver->InitializeSolutionStep(rA, rX, rB);
            }


            /**
             * @brief 完成分析步
             */
            void FinalizeSolutionStep(
                SparseMatrixType& rA,
                VectorType& rX,
                VectorType& rB
            ) override {
                mpLinearSolver->FinalizeSolutionStep(rA, rX, rB);
            }


            /**
             * @brief 清除所有数据
             */
            void Clear() override {
                mpLinearSolver->Clear();
            }


            /**
             * @brief 调用线性求解器进行求解
             */
            bool Solve(SparseMatrixType& rA, VectorType& rX, VectorType& rB) override {
                if(this->IsNotConsistent(rA, rX, rB))
                    return false;

                VectorType scaling_vector(rX.size());

                GetScalingWeights(rA, scaling_vector);

                if(mSymmetricScaling == false){
                    QUEST_THROW_ERROR(std::logic_error,"not yet implemented","");
                } else {
                    IndexPartition<std::size_t>(scaling_vector.size()).for_each([&](std::size_t Index){
                        scaling_vector[Index] = sqrt(std::abs(scaling_vector[Index]));
                    });
                    SymmetricScaling(rA, scaling_vector);
                }

                IndexPartition<std:size_t>(scaling_vector.size()).for_each([&](std::size_t Index){
                    rB[Index] /= scaling_vector[Index];
                });

                bool is_solved = mpLinearSolver->Solve(rA, rX, rB);

                if(mSymmetricScaling == true){
                    IndexPartition<std::size_t>(scaling_vector.size()).for_each([&](std::size_t Index){
                        rX[Index] /= scaling_vector[Index];
                    });
                }
                return is_solved;
            }


            /**
             * @brief 获取迭代次数
             */
            IndexType GetIterationsNumber() override{
                return mpLinearSolver->GetIterationsNumber();
            }


            std::string Info() const override{
                std::stringstream buffer;
                buffer << "Composite Linear Solver. Uses internally the following linear solver " << mpLinearSolver->Info();
                return  buffer.str();
            }


            void PrintInfo(std::ostream& rOstream) const override{
                rOstream << Info();
            }


            void PrintData(std::ostream& rOstream) const override{
                BaseType::PrintData(rOstream);
            }

        protected:

        private:
            /**
             * @brief 对稀疏矩阵进行对称缩放
             */
            static void SymmetricScaling(SparseMatrixType& A, const VectorType& aux){
                OpenMPUtils::PartitionVector partition;
                int number_of_threads = OpenMPUtils::GetNumThreads();
                OpenMPUtils::DivideInPartitions(A.size1(), number_of_threads, partition);

                #pragma omp parallel
                {
                    int thread_id = OpenMPUtils::ThisThread();
                    int number_of_rows = partition[thread_id+1]-partition[thread_id];
                    typename boost::numeric::ublas::compressed_matrix<typename TDenseSpaceType::DataType>::index_array_type::iterator row_iter_begin = A.index1_data().begin()+partition[thread_id];
                    typename boost::numeric::ublas::compressed_matrix<typename TDenseSpaceType::DataType>::index_array_type::iterator index_2_begin = A.index2_data().begin()+*row_iter_begin;
                    typename boost::numeric::ublas::compressed_matrix<typename TDenseSpaceType::DataType>::value_array_type::iterator value_begin = A.value_data().begin()+*row_iter_begin;

                    perform_matrix_scaling(
                        number_of_rows,
                        row_iter_begin,
                        index_2_begin,
                        value_begin,
                        partition[thread_id],
                        aux
                    );
                }
            }


            /**
             * @brief 计算部分乘积，并在开始前将输出重置为零
             */
            static void perform_matrix_scaling(
                int number_of_rows,
                typename boost::numeric::ublas::compressed_matrix<typename TDenseSpaceType::DataType>::index_array_type::iterator row_iter_begin,
                typename boost::numeric::ublas::compressed_matrix<typename TDenseSpaceType::DataType>::index_array_type::iterator index_2_begin,
                typename boost::numeric::ublas::compressed_matrix<typename TDenseSpaceType::DataType>::value_array_type::iterator value_begin,
                unsigned int output_begin_index,
                const VectorType& weights
            ){
                int row_size;
                typename SparseMatrixType::index_array_type::const_iterator row_it = row_iter_begin;
                int kkk = output_begin_index;
                for(int k = 0; k < number_of_rows; k++){
                    row_size = *(row_it+1) - *row_it;
                    row_it++;
                    const typename TDenseSpaceType::DataType* row_weight = weights[kkk++];

                    for(int i = 0; i < row_size; i++){
                        const typename TDenseSpaceType::DataType col_weight = weights[*index_2_begin];
                        typename TDenseSpaceType::DataType t = *value_begin;
                        t /= (col_weight * row_weight);
                        (*value_begin) = t;
                        value_begin++;
                        index_2_begin++;
                    }
                }
            }


            /**
             * @brief 获取缩放因子
             */
            static void GetScalingWeights(const SparseMatrixType& A, VectorType& aux){
                OpenMPUtils::PartitionVector partition;
                int number_of_threads = OpenMPUtils::GetNumThreads();
                OpenMPUtils::DivideInPartitions(A.size1(), number_of_threads, partition);

                #pragma omp parallel
                {
                    int thread_id = OpenMPUtils::ThisThread();
                    int number_of_rows = partition[thread_id+1]-partition[thread_id];
                    typename boost::numeric::ublas::compressed_matrix<typename TDenseSpaceType::DataType>::index_array_type::const_iterator row_iter_begin = A.index1_data().begin()+partition[thread_id];
                    typename boost::numeric::ublas::compressed_matrix<typename TDenseSpaceType::DataType>::index_array_type::const_iterator index_2_begin = A.index2_data().begin()+*row_iter_begin;
                    typename boost::numeric::ublas::compressed_matrix<typename TDenseSpaceType::DataType>::value_array_type::const_iterator value_begin = A.value_data().begin()+*row_iter_begin;

                    GS2Weights(
                        number_of_rows,
                        row_iter_begin,
                        index_2_begin,
                        value_begin,
                        partition[thread_id],
                        aux
                    );
                }
            }


            /**
             * @brief 计算部分值
             */
            static void GS2Weights(
                int number_of_rows,
                typename boost::numeric::ublas::compressed_matrix<typename TDenseSpaceType::DataType>::index_array_type::const_iterator row_iter_begin,
                typename boost::numeric::ublas::compressed_matrix<typename TDenseSpaceType::DataType>::index_array_type::const_iterator index_2_begin,
                typename boost::numeric::ublas::compressed_matrix<typename TDenseSpaceType::DataType>::value_array_type::const_iterator value_begin,
                unsigned int output_begin_index,
                VectorType& weights
            ){
                int row_size;
                typename SparseMatrixType::index_array_type::const_iterator row_it = row_iter_begin;
                int kkk = output_begin_index;
                for(int k = 0; k < number_of_rows; k++){
                    row_size = *(row_it+1) - *row_it;
                    row_it++;
                    double t = 0.0;

                    for(int i = 0; i < row_size; i++){
                        double temp = std::abs(*value_begin);
                        t += temp * temp;
                        value_begin++;
                    }
                    t = sqrt(t);
                    weights[kkk++] = t;
                }
            }

        private:
            /**
             * @brief 线性求解器对象
             */
            typename LinearSolver<TSparseSpaceType, TDenseSpaceType, TReordererType>::Pointer mpLinearSolver;


            /**
             * @brief 是否在矩阵缩放时考虑对称缩放
             */
            bool mSymmetricScaling;

    };

    template<typename TSparseSpaceType, typename TDenseSpaceType, typename TPreconditionerType, typename TReordererType>
    inline std::istream& operator >> (std::istream& rIstream, ScalingSolver<TSparseSpaceType, TDenseSpaceType, TReordererType>& rThis){
        return rIstream;
    }


    template<typename TSparseSpaceType, typename TDenseSpaceType, typename TPreconditionerType, typename TReordererType>
    inline std::ostream& operator << (std::ostream& rOstream, const ScalingSolver<TSparseSpaceType, TDenseSpaceType, TReordererType>& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);
        
        return rOstream;
    }

}


#endif //QUEST_SCALING_SOLVER_HPP