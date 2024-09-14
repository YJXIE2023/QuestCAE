/*------------------------------------
quest_parameters.hpp头文件中的实现
-------------------------------------*/

// 系统头文件
#include <stack>
#include <algorithm>

// 第三方头文件
#include "json/json.hpp"

// 项目头文件
#include "includes/quest_parameters.hpp"
#include "includes/define.hpp"
#include "includes/quest_filesystem.hpp"
#include "IO/logger.hpp"

namespace Quest{

    Parameters::iterator_adaptor::iterator_adaptor(Parameters::iterator_adaptor::value_iterator itValue, nlohmann::json* pValue, Quest::shared_ptr<nlohmann::json> pRoot):
        mDistance(std::distance(pRoot->begin(), itValue)), mrValue(*pValue), mpParameters(new Parameters(itValue, pValue, pRoot)) {}


    Parameters::iterator_adaptor::iterator_adaptor(const Parameters::iterator_adaptor& itValue):
        mDistance(itValue.mDistance), mrValue(itValue.mrValue), mpParameters(new Parameters(itValue->GetUnderlyingStorage(),itValue->GetUnderlyingRootStarage())) {}


    Parameters::iterator_adaptor& Parameters::iterator_adaptor::operator++(){
        ++mDistance;
        return *this;
    }


    Parameters::iterator_adaptor Parameters::iterator_adaptor::operator++(int){
        iterator_adaptor tmp(*this);
        ++mDistance;
        return tmp;
    }


    bool Parameters::iterator_adaptor::operator==(const Parameters::iterator_adaptor& rhs) const{
        return mDistance == rhs.mDistance;
    }


    bool Parameters::iterator_adaptor::operator!=(const Parameters::iterator_adaptor& rhs) const{
        return mDistance!= rhs.mDistance;
    }


    Parameters& Parameters::iterator_adaptor::operator*() const{
        auto it = GetCurrentIterator();
        if (it != mrValue.end()){
            mpParameters->mpValue = &(*it);
        }
        return *mpParameters;
    }


    Parameters* Parameters::iterator_adaptor::operator->() const{
        auto it = GetCurrentIterator();
        if (it != mrValue.end()){
            mpParameters->mpValue = &(*it);
        }
        return mpParameters.get();
    }


    inline Parameters::iterator_adaptor::value_iterator Parameters::iterator_adaptor::GetCurrentIterator() const{
        auto it = mrValue.begin();
        for (std::size_t i = 0; i < mDistance; ++i){
            ++it;
        }
        return it;
    }


    const std::string Parameters::iterator_adaptor::name(){
        auto it = GetCurrentIterator();
        return it.key();
    }


    Parameters::const_iterator_adaptor::const_iterator_adaptor(value_iterator itValue, nlohmann::json* pValue, Quest::shared_ptr<nlohmann::json> pRoot):
        mDistance(std::distance(pValue->cbegin(),itValue)), mrValue(*pValue), mpParameters(new Parameters(itValue,pValue,pRoot)) {}


    Parameters::const_iterator_adaptor::const_iterator_adaptor(const const_iterator_adaptor& itValue):
        mDistance(itValue.mDistance), mrValue(itValue.mrValue), mpParameters(new Parameters(itValue->GetUnderlyingStorage(),itValue->GetUnderlyingRootStarage())) {}


    Parameters::const_iterator_adaptor& Parameters::const_iterator_adaptor::operator++(){
        ++mDistance;
        return *this;
    }


    Parameters::const_iterator_adaptor Parameters::const_iterator_adaptor::operator++(int){
        const_iterator_adaptor tmp(*this);
        ++mDistance;
        return tmp;
    }


    bool Parameters::const_iterator_adaptor::operator==(const Parameters::const_iterator_adaptor& rhs) const{
        return mDistance == rhs.mDistance;
    }


    bool Parameters::const_iterator_adaptor::operator!=(const Parameters::const_iterator_adaptor& rhs) const{
        return mDistance!= rhs.mDistance;
    }


    const Parameters& Parameters::const_iterator_adaptor::operator*() const{
        auto it = GetCurrentIterator();
        if (it != mrValue.cend()){
            mpParameters->mpValue = const_cast<nlohmann::json*>(&(*it));
        }
        return *mpParameters;
    }


    const Parameters* Parameters::const_iterator_adaptor::operator->() const{
        auto it = GetCurrentIterator();
        if (it != mrValue.cend()){
            mpParameters->mpValue = const_cast<nlohmann::json*>(&(*it));
        }
        return mpParameters.get();
    }


    inline Parameters::const_iterator_adaptor::value_iterator Parameters::const_iterator_adaptor::GetCurrentIterator() const{
        auto it = mrValue.cbegin();
        for (std::size_t i = 0; i < mDistance; ++i){
            ++it;
        }
        return it;
    }


