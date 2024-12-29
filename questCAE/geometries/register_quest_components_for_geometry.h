#ifndef QUEST_REGISTER_COMPONENTS_FOR_GEOMETRY_H
#define QUEST_REGISTER_COMPONENTS_FOR_GEOMETRY_H

// 项目头文件
#include "geometries/geometry.hpp"
#include "includes/node.hpp"

namespace Quest{

    QUEST_API_EXTERN template class QUEST_API(QUEST_CORE) QuestComponents<Geometry<Node>>;

    void QUEST_API(QUEST_CORE) AddQuestComponent(const std::string& Name, const Geometry<Node>& ThisComponent);
}

#endif //QUEST_REGISTER_COMPONENTS_FOR_GEOMETRY_H