/*------------------------------
定义一系列用于状态管理的全局标志
------------------------------*/

#ifndef QUEST_QUEST_FLAGS_HPP
#define QUEST_QUEST_FLAGS_HPP

// 项目头文件
#include "includes/define.hpp"
#include "container/flags.hpp"
#include "includes/quest_components.hpp"

namespace Quest{

    QUEST_CREATE_FLAG(STRUCTURE,       63);
    QUEST_CREATE_FLAG(FLUID,           62);
    QUEST_CREATE_FLAG(THERMAL,         61);
    QUEST_CREATE_FLAG(VISITED,         60);
    QUEST_CREATE_FLAG(SELECTED,        59);
    QUEST_CREATE_FLAG(BOUNDARY,        58);
    QUEST_CREATE_FLAG(INLET,           57);
    QUEST_CREATE_FLAG(OUTLET,          56);
    QUEST_CREATE_FLAG(SLIP,            55);
    QUEST_CREATE_FLAG(INTERFACE,       54);
    QUEST_CREATE_FLAG(CONTACT,         53);
    QUEST_CREATE_FLAG(TO_SPLIT,        52);
    QUEST_CREATE_FLAG(TO_ERASE,        51);
    QUEST_CREATE_FLAG(TO_REFINE,       50);
    QUEST_CREATE_FLAG(NEW_ENTITY,      49);
    QUEST_CREATE_FLAG(OLD_ENTITY,      48);
    QUEST_CREATE_FLAG(ACTIVE,          47);
    QUEST_CREATE_FLAG(MODIFIED,        46);
    QUEST_CREATE_FLAG(RIGID,           45);
    QUEST_CREATE_FLAG(SOLID,           44);
    QUEST_CREATE_FLAG(MPI_BOUNDARY,    43);
    QUEST_CREATE_FLAG(INTERACTION,     42);
    QUEST_CREATE_FLAG(ISOLATED,        41);
    QUEST_CREATE_FLAG(MASTER,          40);
    QUEST_CREATE_FLAG(SLAVE,           39);
    QUEST_CREATE_FLAG(INSIDE,          38);
    QUEST_CREATE_FLAG(FREE_SURFACE,    37);
    QUEST_CREATE_FLAG(BLOCKED,         36);
    QUEST_CREATE_FLAG(MARKER,          35);
    QUEST_CREATE_FLAG(PERIODIC,        34);
    QUEST_CREATE_FLAG(WALL,            33);

    const Flags ALL_DEFINED(Flags::AllDefined());
    const Flags ALL_TRUE(Flags::AllTrue());

} // namespace Quest

#endif //QUEST_QUEST_FLAGS_HPP