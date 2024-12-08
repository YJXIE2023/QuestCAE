/*----------------------------------------
节点类，将节点的位置和状态信息集成在一起
----------------------------------------*/

#ifndef QUEST_NODE_HPP
#define QUEST_NODE_HPP

// 系统头文件
#include <string>
#include <iostream>
#include <sstream>
#include <cstddef>
#include <atomic>

// 项目头文件
#include "includes/define.hpp"
#include "includes/lock_object.hpp"
#include "geometries/point.hpp"
#include "includes/dof.hpp"
#include "container/pointer_vector_set.hpp"
#include "container/variables_list_data_value_container.hpp"
#include "container/flags.hpp"
#include "intrusive_ptr/intrusive_ptr.hpp"
#include "container/global_pointers_vector.hpp"
#include "container/data_value_container.hpp"
#include "container/nodal_data.hpp"
#include "includes/quest_flags.hpp"

namespace Quest{

    /**
     * @class Node
     * @brief 节点类
     * @details 包含节点编号、初始位置、实际位置、节点变量数据
     */
    class Node: public Point,public Flags{
        public:
            QUEST_CLASS_INTRUSIVE_POINTER_DEFINITION(Node);

            using NodeType = Node;
            using BaseType = Point;
            using PointType = Point;
            using DofType = Dof<double>;
            using IndexType = std::size_t;
            using SizeType = std::size_t;
            using DofsContainerType = std::vector<std::unique_ptr<Dof<double>>>;
            using SolutionStepsNodalDataContainerType = VariablesListDataValueContainer;
            using BlockType = VariablesListDataValueContainer::BlockType;

        public:
            /**
             * @brief 默认构造函数
             */
            Node():
                BaseType(),
                Flags(),
                mNodalData(0),
                mDofs(),
                mData(),
                mInitialPosition(),
                mNodeLock()
            {
                CreateSolutionStepData();
            }

            /**
             * @brief 构造函数
             * @param NewId 节点编号
             */
            explicit Node(IndexType NewId):
                BaseType(),
                Flags(),
                mNodalData(NewId),
                mDofs(),
                mData(),
                mInitialPosition(),
                mNodeLock()
            {
                QUEST_ERROR << "Calling the default constructor for the node ... illegal operation!" << std::endl;
                CreateSolutionStepData();
            }

            /**
             * @brief 构造函数
             * @param NewId 节点编号
             * @param NewX 节点初始位置的X坐标
             */
            Node(IndexType NewId, const double& NewX):
                BaseType(NewX),
                Flags(),
                mNodalData(NewId),
                mDofs(),
                mData(),
                mInitialPosition(NewX),
                mNodeLock()
            {
                CreateSolutionStepData();
            }

            /**
             * @brief 构造函数
             * @param NewId 节点编号
             * @param NewX 节点初始位置的X坐标
             * @param NewY 节点初始位置的Y坐标
             */
            Node(IndexType NewId, const double& NewX, const double& NewY):
                BaseType(NewX, NewY),
                Flags(),
                mNodalData(NewId),
                mDofs(),
                mData(),
                mInitialPosition(NewX, NewY),
                mNodeLock()
            {
                CreateSolutionStepData();
            }

            /**
             * @brief 构造函数
             * @param NewId 节点编号
             * @param NewX 节点初始位置的X坐标
             * @param NewY 节点初始位置的Y坐标
             * @param NewZ 节点初始位置的Z坐标
             */
            Node(IndexType NewId, const double& NewX, const double& NewY, const double& NewZ):
                BaseType(NewX, NewY, NewZ),
                Flags(),
                mNodalData(NewId),
                mDofs(),
                mData(),
                mInitialPosition(NewX, NewY, NewZ),
                mNodeLock()
            {
                CreateSolutionStepData();
            }

            /**
             * @brief 构造函数
             * @param NewId 节点编号
             * @param rThisPoint 节点初始位置
             */
            Node(IndexType NewId, const PointType& rThisPoint):
                BaseType(rThisPoint),
                Flags(),
                mNodalData(NewId),
                mDofs(),
                mData(),
                mInitialPosition(rThisPoint),
                mNodeLock()
            {
                CreateSolutionStepData();
            }