    Parameters::Parameters(){
        mpRoot = Quest::make_shared<nlohmann::json>(nlohmann::json::parse("{}",nullptr,true,true));
        mpValue = mpRoot.get();
    }


    nlohmann::json Parameters::ReadFile(const std::filesystem::path& rFileName){
        std::ifstream new_file;
        new_file.open(rFileName,std::ios::in);

        std::stringstream strStream;
        strStream << new_file.rdbuf();
        std::string input_json = strStream.str();
        return nlohmann::json::parse(input_json);
    }


    Parameters::Parameters(const std::string& rJsonString){
        mpRoot = Quest::make_shared<nlohmann::json>(nlohmann::json::parse(rJsonString,nullptr,true,true));
        mpValue = mpRoot.get();

        std::vector<std::filesystem::path> include_sequence;

        SolveIncludes(*mpValue,"root",include_sequence);
    }


    void Parameters::SolveIncludes(nlohmann::json& rJson, const std::filesystem::path& rFileName, std::vector<std::filesystem::path>& rIncludeSequence){
        std::stack<std::pair<nlohmann::json*,nlohmann::json::iterator>> s;
        
        if(rJson.is_object()){
            s.push({&rJson,rJson.begin()});
        }

        while(!s.empty()){
            std::pair<nlohmann::json*,nlohmann::json::iterator> pJson_it = s.top();
            s.pop();

            nlohmann::json* act_pJson = pJson_it.first;
            nlohmann::json::iterator act_it = pJson_it.second;

            while(act_it != act_pJson->end()){
                if(act_it.value().is_object()){
                    s.emplace(&act_it.value(),act_it.value().begin());
                } else if(act_it.value().is_array()){
                    for(auto it:act_it.value().items()){
                        SolveIncludes(it.value(),rFileName,rIncludeSequence);
                    }
                } else if(act_it.key() == "@include_json"){
                    const std::string included_file_path_string = *act_it;
                    const std::filesystem::path included_file_path = FilesystemExtensions::ResolveSymlinks(std::filesystem::path(included_file_path_string));
                    QUEST_ERROR_IF_NOT(std::filesystem::is_regular_file(included_file_path)) << "File not found: '" << *act_it << "'";

                    nlohmann::json included_json = ReadFile(included_file_path);

                    auto it_cycle = std::find(rIncludeSequence.begin(),rIncludeSequence.end(),included_file_path);
                    if(it_cycle != rIncludeSequence.end()){
                        std::stringstream message_stream;
                        message_stream << "Include cycle in json files: ";
                        for(;it_cycle != rIncludeSequence.end();++it_cycle){
                            message_stream << *it_cycle << " => ";
                        }
                        message_stream << included_file_path << " => ...";
                        QUEST_ERROR << message_stream.str();
                    }

                    rIncludeSequence.push_back(included_file_path);
                    SolveIncludes(included_json,included_file_path,rIncludeSequence);
                    rIncludeSequence.pop_back();

                    act_it = act_pJson->erase(act_it);
                    act_pJson->insert(included_json.begin(),included_json.end());
                    continue;
                }
                ++act_it;
            }
        }
    }


    Parameters::Parameters(std::ifstream& rStringStream){
        mpRoot = Quest::make_shared<nlohmann::json>(nlohmann::json::parse(rStringStream,nullptr,true,true));
        mpValue = mpRoot.get();
        
        std::vector<std::filesystem::path> include_sequence;

        SolveIncludes(*mpValue,"root",include_sequence);
    }


    Parameters::Parameters(const Parameters& rOther){
        mpRoot = rOther.mpRoot;
        mpValue = rOther.mpValue;
    }


    Parameters::Parameters(Parameters&& rOther):Parameters(){
        swap(rOther);
    }


    Parameters& Parameters::operator=(const Parameters& rOther){
        if(mpRoot.get() == mpValue || mpRoot == nullptr){
            mpRoot = Quest::make_shared<nlohmann::json>(nlohmann::json::parse(rOther.WriteJsonString(),nullptr,true,true));
            mpValue = mpRoot.get();
        } else {
            *mpValue = nlohmann::json(nlohmann::json::parse(rOther.WriteJsonString(),nullptr,true,true));
        }

        return *this;
    }


    Parameters Parameters::operator[](const std::string& rEntry){
        return this->GetValue(rEntry);
    }


    Parameters Parameters::operator[](const std::string& rIndex) const{
        return this->GetValue(rIndex);
    }


    Parameters Parameters::operator[](const IndexType Index){
        return this->GetArrayItem(Index);
    }


    Parameters Parameters::operator[](const IndexType Index) const{
        return this->GetArrayItem(Index);
    }


    Parameters& Parameters::operator=(Parameters&& rOther){
        swap(rOther);
        return *this;
    }


    Parameters Parameters::Clone() const{
        return Parameters(WriteJsonString());
    }


