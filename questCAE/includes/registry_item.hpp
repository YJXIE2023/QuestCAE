#ifndef QUEST_REGISTRY_ITEM_HPP
#define QUEST_REGISTRY_ITEM_HPP

// 系统头文件
#include <string>
#include <iostream>
#include <unordered_map>
#include <any>

// 项目头文件
#include "includes/define.hpp"

namespace Quest{

    /**
     * @class RegistryItem
     * @brief 注册表项类
     * @details 注册表项类是注册表的基本单元，为一个树状节点结构，可以存储与该项相关的数据，包括其名称、一个可选的值和一个无序集合来存储其子数据
     */
    class QUEST_API(QUEST_CORE) RegistryItem{
        public:
            QUEST_CLASS_POINTER_DEFINITION(RegistryItem);

            using SubRegistryItemType = std::unordered_map<std::string, Quest::shared_ptr<RegistryItem>>;
            using SubRegistryItemPointerType = Quest::shared_ptr<SubRegistryItemType>;

            /**
             * @brief 迭代器类型
             * @details 用于遍历std::unordered_map<std::string, Quest::shared_ptr<RegistryItem>>的所有键
             */
            class KeyReturnConstIterator{
                public:
                    using BaseIterator          = SubRegistryItemType::const_iterator;
                    using iterator_category     = std::forward_iterator_tag;
                    using difference_type       = BaseIterator::difference_type;
                    using value_type            = SubRegistryItemType::key_type;
                    using const_pointer         = const value_type*;
                    using const_reference       = const value_type&;

                    KeyReturnConstIterator() {}

                    KeyReturnConstIterator(const BaseIterator Iterator): mIterator(Iterator) {}

                    KeyReturnConstIterator(const KeyReturnConstIterator& rIterator): mIterator(rIterator.mIterator) {}

                    KeyReturnConstIterator& operator = (const KeyReturnConstIterator& rIterator){
                        this->mIterator = rIterator.mIterator;
                        return *this;
                    }

                    const_reference operator*() const{
                        return mIterator->first;
                    }

                    const_pointer operator->() const{
                        return &(mIterator->first);
                    }

                    KeyReturnConstIterator& operator++(){
                        ++mIterator;
                        return *this;
                    }

                    KeyReturnConstIterator operator++(int){
                        KeyReturnConstIterator temp(*this);
                        ++mIterator;
                        return temp;
                    }

                    bool operator == (const KeyReturnConstIterator& rIterator) const{
                        return this->mIterator == rIterator.mIterator;
                    }

                    bool operator!= (const KeyReturnConstIterator& rIterator) const{
                        return this->mIterator != rIterator.mIterator;
                    }

                protected:

                private:
                    BaseIterator mIterator;
            };


            /**
             * @brief 默认构造函数
             */
            RegistryItem() = delete;


            /**
             * @brief 构造函数
             */
            RegistryItem(const std::string& rName): mName(rName) ,mpValue(Quest::make_shared<SubRegistryItemType>()),
                mGetValueStringMethod(&RegistryItem::GetRegistryItemType){}


            /**
             * @brief 构造函数
             * @param rValue 指向std::function的可调用对象，可调用对象参数值可变，返回值为std::shared_ptr<TItemType>
             */
            template<typename TItemType, typename... TArgs>
            RegistryItem(const std::string& rName, const std::function<std::shared_ptr<TItemType>(TArgs...)> &rValue):
                mName(rName), mpValue(rValue()), mGetValueStringMethod(&RegistryItem::GetItemString<TItemType>){}


            /**
             * @brief 构造函数
             */
            template<typename TItemType>
            RegistryItem(const std::string& rName, const TItemType& rValue): mName(rName), mpValue(Quest::make_shared<TItemType>(rValue)),
                mGetValueStringMethod(&RegistryItem::GetItemString<TItemType>){}


            /**
             * @brief 构造函数
             */
            template<typename TItemType>
            RegistryItem(const std::string& rName, const std::shared_ptr<TItemType>& rValue): mName(rName), mpValue(rValue),
                mGetValueStringMethod(&RegistryItem::GetItemString<TItemType>){}


            /**
             * @brief 复制构造函数
             */
            RegistryItem(const RegistryItem& rItem) = delete;


            /**
             * @brief 析构函数
             */
            ~RegistryItem() = default;


            /**
             * @brief 赋值运算符
             */
            RegistryItem& operator = (RegistryItem& rOther) = delete;


            /**
             * @brief 向子项中添加一个注册项
             */
            template<typename TItemType,typename... TArgumentsList>
            RegistryItem& AddItem(const std::string& ItemName, TArgumentsList&&... Arguments){
                QUEST_ERROR_IF(this->HasItem(ItemName))
                    << "The RegistryItem '" << this->Name() << "' already has an item named '" << ItemName << "'." << std::endl;

                //如果 TItemType 是 RegistryItem 类型，则条目的类型将是 SubRegistryItemFunctor；否则，条目的类型将是 SubValueItemFunctor<TItemType>
                using ValueType = typename std::conditional<std::is_same<TItemType,RegistryItem>::value, SubRegistryItemFunctor, SubValueItemFunctor<TItemType>>::type;

                auto insert_result = GetSubRegistryItemMap().emplace(
                    std::make_pair(ItemName, ValueType::Create(ItemName, std::forward<TArgumentsList>(Arguments)...))
                );

                QUEST_ERROR_IF_NOT(insert_result.second)
                    << "Failed to add item '" << ItemName << "' to RegistryItem '" << this->Name() << "'." << std::endl;

                return *insert_result.first->second;
            }


