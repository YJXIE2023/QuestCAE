/*===========================================================
通过实现 JSON 数据结构，使得可以方便地进行数据的输入、输出和配置
============================================================*/

#ifndef QUEST_QUEST_PARAMETERS_HPP
#define QUEST_QUEST_PARAMETERS_HPP

// 系统头文件
#include <filesystem>

// 第三方头文件
#include "json/json.hpp"

// 自定义头文件
#include "includes/serializer.hpp"
#include "includes/ublas_interface.hpp"

namespace Quest{
    
    class QUEST_API(QUEST_CORE) Parameters{
        private:

            class QUEST_API(QUEST_CORE) iterator_adaptor{
                public:
                    using itertor_category = std::forward_iterator_tag;
                    using difference_type  = std::ptrdiff_t;
                    using value_type       = Parameters;
                    using pointer          = Parameters*;
                    using reference        = Parameters&;
                    using value_iterator   = nlohmann::detail::iter_impl<nlohmann::json>;

                    iterator_adaptor(value_iterator itValue, nlohmann::json* pVlaue, Quest::shared_ptr<nlohmann::json> pRoot);

                    iterator_adaptor(const iterator_adaptor& itValue);

                    iterator_adaptor& operator++();

                    iterator_adaptor operator++(int);

                    bool operator==(const iterator_adaptor& itValue) const;

                    bool operator!=(const iterator_adaptor& itValue) const;

                    reference operator*() const;

                    pointer operator->() const;

                    inline value_iterator GetCurrentIterator() const;

                    const std::string name();

                protected:

                private:
                    std::size_t mDistance = 0;
                    nlohmann::json& mrValue;
                    std::unique_ptr<Parameters> mpParameters;

            };

            class QUEST_API(QUEST_CORE) const_iterator_adaptor{
                public:
                    using iterator_category = std::forward_iterator_tag;
                    using difference_type   = std::ptrdiff_t;
                    using value_type        = Parameters;
                    using pointer           = const Parameters*;
                    using reference         = const Parameters&;
                    using value_iterator    = nlohmann::detail::iter_impl<const nlohmann::json>;

                    const_iterator_adaptor(value_iterator itValue, nlohmann::json* pVlaue, Quest::shared_ptr<nlohmann::json> pRoot);

                    const_iterator_adaptor(const const_iterator_adaptor& itValue);

                    const_iterator_adaptor& operator++();

                    const_iterator_adaptor operator++(int);

                    bool operator==(const const_iterator_adaptor& itValue) const;

                    bool operator!=(const const_iterator_adaptor& itValue) const;

                    reference operator*() const;

                    pointer operator->() const;

                    inline value_iterator GetCurrentIterator() const;

                    const std::string name() const;

                protected:

                private:
                    std::size_t mDistance = 0;
                    nlohmann::json& mrValue;
                    std::unique_ptr<Parameters> mpParameters;

            };

        public:
            typedef std::size_t IndexType;
            typedef std::size_t SizeType;

            QUEST_CLASS_POINTER_DEFINITION(Parameters);

            using iterator = iterator_adaptor;
            using const_iterator = const_iterator_adaptor;

            typedef nlohmann::detail::iter_impl<nlohmann::json> json_iterator;
            typedef nlohmann::detail::iter_impl<const nlohmann::json> json_const_iterator;
            typedef nlohmann::detail::iteration_proxy<json_iterator> json_iterator_proxy;
            typedef nlohmann::detail::iteration_proxy<json_const_iterator> json_const_iterator_proxy;

            Parameters();

            Parameters(const std::string& rJsonString);

            Parameters(std::ifstream& rStringStream);

            Parameters(const Parameters& rOther);

            Parameters(Parameters&& rOther);

            virtual ~Parameters(){}

            Parameters& operator=(const Parameters& rOther);

            Parameters operator[](const std::string& rEntry);

            Parameters operator[](const std::string& rEntry) const;

            Parameters operator[](const IndexType rIndex);

            Parameters operator[](const IndexType rIndex) const;

            Parameters& operator=(Parameters&& rOther);

            Parameters Clone() const;

            const std::string WriteJsonString() const;

            const std::string PrettyPrintJsonString() const;

            Parameters GetValue(const std::string& rEntry);

            Parameters GetValue(const std::string& rEntry) const;

            void SetValue(const std::string& rEntry, const Parameters& rOtherValue);

            void AddValue(const std::string& rEntry, const Parameters& rOtherValue);

            Parameters AddEmptyValue(const std::string& rEntry);

            bool RemoveValue(const std::string& rEntry);

            bool RemoveValue(const std::vector<std::string>& rEntries);

            json_iterator_proxy items() noexcept;

            json_const_iterator_proxy items() const noexcept;

            bool Has(const std::string& rEntry) const;

            bool IsNull() const;

            bool IsNumber() const;

            bool IsDouble() const;

            bool IsInt() const;

            bool IsBool() const;

            bool IsString() const;

            bool IsArray() const;

            bool IsStringArray() const;

            bool IsVector() const;

            bool IsMatrix() const;

            template<typename TValue>
            bool Is() const;

            bool IsSubParameter() const;

            double GetDouble() const;

            int GetInt() const;

            bool GetBool() const;

            std::string GetString() const;

            std::vector<std::string> GetStringArray() const;

            Vector GetVector() const;

            Matrix GetMatrix() const;

            template<typename TValue>
            TValue Get() const;

