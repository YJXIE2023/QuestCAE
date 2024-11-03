/*--------------------------------------
master_slave_constraint.hpp 头文件的实现
---------------------------------------*/

// 项目头文件
#include "includes/master_slave_constraint.hpp"
#include "includes/quest_flags.hpp"

namespace Quest{

    bool MasterSlaveConstraint::IsActive() const{
        return IsDefined(ACTIVE) ? Is(ACTIVE) : true;
    }

}