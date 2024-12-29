#include "includes/geometrical_object.hpp"
#include "includes/quest_flags.hpp"


namespace Quest{

    bool GeometricalObject::IsActive() const{
        return IsDefined(ACTIVE) ? Is(ACTIVE) : true;
    }

}