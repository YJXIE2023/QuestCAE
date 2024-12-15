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
    
    /**
     * @class Parameters
     * @brief 提供基于JSON标准的数据结构，用于输入/输出操作
     * @details nlohmann::json是JSON标准的C++实现，它提供了方便的接口来操作JSON数据
     */
    class QUEST_API(QUEST_CORE) Parameters{
        private:

            /**
             * @brief 迭代器适配器，用于适配 Parameters 迭代器
             */
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

            /**
             * @brief 常量迭代器适配器，用于适配 Parameters 常量迭代器
             */
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

            /**
             * @brief 默认构造函数
             */
            Parameters();

            /**
             * @brief 字符串构造函数。它接受一个字符串作为输入，并将其解析为一个 nlohmann::json 类
             * @param rJsonString 需要解析为 nlohmann::json 类的字符串
             */
            Parameters(const std::string& rJsonString);

            /**
             * @brief 文件构造函数。它接受一个文件流作为输入，并将其解析为一个 nlohmann::json 类
             * @param rStringStream JSON 文件的流
             */
            Parameters(std::ifstream& rStringStream);

            /**
             * @brief 复制构造函数
             */
            Parameters(const Parameters& rOther);

            /**
             * @brief 移动构造函数
             */
            Parameters(Parameters&& rOther);

            /**
             * @brief 析构函数
             */
            virtual ~Parameters(){}

            /**
             * @brief 赋值运算符
             */
            Parameters& operator=(const Parameters& rOther);

            /**
             * @brief 下标访问运算符
             * @param rEntry 参数的键标识符
             */
            Parameters operator[](const std::string& rEntry);

            /**
             * @brief 下标访问运算符
             * @param rEntry 参数的键标识符
             */
            Parameters operator[](const std::string& rEntry) const;

            /**
             * @brief 该方法允许通过运算符 [] 访问数组项
             * @param Index 想要提取的项的索引
             * @return 所需的 Parameter
             */
            Parameters operator[](const IndexType rIndex);

            /**
             * @brief 该方法允许通过运算符 [] 访问数组项
             * @param Index 想要提取的项的索引
             * @return 所需的 Parameter
             */
            Parameters operator[](const IndexType rIndex) const;

            /**
             * @brief 移动赋值运算符重载
             */
            Parameters& operator=(Parameters&& rOther);

            /**
             * @brief 克隆函数
             */
            Parameters Clone() const;

            /**
             * @brief 该方法返回一个字符串，包含与等效的 *.json 文件相对应的文本
             * @return 对应的文本
             */
            const std::string WriteJsonString() const;

            /**
             * @brief 该方法返回一个字符串，包含与等效的 *.json 文件相对应的文本（此版本更美观，并考虑了制表符）
             * @return 对应的文本
             */
            const std::string PrettyPrintJsonString() const;

            /**
             * @brief 通过参数的键标识符提取参数的值
             */
            Parameters GetValue(const std::string& rEntry);

            /**
             * @brief 通过参数的键标识符提取参数的值
             */
            Parameters GetValue(const std::string& rEntry) const;

            /**
             * @brief 设置键标识符对应的值
             */
            void SetValue(const std::string& rEntry, const Parameters& rOtherValue);

            /**
             * @brief 该方法使用给定的参数设置一个不存在的参数（添加一个新参数）
             * @param rEntry 参数的键标识符
             * @param rOtherValue 要设置的值
             */
            void AddValue(const std::string& rEntry, const Parameters& rOtherValue);

            /**
             * @brief 该方法添加一个空参数
             * @param rEntry 参数的键标识符
             */
            Parameters AddEmptyValue(const std::string& rEntry);

            /**
             * @brief 删除一个参数
             * @param rEntry 参数的键标识符
             */
            bool RemoveValue(const std::string& rEntry);

            /**
             * @brief 删除多个参数
             * @param rEntries 参数的键标识符列表
             */
            bool RemoveValue(const std::vector<std::string>& rEntries);

            /**
             * @brief 返回当前参数的项
             */
            json_iterator_proxy items() noexcept;

            /**
             * @brief 返回当前参数的项
             */
            json_const_iterator_proxy items() const noexcept;

            /**
             * @brief 判断当前是否存在某个项
             */
            bool Has(const std::string& rEntry) const;

            /**
             * @brief 判断参数是否为空
             */
            bool IsNull() const;

            /**
             * @brief 判断参数是否为数字
             */
            bool IsNumber() const;

            /**
             * @brief 判断参数是否为浮点数
             */
            bool IsDouble() const;

            /**
             * @brief 判断参数是否为整数
             */
            bool IsInt() const;

            /**
             * @brief 判断参数是否为布尔值
             */
            bool IsBool() const;

            /**
             * @brief 判断参数是否为字符串
             */
            bool IsString() const;

            /**
             * @brief 判断参数是否为数组
             */
            bool IsArray() const;

            /**
             * @brief 判断参数是否未字符串数字
             */
            bool IsStringArray() const;

            /**
             * @brief 判断参数是否为向量
             */
            bool IsVector() const;

            /**
             * @brief 判断参数是否为矩阵
             */
            bool IsMatrix() const;

            /**
             * @brief 支持类型的模板化类型检查器。
             * @details 支持的类型：double, int, bool, std::string, @ref Vector, @ref Matrix。
             * @tparam TValue：要解析的值的类型。
             */
            template<typename TValue>
            bool Is() const;

            /**
             * @brief 该方法检查参数是否为子参数
             */
            bool IsSubParameter() const;

            /**
             * @brief 返回当前参数中包含的 double 值 
             */
            double GetDouble() const;

            /**
             * @brief 返回当前参数中包含的 int 值 
             */
            int GetInt() const;

            /**
             * @brief 返回当前参数中包含的 bool 值 
             */
            bool GetBool() const;

            /**
             * @brief 返回当前参数中包含的字符串值 
             */
            std::string GetString() const;

            /**
             * @brief 返回当前参数中包含的字符串数组值 
             */
            std::vector<std::string> GetStringArray() const;

            /**
             * @brief 返回当前参数中包含的向量值 
             */
            Vector GetVector() const;

            /**
             * @brief 返回当前参数中包含的矩阵值 
             */
            Matrix GetMatrix() const;

            /**
             * @brief 返回当前参数中包含的特定类型的值
             */
            template<typename TValue>
            TValue Get() const;

            /**
             * @brief 设置当前参数中包含的 double 值 
             */
            void SetDouble(const double& rValue);

            /**
             * @brief 设置当前参数中包含的 int 值 
             */
            void SetInt(const int& rValue);

            /**
             * @brief 设置当前参数中包含的 bool 值 
             */
            void SetBool(const bool& rValue);

            /**
             * @brief 设置当前参数中包含的字符串值 
             */
            void SetString(const std::string& rValue);

            /**
             * @brief 设置当前参数中包含的字符串数组值 
             */
            void SetStringArray(const std::vector<std::string>& rValue);

            /**
             * @Brief 设置当前参数中包含的向量值
             */
            void SetVector(const Vector& rValue);

            /**
             * @brief 设置当前参数中包含的矩阵值
             */
            void SetMatrix(const Matrix& rValue);

            /**
             * @Brief 设置当前参数中包含的特定类型的值
             */
            template<typename TValue>
            void Set(const TValue& rValue);

            /**
             * @brief 添加一个double值到当前参数中
             */
            void AddDouble(const std::string& rEntry, const double rValue);

            /**
             * @brief 添加一个int值到当前参数中
             */
            void AddInt(const std::string& rEntry, const int rValue);

            /**
             * @brief 添加一个bool值到当前参数中
             */
            void AddBool(const std::string& rEntry, const bool rValue);

            /**
             * @brief 添加一个string值到当前参数中
             */
            void AddString(const std::string& rEntry, const std::string& rValue);

            /**
             * @brief 添加一个字符数组值到当前参数中
             */
            void AddStringArray(const std::string& rEntry, const std::vector<std::string>& rValue);

            /**
             * @brief 添加一个向量值到当前参数中
             */
            void AddVector(const std::string& rEntry, const Vector& rValue);

            /**
             * @brief 添加一个矩阵值到当前参数中
             */
            void AddMatrix(const std::string& rEntry, const Matrix& rValue);

            /**
             * @brief 返回首迭代器
             */
            iterator begin();

            /**
             * @brief 返回尾迭代器
             */
            iterator end();

            /**
             * @brief 返回首迭代器
             */
            const_iterator begin() const;

            /**
             * @brief 返回尾迭代器
             */
            const_iterator end() const;

            /**
             * @brief 返回当前数组参数的大小
             */
            SizeType Size() const;

            /**
             * @brief 交换两个 Parameters 实例
             */
            void swap(Parameters& rOther) noexcept;

            /**
             * @brief 重置参数(将Parameters设置为空)
             */
            void Reset() noexcept;

            /**
             * @brief 根据索引返回数组项
             */
            Parameters GetArrayItem(const IndexType rIndex);

            /**
             * @brief 根据索引返回数组项
             */
            Parameters GetArrayItem(const IndexType rIndex) const;

            /**
             * @brief 设置数组项
             */
            void SetArrayItem(const IndexType rIndex, const Parameters& rOtherValue);

            /**
             * @brief 添加一个空数组项
             */
            void AddEmptyArray(const std::string& rEntry);

            /**
             * @brief 向数组中添加一个 double 值
             */
            void Append(const double rValue);

            /**
             * @brief 向数组中添加一个 int 值
             */
            void Append(const int rValue);

            /**
             * @brief 向数组中添加一个 bool 值
             */
            void Append(const bool rValue);

            /**
             * @brief 向数组中添加一个字符串值
             */
            void Append(const std::string& rValue);

            /**
             * @brief 向数组中添加一个向量值
             */
            void Append(const Vector& rValue);

            /**
             * @brief 向数组中添加一个矩阵值
             */
            void Append(const Matrix& rValue);

            /**
             * @brief 向数组中添加一个 Parameters 值
             */
            void Append(const Parameters& rValue);

            /**
             * @brief 复制现有 Parameters 对象中的值
             * @param OriginParameters 要复制的 Parameters 对象
             * @param rListParametersToCopy 要复制的参数列表
             */
            void CopyValuesFromExistingParameters(const Parameters OriginParameters, const std::vector<std::string>& rListParametersToCopy);

            /**
             * @brief 递归查找参数值
             * @param rBaseValue 要搜索的 JSON 对象
             * @param rValueToFind 要查找的值
             */
            void RecursivelyFindValue(const nlohmann::json& rBaseValue, const nlohmann::json& rValueToFind) const;

            /**
             * @brief 判断是否与另一个 Parameters 实例相等
             */
            bool IsEquivalentTo(Parameters& rParameters);

            /**
             * @brief 检查名称和值的类型是否相同，顺序不重要
             * @param rParameters 要检查的参数
             */
            bool HasSameKeysAndTypeOfValues(Parameters& rParameters);

            /**
             * @brief 验证待测试的参数是否与 rDefaultParameters 中规定的格式匹配
             * @param rDefaultParameters 要验证的默认参数
             */
            void ValidateAndAssignDefaults(const Parameters& rDefaultParameters);

            /**
             * @brief 验证待测试的参数是否与默认值规定的格式匹配
             * @param rDefaultParameters 要验证的默认参数
             */
            void RecursivelyValidateAndAssignDefaults(const Parameters& rDefaultParameters);

            /**
             * @brief 验证待测试的参数是否至少包含 rDefaultParameters 中规定的所有参数
             * @details 只遍历一级
             */
            void AddMissingParameters(const Parameters& rDefaultParameters);

            /**
             * @brief 验证待测试的参数是否至少包含 rDefaultParameters 中规定的所有参数
             * @details 递归遍历所有参数
             */
            void RecursivelyAddMissingParameters(const Parameters& rDefaultParameters);

            /**
             * @brief 验证待测试的参数中的所有参数是否与 rDefaultParameters 中规定的参数匹配
             * @details 只遍历一级
             */
            void ValidateDefaults(const Parameters& rDefaultParameters) const;

            /**
             * @brief 验证待测试的参数中的所有参数是否与 rDefaultParameters 中规定的参数匹配
             * @details 递归遍历所有参数
             */
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
            
            /**
             * @brief 构造函数，接受 nlohmann::json 的原始指针和 Kratos::shared_ptr<nlohmann::json> 类型的共享指针
             */
            Parameters(nlohmann::json* pValue, Quest::shared_ptr<nlohmann::json> pRoot);
            
            /**
             * @brief 直接构造函数，接受一个 JSON 迭代器、nlohmann::json 的原始指针和一个共享指针
             */
            Parameters(json_iterator itValue, nlohmann::json* pValue, Quest::shared_ptr<nlohmann::json> pRoot);

            /**
             * @brief 直接构造函数，接受一个 JSON 常量迭代器、nlohmann::json 的原始指针和一个共享指针
             */
            Parameters(json_const_iterator itValue, nlohmann::json* pValue, Quest::shared_ptr<nlohmann::json> pRoot);

            /**
             * @brief 访问底层的 nlohmann::json 存储
             */
            nlohmann::json* GetUnderlyingStorage();

            /**
             * @brief 访问底层的 nlohmann::json 常量存储
             */
            nlohmann::json* GetUnderlyingStorage() const;
            
            /**
             * @brief 设置底层的 nlohmann::json 存储
             */
            void SetUnderlyingStorage(nlohmann::json* pValue);

            /**
             * @brief 访问底层的根 nlohmann::json 存储
             */
            Quest::shared_ptr<nlohmann::json> GetUnderlyingRootStarage();

            /**
             * @brief 访问底层的根 nlohmann::json 常量存储
             */
            Quest::shared_ptr<nlohmann::json> GetUnderlyingRootStarage() const;

            /**
             * @brief 设置底层的根 nlohmann::json 存储
             */
            void SetUnderlyingRootStorage(Quest::shared_ptr<nlohmann::json> pNewValue);

            /**
             * @brief 将 Parameters 的值设置为另一个 Parameters 实例的值
             */
            void InternalSetValue(const Parameters& rOtherValue);

            /**
             * @brief 此方法解决 JSON 文件中的包含依赖。它将最终的 JSON 对象写入 rJson，消除所有包含依赖
             */
            void SolveIncludes(nlohmann::json& rJson, const std::filesystem::path& rFileName, std::vector<std::filesystem::path>& rIncludeSequence);

            /**
             * @brief 解析指定名称的 JSON 文件，并返回解析得到的 JSON 对象
             */
            nlohmann::json ReadFile(const std::filesystem::path& rFileName);

        private:
            /**
             * @brief JSON文件实际位置
             */
            nlohmann::json* mpValue; 

            /**
             * @brief JSON文件的根节点
             */
            Quest::shared_ptr<nlohmann::json> mpRoot; 

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