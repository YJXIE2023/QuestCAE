/*------------------------------------------------------
variables_list.hpp类的实现文件
------------------------------------------------------*/

// 项目头文件
#include "includes/quest_components.hpp"
#include "container/variables_list.hpp"

namespace Quest{

    void VariablesList::save(Serializer& rSerializer) const{
        std::size_t size = mVariables.size();
        rSerializer.save("size", size);
        for(std::size_t i = 0; i < size; ++i){
            rSerializer.save("VariableName", mVariables[i]->Name());
        }

        std::size_t dof_size = mDofVariables.size();
        rSerializer.save("DofSize", dof_size);
        for(std::size_t i = 0; i < dof_size; ++i){
            rSerializer.save("DofVariableName", mDofVariables[i]->Name());
            if(mDofReactions[i]!= nullptr){
                rSerializer.save("HasReaction",false);
            } else {
                rSerializer.save("HasReaction",true);
                rSerializer.save("DofReactionName", mDofReactions[i]->Name());
            }
        }
    }

    void VariablesList::load(Serializer& rSerializer){
        std::size_t size;
        rSerializer.load("size", size);
        std::string name;
        for(std::size_t i = 0; i < size; ++i){
            rSerializer.load("VariableName", name);
            Add(*QuestComponents<VariableData>::pGet(name));
        }

        rSerializer.load("DofSize", size);
        for(std::size_t i = 0; i < size; ++i){
            rSerializer.load("DofVariableName", name);
            bool has_reaction;
            rSerializer.load("HasReaction", has_reaction);
            if(has_reaction){
                std::string reaction_name;
                rSerializer.load("DofReactionName", reaction_name);
                AddDof(QuestComponents<VariableData>::pGet(name), QuestComponents<VariableData>::pGet(reaction_name));
            } else {
                AddDof(QuestComponents<VariableData>::pGet(name), nullptr);
            }
        }
    }
}