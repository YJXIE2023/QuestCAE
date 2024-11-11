/*-----------------------
model.hpp类的实现代码
-----------------------*/

// 系统头文件
#include <numeric>

// 项目头文件
#include "container/model.hpp"

namespace Quest{

    void Model::Reset(){
        mRootModelPartMap.clear();
    }


    void Model::CreateRootModelPart(const std::string& ModelPartName, ModelPart::IndexType NewBufferSize){
        auto p_var_list = Quest::make_intrusive<VariablesList>();

        ModelPart* p_model_part = new ModelPart(ModelPartName, NewBufferSize, p_var_list, *this);
        mRootModelPartMap[ModelPartName] = std::unique_ptr<ModelPart>(p_model_part);
    }


    ModelPart& Model::CreateModelPart(const std::string& ModelPartName, ModelPart::IndexType NewBufferSize){
        QUEST_TRY

        QUEST_ERROR_IF(ModelPartName.empty()) << "Please don't use empty names (\"\") when creating a ModelPart" << std::endl;

        const auto delim_pos = ModelPartName.find('.');
        const std::string& root_model_part_name = ModelPartName.substr(0, delim_pos);

        if(delim_pos == std::string::npos){
            if(mRootModelPartMap.find(root_model_part_name) == mRootModelPartMap.end()){
                CreateRootModelPart(root_model_part_name, NewBufferSize);
                return *(mRootModelPartMap[root_model_part_name].get());
            } else {
                QUEST_WARNING("Model") << "Trying to create a root modelpart with name " << ModelPartName << " however a ModelPart with the same name already exists. \nReturning the already existent ModelPart.\n";
                return *(mRootModelPartMap[root_model_part_name].get());
            }
        } else {
            if(mRootModelPartMap.find(root_model_part_name) == mRootModelPartMap.end()){
                CreateRootModelPart(root_model_part_name, NewBufferSize);
            }
            return mRootModelPartMap[root_model_part_name]->CreateSubModelPart(ModelPartName.substr(delim_pos+1));
        }
        QUEST_CATCH("")
    }


    void Model::DeleteModelPart(const std::string& rModelPartName){
        QUEST_TRY

        if(this->HasModelPart(rModelPartName)){
            const auto delim_pos = rModelPartName.find('.');
            if(delim_pos == std::string::npos){
                mRootModelPartMap.erase(rModelPartName);
            } else {
                const std::string& root_model_part_name = rModelPartName.substr(0, delim_pos);
                ModelPart& r_root_model_part = this->GetModelPart(root_model_part_name);
                r_root_model_part.RemoveSubModelPart(rModelPartName.substr(delim_pos+1));
            }
        } else {
            QUEST_WARNING("Model") << "Attempting to delete non-existent modelpart : " << rModelPartName << std::endl;
        }
        QUEST_CATCH("")
    }


    void Model::RenameModelPart(const std::string& OldName, const std::string& NewName){
        QUEST_TRY

        QUEST_ERROR_IF_NOT(this->HasModelPart(OldName)) << "The Old Name is not in model (as a root model part). Required old name was : " << OldName << std::endl;
        QUEST_ERROR_IF(this->HasModelPart(NewName)) << "The New Name is already existing in model. Proposed name was : " << NewName << std::endl;

        mRootModelPartMap[OldName]->Name() = NewName;

        CreateModelPart(NewName);

        mRootModelPartMap[NewName].swap(mRootModelPartMap[OldName]);

        mRootModelPartMap.erase(OldName);

        QUEST_CATCH("")
    }


    ModelPart& Model::GetModelPart(const std::string& rFullModelPartName){
        QUEST_TRY

        QUEST_ERROR_IF(rFullModelPartName.empty()) << "Attempting to find a ModelPart with empty name (\"\")!" << std::endl;

        const auto delim_pos = rFullModelPartName.find('.');
        const std::string& root_model_part_name = rFullModelPartName.substr(0, delim_pos);

        if(delim_pos == std::string::npos){
            auto search = mRootModelPartMap.find(root_model_part_name);
            if(search != mRootModelPartMap.end()){
                return *(search->second);
            }

            QUEST_ERROR << "The ModelPart named : " << root_model_part_name
                << "\" was not found as root-ModelPart. The total input string was \""
                << rFullModelPartName << "\"" << std::endl;
        } else {
            auto search = mRootModelPartMap.find(root_model_part_name);
            if(search != mRootModelPartMap.end()){
                ModelPart* p_model_part = (search->second).get();
                return p_model_part->GetSubModelPart(rFullModelPartName.substr(delim_pos+1));
            } else {
                QUEST_ERROR << "root model part " << rFullModelPartName << " not found" << std::endl;
            }
        }
        QUEST_CATCH("")
    }


