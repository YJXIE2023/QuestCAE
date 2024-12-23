#ifndef QUEST_MODEL_HPP
#define QUEST_MODEL_HPP

// 系统头文件
#include <string>
#include <iostream>
#include <unordered_map>

// 项目头文件
#include "includes/define.hpp"
#include "includes/model_part.hpp"

namespace Quest{

    /**
     * @class Model
     * @brief 用于管理多物理场模拟中不同的模型部件
     * @details 该类作为不同模型部件的管理器，利用映射来管理变量和模型部件
     */
    class QUEST_API(QUEST_CORE) Model final{
        public:
            using IndexType = ModelPart::IndexType;
            
            QUEST_CLASS_POINTER_DEFINITION(Model);

        public:
            /**
             * @brief 构造函数
             */
            Model(){}

            /**
             * @brief 析构函数
             */
            ~Model(){
                mRootModelPartMap.clear();
            }

            /**
             * @brief 拷贝构造函数
             */
            Model(const Model& rOther) = delete;

            /**
             * @brief 赋值运算符重载
             */
            Model& operator = (const Model& rOther) = delete;

            /**
             * @brief 清空模型部件的数据库
             * @details 对模型部件映射执行清空操作
             */
            void Reset();

            /**
             * @brief 在当前模型中创建一个具有指定名称和缓冲区大小的新模型部件
             * @param ModelPartName 要创建的新模型部件的名称
             * @param NewBufferSize 新模型部件的缓冲区大小
             */
            ModelPart& CreateModelPart(const std::string& ModelPartName, IndexType NewBufferSize = 1);

            /**
             * @brief 删除指定名称的模型部件
             * @param ModelPartName 要删除的模型部件的名称
             */
            void DeleteModelPart(const std::string& ModelPartName);

            /**
             * @brief 重命名指定名称的模型部件
             * @param OldName 要重命名的模型部件的旧名称
             * @param NewName 要重命名的模型部件的新名称
             */
            void RenameModelPart(const std::string& OldName, const std::string& NewName);

            /**
             * @brief 获取指定名称的模型部件
             * @param rFullModelPartName 要获取的模型部件的完整名称
             */
            ModelPart& GetModelPart(const std::string& rFullModelPartName);

            /**
             * @brief 获取指定名称的模型部件
             * @param rFullModelPartName 要获取的模型部件的完整名称
             */
            const ModelPart& GetModelPart(const std::string& rFullModelPartName) const;

            /**
             * @brief 判断是否存在指定名称的模型部件
             * @param rFullModelPartName 要判断的模型部件的完整名称
             */
            bool HasModelPart(const std::string& rFullModelPartName) const;

            /**
             * @brief 获取当前模型中所有模型部件的名称
             */
            std::vector<std::string> GetModelPartNames() const;


            std::string Info() const;


            void PrintInfo(std::ostream& rOstream) const;


            void PrintData(std::ostream& rOstream) const;

        protected:

        private:
            /**
             * @brief 此方法在模型部件中递归搜索子模型部件
             * @param rModelPartName 要搜索的名称
             * @param pModelPart 指向递归搜索的模型部件的指针
             * @return 指向目标模型部件的指针
             */
            ModelPart* RecursiveSearchByName(const std::string& rModelPartName, ModelPart* pModelPart) const;

            /**
             * @brief 此方法使用 "." 分隔模型部件的名称以定义层级结构
             * @param rFullModelPartName 包含完整层级结构的名称
             * @return 包含名称中每个部分的向量，用于定义模型部件的层级结构
             */
            std::vector<std::string> SplitSubModelPartHierarchy(const std::string& rFullModelPartName) const;

            /**
             * @brief 此方法在当前模型中创建一个具有指定名称和缓冲区大小的新模型部件
             * @param ModelPartName 要创建的新模型部件的名称
             * @param NewBufferSize 新创建的模型部件的缓冲区大小
             */
            void CreateRootModelPart(const std::string& ModelPartName, ModelPart::IndexType NewBufferSize);


            friend class Serializer;


            void save(Serializer& rSerializer) const;


            void load(Serializer& rSerializer);

        private:
            /**
             * @brief 存储所有根模型部件的map
             */
            std::map<std::string, std::unique_ptr<ModelPart>> mRootModelPartMap;

    };


    inline std::istream& operator >> (std::istream& rIstream, Model& rThis);


    inline std::ostream& operator << (std::ostream& rOstream, const Model& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }

} // namespace Quest

#endif //QUEST_MODEL_HPP