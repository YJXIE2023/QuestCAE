#ifndef QUEST_MASTER_SLAVE_CONSTRAINT_HPP   
#define QUEST_MASTER_SLAVE_CONSTRAINT_HPP  

// 项目头文件
#include "includes/define.hpp"
#include "includes/node.hpp"
#include "container/flags.hpp"
#include "container/variable.hpp"
#include "includes/process_info.hpp"
#include "includes/indexed_object.hpp"

namespace Quest{

    /**
     * @class MasterSlaveConstraint
     * @ingroup KratosCore
     * @brief 一个用于在系统中应用不同主从约束的接口类。
     * @details 这是用户从 Python 层面看到的部分。此类的对象是模型部分（ModelPart）的一级成员。
     *
     * 此类允许添加形式如下的主从约束：
     *
     * SlaveDofVector = T * MasterDofVector + rConstantVector.（此形式的处理目前尚未实现。）
     *
     * 或者：
     *
     * SlaveDof = weight * MasterDof + Constant
     *
     * 此类的对象将提供其从属节点（slave）、主节点（master）的详细信息以及它们之间的关系矩阵。
     *
     * 用户可以为同一个从属节点（slave）添加具有不同主节点（master）和权重（weights）的两个 MasterSlaveConstraint 对象。
     * 假设用户添加了以下约束：
     * SlaveDof = weight1 * MasterDof1 + Constant1
     * 和：
     * SlaveDof = weight2 * MasterDof2 + Constant2
     *
     * 这些约束稍后会在构建器和求解器中被合并为：
     * SlaveDof = weight1 * MasterDof1 + weight2 * MasterDof2 + Constant1 + Constant2
     * 然后转换为：
     * SlaveEqID = weight1 * MasterEqId1 + weight2 * MasterEqId2 + Constant1 + Constant2
     * 该唯一的方程稍后将用于修改方程系统。
     */
    class QUEST_API(QUEST_CORE) MasterSlaveConstraint: public IndexedObject, public Flags{
        public:
            QUEST_CLASS_POINTER_DEFINITION(MasterSlaveConstraint);

            using BaseType = IndexedObject;
            using IndexType = std::size_t;
            using DofType = Dof<double>;
            using DofPointerVectorType = std::vector<DofType::Pointer>;
            using NodeType = Node;
            using EquationIdVectorType = std::vector<std::size_t>;
            using MatrixType = Matrix;
            using VectorType = Vector;
            using VariableType = Quest::Variable<double>;


        public:
            /**
             * @brief 构造函数
             * @param Id 主从约束的 Id
             */
            explicit MasterSlaveConstraint(IndexType Id = 0):IndexedObject(Id),Flags(){}

            /**
             * @brief 析构函数
             */
            virtual ~MasterSlaveConstraint() override{}

            /**
             * @brief 复制构造函数
             */
            MasterSlaveConstraint(const MasterSlaveConstraint& rOther):
                BaseType(rOther),
                mData(rOther.mData)
            {}

            /**
             * @brief 赋值运算符
             */
            MasterSlaveConstraint& operator = (const MasterSlaveConstraint& rOther){
                BaseType::operator=(rOther);
                mData = rOther.mData;
                return *this;
            }

            /**
             * @brief 创建一个新的主从约束指针
             * @param Id 主从约束的 Id
             * @param rMasterDofsVector 主节点的 Dof 指针向量
             * @param rSlaveDofsVector 从属节点的 Dof 指针向量
             * @param rRelationMatrix 一个矩阵，用于表示主自由度与从自由度之间的权重关系
             * @param rConstantVector 常量向量，每个从自由度对应一个条目
             */
            virtual MasterSlaveConstraint::Pointer Create(
                IndexType Id,
                DofPointerVectorType& rMasterDofsVector,
                DofPointerVectorType& rSlaveDofsVector,
                const MatrixType& rRelationMatrix,
                const VectorType& rConstantVector
            ) const {
                QUEST_TRY

                QUEST_ERROR << "Create not implemented in MasterSlaveConstraintBaseClass" << std::endl;

                QUEST_CATCH("")
            }