            void SetDouble(const double& rValue);

            void SetInt(const int& rValue);

            void SetBool(const bool& rValue);

            void SetString(const std::string& rValue);

            void SetStringArray(const std::vector<std::string>& rValue);

            void SetVector(const Vector& rValue);

            void SetMatrix(const Matrix& rValue);

            template<typename TValue>
            void Set(const TValue& rValue);

            void AddDouble(const std::string& rEntry, const double rValue);

            void AddInt(const std::string& rEntry, const int rValue);

            void AddBool(const std::string& rEntry, const bool rValue);

            void AddString(const std::string& rEntry, const std::string& rValue);

            void AddStringArray(const std::string& rEntry, const std::vector<std::string>& rValue);

            void AddVector(const std::string& rEntry, const Vector& rValue);

            void AddMatrix(const std::string& rEntry, const Matrix& rValue);

            iterator begin();

            iterator end();

            const_iterator begin() const;

            const_iterator end() const;

            SizeType Size() const;

            void swap(Parameters& rOther) noexcept;

            void Reset() noexcept;

            Parameters GetArrayItem(const IndexType rIndex);

            Parameters GetArrayItem(const IndexType rIndex) const;

            void SetArrayItem(const IndexType rIndex, const Parameters& rOtherValue);

            void AddEmptyArray(const std::string& rEntry);

            void Append(const double rValue);

            void Append(const int rValue);

            void Append(const bool rValue);

            void Append(const std::string& rValue);

            void Append(const Vector& rValue);

            void Append(const Matrix& rValue);

            void Append(const Parameters& rValue);

            void CopyValuesFromExistingParameters(const Parameters OriginParameters, const std::vector<std::string>& rListParametersToCopy);

            void RecursivelyFindValue(const nlohmann::json& rBaseValue, const nlohmann::json& rValueToFind) const;

            bool IsEquivalentTo(Parameters& rParameters);

            bool HasSameKeysAndTypeOfValues(Parameters& rParameters);

            void ValidateAndAssignDefaults(const Parameters& rDefaultParameters);

            void RecursivelyValidateAndAssignDefaults(const Parameters& rDefaultParameters);

            void AddMissingParameters(const Parameters& rDefaultParameters);

            void RecursivelyAddMissingParameters(const Parameters& rDefaultParameters);

            void ValidateDefaults(const Parameters& rDefaultParameters) const;

            void RecursivelyValidateDefaults(const Parameters& rDefaultParameters) const;

            virtual std::string Info() const{
                return this->PrettyPrintJsonString();
            }

            virtual void PrintInfo(std::ostream& rOstream) const{
                rOstream << "Parameters Object " << Info();
            }

            virtual void PrintData(std::ostream& rOstream) const{

            }

        protected:

        private:
            friend class Serializer;

            void save(Serializer& rSerializer) const;

            void load(Serializer& rSerializer);

            nlohmann::json* mpValue;   // JSON文件实际存储位置
            Quest::shared_ptr<nlohmann::json> mpRoot;   // JSON文件根节点
            
            // 构造函数，接受 nlohmann::json 的原始指针和 Kratos::shared_ptr<nlohmann::json> 类型的共享指针
            Parameters(nlohmann::json* pValue, Quest::shared_ptr<nlohmann::json> pRoot);
            
            // 直接构造函数，接受一个 JSON 迭代器、nlohmann::json 的原始指针和一个共享指针
            Parameters(json_iterator itValue, nlohmann::json* pValue, Quest::shared_ptr<nlohmann::json> pRoot);

            // 直接构造函数，接受一个 JSON 常量迭代器、nlohmann::json 的原始指针和一个共享指针
            Parameters(json_const_iterator itValue, nlohmann::json* pValue, Quest::shared_ptr<nlohmann::json> pRoot);

            // 访问底层的 nlohmann::json 存储
            nlohmann::json* GetUnderlyingStorage();

            // 访问底层的 nlohmann::json 常量存储
            nlohmann::json* GetUnderlyingStorage() const;
            
            // 设置底层的 nlohmann::json 存储
            void SetUnderlyingStorage(nlohmann::json* pValue);

            // 访问底层的根 nlohmann::json 存储
            Quest::shared_ptr<nlohmann::json> GetUnderlyingRootStarage();

            // 访问底层的根 nlohmann::json 常量存储
            Quest::shared_ptr<nlohmann::json> GetUnderlyingRootStarage() const;

            // 设置底层的根 nlohmann::json 存储
            void SetUnderlyingRootStorage(Quest::shared_ptr<nlohmann::json> pNewValue);

            // 将 Parameters 的值设置为另一个 Parameters 实例的值
            void InternalSetValue(const Parameters& rOtherValue);

            // 此方法解决 JSON 文件中的包含依赖。它将最终的 JSON 对象写入 rJson，消除所有包含依赖
            void SolveIncludes(nlohmann::json& rJson, const std::filesystem::path& rFileName, std::vector<std::filesystem::path>& rIncludeSequence);

            // 解析指定名称的 JSON 文件，并返回解析得到的 JSON 对象
            nlohmann::json ReadFile(const std::filesystem::path& rFileName);

    };

    inline std::istream& operator >> (std::istream& rIstream, Parameters& rParameters){
        return rIstream;
    }

    inline std::ostream& operator << (std::ostream& rOstream, const Parameters& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }
}



#endif //QUEST_PARAMETERS_HPP