    const std::string Parameters::WriteJsonString() const{
        return mpValue->dump();
    }


    Parameters Parameters::GetValue(const std::string& rEntry){
        auto j = mpValue->find(rEntry);
        QUEST_ERROR_IF(j==mpValue->end()) << "Getting a value that does not exist. entry string : " << rEntry << std::endl;
        return Parameters(&(*j),mpRoot);
    }


    Parameters Parameters::GetValue(const std::string& rEntry) const{
        auto j = mpValue->find(rEntry);
        QUEST_ERROR_IF(j==mpValue->end()) << "Getting a value that does not exist. entry string : " << rEntry << std::endl;
        return Parameters(const_cast<nlohmann::json*>(&(*j)),mpRoot);
    }


    void Parameters::SetValue(const std::string& rEntry, const Parameters& rOtherValue){
        QUEST_ERROR_IF(mpValue->find(rEntry)==mpValue->end()) << "Value must exist to be set. Use AddValue instead" << std::endl;
        (*mpValue)[rEntry] = *(rOtherValue.mpValue);
    }


    void Parameters::AddValue(const std::string& rEntry, const Parameters& rOtherValue){
        if(mpValue->find(rEntry) == mpValue->end()){
            (*mpValue)[rEntry] = *(rOtherValue.mpValue);
        } else {
            QUEST_WARNING("Parameters") << "WARNING: Entry " << rEntry << " already defined. Overwriting it" << std::endl;
            SetValue(rEntry,rOtherValue);
        }
    }


    Parameters Parameters::AddEmptyValue(const std::string& rEntry){
        if(this->Has(rEntry) == false){
            return Parameters(&((*mpValue)[rEntry]),mpRoot);
        }
        return this->GetValue(rEntry);
    }


    bool Parameters::RemoveValue(const std::string& rEntry){
        return static_cast<bool>(mpValue->erase(rEntry));
    }


    bool Parameters::RemoveValue(const std::vector<std::string>& rEntries){
        for(const auto& r_entry:rEntries){
            if(!this->Has(r_entry)){
                return false;
            }
        }
        for(const auto& r_entry:rEntries){
            this->RemoveValue(r_entry);
        }
        return true;
    }


    Parameters::json_iterator_proxy Parameters::items() noexcept{
        return mpValue->items();
    }


    Parameters::json_const_iterator_proxy Parameters::items() const noexcept{
        return json_const_iterator_proxy(mpValue->items());
    }


    bool Parameters::Has(const std::string& rEntry) const{
        return mpValue->contains(rEntry);
    }


    bool Parameters::IsNull() const{
        return mpValue->is_null();
    }


    bool Parameters::IsNumber() const{
        return mpValue->is_number();
    }


    bool Parameters::IsDouble() const{
        return mpValue->is_number_float();
    }


    bool Parameters::IsInt() const{
        return mpValue->is_number_integer();
    }


    bool Parameters::IsBool() const{
        return mpValue->is_boolean();
    }


    bool Parameters::IsString() const{
        return mpValue->is_string();
    }


    bool Parameters::IsArray() const{
        return mpValue->is_array();
    }


    bool Parameters::IsStringArray() const{
        if(!mpValue->is_array()){
            return false;
        } else {
            const auto& r_array = (*mpValue);
            for(IndexType i=0;i<r_array.size();++i){
                if(!r_array[i].is_string()){
                    return false;
                }
            }
            return true;
        }
    }


    bool Parameters::IsVector() const{
        if(!mpValue->is_array()){
            return false;
        }

        auto& r_array = (*mpValue);
        for(IndexType i=0;i<r_array.size();++i){
            if(!r_array[i].is_number()){
                return false;
            }
        }
        return true;
    }


    bool Parameters::IsMatrix() const{
        if(!mpValue->is_array()){
            return false;
        }

        const SizeType n_rows = mpValue->size();
        if(n_rows == 0){
            return false;
        }

        for(IndexType i=0;i<n_rows;++i){
            auto& row_i = (*mpValue)[i];
            if(!row_i.is_array()){
                return false;
            }

            IndexType ncols = row_i.size();
            if(ncols != (*mpValue)[0].size()){
                return false;
            }

            for(IndexType j=0;j<ncols;++j){
                if(!row_i[j].is_number()){
                    return false;
                }
            }
        }
        return true;
    }


    bool Parameters::IsSubParameter() const{
        return mpValue->is_object();
    }


    double Parameters::GetDouble() const{
        QUEST_ERROR_IF_NOT(mpValue->is_number()) << "Argument must be a number" << std::endl;
        return mpValue->get<double>();
    }


    int Parameters::GetInt() const{
        QUEST_ERROR_IF_NOT(mpValue->is_number_integer()) << "Argument must be an integer" << std::endl;
        return mpValue->get<int>();
    }


