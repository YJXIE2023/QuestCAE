/*-------------------------------------------------------------
全局的注册表类，用于存储和检索名称和对象的映射关系
--------------------------------------------------------------*/

#ifndef REGISTRY_HPP
#define REGISTRY_HPP

// 系统头文件
#include <string>
#include <iostream>

// 项目头文件
#include "includes/registry_item.hpp"
#include "utilities/parallel_utilities.hpp"
#include "utilities/string_utilities.hpp"

namespace Quest{

    class QUEST_API Registry final{
        public:
            QUEST_CLASS_POINTER_DEFINITION(Registry);

            Registry(){}

            ~Registry(){}

            template<typename TItemType,typename... TArgumentsList>
            static Registry& AddItem(const std::string& rItemFullName, TArgumentsList&&... Arguments){
                QUEST_TRY

                const std::lock_guard<LockObject> scope_lock(ParallelUtilities::GetLockObject());

                auto item_path = StringUtilities::SplitStringByDelimiter(rItemFullName, ".");
                QUEST_ERROR_IF(item_path.empty()) << "The item full name is empty" << std::endl;

                RegistryItem* p_current_item = &GetRootRegistryItem();
                for(std::size_t i = 0; i < item_path.size() - 1; ++i){
                    auto& r_item_name = item_path[i];
                    if(p_current_item->HasItem(r_item_name)){
                        p_current_item = &p_current_item->GetItem(r_item_name);
                    } else {
                        p_current_item = &p_current_item->AddItem<RegistryItem>(r_item_name);
                    }
                }

                auto& r_item_name = item_path.back();
                if(p_current_item->HasItem(r_item_name)){
                    QUEST_ERROR << "The item \"" << rItemFullName << "\" already exists" << std::endl;
                } else {
                    p_current_item = &p_current_item->AddItem<TItemType>(r_item_name, std::forward<TArgumentsList>(Arguments)...);
                }

                return *p_current_item;

                QUEST_CATCH("")
            }

            template<typename... Types>
            static std::string RegistryTemplateToString(Types&&... Arguments){
                std::string f_name = (... + ("," + std::to_string(Arguments)));
                f_name.erase(0, 1);
                return f_name;
            }

            static auto begin(){
                return mspRootRegistryItem->begin();
            }

            static auto cbegin(){
                return mspRootRegistryItem->cbegin();
            }

            static auto end(){
                return mspRootRegistryItem->end();
            }

            static auto cend(){
                return mspRootRegistryItem->cend();
            }

            static RegistryItem& GetItem(const std::string& rItemFullName);

            template<typename TDataType>
            static const TDataType& GetValue(const std::string& rItemFullName){
                return GetItem(rItemFullName).GetValue<TDataType>();
            }

            template<typename TDataType, typename TCastType>
            static const typename std::enable_if<std::is_base_of<TDataType,TCastType>::value, TCastType>::type GetValueAs(const std::string& rItemFullName){
                return GetItem(rItemFullName).GetValueAs<TDataType, TCastType>();
            }

            static void RemoveItem(const std::string& rItemFullName);

            static std::size_t size();

            static bool HasItem(const std::string& rItemFullName);

            static bool HasValue(const std::string& rItemFullName);

            static bool HasItems(const std::string& rItemFullName);

            std::string Info() const;

            void PrintInfo(std::ostream& rOstream) const;

            void PrintData(std::ostream& rOstream) const;

            std::string ToJson(const std::string& Indentation) const;

        protected:

        private:
            static RegistryItem& GetRootRegistryItem();

            static std::vector<std::string> SplitFullName(const std::string& FullName);

            Registry& operator = (const Registry& rOther);

            Registry(const Registry& rOther);

        private:
            static RegistryItem* mspRootRegistryItem;

    };

    inline std::istream& operator >> (std::istream& rIstream, Registry& rThis) {}

    inline std::ostream& operator << (std::ostream& rOstream, const Registry& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }
}

#endif // REGISTRY_HPP