            /**
             * @brief 返回存储子项的无序map容器的头指针
             */
            SubRegistryItemType::iterator begin();


            /**
             * @brief 返回存储子项的无序map容器的常量头指针
             */
            SubRegistryItemType::const_iterator cbegin() const;


            /**
             * @brief 返回存储子项的无序map容器的尾指针
             */
            SubRegistryItemType::iterator end();


            /**
             * @brief 返回存储子项的无序map容器的常量尾指针
             */
            SubRegistryItemType::const_iterator cend() const;


            /**
             * @brief 返回存储子项的无序map容器的键的头位置的常量迭代器
             */
            KeyReturnConstIterator KeyConstBegin() const;


            /**
             * @brief 返回存储子项的无序map容器的键的尾位置的常量迭代器
             */
            KeyReturnConstIterator KeyConstEnd() const;


            /**
             * @brief 返回注册表项名称
             */
            const std::string& Name() const { return mName; }


            /**
             * @brief 在注册表项子项中获取rItemName对应的注册项
             */
            const RegistryItem& GetItem(const std::string& rItemName) const;


            /**
             * @brief 在注册表项子项中获取rItemName对应的注册项
             */
            RegistryItem& GetItem(const std::string& rItemName);


            /**
             * @brief 将 mpValue 中存储的值转换为一个 std::shared_ptr<TDataType> 类型的智能指针
             */
            template<typename TDataType>
            const TDataType& GetValue() const{
                QUEST_TRY
                
                return *(std::any_cast<std::shared_ptr<TdataType>>(mpValue));

                QUEST_CATCH("")
            }


            /**
             * 将 mpValue 中存储的值从一个基类（TDataType）转换为一个派生类（TCastType）
             */
            template<typename TDataType, typename TCastType>
            const TCastType& GetValue() const{
                QUEST_TRY

                return *std::dynamic_pointer_cast<TCastType>(std::any_cast<std::shared_ptr<TDataType>>(mpValue));

                QUEST_CATCH("")
            }


            /**
             * @brief 移除注册表项子项
             */
            void RemoveItem(const std::string& rItemName);


            /**
             * @brief 注册表项中子项的数量
             */
            std::size_t size();


            /**
             * @brief 检查当前 RegistryItem 是否存储了一个值
             */
            bool HasValue() const;


            /**
             * @brief 检查当前 RegistryItem 是否包含任何子项
             */
            bool HasItems() const;


            /**
             * @brief 检查当前 RegistryItem 是否包含指定名称的子项
             */
            bool HasItem(const std::string& rItemName) const;

            std::string Info() const;

            void PrintInfo(std::ostream& rOstream) const;

            void PrintData(std::ostream& rOstream) const;


            /**
             * @brief 将注册表项转换为Json字符串
             */
            std::string ToJson(const std::string& rTabSpacing = "", const std::size_t Level = 0) const;

        protected:

        private:
            /**
             * @brief 返回注册表子项的类型名称
             */
            std::string GetRegistryItemType() const{
                return mpValue.type().name();
            }


            /**
             * @brief 将存储的值转换为字符串形式返回
             * @details 需要在调用时明确指定 TItemType，告诉函数存储的值的具体类型
             */
            template<typename TItemType>
            std::string GetItemString() const{
                std::stringstream buffer;
                buffer << this->GetValue<TItemType>();
                return buffer.str();
            }


            /**
             * @brief 创建一个空的注册项
             */
            class SubRegistryItemFunctor{
                public:
                    template<typename... TArgumentsList>
                    static inline RegistryItem::Pointer Create(const std::string& ItemName, TArgumentsList&&... Arguments){
                        return Quest::make_shared<RegistryItem>(ItemName);
                    }
            };


            /**
             * @brief 提供更灵活的创建子项的方法
             */
            template<typename TItemType>
            class SubRegistryItemFunctor{
                public:
                    template<typename... TArgumentsList, typename TFunctionType = std::function<std::shared_ptr<TItemType>(TArgumentsList...)>>
                    static inline RegistryItem::Pointer Create(const std::string& ItemName, TFunctionType&& Function){
                        return Quest::make_shared<RegistryItem>(ItemName, std::forward<TFunctionType>(Function));
                    }

                    template<typename... TArgumentsList>
                    static inline RegistryItem::Pointer Create(const std::string& ItemName, TArgumentsList&&... Arguments){
                        return Quest::make_shared<RegistryItem>(ItemName, Quest::make_shared<TItemType>(std::forward<TArgumentsList>(Arguments)...));
                    }
            };


            /**
             * @brief 将存储的值转换为字符串形式返回
             * @details 不需要在调用时指定类型，适用于动态类型存储的场景
             */
            std::string GetValueString() const;


            /**
             * @brief 获取子项的无序map容器
             */
            SubRegistryItemType& GetSubRegistryItemMap();


            /**
             * @brief 获取子项的常量无序map容器
             */
            SubRegistryItemType& GetSubRegistryItemMap() const;


        private:
            /**
             * @brief 注册表项名称
             */
            std::string mName;

            /**
             * @brief 存储当前注册表项的实际数据
             */
            std::any mpValue;

            /**
             * @brief 函数指针，该函数无参数，返回值为std::string，用于获取注册表项的值的字符串表示
             */
            std::string (RegistryItem::*mGetValueStringMethod)() const;
    };

    inline std::istream& operator >> (std::istream& rIstream, RegistryItem& rItem);

    inline std::ostream& operator << (std::ostream& rOstream, const RegistryItem& rItem){
        rItem.PrintInfo(rOstream);
        rOstream << std::endl;
        rItem.PrintData(rOstream);

        return rOstream;
    }

}


#endif // REGISTRY_ITEM_HPP