    bool Parameters::GetBool() const{
        QUEST_ERROR_IF_NOT(mpValue->is_boolean()) << "Argument must be a boolean" << std::endl;
        return mpValue->get<bool>();
    }


    std::string Parameters::GetString() const{
        QUEST_ERROR_IF_NOT(mpValue->is_string()) << "Argument must be a string" << std::endl;
        return mpValue->get<std::string>();
    }


    std::vector<std::string> Parameters::GetStringArray() const{
        QUEST_ERROR_IF_NOT(this->IsStringArray()) << "Argument must be a string array" << std::endl;
        std::vector<std::string> result(this->Size());
        for(std::size_t i = 0;i<result.size();++i){
            result[i] = this->GetArrayItem(i).GetString();
        }
        return result;
    }


    Vector Parameters::GetVector() const{
        QUEST_ERROR_IF_NOT(mpValue->is_array()) << "Argument must be a Vector " << std::endl;
        const SizeType size = mpValue->size();

        Vector aux_v(size);

        for(IndexType i=0;i<size;++i){
            QUEST_ERROR_IF_NOT((*mpValue)[i].is_number()) << "Entry " << i << " is not a number!" << std::endl;
            aux_v(i) = (*mpValue)[i].get<double>();
        }
        return aux_v;
    }


    Matrix Parameters::GetMatrix() const{
        QUEST_ERROR_IF_NOT(mpValue->is_array()) << "Argument must be a Matrix " << std::endl;
        const SizeType n_rows = mpValue->size();
        QUEST_ERROR_IF(n_rows == 0) << "Matrix must have at least one row" << std::endl;

        IndexType ncols = 0;
        if((*mpValue)[0].is_array()){
            ncols = (*mpValue)[0].size();
        } 

        Matrix aux_m(n_rows,ncols);

        for(IndexType i=0;i<n_rows;++i){
            auto& row_i = (*mpValue)[i];
            QUEST_ERROR_IF_NOT(row_i.is_array()) << "Row " << i << " is not an array!" << std::endl;
            QUEST_ERROR_IF(row_i.size() != ncols) << "Row " << i << " is wrong size!" << std::endl;

            for(IndexType j=0;j<ncols;++j){
                QUEST_ERROR_IF_NOT(row_i[j].is_number()) << "Entry (" << i << "," << j << ") is not a number!" << std::endl;
                aux_m(i,j) = row_i[j].get<double>();
            }
        }
        return aux_m;
    }


