#ifndef QUEST_NODAL_DATA_HPP
#define QUEST_NODAL_DATA_HPP

// 系统头文件
#include <iostream>

// 项目头文件
#include "container/variables_list_data_value_container.hpp"


namespace Quest{

    
    class QUEST_API(QUEST_CORE) NodalData{
        public:
            QUEST_CLASS_POINTER_DEFINITION(NodalData);

            using IndexType = std::size_t;
            using SizeType = std::size_t;
            using SolutionStepsNodalDataContainerType = VariablesListDataValueContainer;
            using BlockType = VariablesListDataValueContainer::BlockType;

            /**
             * @brief 默认构造函数
             * @param TheId 节点编号
             */
            NodalData(IndexType TheId);


            /**
             * @brief 构造函数
             * @param TheId 节点编号
             * @param pVariablesList 变量列表
             * @param NewQueueSize 分析步数量
             */
            NodalData(IndexType TheId, VariablesList::Pointer pVariablesList, SizeType NewQueueSize = 1);

            
            /**
             * @brief 构造函数
             * @param TheId 节点编号
             * @param pVariablesList 变量列表
             * @param ThisData 指向节点所有数据的数据块的头指针
             * @param NeQueueSize 分析步数量
             */
            NodalData(IndexType TheId, VariablesList::Pointer pVariablesList, const BlockType* ThisData, SizeType NeQueueSize = 1);


            /**
             * @brief 析构函数
             */
            ~NodalData(){}


            /**
             * @brief 赋值运算符
             */
            NodalData& operator = (const NodalData& rOther);


            /**
             * @brief 获取节点编号
             */
            IndexType Id() const{
                return mId;
            }


            /**
             * @brief 获取节点编号
             */
            IndexType GetId() const{
                return mId;
            }


            /**
             * @brief 设置节点编号
             */
            void SetId(IndexType NewId){
                mId = NewId;
            }


            /**
             * @brief 设置节点数据存储容器
             * @param TheData 节点数据存储容器
             */
            void SetSolutionStepData(const SolutionStepsNodalDataContainerType& TheData){
                mSolutionStepsNodalData = TheData;
            }


            /**
             * @brief 获取所有分析步的数据存储容器的引用
             */
            VariablesListDataValueContainer& GetSolutionStepData(){
                return mSolutionStepsNodalData;
            }


            /**
             * @brief 获取所有分析步的数据存储容器的常量引用
             */
            const VariablesListDataValueContainer& GetSolutionStepData() const{
                return mSolutionStepsNodalData;
            }

            std::string Info() const;

            void PrintInfo(std::ostream& rOstream) const;

            void PrintData(std::ostream& rOstream) const;

        protected:

        private:
            friend class Serializer;

            void save(Serializer& rSerializer) const;
            void load(Serializer& rSerializer);

            /**
             * @brief 默认构造函数
             */
            NodalData(): mId(0), mSolutionStepsNodalData(){}

            /**
             * @brief 复制构造函数
             */
            NodalData(const NodalData& rOther);

        private:
            /**
             * @brief 节点编号
             */
            IndexType mId;

            /**
             * @brief 存储节点每个分析步的数据的容器
             */
            SolutionStepsNodalDataContainerType mSolutionStepsNodalData;

    };

    inline std::istream& operator >> (std::istream& rIstream, NodalData& rThis){}

    inline std::ostream& operator << (std::ostream& rOstream, const NodalData& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }

} // namespace Quest

#endif //QUEST_NODAL_DATA_HPP