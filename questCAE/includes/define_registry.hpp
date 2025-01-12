#ifndef QUEST_DEFINE_REGISTRY_HPP
#define QUEST_DEFINE_REGISTRY_HPP

// 系统头文件
#include <stdexcept>
#include <sstream>

// 项目头文件
#include "includes/registry.hpp"
#include "includes/registry_item.hpp"

#define QUEST_REGISTRY_NAME_(A,B) A##B
#define QUEST_REGISTRY_NAME(A,B) QUEST_REGISTRY_NAME_(A,B)

/**
 * @brief 该宏用于注册类原型
 * @details 该宏在类中创建一个静态布尔值（其值不重要，也不应该被访问）
 * 该宏必须放置在类的私有部分，它将首先检查提供的键是否已经注册，然后它创建一个分发器函数，当第一次访问时会初始化注册对象
 */
#define QUEST_REGISTRY_ADD_PROTOTYPE(NAME, X, Y)                                         \
    static inline bool QUEST_REGISTRY_NAME(_is_registered_, __LINE__) = []() -> bool {   \
        using TFunctionType = std::function<std::shared_ptr<X>()>;                        \
        std::string key_name = NAME + std::string(".") + std::string(#Y);                 \
        if (!Registry::HasItem(key_name))                                                 \
        {                                                                                 \
            auto &r_item = Registry::AddItem<RegistryItem>(key_name);                     \
            TFunctionType dispatcher = [](){return std::make_shared<Y>();};               \
            r_item.AddItem<TFunctionType>("Prototype", std::move(dispatcher));            \
        }                                                                                 \
        return Registry::HasItem(key_name);                                               \
    }();


#define QUEST_REGISTRY_ADD_TEMPLATE_PROTOTYPE(NAME, X, Y, ...)                                            \
    static inline bool QUEST_REGISTRY_NAME(_is_registered_, __LINE__) = []() -> bool {                 \
        using TFunctionType = std::function<std::shared_ptr<X>()>;                         \
        std::string key_name = NAME + std::string(".") + std::string(#Y) + "<" + Quest::Registry::RegistryTemplateToString(__VA_ARGS__) + ">";    \
        if (!Registry::HasItem(key_name))                                                               \
        {                                                                                               \
            auto &r_item = Registry::AddItem<RegistryItem>(key_name);                                   \
            TFunctionType dispatcher = [](){return std::make_shared<Y<__VA_ARGS__>>();};                \
            r_item.AddItem<TFunctionType>("Prototype", std::move(dispatcher));                          \
        } else {                                                                                        \
        }                                                                                               \
        return Registry::HasItem(key_name);                                                             \
    }();


#endif //QUEST_DEFINE_REGISTRY_HPP