            Node(const Node& rOtherNode) = delete;

            /**
             * @brief 构造函数
             * @param NewId 节点编号
             * @param rOtherCoordinates 节点初始位置的坐标向量
             */
            template<typename TVectorType>
            Node(IndexType NewId, const vector_expression<TVectorType>& rOtherCoordinates):
                BaseType(rOtherCoordinates),
                Flags(),
                mNodalData(NewId),
                mDofs(),
                mData(),
                mInitialPosition(rOtherCoordinates),
                mNodeLock()
            {
                CreateSolutionStepData();
            }

            /**
             * @brief 构造函数
             * @param NewId 节点编号
             * @param rOtherCoordinates 节点当前位置
             */
            Node(IndexType NewId, const std::vector<double>& rOtherCoordinates):
                BaseType(rOtherCoordinates),
                Flags(),
                mNodalData(NewId),
                mDofs(),
                mData(),
                mInitialPosition(),
                mNodeLock()
            {
                CreateSolutionStepData();
            }

            /**
             * @brief 构造函数
             * @param NewId 节点编号
             * @param NewX 节点初始位置的X坐标
             * @param NewY 节点初始位置的Y坐标
             * @param NewZ 节点初始位置的Z坐标
             * @param pVariablesList 节点变量列表
             * @param ThisData 节点变量数据
             * @param NewQueueSize 求解步大小
             */
            Node(IndexType NewId, const double& NewX, const double& NewY, const double& NewZ, VariablesList::Pointer pVariablesList, const BlockType* ThisData, SizeType NewQueueSize = 1):
                BaseType(NewX, NewY, NewZ),
                Flags(),
                mNodalData(NewId, pVariablesList, ThisData, NewQueueSize),  
                mDofs(),
                mData(),
                mInitialPosition(NewX, NewY, NewZ),
                mNodeLock()
            {}

            /**
             * @brief 克隆函数
             */
            typename Node::Pointer Clone(){
                Node::Pointer p_new_node = Quest::make_intrusive<Node>(this->Id(),(*this)[0],(*this)[1],(*this)[2]);
                p_new_node->mNodalData = this->mNodalData;

                Node::DofsContainerType& my_dofs = (this)->GetDofs();
                for(typename DofsContainerType::const_iterator it_dof = my_dofs.begin(); it_dof!= my_dofs.end(); ++it_dof){
                    p_new_node->pAddDof(**it_dof);
                }

                p_new_node->mData = this->mData;
                p_new_node->mInitialPosition = this->mInitialPosition;

                p_new_node->Set(Flags(*this));

                return p_new_node;
            }

            /**
             * @brief 析构函数
             */
            ~Node() override{
                ClearSolutionStepData();
            }


            unsigned int use_count() const noexcept{
                return mReferenceCounter;
            }

            /**
             * @brief 获取节点编号
             */
            IndexType Id() const{
                return mNodalData.Id();
            }

            /**
             * @brief 获取节点编号
             */
            IndexType GetId() const{
                return mNodalData.Id();
            }

            /**
             * @brief 设置节点编号
             */
            void SetId(IndexType NewId){
                mNodalData.SetId(NewId);
            }

            /**
             * @brief 获取节点锁
             */
            LockObject& GetLock(){
                return mNodeLock;
            }

            /**
             * @brief 设置节点锁
             */
            inline void SetLock(){
                mNodeLock.lock();
            }

            /**
             * @brief 解锁节点锁
             */
            inline void UnSetLock(){
                mNodeLock.unlock();
            }

            /**
             * @brief 赋值运算符重载
             */
            Node& operator = (const Node& rOther){
                BaseType::operator=(rOther);
                Flags::operator=(rOther);

                mNodalData = rOther.mNodalData;

                for(typename DofsContainerType::const_iterator it_dof = rOther.mDofs.begin(); it_dof!= rOther.mDofs.end(); ++it_dof){
                    pAddDof(**it_dof);
                }

                mData = rOther.mData;
                mInitialPosition = rOther.mInitialPosition;

                return *this;
            }

            /**
             * @brief 相等运算符重载
             */
            bool operator == (const Node& rOther){
                return PointType::operator == (rOther);
            }