            /**
             * @brief 创建一个新的约束指针
             * @param Id 新约束的ID
             * @param rMasterNode 作为此约束主节点的节点
             * @param rMasterVariable 主节点上的标量变量 (自由度, DOF)
             * @param rSlaveNode 作为此约束从节点的节点
             * @param rSlaveVariable 从节点上的标量变量 (自由度, DOF)
             * @param Weight 主节点和从节点之间的关系权重，关系为 s = w
             */
            virtual MasterSlaveConstraint::Pointer Create(
                IndexType Id,
                NodeType& rMasterNode,
                const VariableType& rMasterVariable,
                NodeType& rSlaveNode,
                const VariableType& rSlaveVariable,
                const double Weight,
                const double Constant
            ) const {
                QUEST_TRY

                QUEST_ERROR << "Create not implemented in MasterSlaveConstraintBaseClass" << std::endl;

                QUEST_CATCH("")
            }

            /**
             * @brief 创建一个新的约束指针并克隆之前约束的数据
             * @param NewId 新约束的ID
             * @return 指向新约束的指针
             */
            virtual Pointer Clone(IndexType NewId) const{
                QUEST_TRY

                QUEST_WARNING("MasterSlaveConstraint") << " Call base class constraint Clone " << std::endl;
                MasterSlaveConstraint::Pointer p_new_const = Quest::make_shared<MasterSlaveConstraint>(*this);
                p_new_const->SetId(NewId);
                p_new_const->SetData(this->GetData());
                p_new_const->Set(Flags(*this));
                return p_new_const;

                QUEST_CATCH("")
            }

            /**
             * @brief 清空map的内容
             */
            virtual void Clear(){}

            /**
             * @brief 用于初始化约束
             * @details 如果约束在计算之前需要执行任何操作，则调用此函数
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void Initailize(const ProcessInfo& rCurrentProcessInfo){}

            /**
             * @brief 用于最终化约束
             * @details 如果约束在计算之前需要执行任何操作，则调用此函数
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void Finalize(const ProcessInfo& rCurrentProcessInfo){
                this->Clear();
            }

            /**
             * @brief 在每个求解步骤开始时调用
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void InitializeSolutionStep(const ProcessInfo& rCurrentProcessInfo){}

            /**
             * @brief 在非线性分析的迭代过程开始时调用
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void InitializeNonLinearIteration(const ProcessInfo& rCurrentProcessInfo){}

            /**
             * @brief 这是在非线性分析的迭代过程结束时调用的
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void FinalizeNonLinearIteration(const ProcessInfo& rCurrentProcessInfo){}

            /**
             * @brief 在每个求解步骤结束时调用
             */
            virtual void FinalizeSolutionStep(const ProcessInfo& rCurrentProcessInfo){}

