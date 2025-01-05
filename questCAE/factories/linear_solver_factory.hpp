#ifndef QUEST_LINEAR_SOLVER_FACTORY_HPP
#define QUEST_LINEAR_SOLVER_FACTORY_HPP

// 项目头文件
#include "includes/define.hpp"
#include "includes/quest_parameters.hpp"
#include "includes/quest_components.hpp"
#include "linear_solvers/linear_solver.hpp"
#include "space/ublas_space.hpp"


namespace Quest{

    /**
     * @brief 基础线性求解器工厂，包含线性求解器注册所需要的函数
     * @tparam TSparseSpace 稀疏空间
     * @tparam TLocalSpace 稠密空间
     */
    template<typename TSparseSpace, typename TLocalSpace>
    class LinearSolverFactory{
        public:
            using FactoryType = LinearSolverFactory<TSparseSpace, TLocalSpace>;

            QUEST_CALSS_POINTER_DEFINITION(LinearSolverFactory);

        public:
            /**
             * @brief 析构函数
             */
            virtual ~LinearSolverFactory(){}


            /**
             * @brief 检查线性求解器是否注册
             */
            virtual bool Has(const std::string& SolverType) const{
                return QuestComponents<FactoryType>::Has(SolverType);
            }
            

            /**
             * @brief 创建一个线性求解器
             */
            virtual typename LinearSolver<TSparseSpace, TLocalSpace>::Pointer Create(Quest::Parameters Settings) const{
                std::string solver_name = Settings["solver_type"].GetString();
                solver_name = solver_name.substr(solver_name.find('.')+1);

                QUEST_ERROR_IF_NOT(Has(solver_name))
                    << "Trying to construct a Linear solver with solver_type:\n\""
                    << solver_name << "\" which does not exist.\n"
                    << "The list of available options (for currently loaded applications) is:\n"
                    << KratosComponents< FactoryType >() << std::endl;

                const auto& aux = QuestComponents<FactoryType>::Get(solver_name);
                return aux.CreateSover(Settings);
            }

        protected:
            /**
             * @brief 创建一个线性求解器
             */
            virtual typename LinearSolver<TSparseSpace, TLocalSpace>::Pointer CreateSolver(Quest::Parameters Setting) const{
                QUEST_ERROR << "Calling the base class LinearSolverFactory" << std::endl;
            }

        private:

        private:

    };


    using SparseSpaceType = TUblasSparseSpace<double>;
    using LocalSparseSpaceType = TUblasDenseSpace<double>;

    using LinearSolverFactoryType = LinearSolverFactory<SparseSpaceType, LocalSparseSpaceType>;

    QUEST_API_EXTERN template class QUEST_API(QUEST_CORE) QuestComponents<LinearSolverFactoryType>;

    #ifdef QUEST_REGISTER_LINEAR_SOLVER
    #undef QUEST_REGISTER_LINEAR_SOLVER
    #endif
    #define QUEST_REGISTER_LINEAR_SOLVER(name, reference) \
        QuestComponents<LinearSolverFactoryType>::Add(name, reference);

    
    using ComplexSparseSpaceType = TUblasSparseSpace<std::complex<double>>;
    using ComplexLocalSparseSpaceType = TUblasDenseSpace<std::complex<double>>;

    using ComplexLinearSolverFactoryType = LinearSolverFactory<ComplexSparseSpaceType, ComplexLocalSparseSpaceType>;

    QUEST_API_EXTERN template class QUEST_API(QUEST_CORE) QuestComponents<ComplexLinearSolverFactoryType>;

    #ifdef QUEST_REGISTER_COMPLEX_LINEAR_SOLVER
    #undef QUEST_REGISTER_COMPLEX_LINEAR_SOLVER
    #endif
    #define QUEST_REGISTER_COMPLEX_LINEAR_SOLVER(name, reference) \
        QuestComponents<ComplexLinearSolverFactoryType>::Add(name, reference);

}


#endif //QUEST_LINEAR_SOLVER_FACTORY_HPP