/*-------------------------------------------------------------
节点自由度的轻量级的表示
存储自由度的物理量、状态（自由或受约束）以及该自由度在数据结构中的实际值
-------------------------------------------------------------*/

#ifndef QUEST_DOF_HPP
#define QUEST_DOF_HPP

// 系统头文件
#include <string>
#include <iostream>
#include <sstream>
#include <cstddef>

// 项目头文件
#include "includes/define.hpp"
#include "container/data_value_container.hpp"
#include "container/nodal_data.hpp"

namespace Quest{
    
    #define QUEST_DOF_TRAITS  \
        QUEST_MAKE_DOF_TRAIT(0) Variable<TDataType> QUEST_END_DOF_TRAIT(0);

    template<typename TDataType,typename TVariableType = Variable<TDataType>>
    struct DofTrait{
        static const int Id;
    };

    #define QUEST_MAKE_DOF_TRAIT(id) \
        template<typename TDataType> \
        struct DofTrait<TDataType,

    #define QUEST_END_DOF_TRAIT(id) \
        >{ \
        static const int Id = id; \
        }

    QUEST_DOF_TRAITS

    #undef QUEST_MAKE_DOF_TRAIT
    #undef QUEST_END_DOF_TRAIT

    template<typename TDataType>
    class Dof{
        public:
            using Pointer = Dof*;
            using IndexType = std::size_t;
            using EquationIdType = std::size_t;
            using SolutionStepsDataContainerType = VariablesListDataValueContainer;

            /**
             * @brief 构造函数
             * @details 检查变量列表中是否含有该变量，不含有该变量时将其添加进入变量列表
             * @param pThisNodalData 节点数据
             * @param rThisVariable 自由度变量
             */
            template<typename TVariableType>
            Dof(NodalData* pThisNodalData, const TVariableType& rThisVariable):
                mIsFixed(false),
                mVariableType(DofTrait<TDataType, TVariableType>::Id),
                mReactionType(DofTrait<TDataType, Variable<TDataType>>::Id),
                mEquationId(IndexType()),
                mpNodalData(pThisNodalData)
            {
                QUEST_DEBUG_ERROR_IF_NOT(pThisNodalData->GetSolutionStepData().Has(rThisVariable))
                    << "The Dof-Variable " << rThisVariable.Name() << " is not "
                    << "in the list of variables" << std::endl;

                mIndex = mpNodalData->GetSolutionStepData().pGetVariablesList()->AddDof(&rThisVariable);
            }


            /**
             * @brief 构造函数
             * @details 检查变量列表中是否含有该约束变量和反力变量，不含有该变量时将其添加进入变量列表
             * @param pThisNodalData 节点数据
             * @param rThisVariable 自由度变量
             * @param rThisReaction 自由度反力变量
             */
            template<typename TVariableType, typename TReactionType>
            Dof(NodalData* pThisNodalData, const TVariableType& rThisVariable, const TReactionType& rThisReaction):
                mIsFixed(false),
                mVariableType(DofTrait<TDataType, TVariableType>::Id),
                mReactionType(DofTrait<TDataType, TReactionType>::Id),
                mEquationId(IndexType()),
                mpNodalData(pThisNodalData)
            {
                QUEST_DEBUG_ERROR_IF_NOT(pThisNodalData->GetSolutionStepData().Has(rThisVariable))
                    << "The Dof-Variable " << rThisVariable.Name() << " is not "
                    << "in the list of variables" << std::endl;

                QUEST_DEBUG_ERROR_IF_NOT(pThisNodalData->GetSolutionStepData().Has(rThisReaction))
                    << "The Dof-Reaction " << rThisReaction.Name() << " is not "
                    << "in the list of variables" << std::endl;
                
                return mpNodalData->GetSolutionStepData().pGetVariablesList()->AddDof(&rThisVariable, &rThisReaction);
            }


