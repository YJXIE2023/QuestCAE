/*----------------------------------
用于存储和管理注册信息的基础类
----------------------------------*/

#ifndef REGISTRY_ITEM_HPP
#define REGISTRY_ITEM_HPP

// 系统头文件
#include <string>
#include <iostream>
#include <unordered_map>
#include <any>

// 项目头文件
#include "includes/define.hpp"

namespace Quest{

    class QUEST_API RegistryItem{
        public:
            QUEST_CLASS_POINTER_DEFINITION(RegistryItem);

            using SubRegistryItemType = std::unordered_map<std::string, Quest::shared_ptr<RegistryItem>>;
            using SubRegistryItemPointerType = Quest::shared_ptr<SubRegistryItemType>;

            // 迭代器类，用于在python中导出数据
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

            RegistryItem() = delete;

            RegistryItem(const std::string& rName): mName(rName) ,mpValue(Quest::make_shared<SubRegistryItemType>()),
                mGetValueStringMethod(&RegistryItem::GetRegistryItemType){}

            template<typename TItemType, typename... TArgs>
            RegistryItem(const std::string& rName, const std::function<std::shared_ptr<TItemType>(TArgs...)> &rValue):
                mName(rName), mpValue(rValue()), mGetValueStringMethod(&RegistryItem::GetItemString<TItemType>){}

            template<typename TItemType>
            RegistryItem(const std::string& rName, const TItemType& rValue): mName(rName), mpValue(Quest::make_shared<TItemType>(rValue)),
                mGetValueStringMethod(&RegistryItem::GetItemString<TItemType>){}

            template<typename TItemType>
            RegistryItem(const std::string& rName, const std::shared_ptr<TItemType>& rValue): mName(rName), mpValue(rValue),
                mGetValueStringMethod(&RegistryItem::GetItemString<TItemType>){}

            RegistryItem(const RegistryItem& rItem) = delete;

            ~RegistryItem() = default;

            RegistryItem& operator = (RegistryItem& rOther) = delete;

            template<typename TItemType,typename... TArgumentsList>
            RegistryItem& AddItem(const std::string& ItemName, TArgumentsList&&... Arguments){
                QUEST_ERROR_IF(this->HasItem(ItemName))
                    << "The RegistryItem '" << this->Name() << "' already has an item named '" << ItemName << "'." << std::endl;

                using ValueType = typename std::conditional<std::is_same<TItemType,RegistryItem>::value, SubRegistryItemFunctor, SubValueItemFunctor<TItemType>>::type;

                auto insert_result = GetSubRegistryItemMap().emplace(
                    std::make_pair(ItemName, ValueType::Create(ItemName, std::forward<TArgumentsList>(Arguments)...))
                );

                QUEST_ERROR_IF_NOT(insert_result.second)
                    << "Failed to add item '" << ItemName << "' to RegistryItem '" << this->Name() << "'." << std::endl;

                return *insert_result.first->second;
            }

            SubRegistryItemType::iterator begin();

            SubRegistryItemType::const_iterator cbegin() const;

            SubRegistryItemType::iterator end();

            SubRegistryItemType::const_iterator cend() const;

            KeyReturnConstIterator KeyConstBegin() const;

            KeyReturnConstIterator KeyConstEnd() const;

            const std::string& Name() const { return mName; }

            const RegistryItem& GetItem(const std::string& rItemName) const;

            RegistryItem& GetItem(const std::string& rItemName);

            template<typename TDataType>
            const TDataType& GetValue() const{
                QUEST_TRY
                
                return *(std::any_cast<std::shared_ptr<TdataType>>(mpValue));

                QUEST_CATCH("");
            }

            template<typename TDataType, typename TCastType>
            const TCastType& GetValue() const{
                QUEST_TRY

                return *std::dynamic_pointer_cast<TCastType>(std::any_cast<std::shared_ptr<TDataType>>(mpValue));

                QUEST_CATCH("");
            }

            void RemoveItem(const std::string& rItemName);

            std::size_t size();

            bool HasValue() const;

            bool HasItems() const;

            bool HasItem(const std::string& rItemName) const;

            std::string Info() const;

            void PrintInfo(std::ostream& rOstream) const;

            void PrintData(std::ostream& rOstream) const;

            std::string ToJson(const std::string& rTabSpacing = "", const std::size_t Level = 0) const;

        protected:

        private:
            std::string GetRegistryItemType() const{
                return mpValue.type().name();
            }

            template<typename TItemType>
            std::string GetItemString() const{
                std::stringstream buffer;
                buffer << this->GetValue<TItemType>();
                return buffer.str();
            }

            class SubRegistryItemFunctor{
                public:
                    template<typename... TArgumentsList>
                    static inline RegistryItem::Pointer Create(const std::string& ItemName, TArgumentsList&&... Arguments){
                        return Quest::make_shared<RegistryItem>(ItemName);
                    }
            };

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

            std::string GetValueString() const;

            SubRegistryItemType& GetSubRegistryItemMap();

            SubRegistryItemType& GetSubRegistryItemMap() const;


        private:
            std::string mName;
            std::any mpValue;
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