    const ModelPart& Model::GetModelPart(const std::string& rFullModelPartName) const{
        QUEST_TRY

        QUEST_ERROR_IF(rFullModelPartName.empty()) << "Attempting to find a ModelPart with empty name (\"\")!" << std::endl;

        const auto delim_pos = rFullModelPartName.find('.');
        const std::string& root_model_part_name = rFullModelPartName.substr(0, delim_pos);

        if(delim_pos == std::string::npos){
            auto search = mRootModelPartMap.find(root_model_part_name);
            if(search != mRootModelPartMap.end()){
                return *(search->second);
            }

            QUEST_ERROR << "The ModelPart named : " << root_model_part_name
                << "\" was not found as root-ModelPart. The total input string was \""  
                << rFullModelPartName << "\"" << std::endl;
        } else {
            auto search = mRootModelPartMap.find(root_model_part_name);
            if(search != mRootModelPartMap.end()){
                const ModelPart* p_model_part = (search->second).get();
                return p_model_part->GetSubModelPart(rFullModelPartName.substr(delim_pos+1));
            } else {
                QUEST_ERROR << "root model part " << rFullModelPartName << " not found" << std::endl;
            }
        }
        QUEST_CATCH("")
    }


    bool Model::HasModelPart(const std::string& rFullModelPartName) const{
        QUEST_TRY

        QUEST_ERROR_IF(rFullModelPartName.empty()) << "Attempting to find a ModelPart with empty name (\"\")!" << std::endl;

        const auto delim_pos = rFullModelPartName.find('.');
        const std::string& root_model_part_name = rFullModelPartName.substr(0, delim_pos);

        auto search = mRootModelPartMap.find(root_model_part_name);
        if(search != mRootModelPartMap.end()){
            if(delim_pos == std::string::npos){
                return true;
            } else {
                ModelPart* p_model_part = (search->second).get();
                return p_model_part->HasSubModelPart(rFullModelPartName.substr(delim_pos+1));
            }
        } else {
            return false;
        }
        QUEST_CATCH("")
    }


    std::vector<std::string> Model::GetModelPartNames() const{
        std::vector<std::string> model_parts_names;

        for(auto& mps : mRootModelPartMap){
            const std::string& r_root_mp_name = mps.first;
            model_parts_names.push_back(r_root_mp_name);

            auto& p_root_mp = mps.second;
            if(p_root_mp->NumberOfSubModelParts() > 0){
                const std::vector<std::string>& sub_model_part_names = p_root_mp->GetSubModelPartNames();
                for(auto& r_sub_name : sub_model_part_names){
                    model_parts_names.push_back(r_root_mp_name + "." + r_sub_name);
                }

                for(auto& r_sub_mp : p_root_mp->SubModelParts()){
                    if(r_sub_mp.NumberOfSubModelParts() > 0){
                        const std::string& r_sub_name = r_sub_mp.Name();
                        const std::vector<std::string>& sub_sub_model_part_names = r_sub_mp.GetSubModelPartNames();
                        for(auto& r_sub_sub_name : sub_sub_model_part_names){
                            model_parts_names.push_back(r_root_mp_name + "." + r_sub_name + "." + r_sub_sub_name);
                        }
                    }
                }
            }
        }
        return model_parts_names;
    }


    std::string Model::Info() const{
        std::stringstream buffer;
        for(auto it = mRootModelPartMap.begin(); it != mRootModelPartMap.end(); ++it){
            buffer << *((it->second).get()) << std::endl << std::endl;
        }
        return buffer.str();
    }


    void Model::PrintInfo(std::ostream& rOStream) const{
        rOStream << Info();
    }


    void Model::PrintData(std::ostream& rOStream) const{}


    ModelPart* Model::RecursiveSearchByName(const std::string& ModelPartName, ModelPart* pModelPart) const{
        for(auto& part : pModelPart->SubModelParts()){
            if(part.Name() == ModelPartName){
                return &part;
            } else {
                ModelPart* pmodel_part = RecursiveSearchByName(ModelPartName, &part);
                if(pmodel_part != nullptr){
                    return pmodel_part;
                }
            }
        }
        return nullptr;
    }


    void Model::save(Serializer& rSerializer) const{
        std::vector<std::string> aux_names;
        aux_names.reserve(mRootModelPartMap.size());

        for(auto it = mRootModelPartMap.begin(); it != mRootModelPartMap.end(); ++it){
            aux_names.push_back(it->first);
        }

        rSerializer.save("ModelPartNames", aux_names);

        for(auto it = mRootModelPartMap.begin(); it != mRootModelPartMap.end(); ++it){
            rSerializer.save(it->first, (it->second).get());
        }
    }


    void Model::load(Serializer& rSerializer){
        std::vector<std::string> aux_names;

        rSerializer.load("ModelPartNames", aux_names);

        for(IndexType i = 0; i < aux_names.size(); ++i){
            auto dummy_list = Quest::make_intrusive<VariablesList>();
            ModelPart* p_model_part = new ModelPart(aux_names[i], 1, dummy_list, *this);
            rSerializer.load(aux_names[i], p_model_part);
            mRootModelPartMap.insert(std::make_pair(aux_names[i], std::unique_ptr<ModelPart>(p_model_part)));
        }
    }

} // namespace Quest