            /**
             * @brief 构造函数
             */
            Dof():
                mIsFixed(false),
                mVariableType(DofTrait<TDataType, Variable<TDataType>>::Id),
                mReactionType(DofTrait<TDataType, Variable<TDataType>>::Id),
                mEquationId(IndexType()),
                mIndex(),
                mpNodalData(){}


            /**
             * @brief 复制构造函数
             */
            Dof(const Dof& rOther):
                mIsFixed(rOther.mIsFixed),
                mVariableType(rOther.mVariableType),
                mReactionType(rOther.mReactionType),
                mEquationId(rOther.mEquationId),
                mIndex(rOther.mIndex),
                mpNodalData(rOther.mpNodalData){}


            /**
             * @brief 析构函数
             */
            ~Dof(){}


            /**
             * @brief 赋值运算符重载
             */
            Dof& operator=(const Dof& rOther){
                mIsFixed = rOther.mIsFixed;
                mVariableType = rOther.mVariableType;
                mReactionType = rOther.mReactionType;
                mEquationId = rOther.mEquationId;
                mIndex = rOther.mIndex;
                mpNodalData = rOther.mpNodalData;

                return *this;
            }


            /**
             * @brief 函数调用运算符重载
             * @details 返回自由度变量在指定求解步的值的引用
             * @param rThisVariable 自由度变量
             * @param SolutionStepIndex 求解步索引
             * @return 自由度变量在指定求解步的值的引用（变量类型的引用）
             */
            template<typename TVariableType>
            typename TVariableType::Type& operator()(const TVariableType& rThisVariable,IndexType SolutionStepIndex = 0){
                return GetSolutionStepValue(rThisVariable,SolutionStepIndex);
            }


            /**
             * @brief 函数调用运算符重载
             * @details 返回自由度变量在指定求解步的值的常量引用
             * @param rThisVariable 自由度变量
             * @param SolutionStepIndex 求解步索引
             * @return 自由度变量在指定求解步的值的常量引用（变量类型的引用）
             */
            template<typename TVariableType>
            const typename TVariableType::Type& operator()(const TVariableType& rThisVariable,IndexType SolutionStepIndex = 0) const{
                return GetSolutionStepValue(rThisVariable,SolutionStepIndex);
            }


            /**
             * @brief 重载输出流运算符
             * @details 通过mIndex获取变量在节点数据容器中位置，并输出变量对应的求解器的值得引用
             * @param SolutionStepIndex 求解步索引
             */
            TDataType& operator()(IndexType SolutionStepIndex = 0){
                return GetSolutionStepValue(SolutionStepIndex);
            }


            /**
             * @brief 重载输出流运算符
             * @details 通过mIndex获取变量在节点数据容器中位置，并输出变量对应的求解器的值的常量引用
             * @param SolutionStepIndex 求解步索引
             */
            const TDataType& operator()(IndexType SolutionStepIndex = 0) const{
                return GetSolutionStepValue(SolutionStepIndex);
            }


            /**
             * @brief 重载下标运算符
             * @details 通过mIndex获取变量在节点数据容器中位置，并输出变量对应的求解器的值得引用
             * @param SolutionStepIndex 求解步索引
             */
            TDataType& operator[](IndexType SolutionStepIndex){
                return GetSolutionStepValue(SolutionStepIndex);
            }


            /**
             * @brief 重载下标运算符
             * @details 通过mIndex获取变量在节点数据容器中位置，并输出变量对应的求解器的值的常量引用
             * @param SolutionStepIndex 求解步索引
             */
            const TDataType& operator[](IndexType SolutionStepIndex) const{
                return GetSolutionStepValue(SolutionStepIndex);
            }


            /**
             * @brief 基于自由度变量索引mIndex获取变量对应分析步的值的引用
             * @param SolutionStepIndex 求解步索引
             */
            TDataType& GetSolutionStepValue(IndexType SolutionStepIndex = 0){
                return GetReference(GetVariable(),mpNodalData->GetSolutionStepData(),SolutionStepIndex,mVariableType);
            }