            /**
             * @brief 函数调用运算符重载，获取节点对应求解步的数据
             * @param rThisVariable 节点变量
             * @param SolutionStepIndex 对应求解步的索引
             */
            template<typename TVariableType>
            typename TVariableType::Type& operator()(const TVariableType& rThisVariable, IndexType SolutionStepIndex){
                return GetSolutionStepValue(rThisVariable, SolutionStepIndex);
            }

            /**
             * @brief 函数调用运算符重载，获取节点最新求解步的数据
             */
            template<typename TVariableType>
            typename TVariableType::Type& operator()(const TVariableType& rThisVariable){
                return GetSolutionStepValue(rThisVariable);
            }

            /**
             * @brief 下标运算符重载，获取变量的数据
             * @param rThisVariable 节点变量
             */
            template<typename TDataType>
            TDataType& operator[](const Variable<TDataType>& rThisVariable){
                return GetValues(rThisVariable);
            }

            /**
             * @brief 下标运算符重载，获取变量的数据
             * @param rThisVariable 节点变量
             */
            template<typename TDataType>
            const TDataType& operator[](const Variable<TDataType>& rThisVariable) const{
                return GetValues(rThisVariable);
            }

            /**
             * @brief 下标运算符重载，获取求解步数据
             */
            double& operator[](IndexType ThisIndex){
                return BaseType::operator[](ThisIndex);
            }

            /**
             * @brief 下标运算符重载，获取求解步数据
             */
            double operator[](IndexType ThisIndex) const{
                return BaseType::operator[](ThisIndex);
            }

            /**
             * @brief 在结果中增加一个求解步的数据
             */
            void CreateSolutionStepData(){
                SolutionStepData().PushFront();
            }

            /**
             * @brief 克隆当前求解步的数据
             */
            void CloneSolutionStepData(){
                SolutionStepData().CloneFront();
            }

            /**
             * @brief 用目标求解步的数据覆盖指定求解步的数据
             * @param SourceSolutionStepIndex 源求解步索引
             * @param DestinationSourceSolutionStepIndex 目标求解步索引
             */
            void OverwriteSolutionStepData(IndexType SourceSolutionStepIndex, IndexType DestinationSourceSolutionStepIndex){
                SolutionStepData().AssignData(SolutionStepData().Data(SourceSolutionStepIndex), DestinationSourceSolutionStepIndex);
            }

            /**
             * @brief 清除求解步数据
             */
            void ClearSolutionStepData(){
                SolutionStepData().Clear();
            }

            /**
             * @brief 设置节点变量列表
             * @param pVariablesList 节点变量列表
             */
            void SetSolutionStepVariablesList(VariablesList::Pointer pVariablesList){
                SolutionStepData().SetVariablesList(pVariablesList);
            }

            /**
             * @brief 获取求解步数据
             */
            VariablesListDataValueContainer& SolutionStepData(){
                return mNodalData.GetSolutionStepData();
            }

            /**
             * @brief 获取求解步数据
             */
            const VariablesListDataValueContainer& SolutionStepData() const{
                return mNodalData.GetSolutionStepData();
            }

            /**
             * @brief 获取节点变量数据
             */
            DataValueContainer& GetData(){
                return mData;
            }

            /**
             * @brief 获取节点变量数据
             */
            const DataValueContainer& GetData() const{
                return mData;
            }

            /**
             * @brief 获取变量求解步数据
             * @param rThisVariable 节点变量
             */
            template<typename TVariableType>
            typename TVariableType::Type& GetSolutionStepValue(const TVariableType& rThisVariable){
                return SolutionStepData().GetValue(rThisVariable);
            }

            /**
             * @brief 获取变量求解步数据
             * @param rThisVariable 节点变量
             */
            template<typename TVariableType>
            const typename TVariableType::Type& GetSolutionStepValue(const TVariableType& rThisVariable) const{
                return SolutionStepData().GetValue(rThisVariable);
            }

            /**
             * @brief 获取变量对应分析步的数据
             * @param rThisVariable 节点变量
             * @param SolutionStepIndex 对应分析步的索引
             */
            template<typename TVariableType>
            typename TVariableType::Type& GetSolutionStepValue(const TVariableType& rThisVariable, IndexType SolutionStepIndex){
                return SolutionStepData().GetValue(rThisVariable, SolutionStepIndex);
            }

