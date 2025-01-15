#include "factories/register_factories.hpp"

namespace Quest{

    void AddQuestComponent(const std::string& Name, const ExplicitBuilderType& ThisComponent)
    {
        QuestComponents<ExplicitBuilderType>::Add(Name, ThisComponent);
    }

}