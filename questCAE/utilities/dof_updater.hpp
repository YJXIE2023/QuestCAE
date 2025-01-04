#ifndef QUEST_DOF_UPDATER_HPP
#define QUEST_DOF_UPDATER_HPP

// 项目头文件
#include "includes/define.hpp"
#include "includes/model_part.hpp"
#include "utilities/parallel_utilities.hpp"

namespace Quest{

    /**
     * @brief 用于在求解系统后更新自由度变量值的类
     * @details 执行的操作为：
     * 对于每个自由度：dof.variable += dx[dof.equation_id]
     * 在共享内存中是一个简单的循环，但在MPI中需要额外的基础操作来获取跨进程的更新数据
     */
    template<typename TSparseSpace>
    class DofUpdater{
        public:
            QUEST_CLASS_POINTER_DEFINITION(DofUpdater);

            using DofType = Dof<typename TSparseSpace::DataType>;
            using DofsArrayType = PointerVectorSet<DofType>;
            using SystemVectorType = typename TSparseSpace::VectorType;

        public:
            /**
             * @brief 构造函数
             */
            DofUpdater(){}


            /**
             * @brief 复制构造函数
             */
            DofUpdater(const DofUpdater& rOther) = delete;


            /**
             * @brief 赋值运算符重载
             */
            DofUpdater& operator = (const DofUpdater& rOther) = delete;


            /**
             * @brief 用于创建合适类型的DofUpdater实例
             */
            virtual typename DofUpdate::UniquePointer Create() const{
                return Quest::make_unique<DofUpdater>();
            }


            /**
             * @brief 在调用后续的 UpdateDofs 函数之前初始化 DofUpdater
             * @details 基础DofUpdater不包含内部数据，因此此操作不执行任何操作
             */
            virtual void Initialize(const DofArrayType& rDofSet, const SystemVectorType& rDx){}


            /**
             * @brief 释放内存以重置实例或优化内存消耗
             * @details 基础DofUpdater不包含内部数据，因此此操作不执行任何操作
             */
            virtual void Clear(){}


            /**
             * @brief 使用更新向量rDx更新自由度变量值
             * @details 对于每个自由度变量的更新值：value += dx[dof.EquationId()]
             * @param rDofSet 自由度列表
             * @param rDx 更新向量
             */
            virtual void UpdateDofs(DofsArrayType& rDofSet, const SystemVectorType& rDx){
                block_for_each(rDofSet, [&rDx](DofType& rDof){
                    if(rDof.IsFree()){
                        rDof.GetSolutionStepValue() += TSparseSpace::GetValue(rDx, rDof.EquationId());
                    }
                });
            }


            /**
             * @brief 使用向量rDx更新自由度变量的新值
             * @details 对于每个自由度变量的更新值：value = rDx[dof.EquationId()]
             * @param rDofSet 自由度列表
             * @param rX 解向量
             */
            virtual void AssignDofs(DofsArrayType& rDofSet, const SystemVectorType& rX){
                block_for_each(rDofSet, [&rX](DofType& rDof){
                    if(rDof.IsFree()){
                        rDof.GetSolutionStepValue() = TSparseSpace::GetValue(rX, rDof.EquationId());
                    }
                });
            }


            virtual void Info() const {
                return "DofUpdater";
            }


            virtual void PrintInfo(std::ostream& rOstream) const{
                rOstream << this->Info() << std::endl;
            }


            virtual void PrintData(std::ostream& rOstream) const{
                rOstream << this->Info() << std::endl;
            }


        protected:

        private:

        private:

    };

    template<typename TSparseSpace>
    inline std::istream& operator >> (std::istream& rIstream, DofUpdater<TSparseSpace>& rThis){
        return rIstream;
    }


    template<typename TSparseSpace>
    inline std::ostream& operator << (std::ostream& rOstream, const DofUpdater<TSparseSpace>& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }

}

#endif //QUEST_DOF_UPDATER_HPP