            /**
             * @brief 获取变量对应分析步的数据
             * @param rThisVariable 节点变量
             * @param SolutionStepIndex 对应分析步的索引
             */
            template<typename TVariableType>
            const typename TVariableType::Type& GetSolutionStepValue(const TVariableType& rThisVariable, IndexType SolutionStepIndex) const{
                return SolutionStepData().GetValue(rThisVariable, SolutionStepIndex);
            }

            /**
             * @brief 判断求解步变量是否存在
             */
            bool SolutionStepsDataHas(const VariableData& rThisVariable) const{
                return SolutionStepData().Has(rThisVariable);
            }

            /**
             * @brief 快速获取求解步变量数据
             * @param rThisVariable 节点变量
             */
            template<typename TVariableType>
            typename TVariableType::Type& FastGetSolutionStepValue(const TVariableType& rThisVariable){
                return SolutionStepData().FastGetValue(rThisVariable);
            }

            /**
             * @brief 快速获取求解步变量数据
             * @param rThisVariable 节点变量
             */
            template<typename TVariableType>
            const typename TVariableType::Type& FastGetSolutionStepValue(const TVariableType& rThisVariable) const{
                return SolutionStepData().FastGetValue(rThisVariable);
            }   

            /**
             * @brief 快速获取求解步变量数据
             * @param rThisVariable 节点变量
             * @param SolutionStepIndex 对应求解步的索引
             */
            template<typename TVariableType>
            typename TVariableType::Type& FastGetSolutionStepValue(const TVariableType& rThisVariable, IndexType SolutionStepIndex){
                return SolutionStepData().FastGetValue(rThisVariable, SolutionStepIndex);
            }

            /**
             * @brief 快速获取求解步变量数据
             * @param rThisVariable 节点变量
             * @param SolutionStepIndex 对应求解步的索引
             */
            template<typename TVariableType>
            const typename TVariableType::Type& FastGetSolutionStepValue(const TVariableType& rThisVariable, IndexType SolutionStepIndex) const{
                return SolutionStepData().FastGetValue(rThisVariable, SolutionStepIndex);
            }

            /**
             * @brief 快速获取求解步变量数据
             * @param rThisVariable 节点变量
             * @param SolutionStepIndex 对应求解步的索引
             * @param ThisPosition 节点位置
             */
            template<typename TVariableType>
            typename TVariableType::Type& FastGetSolutionStepValue(const TVariableType& rThisVariable, IndexType SolutionStepIndex, IndexType ThisPosition){
                return SolutionStepData().FastGetValue(rThisVariable, SolutionStepIndex, ThisPosition);
            } 

            /**
             * @brief 快速获取求解步变量数据
             * @param rThisVariable 节点变量
             * @param ThisPosition 节点位置
             */
            template<typename TVariableType>
            typename TVariableType::Type& FastGetSolutionStepValue(const TVariableType& rThisVariable, IndexType ThisPosition){
                return SolutionStepData().FastGetCurrentValue(rThisVariable, ThisPosition);
            }

            /**
             * @brief 获取变量数据
             * @param rThisVariable 节点变量
             */
            template<typename TVariableType>
            typename TVariableType::Type& GetValue(const TVariableType& rThisVariable){
                return mData.GetValue(rThisVariable);
            }

            /**
             * @brief 获取变量数据
             * @param rThisVariable 节点变量
             */
            template<typename TVariableType>    
            const typename TVariableType::Type& GetValue(const TVariableType& rThisVariable) const{
                return mData.GetValue(rThisVariable);
            }

            /**
             * @brief 获取对应求解步的变量数据
             * @param rThisVariable 节点变量
             * @param SolutionStepIndex 对应求解步的索引
             */
            template<typename TVariableType>
            typename TVariableType::Type& GetValues(const TVariableType& rThisVariable, IndexType SolutionStepIndex){
                if(!mData.Has(rThisVariable)){
                    return SolutionStepData().GetValue(rThisVariable, SolutionStepIndex);
                }

                return mData.GetValue(rThisVariable);
            }