            /**
             * @brief 基于自由度变量索引mIndex获取变量对应分析步的值的引用
             * @param SolutionStepIndex 求解步索引
             */
            const TDataType& GetSolutionStepValue(IndexType SolutionStepIndex = 0) const{
                return GetReference(GetVariable(),mpNodalData->GetSolutionStepData(),SolutionStepIndex,mVariableType);
            }


            /**
             * @brief 基于指定的变量获取变量对应分析步的值的引用
             * @param rThisVariable 变量名称
             * @param SolutionStepIndex 求解步索引
             */
            template<typename TVariableType>
            typename TVariableType::Type& GetSolutionStepValue(const TVariableType& rThisVariable,IndexType SolutionStepIndex = 0){
                return mpNodalData->GetSolutionStepData().GetValue(rThisVariable,SolutionStepIndex);
            }


            /**
             * @brief 基于指定的变量获取变量对应分析步的值的引用
             * @param rThisVariable 变量名称
             * @param SolutionStepIndex 求解步索引
             */
            template<typename TVariableType>
            const typename TVariableType::Type& GetSolutionStepValue(const TVariableType& rThisVariable,IndexType SolutionStepIndex = 0) const{
                return mpNodalData->GetSolutionStepData().GetValue(rThisVariable,SolutionStepIndex);
            }


            /**
             * @brief 基于自由度变量索引获取自由度反力对应分析步的值的引用
             * @param SolutionStepIndex 求解步索引
             */
            TDataType& GetSolutionStepReactionValue(IndexType SolutionStepIndex = 0){
                return GetReference(GetReaction(),mpNodalData->GetSolutionStepData(),SolutionStepIndex,mReactionType);
            }


            /**
             * @brief 基于自由度变量索引获取自由度反力对应分析步的值的引用
             * @param SolutionStepIndex 求解步索引
             */
            const TDataType& GetSolutionStepReactionValue(IndexType SolutionStepIndex = 0) const{
                return GetReference(GetReaction(),mpNodalData->GetSolutionStepData(),SolutionStepIndex,mReactionType);
            }


            /**
             * @brief 获取节点编号
             */
            IndexType Id() const{
                return mpNodalData->GetId();
            }


            /**
             * @brief 获取节点编号
             */
            IndexType GetId() const{
                return mpNodalData->GetId();
            }


            /**
             * @brief 基于mIndex获取自由度变量
             */
            const VariableData& GetVariable() const{
                return mpNodalData->GetSolutionStepData().GetVariablesList().GetDofVariable(mIndex);
            }


            /**
             * @brief 基于mIndex获取自由度反力变量
             */
            const VariableData& GetReaction() const{
                auto p_reaction = mpNodalData->GetSolutionStepData().GetVariablesList().pGetDofReaction(mIndex);
                return (p_reaction == nullptr)? msNone : *p_reaction;
            }


            /**
             * @brief 设置自由度变量，mIndex为自由度反力变量的对应Vector的索引
             * @param rVariable 自由度变量
             */
            template<typename TReactionType>
            void SetReaction(const TReactionType& rReaction){
                mReactionType = DofTrait<TDataType, TReactionType>::Id;
                mpNodalData->GetSolutionStepData().pGetVariablesList()->SetDofReaction(&rReaction, mIndex);
            }


            /**
             * @brief 获取自由度在总刚度矩阵中的索引
             */
            EquationIdType GetEquationId() const{
                return mEquationId;
            }


            /**
             * @brief 设置自由度在总刚度矩阵中的索引
             */
            void SetEquationId(EquationIdType EquationId){
                mEquationId = EquationId;
            }


            /**
             * @brief 设置自由度
             */
            void FixDof(){
                mIsFixed = false;
            }


            /**
             * @brief 获取该节点数据
             */
            SolutionStepsDataContainerType* GetSolutionStepsData(){
                return &(mpNodalData->GetSolutionStepData());
            }