            /**
             * @brief 获取约束的从节点和主节点的自由度（DOF）列表
             * @param rSlaveDofsVector 从节点自由度列表
             * @param rMasterDofsVector 主节点自由度列表
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void GetDofList(
                DofPointerVectorType& rSlaveDofsVector,
                DofPointerVectorType& rMasterDofsVector,
                const ProcessInfo& rCurrentProcessInfo
            ) const {
                QUEST_ERROR << "GetDofList not implemented in MasterSlaveConstraintBaseClass" << std::endl;
            }

            /**
             * @brief 设置约束的从节点和主节点的自由度（DOF）列表
             * @param rSlaveDofsVector 从节点自由度列表
             * @param rMasterDofsVector 主节点自由度列表
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void SetDofList(
                const DofPointerVectorType& rSlaveDofsVector,
                const DofPointerVectorType& rMasterDofsVector,
                const ProcessInfo& rCurrentProcessInfo
            ){
                QUEST_ERROR << "SetDofList not implemented in MasterSlaveConstraintBaseClass" << std::endl;
            }

            /**
             * @brief 确定与此约束相关的主方程ID
             * @param rSlaveEquationIds 从属方程ID的向量
             * @param rMasterEquationIds 主方程ID的向量
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void EquationIdVector(
                EquationIdVectorType& rSlaveEquationIds,
                EquationIdVectorType& rMasterEquationIds,
                const ProcessInfo& rCurrentProcessInfo
            ) const {
                if (rSlaveEquationIds.size() != 0){
                    rSlaveEquationIds.resize(0);
                }

                if (rMasterEquationIds.size() != 0){
                    rMasterEquationIds.resize(0);
                }
            }

            /**
             * @brief 返回从属自由度向量
             * @return 包含从属自由度的向量
             */
            virtual const DofPointerVectorType& GetSlaveDofsVector() const{
                QUEST_ERROR << "GetSlaveDofsVector not implemented in MasterSlaveConstraintBaseClass" << std::endl;
            }

            /**
             * @brief 设置从属自由度向量
             * @return 包含从属自由度的向量
             */
            virtual void SetSlaveDofsVector(const DofPointerVectorType& rSlaveDofsVector){
                QUEST_ERROR << "SetSlaveDofsVector not implemented in MasterSlaveConstraintBaseClass" << std::endl;
            }

            /**
             * @brief 返回主自由度向量
             * @return 包含主自由度的向量
             */
            virtual const DofPointerVectorType& GetMasterDofsVector() const{
                QUEST_ERROR << "GetMasterDofsVector not implemented in MasterSlaveConstraintBaseClass" << std::endl;
            }

            /**
             * @brief 设置主自由度向量
             * @return 包含主自由度的向量
             */
            virtual void SetMasterDofsVector(const DofPointerVectorType& rMasterDofsVector){    
                QUEST_ERROR << "SetMasterDofsVector not implemented in MasterSlaveConstraintBaseClass" << std::endl;
            }

            /**
             * @brief 重置从属自由度的值
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void ResetSlaveDofs(const ProcessInfo& rCurrentProcessInfo){
                QUEST_ERROR << "ResetSlaveDofs not implemented in MasterSlaveConstraintBaseClass" << std::endl;
            }

            /**
             * @brief 直接应用主/从关系
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void Apply(const ProcessInfo& rCurrentProcessInfo){
                QUEST_ERROR << "Apply not implemented in MasterSlaveConstraintBaseClass" << std::endl;
            }

            /**
             * @brief 如果在运行时没有计算，可以设置局部系统（内部变量）
             * @param rRelationMatrix 关联主从自由度的矩阵
             * @param rConstant 常量向量（每个从节点一个条目）
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void SetLocalSystem(
                const MatrixType& rRelationMatrix,
                const VectorType& rConstantVector,
                const ProcessInfo& rCurrentProcessInfo
            ){
                QUEST_TRY

                QUEST_ERROR << "SetLocalSystem not implemented in MasterSlaveConstraintBaseClass" << std::endl;

                QUEST_CATCH("")
            }

            /**
             * @brief 如果在运行时没有计算，可以获取局部系统（内部变量）
             * @param rRelationMatrix 关联主从自由度的矩阵
             * @param rConstant 常量向量（每个从节点一个条目）
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void GetLocalSystem(
                MatrixType& rRelationMatrix,
                VectorType& rConstantVector,
                const ProcessInfo& rCurrentProcessInfo
            ) const {
                QUEST_TRY

                this->CalculateLocalSystem(rRelationMatrix, rConstantVector, rCurrentProcessInfo);

                QUEST_CATCH("")
            }

            /**
             * @brief 在组装过程中调用
             * @details 用于计算主从自由度之间的关系
             * @param rRelationMatrix 关联主从自由度的矩阵
             * @param rConstant 常量向量（每个从节点一个条目）
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void CalculateLocalSystem(
                MatrixType& rRelationMatrix,
                VectorType& rConstantVector,
                const ProcessInfo& rCurrentProcessInfo
            ) const {
                if(rRelationMatrix.size1() != 0){
                    rRelationMatrix.resize(0,0,false);
                }

                if(rConstantVector.size() != 0){
                    rConstantVector.resize(0,false);
                }
            }

            /**
             * @brief 此方法用于检查输入的完整性以及与问题选项的兼容性
             * @details 该方法设计为仅在计算开始时调用一次（或者通常不常调用），用于验证输入中没有遗漏或者没有发现常见错误。
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual int Check(const ProcessInfo& rCurrentProcessInfo) const{
                QUEST_TRY

                QUEST_ERROR_IF(this->Id() < 1) << "MasterSlaveConstraint found with Id " << this->Id() << std::endl;

                return 0;

                QUEST_CATCH("")
            }

            /**
             * @brief 返回包含此对象详细描述的字符串
             * @return 包含信息的字符串
             */
            virtual std::string GetInfo() const{
                return " Constraint base class !";
            }