            /**
             * @brief 获取对应求解步的变量数据
             * @param rThisVariable 节点变量
             * @param SolutionStepIndex 对应求解步的索引
             */
            template<typename TVariableType>
            const typename TVariableType::Type& GetValues(const TVariableType& rThisVariable, IndexType SolutionStepIndex) const{
                if(!mData.Has(rThisVariable)){
                    return SolutionStepData().GetValue(rThisVariable, SolutionStepIndex);
                }

                return mData.GetValue(rThisVariable);
            }

            /**
             * @brief 设置节点变量的值
             * @param rThisVariable 节点变量
             * @param rValue 节点变量的值
             */
            template<typename TVariableType>
            void SetValue(const TVariableType& rThisVariable, const typename TVariableType::Type& rValue){
                mData.SetValue(rThisVariable, rValue);
            }

            /**
             * @brief 判断节点是否有指定变量
             * @param rThisVariable 节点变量
             */
            template<typename TDataType>
            bool Has(const Variable<TDataType>& rThisVariable) const{
                return mData.Has(rThisVariable);
            }

            /**
             * @brief 设置节点约束
             * @param rDofVariable 节点约束变量
             */
            template<typename TVariableType>
            inline void Fix(const TVariableType& rDofVariable){
                for(auto it_dof = mDofs.begin(); it_dof != mDofs.end(); ++it_dof){
                    if((*it_dof)->GetVariable() == rDofVariable){
                        (*it_dof)->FixDof();
                        return;
                    }
                }

                #ifdef QUEST_DEBUG
                    if(OpenMPUtils::IsInParallel() != 0){
                        QUEST_ERROR << "Attempting to Fix the variable: " << rDofVariable << " within a parallel region. This is not permitted. Create the Dof first by pAddDof" << std::endl;
                    }
                #endif

                pAddDof(rDofVariable)->FixDof();
            }

            /**
             * @brief 解除节点约束
             * @param rDofVariable 节点约束变量
             */
            template<typename TVariableType>
            inline void Free(const TVariableType& rDofVariable){
                for(auto it_dof = mDofs.begin(); it_dof != mDofs.end(); ++it_dof){
                    if((*it_dof)->GetVariable() == rDofVariable){
                        (*it_dof)->FreeDof();
                        return;
                    }
                }

                #ifdef QUEST_DEBUG
                    if(OpenMPUtils::IsInParallel() != 0){
                        QUEST_ERROR << "Attempting to Free the variable: " << rDofVariable << " within a parallel region. This is not permitted. Create the Dof first by pAddDof" << std::endl;
                    }
                #endif

                pAddDof(rDofVariable)->FreeDof();
            }

            /**
             * @brief 获取求解步数量
             */
            IndexType GetBufferSize() const{
                return SolutionStepData().QueueSize();
            }

            /**
             * @brief 设置求解步数量
             */
            void SetBufferSize(IndexType NeBufferSize){
                SolutionStepData().Resize(NeBufferSize);
            }

            /**
             * @brief 获取节点初始位置
             */
            const PointType& GetInitialPosition() const{
                return mInitialPosition;
            }

            /**
             * @brief 获取节点初始位置
             */
            PointType& GetInitialPosition(){
                return mInitialPosition;
            }

            /**
             * @brief 获取节点X坐标
             */
            double& X0(){
                return mInitialPosition.X();
            }

            /**
             * @brief 获取节点Y坐标
             */
            double& Y0(){
                return mInitialPosition.Y();
            }

            /**
             * @brief 获取节点Z坐标
             */
            double& Z0(){
                return mInitialPosition.Z();
            }

            /**
             * @brief 获取节点X坐标
             */
            double X0() const{
                return mInitialPosition.X();
            }

            /**
             * @brief 获取节点Y坐标
             */
            double Y0() const{
                return mInitialPosition.Y();
            }

            /**
             * @brief 获取节点Z坐标
             */
            double Z0() const{
                return mInitialPosition.Z();
            }

            /**
             * @brief 设置节点初始位置
             * @param NewInitialPosition 节点初始位置
             */
            void SetInitialPosition(const PointType& NewInitialPosition){
                mInitialPosition.X() = NewInitialPosition.X();
                mInitialPosition.Y() = NewInitialPosition.Y();
                mInitialPosition.Z() = NewInitialPosition.Z();
            }

