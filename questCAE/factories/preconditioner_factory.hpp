#ifndef QUEST_PRECONDITIONER_FACTORY_HPP
#define QUEST_PRECONDITIONER_FACTORY_HPP

// 项目头文件
#include "includes/define.hpp"
#include "includes/quest_parameters.hpp"
#include "includes/quest_components.hpp"
#include "linear_solvers/preconditioner/preconditioner.hpp"
#include "space/ublas_space.hpp"


namespace Quest{

    /**
     * @brief 提供了注册预处理器所需的功能
     * @details 定义了基础预条件器工厂
     * @tparam TSparseSpace 稀疏空间的类型
     * @tparam TLocalSpace 稠密空间的定义
     */
    template<typename TSparseSpace, typename TLocalSpace>
    class PreconditionerFactory{
        public:
            using FactoryType = PreconditionerFactory<TSparseSpace, TLocalSpace>;
            using PreconditionerType = Preconditioner<TSparseSpace, TLocalSpace>;

            QUEST_CALSS_POINTER_DEFINITION(PreconditionerFactory);

        public:
            /**
             * @brief 析构函数
             */
            virtual ~PreconditionerFactory(){}


            /**
             * @brief 检查线性求解器是否被注册
             */
            virtual bool Has(const std::string& rSolverType) const{
                return QuestComponents<FactoryType>::Has(rSolverType);
            }


            /**
             * @brief 创建预处理器
             */
            virtual typename PreconditionerType::Pointer Create(const std::string& rPreconditionerType) const{
                const std::string raw_precond_name = rPreconditionerType.substr(rPreconditionerType.find('.')+1);

                QUEST_ERROR_IF_NOT(Has(raw_precond_name))
                    << "Trying to construct a preconditioner with preconditioner_type:\n\""
                    << raw_precond_name << "\" which does not exist.\n"
                    << "The list of available options (for currently loaded applications) is:\n"
                    << KratosComponents< FactoryType >() << std::endl;

                const auto& aux = QuestComponents<FactoryType>::Get(raw_precond_name);
                return aux->CreatePreconditioner();
            }

        protected:
            /**
             * @brief 创建新预处理器的辅助方法
             */
            virtual typename PreconditionerType::Pointer CreatePreconditioner() const{
                QUEST_ERROR << "calling the base class PreconditionerFactory" << std::endl;
            }

        private:

        private:

    };


    template<typename TSparseSpace, typename TLocalSpace>
    inline std::ostream& operator << (std::ostream& rOstream, const PreconditionerFactory<TSparseSpace, TLocalSpace>& rThis){
        rOstream << "PreconditionerFactory:" << std::endl;
        return rOstream;
    }


    using SparseSpaceType = TUblasSparseSpace<double>;
    using LocalSparseSpaceType = TUblasDenseSpace<double>;

    using PreconditionerFactoryType = PreconditionerFactory<SparseSpaceType, LocalSparseSpaceType>;

    QUEST_API_EXTERN template class QUEST_API(QUEST_CORE) QuestComponents<PreconditionerFactoryType>;

    #ifdef QUEST_REGISTER_PRECONDITIONER
    #undef QUEST_REGISTER_PRECONDITIONER
    #endif
    #define QUEST_REGISTER_PRECONDITIONER(name, reference) \
        QuestComponents<PreconditionerFactoryType>::Add(name, reference);

}


#endif //QUEST_PRECONDITIONER_FACTORY_HPP