    #define QUEST_DEFINE_PARAMETERS_VALUE_ACCESSORS(TYPE, TYPE_NAME)                  \
        template <> bool Parameters::Is<TYPE>() const{ return this->Is ## TYPE_NAME(); } \
        template <> TYPE Parameters::Get<TYPE>() const{ return this->Get ## TYPE_NAME(); } \
        template <> void Parameters::Set<TYPE>(const TYPE& value){ this->Set ## TYPE_NAME(value); }
    
    QUEST_DEFINE_PARAMETERS_VALUE_ACCESSORS(double, Double)
    QUEST_DEFINE_PARAMETERS_VALUE_ACCESSORS(int, Int)
    QUEST_DEFINE_PARAMETERS_VALUE_ACCESSORS(bool, Bool) 
    QUEST_DEFINE_PARAMETERS_VALUE_ACCESSORS(std::string, String)
    QUEST_DEFINE_PARAMETERS_VALUE_ACCESSORS(std::vector<std::string>, StringArray)
    QUEST_DEFINE_PARAMETERS_VALUE_ACCESSORS(Vector, Vector)
    QUEST_DEFINE_PARAMETERS_VALUE_ACCESSORS(Matrix, Matrix)

    #undef QUEST_DEFINE_PARAMETERS_VALUE_ACCESSORS


    template <>
    bool Parameters::Is<Parameters>() const{
        return this->IsSubParameter();
    }


    void Parameters::SetDouble(const double& value){
        *mpValue=value;
    }


    void Parameters::SetInt(const int& value){
        *mpValue=value;
    }


    void Parameters::SetBool(const bool& value){
        *mpValue=value;
    }


    void Parameters::SetString(const std::string& value){
        *mpValue=value;
    }


    void Parameters::SetStringArray(const std::vector<std::string>& value){
        const SizeType size = value.size();

        nlohmann::json j_array(0.0,size);
        (*mpValue) = j_array;

        for(std::size_t i = 0;i<size;++i){
            (*mpValue)[i] = value[i];
        }
    }


    void Parameters::SetVector(const Vector& value){
        const SizeType size = value.size();

        nlohmann::json j_array(0.0,size);
        (*mpValue) = j_array;

        for(std::size_t i = 0;i<size;++i){
            (*mpValue)[i] = value(i);
        }
    }


    void Parameters::SetMatrix(const Matrix& value){
        const SizeType n_rows = value.size1();
        const SizeType n_cols = value.size2();

        nlohmann::json j_col_array(0.0,n_cols);
        nlohmann::json j_row_array(0.0,n_rows);

        for(IndexType i=0;i<n_rows;++i){
            (*mpValue)[i] = j_col_array;

            for(IndexType j=0;j<n_cols;++j){
                (*mpValue)[i][j] = value(i,j);
            }
        }
    }


    void Parameters::AddDouble(const std::string& rEntry, const double Value){
        Parameters aux_parameters(R"({"value":0.0})");
        aux_parameters["value"].SetDouble(Value);
        this->AddValue(rEntry,aux_parameters["value"]);
    }


    void Parameters::AddInt(const std::string& rEntry, const int Value){
        Parameters aux_parameters(R"({"value":0})");
        aux_parameters["value"].SetInt(Value);
        this->AddValue(rEntry,aux_parameters["value"]);
    }


    void Parameters::AddBool(const std::string& rEntry, const bool Value){
        Parameters aux_parameters(R"({"value":false})");
        aux_parameters["value"].SetBool(Value);
        this->AddValue(rEntry,aux_parameters["value"]);
    }


    void Parameters::AddString(const std::string& rEntry, const std::string& Value){
        Parameters aux_parameters(R"({"value":""})");
        aux_parameters["value"].SetString(Value);
        this->AddValue(rEntry,aux_parameters["value"]);
    }


    void Parameters::AddStringArray(const std::string& rEntry, const std::vector<std::string>& Value){
        Parameters aux_parameters(R"({"value":[]})");
        aux_parameters["value"].SetStringArray(Value);
        this->AddValue(rEntry,aux_parameters["value"]);
    }


    void Parameters::AddVector(const std::string& rEntry, const Vector& Value){
        Parameters aux_parameters(R"({"value":[]})");
        aux_parameters["value"].SetVector(Value);
        this->AddValue(rEntry,aux_parameters["value"]);
    }


    void Parameters::AddMatrix(const std::string& rEntry, const Matrix& Value){
        Parameters aux_parameters(R"({"value":[]})");
        aux_parameters["value"].SetMatrix(Value);
        this->AddValue(rEntry,aux_parameters["value"]);
    }


    Parameters::iterator Parameters::begin(){
        return iterator(mpValue->begin(),mpValue,mpRoot);
    }


    Parameters::iterator Parameters::end(){
        return iterator(mpValue->end(),mpValue,mpRoot);
    }


    Parameters::const_iterator Parameters::begin() const{
        return const_iterator(mpValue->cbegin(),mpValue,mpRoot);
    }


    Parameters::const_iterator Parameters::end() const{
        return const_iterator(mpValue->cend(),mpValue,mpRoot);
    }


    Parameters::SizeType Parameters::Size() const{
        QUEST_ERROR_IF_NOT(mpValue->is_array()) << "Size can only be called on arrays" << std::endl;
        return mpValue->size();
    }


    void Parameters::swap(Parameters& rOther) noexcept{
        std::swap(mpValue,rOther.mpValue);
        std::swap(mpRoot,rOther.mpRoot);
    }


    void Parameters::Reset() noexcept{
        Parameters p;
        swap(p);
    }


    Parameters Parameters::GetArrayItem(const IndexType Index){
        if (mpValue->is_array() == false){
            QUEST_ERROR << "GetArrayItem only makes sense if the value if of Array type" << std::endl;
        } else {
            QUEST_ERROR_IF(Index >= mpValue->size()) << "Index " << Index << " is out of bounds for array of size " << mpValue->size() << std::endl;
            return Parameters(&((*mpValue)[Index]),mpRoot);
        }
    }


    Parameters Parameters::GetArrayItem(const IndexType Index) const{
        if (mpValue->is_array() == false){
            QUEST_ERROR << "GetArrayItem only makes sense if the value if of Array type" << std::endl;
        } else {
            QUEST_ERROR_IF(Index >= mpValue->size()) << "Index " << Index << " is out of bounds for array of size " << mpValue->size() << std::endl;
            return Parameters(const_cast<nlohmann::json*>(&((*mpValue)[Index])),mpRoot);
        }
    }


    void Parameters::SetArrayItem(const IndexType Index, const Parameters& rOtherValue){
        if (mpValue->is_array() == false){
            QUEST_ERROR << "SetArrayItem only makes sense if the value if of Array type" << std::endl;
        } else {
            QUEST_ERROR_IF(Index >= mpValue->size()) << "Index " << Index << " is out of bounds for array of size " << mpValue->size() << std::endl;
            (*mpValue)[Index] = *(rOtherValue.mpValue);
        }
    }


    void Parameters::AddEmptyArray(const std::string& rEntry){
        if(mpValue->find(rEntry) == mpValue->end()){
            nlohmann::json j_array(nlohmann::json::value_t::array);
            (*mpValue)[rEntry] = j_array;
        }
    }


    void Parameters::Append(const double Value){
        QUEST_ERROR_IF_NOT(mpValue->is_array()) << "Append only makes sense if the value if of Array type" << std::endl;
        nlohmann::json j_number_float(nlohmann::json::value_t::number_float);
        j_number_float = Value;
        mpValue->push_back(j_number_float);
    }


    void Parameters::Append(const int Value){
        QUEST_ERROR_IF_NOT(mpValue->is_array()) << "Append only makes sense if the value if of Array type" << std::endl;
        nlohmann::json j_number_integer(nlohmann::json::value_t::number_integer);
        j_number_integer = Value;
        mpValue->push_back(j_number_integer);
    }


    void Parameters::Append(const bool Value){
        QUEST_ERROR_IF_NOT(mpValue->is_array()) << "Append only makes sense if the value if of Array type" << std::endl;
        nlohmann::json j_boolean(nlohmann::json::value_t::boolean);
        j_boolean = Value;
        mpValue->push_back(j_boolean);
    }


    void Parameters::Append(const std::string& Value){
        QUEST_ERROR_IF_NOT(mpValue->is_array()) << "Append only makes sense if the value if of Array type" << std::endl;
        nlohmann::json j_string(nlohmann::json::value_t::string);
        j_string = Value;
        mpValue->push_back(j_string);
    }


    void Parameters::Append(const Vector& Value){
        QUEST_ERROR_IF_NOT(mpValue->is_array()) << "Append only makes sense if the value if of Array type" << std::endl;
        mpValue->push_back(nlohmann::json(Value));
    }


    void Parameters::Append(const Matrix& Value){
        QUEST_ERROR_IF_NOT(mpValue->is_array()) << "Append only makes sense if the value if of Array type" << std::endl;
        const SizeType n_rows = Value.size1();
        const SizeType n_cols = Value.size2();

        nlohmann::json j_col_array(0.0,n_cols);
        nlohmann::json j_row_array(0.0,n_rows);

        for(IndexType i=0;i<n_rows;++i){
            for(IndexType j=0;j<n_cols;++j){
                j_col_array[j] = Value(i,j);
            }
            j_row_array[i] = j_col_array;
        }
        mpValue->push_back(j_row_array);
    }


    void Parameters::Append(const Parameters& rValue){
        QUEST_ERROR_IF_NOT(mpValue->is_array()) << "Append only makes sense if the value if of Array type" << std::endl;
        nlohmann::json j_object = nlohmann::json(nlohmann::json::parse(rValue.WriteJsonString(),nullptr,true,true));
        mpValue->push_back(j_object);
    }


    void Parameters::CopyValuesFromExistingParameters(const Parameters OriginParameters, const std::vector<std::string>& rListParametersToCopy){
        for(const auto& r_value_name : rListParametersToCopy){
            if(OriginParameters.Has(r_value_name)){
                if(this->Has(r_value_name)){
                    QUEST_ERROR << r_value_name << " already defined in destination (check keyword is not duplicated in the list) Parameters:\n\n" << this->PrettyPrintJsonString() << std::endl;
                } else {
                    this->AddValue(r_value_name,OriginParameters[r_value_name]);
                }
            } else {
                QUEST_ERROR << r_value_name << " not found in origin Parameters:\n\n" << OriginParameters.PrettyPrintJsonString() << std::endl;
            }
        }
    }


    void Parameters::RecursivelyFindValue(const nlohmann::json& rBaseValue, const nlohmann::json& rValueToFind) const{
        for (auto itr = rBaseValue.begin(); itr != rBaseValue.end(); ++itr){
            const auto value = itr.value();
            if(&(value) == &rValueToFind){
                const std::string value_string = value.dump();
                QUEST_INFO("Parameters") << "Base = " << PrettyPrintJsonString() << "\nProblematic var name " << itr.key() << " value " << value_string <<std::endl;
            } else {
                if (itr->is_object()){
                    RecursivelyFindValue(value,rValueToFind);
                }
            }
        }
    }


    bool Parameters::IsEquivalentTo(Parameters& rParameters){
        for(auto itr = this->mpValue->begin(); itr != this->mpValue->end(); ++itr){
            const std::string& r_item_name = itr.key();
            bool found = false;

            for(auto& r_parameters_reference: rParameters.items()){
                if(r_item_name == r_parameters_reference.key()){
                    found = true;
                    Parameters subobject = (*this)[r_item_name];
                    Parameters reference_subobject = rParameters[r_item_name];

                    if(itr->is_object()){
                        if(!subobject.IsEquivalentTo(reference_subobject)){
                            return false;
                        }
                    } else {
                        if(itr.value() != r_parameters_reference.value()){
                            return false;
                        }
                    }
                    break;
                }
            }
            
            if(!found){
                return false;
            }
        }

        for(auto& r_parameters:rParameters.items()){
            const std::string& r_item_name = r_parameters.key();
            bool found = false;

            for(auto& r_parameter_reference: this->items()){
                if(r_item_name == r_parameter_reference.key()){
                    found = true;
                    break;
                }
            }
            if(!found){
                return false;
            }
        }

        return true;
    }


    bool Parameters::HasSameKeysAndTypeOfValues(Parameters& rParameters){
        for(auto itr = this->mpValue->begin(); itr != this->mpValue->end(); ++itr){
            const std::string& r_item_name = itr.key();
            bool found = false;

            for(auto& r_parameters_reference: rParameters.items()){
                if(r_item_name == r_parameters_reference.key()){
                    found = true;
                    Parameters subobject = (*this)[r_item_name];
                    Parameters reference_subobject = rParameters[r_item_name];

                    if(itr->is_object()){
                        if(!subobject.HasSameKeysAndTypeOfValues(reference_subobject)){
                            return false;
                        }
                    } else {
                        if(itr.value().type() != r_parameters_reference.value().type()){
                            return false;
                        }
                    }
                    break;
                }
            }
            if(!found){
                return false;
            }
        }

        for(auto& r_parameters:rParameters.items()){
            const std::string& r_item_name = r_parameters.key();
            bool found = false;

            for(auto& r_parameter_reference: this->items()){
                if(r_item_name == r_parameter_reference.key()){
                    found = true;
                    break;
                }
            }
            if(!found){
                return false;
            }
        }
        return true;
    }


    void Parameters::ValidateAndAssignDefaults(const Parameters& rDefaultParameters){
        QUEST_TRY

        this->ValidateDefaults(rDefaultParameters);
        this->AddMissingParameters(rDefaultParameters);

        QUEST_CATCH("")
    }


    void Parameters::ValidateDefaults(const Parameters& rDefaultParameters) const{
        QUEST_TRY

        for(auto itr = this->mpValue->begin(); itr != this->mpValue->end(); ++itr){
            const std::string& r_item_name = itr.key();
            if(!rDefaultParameters.Has(r_item_name)){
                std::stringstream msg;
                msg << "The item with name \"" << r_item_name << "\" is present in this Parameters but NOT in the default values" << std::endl;
                msg << "Hence Validation fails" << std::endl;
                msg << "Parameters being validated are : " << std::endl;
                msg << this->PrettyPrintJsonString() << std::endl;
                msg << "Defaults against which the current parameters are validated are :" << std::endl;
                msg << rDefaultParameters.PrettyPrintJsonString() << std::endl;
                QUEST_ERROR << msg.str() << std::endl;
            }

            bool type_coincides = false;
            auto value_defaults = (rDefaultParameters[r_item_name]).GetUnderlyingRootStarage();
            if(itr->is_number() && value_defaults->is_number()) type_coincides = true;
            if(itr->is_boolean() && value_defaults->is_boolean()) type_coincides = true;
            if(itr->is_null() && value_defaults->is_null()) type_coincides = true;
            if(itr->is_array() && value_defaults->is_array()) type_coincides = true;
            if(itr->is_string() && value_defaults->is_string()) type_coincides = true;
            if(itr->is_object() && value_defaults->is_object()) type_coincides = true;

            if(type_coincides == false){
                std::stringstream msg;
                msg << "******************************************************************************************************" << std::endl;
                msg << "The item with name :\"" << r_item_name << "\" does not have the same type as the corresponding one in the default values" << std::endl;
                msg << "******************************************************************************************************" << std::endl;
                msg << "Parameters being validated are : " << std::endl;
                msg << this->PrettyPrintJsonString() << std::endl;
                msg << "Defaults against which the current parameters are validated are :" << std::endl;
                msg << rDefaultParameters.PrettyPrintJsonString() << std::endl;
            }
        }

        QUEST_CATCH("")
    }


    void Parameters::AddMissingParameters(const Parameters& rDefaultParameters){
        QUEST_TRY

        if(rDefaultParameters.IsSubParameter()){
            for(auto& r_parameter : rDefaultParameters.items()){
                const std::string& r_item_name = r_parameter.key();
                if(mpValue->find(r_item_name) == mpValue->end()){
                    (*mpValue)[r_item_name] = r_parameter.value();
                }
            }
        }
        QUEST_CATCH("")
    }


    void Parameters::RecursivelyValidateAndAssignDefaults(const Parameters& rDefaultParameters){
        QUEST_TRY

        this->RecursivelyValidateDefaults(rDefaultParameters);
        this->RecursivelyAddMissingParameters(rDefaultParameters);

        QUEST_CATCH("")
    }


    void Parameters::RecursivelyValidateDefaults(const Parameters& rDefaultParameters) const{
        QUEST_TRY

        for(auto itr = this->mpValue->cbegin(); itr != this->mpValue->cend(); ++itr){
            const std::string& r_item_name = itr.key();
            if(!rDefaultParameters.Has(r_item_name)){
                std::stringstream msg;
                msg << "The item with name \"" << r_item_name << "\" is present in this Parameters but NOT in the default values" << std::endl;
                msg << "Hence Validation fails" << std::endl;
                msg << "Parameters being validated are : " << std::endl;
                msg << this->PrettyPrintJsonString() << std::endl;
                msg << "Defaults against which the current parameters are validated are :" << std::endl;
                msg << rDefaultParameters.PrettyPrintJsonString() << std::endl;
                QUEST_ERROR << msg.str() << std::endl;
            }

            bool type_coincides = false;
            auto value_defaults = (rDefaultParameters[r_item_name]).GetUnderlyingRootStarage();
            if(itr->is_number() && value_defaults->is_number()) type_coincides = true;
            if(itr->is_boolean() && value_defaults->is_boolean()) type_coincides = true;
            if(itr->is_null() && value_defaults->is_null()) type_coincides = true;
            if(itr->is_array() && value_defaults->is_array()) type_coincides = true;
            if(itr->is_string() && value_defaults->is_string()) type_coincides = true;
            if(itr->is_object() && value_defaults->is_object()) type_coincides = true;

            if(type_coincides == false){
                std::stringstream msg;
                msg << "The item with name :\"" << r_item_name << "\" does not have the same type as the corresponding one in the default values" << std::endl;
                msg << "Parameters being validated are : " << std::endl;
                msg << this->PrettyPrintJsonString() << std::endl;
                msg << "Defaults against which the current parameters are validated are :" << std::endl;
                msg << rDefaultParameters.PrettyPrintJsonString() << std::endl;
                QUEST_ERROR << msg.str() << std::endl;
            }

            if(itr->is_object()){
                Parameters subobject = (*this)[r_item_name];
                Parameters reference_subobject = rDefaultParameters[r_item_name];
                subobject.RecursivelyValidateDefaults(reference_subobject);
            }
        }
        QUEST_CATCH("")
    }


    void Parameters::RecursivelyAddMissingParameters(const Parameters& rDefaultParameters){
        QUEST_TRY

        if(rDefaultParameters.IsSubParameter()){
            for(auto itr = rDefaultParameters.mpValue->begin(); itr != rDefaultParameters.mpValue->end(); ++itr){
                const std::string& r_item_name = itr.key();
                if(mpValue->find(r_item_name) == mpValue->end()){
                    (*mpValue)[r_item_name] = itr.value();
                }   

                if(itr->is_object()){
                    Parameters subobject = (*this)[r_item_name];
                    Parameters reference_subobject = rDefaultParameters[r_item_name];
                    subobject.RecursivelyAddMissingParameters(reference_subobject);
                }
            }
        }
        QUEST_CATCH("")
    }


    void Parameters::save(Serializer& rSerializer) const{
        rSerializer.save("Data",this->WriteJsonString());
    }


    void Parameters::load(Serializer& rSerializer){
        std::string data;
        rSerializer.load("Data",data);
        *this = Parameters(data);
    }


    Parameters::Parameters(nlohmann::json* pValue, Quest::shared_ptr<nlohmann::json> pRoot) : mpValue(pValue), mpRoot(pRoot){}


    Parameters::Parameters(json_iterator itValue, nlohmann::json* pValue, Quest::shared_ptr<nlohmann::json> pRoot):
        mpValue(nullptr), mpRoot(pRoot){
        if(itValue != pValue->end()){
            mpValue = &(*itValue);
        }
    }


    Parameters::Parameters(json_const_iterator itValue, nlohmann::json* pValue, Quest::shared_ptr<nlohmann::json> pRoot):
        mpValue(nullptr), mpRoot(pRoot){
        if(itValue != pValue->cend()){
            mpValue = const_cast<nlohmann::json*>(&(*itValue));
        }
    }


    nlohmann::json* Parameters::GetUnderlyingStorage(){
        return mpValue;
    }


    nlohmann::json* Parameters::GetUnderlyingStorage() const{
        return mpValue;
    }


    void Parameters::SetUnderlyingStorage(nlohmann::json* pValue){
        mpValue = pValue;
    }


    Quest::shared_ptr<nlohmann::json> Parameters::GetUnderlyingRootStarage(){
        return mpRoot;
    }


    Quest::shared_ptr<nlohmann::json> Parameters::GetUnderlyingRootStarage() const{
        return mpRoot;
    }


    void Parameters::SetUnderlyingRootStorage(Quest::shared_ptr<nlohmann::json> pRoot){
        mpRoot = pRoot;
    }


    void Parameters::InternalSetValue(const Parameters& rOtherValue){
        delete[] mpValue;
        mpValue = new nlohmann::json(nlohmann::json::parse(rOtherValue.WriteJsonString(),nullptr,true,true));
    }

} // namespace Quest