            /**
             * @brief 设置节点初始位置
             * @param NewInitialPosition 节点初始位置
             */
            void SetInitialPosition(double NewX, double NewY, double NewZ){
                mInitialPosition.X() = NewX;
                mInitialPosition.Y() = NewY;
                mInitialPosition.Z() = NewZ;
            }

            /**
             * @brief 获取节点求解步变量列表
             */
            VariablesList::Pointer pGetVariablesList(){
                return SolutionStepData().pGetVariablesList();
            }

            /**
             * @brief 获取节点求解步变量列表
             */
            const VariablesList::Pointer pGetVariablesList() const{
                return SolutionStepData().pGetVariablesList();
            }

            /**
             * @brief 获取自由度变量的位置
             * @param rDofVariable 自由度变量
             */
            template<typename TVariableType>
            inline unsigned int GetDofPosition(const TVariableType& rDofVariable) const{
                typename DofsContainerType::const_iterator it_dof = mDofs.end();
                for(it_dof = mDofs.begin(); it_dof != mDofs.end(); ++it_dof){
                    if((*it_dof)->GetVariable() == rDofVariable){
                        break;
                    }
                }

                return it_dof - mDofs.begin();
            }

            /**
             * @brief 获得给定位置的自由度
             * @param rDofVariable 自由度变量
             * @param pos 自由度位置
             */
            template<typename TVariableType>
            inline const DofType& GetDof(const TVariableType& rDofVariable, int pos) const{
                typename DofsContainerType::const_iterator it_begin = mDofs.begin();
                typename DofsContainerType::const_iterator it_end = mDofs.end();
                typename DofsContainerType::const_iterator it;
                if(pos < it_end - it_begin){
                    it = it_begin + pos;
                    if((*it)->GetVariable() == rDofVariable){
                        return **it;
                    }
                }

                for(auto it_dof = mDofs.begin(); it_dof != mDofs.end(); ++it_dof){
                    if((*it_dof)->GetVariable() == rDofVariable){
                        return **it_dof;
                    }
                }

                QUEST_ERROR << "Non-existent DOF in node #" << Id() << " for variable: " << rDofVariable.Name() << std::endl;
            }

            /**
             * @brief 获得给定自由度变量的自由度
             * @param rDofVariable 自由度变量
             */
            template<typename TVariableType>
            inline const DofType& GetDof(const TVariableType& rDofVariable) const{
                for(auto it_dof = mDofs.begin(); it_dof != mDofs.end(); ++it_dof){
                    if((*it_dof)->GetVariable() == rDofVariable){
                        return **it_dof;
                    }
                }

                QUEST_ERROR << "Non-existent DOF in node #" << Id() << " for variable: " << rDofVariable.Name() << std::endl;
            }

            /**
             * @brief 获得节点所有自由度
             */
            DofsContainerType& GetDofs(){
                return mDofs;
            }

            /**
             * @brief 获得节点所有自由度
             */
            const DofsContainerType& GetDofs() const{
                return mDofs;
            }

            /**
             * @brief 获得变量对应的自由度指针
             * @param rDofVariable 自由度变量
             */
            template<typename TVariableType>
            inline typename DofType::Pointer pGetDof(const TVariableType& rDofVariable) const{
                for(auto it_dof = mDofs.begin(); it_dof != mDofs.end(); ++it_dof){
                    if((*it_dof)->GetVariable() == rDofVariable){
                        return (*it_dof).get();
                    }
                }

                QUEST_ERROR << "Non-existent DOF in node #" << Id() << " for variable: " << rDofVariable.Name() << std::endl;
            }

            /**
             * @brief 获得变量对应的自由度指针
             * @param rDofVariable 自由度变量
             * @param pos 自由度变量的位置
             */
            template<typename TVariableType>
            inline typename DofType::Pointer pGetDof(const TVariableType& rDofVariable, int pos) const{
                const auto it_begin = mDofs.begin();
                const auto it_end = mDofs.end();

                if(pos < it_end - it_begin){
                    auto it = it_begin + pos;
                    if((*it)->GetVariable() == rDofVariable){
                        return (*it).get();
                    }
                }

                for(auto it_dof = it_begin; it_dof != it_end; ++it_dof){
                    if((*it_dof)->GetVariable() == rDofVariable){
                        return (*it_dof).get();
                    }
                }

                QUEST_ERROR << "Non-existent DOF in node #" << Id() << " for variable: " << rDofVariable.Name() << std::endl;
            }