            /**
             * @brief 设置该节点数据
             * @param pNewNodalData 新的节点数据
             */
            void SetNodalData(NodalData* pNewNodalData){
                auto p_variable = &GetVariable();
                auto p_reaction = mpNodalData->GetSolutionStepData().pGetVariablesList()->pGetDofReaction(mIndex);
                mpNodalData = pNewNodalData;
                if(p_reaction != nullptr){
                    mIndex = mpNodalData->GetSolutionStepData().pGetVariablesList()->AddDof(p_variable,p_reaction);
                } else {
                    mIndex = mpNodalData->GetSolutionStepData().pGetVariablesList()->AddDof(p_variable);
                }
            }


            /**
             * @brief 该自由度是否有反力变量
             */
            bool HasReaction() const{
                return (mpNodalData->GetSolutionStepData().pGetVariablesList()->pGetDofReaction(mIndex) != nullptr);
            }


            /**
             * @brief 该自由度是否固定
             */
            bool IsFixed() const{
                return mIsFixed;
            }


            /**
             * @brief 该自由度是否释放
             */
            bool IsFree() const{
                return !IsFixed();
            }

            std::string Info() const{
                std::stringstream buffer;

                if(IsFixed()){
                    buffer << "Fix " << GetVariable().Name() << " degree of freedom";
                } else {
                    buffer << "Free " << GetVariable().Name() << " degree of freedom";
                }

                return buffer.str();
            }

            void PrintInfo(std::ostream& rOstream) const{
                rOstream << Info();
            }

            void PrintData(std::ostream& rOstream) const{
                rOstream << "    Variables                 : " << GetVariable().Name() << std::endl;
                rOstream << "    Reaction                  : " << GetReaction().Name() << std::endl;
                if(IsFixed()){
                    rOstream << "    IsFixed                     : True" << std::endl;
                } else {
                    rOstream << "    IsFixed                     : False" << std::endl;
                }
                rOstream << "    Equation Id                 : " << mEquationId << std::endl;
            }

        protected:

        private:
            #define QUEST_MAKE_DOF_TRAIT(id) \
                case id: \
                    return rData.GetValue(static_cast<

            #define QUEST_END_DOF_TRAIT(id) \
                const&>(ThisVariable),SolutionStepIndex) \

            /**
             * @brief 获取对应求解步的变量值的引用
             * @details ThisId不为0时报错
             * @param ThisVariable 变量名称
             * @param rData 变量列表数据
             * @param SolutionStepIndex 求解步索引
             * @param ThisId 变量类型
             */
            TDataType& GetReference(const VariableData& ThisVariable,VariablesListDataValueContainer& rData,IndexType SolutionStepIndex,int ThisId){
                switch(ThisId){
                    QUEST_DOF_TRAITS
                }
                QUEST_ERROR << "Not supported type for Dof" << std::endl;
            }


            const TDataType& GetReference(const VariableData& ThisVariable,const VariablesListDataValueContainer& rData,IndexType SolutionStepIndex,int ThisId) const{
                switch(ThisId){
                    QUEST_DOF_TRAITS
                }
                QUEST_ERROR << "Not supported type for Dof" << std::endl;
            }

            
            friend class Serializer;

            void save(Serializer& rSerializer) const{
                rSerializer.save("IsFixed",static_cast<bool>(mIsFixed));
                rSerializer.save("EquationId",static_cast<EquationIdType>(mEquationId));
                rSerializer.save("NodalData",mpNodalData);
                rSerializer.save("VariableType",static_cast<int>(mVariableType));
                rSerializer.save("ReactionType",static_cast<int>(mReactionType));
                rSerializer.save("Index",static_cast<int>(mIndex));
            }

