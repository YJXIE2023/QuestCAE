/*-------------------------------------------------------------
全局的注册表类，用于存储和检索名称和对象的映射关系
--------------------------------------------------------------*/

#ifndef QUEST_REGISTRY_HPP
#define QUEST_REGISTRY_HPP

// 系统头文件
#include <string>
#include <iostream>

// 项目头文件
#include "includes/registry_item.hpp"
#include "utilities/parallel_utilities.hpp"
#include "utilities/string_utilities.hpp"

namespace Quest{

    class QUEST_API(QUEST_CORE) Registry final{
        public:
            QUEST_CLASS_POINTER_DEFINITION(Registry);

            /**
             * @brief 默认构造函数
             */
            Registry(){}


            /**
             * @brief 析构函数
             */
            ~Registry(){}


            /**
             * @brief 添加新的注册表项
             */
            template<typename TItemType,typename... TArgumentsList>
            static RegistryItem& AddItem(const std::string& rItemFullName, TArgumentsList&&... Arguments){
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


            /**
             * @brief 将传入的参数列表转换为一个以逗号分隔的字符串
             */
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


            /**
             * @brief 通过完成的注册表项名称获取注册表项
             * @details ItemFullName 格式为 "Item1.Item2.Item3"
             */
            static RegistryItem& GetItem(const std::string& rItemFullName);


            /**
             * @brief 通过完成的注册表项名称获取注册表项
             * @details 知道确定的类型，可以避免类型转换
             */
            template<typename TDataType>
            static const TDataType& GetValue(const std::string& rItemFullName){
                return GetItem(rItemFullName).GetValue<TDataType>();
            }


            /**
             * @brief  从注册表中获取特定名称的注册项值
             * @details 将其转换为目标类型 TCastType，前提是目标类型 TCastType 必须是数据类型 TDataType 的派生类
             */
            template<typename TDataType, typename TCastType>
            static const typename std::enable_if<std::is_base_of<TDataType,TCastType>::value, TCastType>::type GetValueAs(const std::string& rItemFullName){
                return GetItem(rItemFullName).GetValueAs<TDataType, TCastType>();
            }


            /**
             * @brief 移除注册表项
             */
            static void RemoveItem(const std::string& rItemFullName);


            /**
             * @brief 注册表大小
             */
            static std::size_t size();


            /**
             * @brief 检查当前 RegistryItem 是否包含指定名称的子项
             */
            static bool HasItem(const std::string& rItemFullName);


            /**
             * @brief 检查当前 RegistryItem 是否存储了一个值
             */
            static bool HasValue(const std::string& rItemFullName);


            /**
             * @brief 检查当前 RegistryItem 是否包含任何子项
             */
            static bool HasItems(const std::string& rItemFullName);

            std::string Info() const;

            void PrintInfo(std::ostream& rOstream) const;

            void PrintData(std::ostream& rOstream) const;


            /**
             * @brief 将注册表转换为 JSON 格式的字符串
             */
            std::string ToJson(const std::string& Indentation) const;

        protected:

        private:
            /**
             * @brief 获取根注册表项
             */
            static RegistryItem& GetRootRegistryItem();


            /**
             * @brief 将一个完整的名称字符串（FullName）分割为多个部分，并以字符串向量的形式返回
             * @details 例如，FullName 为 "Item1.Item2.Item3"，则返回 {"Item1", "Item2", "Item3"}
             */
            static std::vector<std::string> SplitFullName(const std::string& FullName);


            /**
             * @brief 赋值运算符重载
             */
            Registry& operator = (const Registry& rOther);


            /**
             * @brief 复制构造函数
             */
            Registry(const Registry& rOther);

        private:
            /**
             * @brief 根注册表项指针
             */
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