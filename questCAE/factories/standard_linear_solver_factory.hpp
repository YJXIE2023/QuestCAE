#ifndef QUEST_STANDARD_LINEAR_SOLVER_FACTORY_HPP
#define QUEST_STANDARD_LINEAR_SOLVER_FACTORY_HPP

// 项目头文件
#include "includes/define.hpp"
#include "factories/linear_solver_factory.hpp"
#include "linear_solvers/linear_solver.hpp"
#include "linear_solvers/scaling_solver.hpp"


namespace Quest{

    /**
     * @brief 标准线性求解器工厂类
     * @details 添加了注册线性求解器所需的功能
     * @tparam TSparseSpace 稀疏空间
     * @tparam TLocalSpace 稠密空间
     * @tparam TLinearSolverType 线性求解器类型
     */
    template<typename TSparseSpace, typename TLocalSpace, typename TLinearSolverType>
    class StandardLinearSolverFactory : public LinearSolverFactory<TSparseSpace, TLocalSpace>{
        public:


        public:

        protected:
            /**
             * @brief 创建求解器对象
             */
            typename LinearSolverType::Pointer CreateSolver(Quest::Parameters settings) const override{
                if(settings.Has("scaling") && settings["scaling"].GetBool()){
                    auto pinner_solver = typename LinearSolverType::Pointer(new TLinearSolverType(settings));

                    return typename LinearSolverType::Pointer(new ScalingSolver<TSparseSpace, TLocalSpace>(pinner_solver, true)); 
                } else {
                    return typename LinearSolverType::Pointer(new TLinearSolverType(settings));
                }
            }

        private:
            using LinearSolverType = LinearSolver<TSparseSpace, TLocalSpace>;

        private:


    };


    template<typename TSparseSpace, typename TLocalSpace, typename TLinearSolverType>
    inline std::ostream& operator << (std::ostream& rOstream, const StandardLinearSolverFactory<TSparseSpace, TLocalSpace, TLinearSolverType>& rThis){
        rOstream << "StandardLinearSolverFactory:" << std::endl;
        return rOstream;
    }


    void RegisterLinearSolvers();

}


#endif //QUEST_STANDARD_LINEAR_SOLVER_FACTORY_HPP