            void load(Serializer& rSerializer){
                std::string name;
                bool is_fixed;
                rSerializer.load("IsFixed",is_fixed);
                mIsFixed = is_fixed;
                EquationIdType equation_id; 
                rSerializer.load("EquationId",equation_id);
                mEquationId = equation_id;
                rSerializer.load("NodalData",mpNodalData);

                int variable_type;
                int reaction_type;
                rSerializer.load("VariableType",variable_type);
                rSerializer.load("ReactionType",reaction_type);
                mVariableType = variable_type;
                mReactionType = reaction_type;

                int index;
                rSerializer.load("Index",index);
                mIndex = index;
            }

        private:
            /**
             * @brief 空的变量，用于表示没有约束的自由度
             */
            static const Variable<TDataType> msNone;

            /**
             * @brief 用于表示自由度变量索引的最大值
             */
            static constexpr int msIsFixedPosition = 63;

            /**
             * @brief 位域，用于表示该自由度是否固定
             */
            int mIsFixed : 1;

            /**
             * @brief 位域，用于表示自由度变量的类型
             */
            int mVariableType : 4;

            /**
             * @brief 位域，用于表示自由度反力的类型
             */
            int mReactionType : 4;

            /**
             * @brief 位域，用于表示自由度在VariablesList中自由度变量的索引
             */
            int mIndex : 6;

            /**
             * @brief 位域，用于表示自由度在总刚度矩阵中的索引
             */
            EquationIdType mEquationId : 48;

            /**
             * @brief 指向该自由度所属的节点的数据的指针
             */
            NodalData* mpNodalData;

    };

    template<typename TDataType>
    const Variable<TDataType> Dof<TDataType>::msNone("NONE");

    template<typename TDataType>
    inline std::istream& operator>>(std::istream& rIStream,Dof<TDataType>& rThis){}

    template<typename TDataType>
    inline std::ostream& operator<<(std::ostream& rOStream,const Dof<TDataType>& rThis){
        rThis.PrintInfo(rOStream);
        rOStream << std::endl;
        rThis.PrintData(rOStream);

        return rOStream;
    }

    /**
     * @brief 比较运算符重载
     * @details 通过自由度变量的Key值进行比较
     */
    template<typename TDataType>
    inline bool operator > (const Dof<TDataType>& First,const Dof<TDataType>& Second){
        if(First.Id() == Second.Id()){
            return (First.GetVariable().Key() > Second.GetVariable().Key());
        }

        return (First.Id() > Second.Id());
    }


    /**
     * @brief 比较运算符重载
     * @details 通过自由度变量的Key值进行比较
     */
    template<typename TDataType>
    inline bool operator < (const Dof<TDataType>& First,const Dof<TDataType>& Second){
        if(First.Id() == Second.Id()){
            return (First.GetVariable().Key() < Second.GetVariable().Key());
        }

        return (First.Id() < Second.Id());
    }


    /**
     * @brief 比较运算符重载
     * @details 通过自由度变量的Key值进行比较
     */
    template<typename TDataType>
    inline bool operator >= (const Dof<TDataType>& First,const Dof<TDataType>& Second){ 
        if(First.Id() == Second.Id()){  
            return (First.GetVariable().Key() >= Second.GetVariable().Key());  
        }  

        return (First.Id() > Second.Id());  
    }


    /**
     * @brief 比较运算符重载
     * @details 通过自由度变量的Key值进行比较
     */
    template<typename TDataType>
    inline bool operator <= (const Dof<TDataType>& First,const Dof<TDataType>& Second){  
        if(First.Id() == Second.Id()){  
            return (First.GetVariable().Key() <= Second.GetVariable().Key());  
        }  

        return (First.Id() < Second.Id());  
    }


    /**
     * @brief 比较运算符重载
     * @details 通过自由度变量的Key值进行比较
     */
    template<typename TDataType>
    inline bool operator == (const Dof<TDataType>& First,const Dof<TDataType>& Second){
        return (First.Id() == Second.Id() && First.GetVariable().Key() == Second.GetVariable().Key());
    }

} // namespace Quest

#endif //QUEST_DOF_HPP