            /**
             * @brief 向节点添加一个自由度并且返回新添加的自由度指针或已存在的自由度指针
             */
            template<typename TVariableType>
            inline typename DofType::Pointer pAddDof(const TVariableType& rDofVariable){
                QUEST_TRY
                
                for(auto it_dof = mDofs.begin(); it_dof != mDofs.end(); ++it_dof){
                    if((*it_dof)->GetVariable() == rDofVariable){
                        return (*it_dof).get();
                    }
                }

                mDofs.push_back(Quest::make_unique<DofType>(&mNodalData, rDofVariable));

                DofType* p_new_dof = mDofs.back().get();

                SortDofs();

                return p_new_dof;

                QUEST_CATCH(*this);
            }

            /**
             * @brief 向节点添加一个自由度并且返回新添加的自由度指针或已存在的自由度指针
             */
            inline typename DofType::Pointer pAddDof(const DofType& SourceDof){
                QUEST_TRY

                for(auto it_dof = mDofs.begin(); it_dof != mDofs.end(); ++it_dof){
                    if((*it_dof)->GetVariable() == SourceDof.GetVariable()){
                        if((*it_dof)->GetReaction() != SourceDof.GetReaction()){
                            **it_dof = SourceDof;
                            (*it_dof)->SetNodalData(&mNodalData);
                        }
                        return (*it_dof).get();
                    }
                }

                mDofs.push_back(Quest::make_unique<DofType>(SourceDof));
                mDofs.back()->SetNodalData(&mNodalData);

                DofType* p_new_dof = mDofs.back().get();
                SortDofs();

                return p_new_dof;

                QUEST_CATCH(*this);
            }

            /**
             * @brief 向节点添加一个自由度并且返回新添加的自由度指针或已存在的自由度指针
             */
            template<typename TVariableType, typename TReactionType>
            inline typename DofType::Pointer pAddDof(const TVariableType& rDofVariable, const TReactionType& rDofReaction){
                QUEST_TRY

                for(auto it_dof = mDofs.begin(); it_dof != mDofs.end(); ++it_dof){
                    if((*it_dof)->GetVariable() == rDofVariable){
                        (*it_dof)->SetReaction(rDofReaction);
                        return (*it_dof).get();
                    }
                }

                mDofs.push_back(Quest::make_unique<DofType>(&mNodalData, rDofVariable, rDofReaction));

                DofType* p_new_dof = mDofs.back().get();

                SortDofs();

                return p_new_dof;

                QUEST_CATCH(*this);
            }

            /**
             * @brief 向节点添加一个自由度并且返回新添加的自由度或已存在的自由度
             */
            template<typename TVariableType>
            inline DofType& AddDof(const TVariableType& rDofVariable){
                QUEST_TRY

                for(auto it_dof = mDofs.begin(); it_dof != mDofs.end(); ++it_dof){
                    if((*it_dof)->GetVariable() == rDofVariable){
                        return **it_dof;
                    }
                }

                mDofs.push_back(Quest::make_unique<DofType>(&mNodalData, rDofVariable));

                DofType* p_new_dof = mDofs.back().get();

                SortDofs();

                return *p_new_dof;

                QUEST_CATCH(*this);
            }

            /**
             * @brief 向节点添加一个自由度并且返回新添加的自由度或已存在的自由度
             */
            template<typename TVariableType, typename TReactionType>
            inline DofType& AddDof(const TVariableType& rDofVariable, const TReactionType& rDofReaction){
                QUEST_TRY

                for(auto it_dof = mDofs.begin(); it_dof != mDofs.end(); ++it_dof){
                    if((*it_dof)->GetVariable() == rDofVariable){
                        (*it_dof)->SetReaction(rDofReaction);
                        return **it_dof;
                    }
                }

                mDofs.push_back(Quest::make_unique<DofType>(&mNodalData, rDofVariable, rDofReaction));

                DofType* p_new_dof = mDofs.back().get();

                SortDofs(); 

                return *p_new_dof;  

                QUEST_CATCH(*this);
            }

