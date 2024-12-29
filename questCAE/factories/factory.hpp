#ifndef QUEST_FACTORY_HPP
#define QUEST_FACTORY_HPP

// 项目头文件
#include "includes/define.hpp"
#include "includes/quest_parameters.hpp"
#include "includes/quest_components.hpp"

namespace Quest{

    /**
     * @brief 定义了工厂类中一些共同的方法
     */
    class FactoryBase{
        public:
            QUEST_CLASS_POINTER_DEFINITION(FactoryBase);

        public:
            /**
             * @brief 构造函数
             */
            explicit FactoryBase(){}

            /**
             * @brief 析构函数
             */
            virtual ~FactoryBase(){}


            /**
             * @brief 判断某个子方法是否已经被注册
             */
            virtual bool Has(const std::string& rClassName) const{
                QUEST_ERROR << "Methods must be implemented in the derived class" << std::endl;
                return false;
            }


            virtual std::string Info() const{
                return "FactoryBase";
            }


            virtual void PrintInfo(std::ostream& rOstream) const{
                rOstream << this->Info() << std::endl;
            }


            virtual void PrintData(std::ostream& rOstream) const{}

        protected:

        private:

        private:

    };


    /**
     * @brief 工厂方法基类，添加了一些注册类所需的函数
     */
    template<typename TClass>
    class Factory : public FactoryBase{
        public:
            using ClassType = TClass;
            QUEST_CLASS_POINTER_DEFINITION(Factory);

        public:
            explicit Factory(){}


            virtual ~Factory() = default;


            /**
             * @brief 判断某个子方法是否已经被注册
             */
            bool Has(const std::string& rClassName) const override{
                return QuestComponents<ClassType>::Has(rClassName);
            }


            /**
             * @brief 创建一个子方法
             */
            template<typename... TArgumentsType>
            typename ClassType::Pointer Create(TArgumentsType&&... Arguments) const{
                std::tuple<TArgumentsType...> args(Arguments...);
                constexpr std::size_t args_size = std::tuple_size<std::tuple<TArgumentsType...>>::value;
                Quest::Parameters Settings = std::get<args_size-1>(args);
                const std::string& r_name = Settings["name"].GetString();
                QUEST_ERROR_IF_NOT(Has(r_name)) << "Trying to construct a class with type name= " << r_name << "\n" <<
                                            "Which does not exist. The list of available options (for currently loaded applications) are: \n" <<
                                            QuestComponents<ClassType>() << std::endl;
                const ClassType& aux = QuestComponents<ClassType>::Get(r_name);
                return aux.Create(std::forward<TArgumentsType>(Arguments)...);
            }


            std::string Info() const override{
                return "Factory";
            }


            void PrintInfo(std::ostream& rOstream) const override{
                rOstream << this->Info() << std::endl;
                const auto factory_components = QuestComponents<ClassType>::GetComponents();
                for(const auto& component : factory_components){
                    rOstream << "\t" << component.first << std::endl;
                }
            }


            virtual void PrintData(std::ostream& rOstream) const override{}

        protected:

        private:

        private:

    };


namespace Internals{
    template<typename TBaseCategoryType>
    class RegisteredPrototypeBase{
        public:
            RegisteredPrototypeBase() = default;
    };


    template<typename TClassType, typename TBaseCategoryType>
    class RegisteredPrototype : public RegisteredPrototypeBase<TBaseCategoryType>{
        public:
            explicit RegisteredPrototype(const std::string& rClassName, const TClassType& rPrototype){
                QuestComponents<TBaseCategoryType>::Add(rClassName, rPrototype);
            }
    };
}


    template<typename TClass>
    inline std::ostream& operator << (std::ostream& rOstream, const Factory<TClass>& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }
}

#endif //QUEST_FACTORY_HPP