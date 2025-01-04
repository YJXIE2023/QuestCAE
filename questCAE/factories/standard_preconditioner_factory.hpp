#ifndef QUEST_STANDARD_PRECONDITIONER_FACTORY_HPP
#define QUEST_STANDARD_PRECONDITIONER_FACTORY_HPP

// 项目头文件
#include "includes/define.hpp"
#include "factories/preconditioner_factory.hpp"
#include "linear_solvers/preconditioner/preconditioner.hpp"


namespace Quest{

    /**
     * @brief 包含用于预处理器注册的必要函数
     * @details 定义了标准预处理器工厂
     * @tparam TSparseSpace 稀疏空间定义
     * @tparam TLocalSpace 稠密空间定义
     * @tparam TPreconditionerType 预处理器类型
     */
    template<typename TSparseSpace, typename TLocalSpace, typename TPreconditionerType>
    class StandardPreconditionerFactory : public PreconditionerFactory<TSparseSpace, TLocalSpace> {
        public:

        protected:
            /**
             * @brief 创建预处理器
             */
            typename PreconditionerType::Pointer CreatePreconditioner() const override{
                return typename PreconditionerType::Pointer(new TPreconditionerType());
            }

        private:
            using PreconditionerType = Preconditioner<TSparseSpace, TLocalSpace>;
    };

    template<typename TSparseSpace, typename TLocalSpace, typename TPreconditionerType>
    inline std::ostream& operator << (std::ostream& rOstream, const StandardPreconditionerFactory<TSparseSpace, TLocalSpace, TPreconditionerType>& rThis){
        rOstream << "StandardPreconditionerFactory" << std::endl;
        return rOstream;
    }


    void RegisterPreconditioners();

}


#endif //QUEST_STANDARD_PRECONDITIONER_FACTORY_HPP