            /**
             * @brief 判断节点是否有指定变量的自由度
             */
            inline bool HasDofFor(const VariableData& rDofVariable) const{
                for(auto it_dof = mDofs.begin(); it_dof != mDofs.end(); ++it_dof){
                    if((*it_dof)->GetVariable() == rDofVariable){
                        return true;
                    }
                }

                return false;
            }

            /**
             * @brief 判断该自由度变量是否固定
             */
            inline bool IsFixed(const VariableData& rDofVariable) const{
                for(auto it_dof = mDofs.begin(); it_dof != mDofs.end(); ++it_dof){
                    if((*it_dof)->GetVariable() == rDofVariable){
                        return (*it_dof)->IsFixed();
                    }
                }

                return false;
            }

            /**
             * @brief 判断节点是否被激活
             */
            inline bool IsActive() const{
                return IsDefined(ACTIVE) ? Is(ACTIVE) : true;
            }


            std::string Info() const override{
                std::stringstream buffer;
                buffer << "Node #" << Id();
                return buffer.str();
            }


            void PrintInfo(std::ostream& rOstream) const override{
                rOstream << Info();
            }


            void PrintData(std::ostream& rOstream) const override{
                BaseType::PrintData(rOstream);
                if(!mDofs.empty()){
                    rOstream << std::endl << "    Dofs :" << std::endl;
                }
                for(typename DofsContainerType::const_iterator i = mDofs.begin(); i != mDofs.end(); ++i){
                    rOstream << "        " << (*i)->Info() << std::endl;
                }
            }


        protected:

        private:
            friend void intrusive_ptr_add_ref(const NodeType* x){
                x->mReferenceCounter.fetch_add(1, std::memory_order_relaxed);
            }

            friend void intrusive_ptr_release(const NodeType* x){
                if(x->mReferenceCounter.fetch_sub(1, std::memory_order_release) == 1){
                    std::atomic_thread_fence(std::memory_order_acquire);
                    delete x;
                }
            }


            /**
             * @brief 对自由度变量进行排序
             */
            void SortDofs(){
                std::sort(mDofs.begin(), mDofs.end(), [](const Quest::unique_ptr<DofType>& First, const Quest::unique_ptr<DofType>& Second)->bool{
                    return First->GetVariable().Key() < Second->GetVariable().Key();
                });
            }


            friend class Serializer;

            void save(Serializer& rSerializer) const override{
                QUEST_SERIALIZE_SAVE_BASE_CLASS(rSerializer, Point);
                QUEST_SERIALIZE_SAVE_BASE_CLASS(rSerializer, Flags);
                rSerializer.save("NodelData", &mNodalData);
                rSerializer.save("Data", mData);
                rSerializer.save("Initial Position", mInitialPosition);
                rSerializer.save("Data", mDofs);
            }


            void load(Serializer& rSerializer) override{
                QUEST_SERIALIZE_LOAD_BASE_CLASS(rSerializer, Point);
                QUEST_SERIALIZE_LOAD_BASE_CLASS(rSerializer, Flags);
                NodalData* p_nodal_data = &mNodalData;
                rSerializer.load("NodelData", p_nodal_data);
                rSerializer.load("Data", mData);
                rSerializer.load("Initial Position", mInitialPosition);
                rSerializer.load("Data", mDofs);
            }

        private:
            /**
             * @brief 节点数据
             */
            NodalData mNodalData;

            /**
             * @brief 存储节点自由度变量的容器
             * @details std::vector<std::unique_ptr<Dof<double>>> mDofs;
             */
            DofsContainerType mDofs;

            /**
             * @brief 存储节点其他变量值（值不随求解步变化）的容器
             */
            DataValueContainer mData;

            /**
             * @brief 节点初始位置
             */
            PointType mInitialPosition;

            /**
             * @brief 节点锁
             */
            LockObject mNodeLock;
            mutable std::atomic<int> mReferenceCounter{0};

    };

    inline std::istream& operator >> (std::istream& rIstream, Node& rThis);

    inline std::ostream& operator << (std::ostream& rOstream, const Node& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }

} // namespace Quest

#endif //QUEST_NODE_HPP