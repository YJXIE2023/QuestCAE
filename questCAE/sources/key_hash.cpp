/*-------------------------------------------------
key_hash.hpp头文件的实现代码
-------------------------------------------------*/

// 项目头文件
#include "includes/key_hash.hpp"
#include "container/variable_data.hpp"

namespace Quest{

    HashType VariableHasher::operator() (const VariableData& var) const{
        return var.Key();
    }


    bool VariableComparator::operator() (const VariableData& var1, const VariableData& var2) const{
        return var1.Key() == var2.Key();
    }


    HashType VariableHasher::operator() (const VariableData& rVariable) const{
        return rVariable.Key();
    }

    bool pVariableComparator::operator() (const VariableData* pVar1, const VariableData* pVar2) const{
        return pVar1->Key() == pVar2->Key();
    }

} // namespace Quest