            virtual void PrintInfo(std::ostream& rOstream) const override{
                rOstream << " MasterSlaveConstraint Id : " << this->Id() << std::endl;
            }

            /**
             * @brief 返回存储与约束相关的数据的指针
             */
            DataValueContainer& Data(){
                return mData;
            }

            /**
             * @brief 返回存储与约束相关的数据的指针
             */
            const DataValueContainer& GetData() const{
                return mData;
            }

            /**
             * @brief 设置存储与约束相关的数据的指针
             */
            void SetData(const DataValueContainer& rThisData){
                mData = rThisData;
            }

            /**
             * @brief 确定是否存在某个变量
             */
            template<typename TDataType>
            bool Has(const Variable<TDataType>& rThisVariable) const{
                return mData.Has(rThisVariable);
            }

            /**
             * @brief 设置某个变量的值
             */
            template<typename TVariableType>
            void SetValue(
                const TVariableType& rThisVariable,
                const typename TVariableType::Type& rValue
            ){
                mData.SetValue(rThisVariable, rValue);
            }

            /**
             * @brief 获取某个变量的值
             */
            template<typename TVariableType>
            typename TVariableType::Type& GetValue(const TVariableType& rThisVariable){
                return mData.GetValue(rThisVariable);
            }

            /**
             * @brief 获取某个变量的值
             */
            template<typename TVariableType>
            typename TVariableType::Type& GetValue(const TVariableType& rThisVariable) const{
                return mData.GetValue(rThisVariable);
            }

            /**
             * @brief 判断是否激活
             */
            bool IsActive() const;

        protected:

        private:
            friend class Serializer;

            virtual void save(Serializer& rSerializer) const override{
                QUEST_SERIALIZE_SAVE_BASE_CLASS(rSerializer, IndexedObject);
                QUEST_SERIALIZE_SAVE_BASE_CLASS(rSerializer, Flags);
                rSerializer.save("Data", mData);
            }

            virtual void load(Serializer& rSerializer) override{
                QUEST_SERIALIZE_LOAD_BASE_CLASS(rSerializer, IndexedObject);
                QUEST_SERIALIZE_LOAD_BASE_CLASS(rSerializer, Flags);
                rSerializer.load("Data", mData);
            }

        private:
            /**
             * @brief 用于存储与约束相关的数据的指针
             */
            DataValueContainer mData;

    };

    QUEST_API_EXTERN template class QUEST_API(QUEST_CORE) QuestComponents<MasterSlaveConstraint>;

    inline std::istream& operator >> (std::istream& rIstream, MasterSlaveConstraint& rThis);

    inline std::ostream& operator << (std::ostream& rOstream, const MasterSlaveConstraint& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;

        return rOstream;
    }

} // namespace Quest

#endif  //QUEST_MASTER_SLAVE_CONSTRAINT_HPP