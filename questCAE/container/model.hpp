/*----------------------------------------
负责管理不同的模型部件（model parts）
通过维护无序映射来管理仿真中的变量和模型部件
----------------------------------------*/

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

    class QUEST_API(QUEST_CORE) Model final{
        public:
            using IndexType = ModelPart::IndexType;
            
            QUEST_CLASS_POINTER_DEFINITION(Model);

        public:
            Model(){}


            ~Model(){
                mRootModelPartMap.clear();
            }


            Model(const Model& rOther) = delete;


            Model& operator = (const Model& rOther) = delete;


            void Reset();


            ModelPart& CreateModelPart(const std::string& ModelPartName, IndexType NewBufferSize = 1);


            void DeleteModelPart(const std::string& ModelPartName);


            void RenameModelPart(const std::string& OldName, const std::string& NewName);


            ModelPart& GetModelPart(const std::string& rFullModelPartName);


            const ModelPart& GetModelPart(const std::string& rFullModelPartName) const;


            bool HasModelPart(const std::string& rFullModelPartName) const;


            std::vector<std::string> GetModelPartNames() const;


            std::string Info() const;


            void PrintInfo(std::ostream& rOstream) const;


            void PrintData(std::ostream& rOstream) const;

        protected:

        private:
            ModelPart* RecursiveSearchByName(const std::string& rModelPartName, ModelPart* pModelPart) const;


            std::vector<std::string> SplitSubModelPartHierarchy(const std::string& rFullModelPartName) const;


            void CreateRootModelPart(const std::string& ModelPartName, ModelPart::IndexType NewBufferSize);


            friend class Serializer;


            void save(Serializer& rSerializer) const;


            void load(Serializer& rSerializer);

        private:
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