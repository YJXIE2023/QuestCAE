#ifndef QUEST_REGISTER_FACTORIES_HPP
#define QUEST_REGISTER_FACTORIES_HPP

// 项目头文件
#include "includes/define.hpp"
#include "factories/factory.hpp"
#include "includes/quest_components.hpp"
#include "solving_strategies/builder_and_solvers/explicit_builder.HPP"
#include "space/ublas_space.hpp"

namespace Quest{

    using SparseSpaceType = TUblasSparseSpace<double>;
    using LocalSpaceType = TUblasDenseSpace<double>;

    using ExplicitBuilderType = ExplicitBuilder<SparseSpaceType, LocalSpaceType>;

    QUEST_API_EXTERN template class QUEST_API(QUEST_CORE) QuestComponents<ExplicitBuilderType>;
    
    void QUEST_API(QUEST_CORE) AddQuestComponent(std::string const& Name, ExplicitBuilderType const& ThisComponent);
}

#endif //QUEST_REGISTER_FACTORIES_HPP