/*------------------------------------------
registry_item.hpp头文件的实现
------------------------------------------*/

// 项目头文件
#include "includes/registry_item.hpp"

namespace Quest{

    RegistryItem::SubRegistryItemType& RegistryItem::GetSubRegistryItemMap(){
        QUEST_ERROR_IF(HasValue()) << "Item " << Name() << " has value and cannot be interated." << std::endl;
        return *(std::any_cast<SubRegistryItemPointerType>(mpValue));
    }

    RegistryItem::SubRegistryItemType& RegistryItem::GetSubRegistryItemMap() const{
        QUEST_ERROR_IF(HasValue()) << "Item " << Name() << " has value and cannot be interated." << std::endl;
        return *(std::any_cast<SubRegistryItemPointerType>(mpValue));
    }

    RegistryItem::SubRegistryItemType::iterator RegistryItem::begin(){
        return GetSubRegistryItemMap().begin();
    }

    RegistryItem::SubRegistryItemType::const_iterator RegistryItem::cbegin() const{
        return GetSubRegistryItemMap().cbegin();
    }

    RegistryItem::SubRegistryItemType::iterator RegistryItem::end(){
        return GetSubRegistryItemMap().end();
    }

    RegistryItem::SubRegistryItemType::const_iterator RegistryItem::cend() const{
        return GetSubRegistryItemMap().cend();
    }

    RegistryItem::KeyReturnConstIterator RegistryItem::KeyConstBegin() const{
        return KeyReturnConstIterator(GetSubRegistryItemMap().cbegin());
    }

    RegistryItem::KeyReturnConstIterator RegistryItem::KeyConstEnd() const{
        return KeyReturnConstIterator(GetSubRegistryItemMap().cend());
    }

    std::string RegistryItem::Info() const{
        return mName + " RegistryItem ";
    }

    void RegistryItem::PrintInfo(std::ostream& rOstream) const{
        rOstream << Info();
    }

    void RegistryItem::PrintData(std::ostream& rOstream) const{
        if (HasValue()){
            rOstream << this->GetValueString();
        } else {
            for(auto& item : GetSubRegistryItemMap()){
                rOstream << item.second->GetValueString() << std::endl;
            }
        }
    }

    std::string RegistryItem::GetValueString() const{
        return (this->*(this->mGetValueStringMethod))();
    }

    std::string RegistryItem::ToJson(const std::string& rTabSpacing, const std::size_t Leval) const{
        std::string tabbing;
        for(std::size_t i = 0; i < Leval; ++i){
            tabbing += rTabSpacing;
        }

        std::stringstream buffer;

        if (Leval == 0){
            buffer << "{" << std::endl;
        }

        if (HasValue()){
            buffer << tabbing << "\"" << mName << "\": \"" << this->GetValueString() << "\"";
        } else {
            buffer << tabbing << "\"" << mName << "\": {" << std::endl;

            for (auto& item : GetSubRegistryItemMap()){
                buffer << std::endl;
                buffer << item.second->ToJson(rTabSpacing, Leval + 1);
                buffer << ",";
            }

            if(HasItems()){
                buffer.seekp(-1, std::ios_base::end);
                buffer << std::endl << tabbing;
            }
            buffer << "}";
        }

        if (Leval == 0){
            buffer << std::endl << "}";
        }

        return buffer.str();
    }

    bool RegistryItem::HasValue() const{
        return (mpValue.type() != typeid(SubRegistryItemPointerType));
    }

    bool RegistryItem::HasItem(const std::string& rName) const{
        if(!HasValue()){
            SubRegistryItemType& r_map = GetSubRegistryItemMap();
            return (r_map.find(rName) != r_map.end());
        } else {
            return false;
        }
    }

    bool RegistryItem::HasItems() const{
        return (!HasValue() && !GetSubRegistryItemMap().empty());
    }

    std::size_t RegistryItem::size(){
        return GetSubRegistryItemMap().size();
    }

    const RegistryItem& RegistryItem::GetItem(const std::string& rName) const {
        SubRegistryItemType& r_map = GetSubRegistryItemMap();        
        auto it = r_map.find(rName);
        QUEST_ERROR_IF(it == r_map.end()) << "The RegistryItem " << this->Name() << " does not have an item with name " << rName << std::endl;
        return *(it->second);
    }

    RegistryItem& RegistryItem::GetItem(const std::string& rName){
        SubRegistryItemType& r_map = GetSubRegistryItemMap();
        auto it = r_map.find(rName);
        QUEST_ERROR_IF(it == r_map.end()) << "The RegistryItem " << this->Name() << " does not have an item with name " << rName << std::endl;
        return *(it->second);
    }

    void RegistryItem::RemoveItem(const std::string& rName){
        SubRegistryItemType& r_map = GetSubRegistryItemMap();
        auto it = r_map.find(rName);
        QUEST_ERROR_IF(it == r_map.end()) << "The RegistryItem " << this->Name() << " does not have an item with name " << rName << std::endl;
        r_map.erase(it);
    }

} // namespace Quest