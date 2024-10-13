/*---------------------------------------------------------
处理hash值的类和函数
---------------------------------------------------------*/

#ifndef QUEST_KEY_HASH_HPP
#define QUEST_KEY_HASH_HPP

// 项目头文件
#include "includes/dof.hpp"

namespace Quest{

    using IndexType = std::size_t;
    using HashType = std::size_t;

    // 结合给定的哈希种子 (Seed) 和输入值 (Value)，生成一个新的、更"独特"的哈希值
    template<typename TClassType>
    inline void HashCombine(HashType& seed, const TClassType& value){
        std::hash<TClassType> hasher;
        seed ^= hasher(value) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    }

    // 遍历从 First 到 Last 的一系列对象，并依次对每个对象进行哈希计算，
    // 然后使用 HashCombine 方法将这些对象的哈希值组合起来，最终返回一个综合的哈希种子值 (seed)
    template<typename TClassType>
    inline HashType HashRange(TClassType First, TClassType Last){
        HashType seed = 0;

        while(First!= Last){
            HashCombine(seed, *First);
            ++First;
        }

        return seed;
    }

    class VariableData;

    // 模板结构体，定义了一个比较操作符 operator()，
    // 用于比较两个对象（first 和 second）是否相等
    template<typename TClassType>
    struct KeyComparorRange{
        bool operator()(const TClassType& first, const TClassType& second) const{
            if (first.size()!= second.size()){
                return false;
            }

            auto it_first = first.begin();
            auto it_second = second.begin();

            while(it_first!= first.end()){
                if (*it_first!= *it_second){
                    return false;
                }
                if(it_first != first.end()){
                    ++it_first;
                    ++it_second;
                }
            }

            return true;
        }
    };

    // 定义了一个泛型模板结构体 KeyHasherRange
    // 用于计算某种类型 TClassType 的对象的哈希值
    template<typename TClassType>
    struct KeyHasherRange{
        HashType operator()(const TClassType& value) const{
            return HashRange(value.begin(), value.end());
        }
    };

    // 定义了一个结构体 VariableHasher，用于对 VariableData 类型的对象进行哈希计算
    struct QUEST_API(QUEST_CORE) VariableHasher{
        HashType operator()(const VariableData& rVariable) const;
    };

    // 定义了一个结构体 VariableComparator，它用于比较两个 VariableData 类型的变量，以判断它们是否相等
    struct QUEST_API(QUEST_CORE) VariableComparator{
        bool operator()(const VariableData& first, const VariableData& second) const;
    };

    // 定义了一个结构体 pVariableHasher，它是一个哈希函数对象（仿函数），
    // 用于对指向 VariableData 类型的指针进行哈希计算
    struct QUEST_API(QUEST_CORE) pVariableHasher{
        HashType operator()(const VariableData* pVariable) const;
    };

    // 定义了一个结构体 pVariableComparator，它是一个仿函数（函数对象），
    // 用于比较两个 VariableData 类型的指针，判断它们是否相等
    struct QUEST_API(QUEST_CORE) pVariableComparator{
        bool operator()(const VariableData* first, const VariableData* second) const;
    };

    // 定义了一个名为 IndexedObjectHasher 的模板结构体，它是一个哈希函数，用于对索引对象进行哈希操作
    template<typename TIndexedObject>
    struct IndexedObjectHasher{
        HashType operator()(const TIndexedObject& rIndexedObject) const{
            return rIndexedObject.Id();
        }
    };

    // 定义了一个名为 IndexedObjectComparator 的模板结构体，用于比较两个索引对象是否相等
    template<typename TIndexedObject>
    struct IndexedObjectComparator{
        bool operator()(const TIndexedObject& rFirst, const TIndexedObject& rSecond) const{
            return rFirst.Id() == rSecond.Id();
        }
    };

    // 定义了一个名为 IndexedObjectPointerHasher 的模板结构体，用于对指向索引对象的指针进行哈希操作
    template<typename TIndexedObject>
    struct IndexedObjectPointerHasher{
        HashType operator()(const TIndexedObject pIndexedObject) const{
            return pIndexedObject->Id();
        }
    };

    // 定义了一个名为 IndexedObjectPointerComparator 的模板结构体，用于比较指向索引对象的指针
    template<typename TIndexedObject>
    struct IndexedObjectPointerComparator{
        bool operator()(const TIndexedObject pFirst, const TIndexedObject pSecond) const{
            return pFirst->Id() == pSecond->Id();
        }
    };

    // 定义了一个名为 SharedPointerHasher 的模板结构体，用于计算共享指针（shared_ptr）的哈希值
    template<typename TSharedPointer>
    struct SharedPointerHasher{
        HashType operator()(const TSharedPointer& pSharedPointer) const{
            return reinterpret_cast<HashType>(pSharedPointer.get());
        }
    };

    // 定义了一个名为 SharedPointerComparator 的模板结构体，用于比较两个共享指针（shared_ptr）是否相等
    template<typename TSharedPointer>
    struct SharedPointerComparator{
        bool operator()(const TSharedPointer& pFirst, const TSharedPointer& pSecond) const{
            return pFirst.get() == pSecond.get();
        }
    };

    // 定义了一个名为 VectorIndexHasher 的模板结构体，用于计算给定索引向量的哈希值
    template<typename TVectorIndex>
    struct VectorIndexHasher{
        HashType operator()(const TVectorIndex& rVectorIndex) const{
            return HashRange(rVectorIndex.begin(), rVectorIndex.end());
        }
    };

    // 定义了一个名为 VectorIndexComparor 的模板结构体，用于比较两个索引向量是否相等
    template<typename TVectorIndex>
    struct VectorIndexComparor{
        bool operator()(const TVectorIndex& rFirst, const TVectorIndex& rSecond) const{
            if (rFirst.size() != rSecond.size()){
                return false;
            }

            for (IndexType i = 0; i < rFirst.size(); ++i){
                if (rFirst[i] != rSecond[i]){
                    return false;
                }
            }

            return true;
        }
    };

    // 定义了一个名为 DofPointerHasher 的结构体，
    // 专门用于为指向自由度（DoF，Degree of Freedom）对象的指针计算哈希值
    struct DofPointerHasher{
        HashType operator() (const Dof<double>::Pointer& pDof) const{
            HashType seed = 0;
            HashCombine(seed, pDof->Id());
            HashCombine(seed, (pDof->GetVariable()).Key());
            return seed;
        }
    };

    // 定义了一个名为 DofPointerComparor 的结构体，
    // 用于比较两个自由度（DoF，Degree of Freedom）指针的相等性
    struct DofPointerComparor{
        bool operator() (const Dof<double>::Pointer& pFirst, const Dof<double>::Pointer& pSecond) const{
            return pFirst->Id() == pSecond->Id() && (pFirst->GetVariable()).Key() == (pSecond->GetVariable()).Key();
        }
    };

    // 定义了一个名为 PairHasher 的模板结构体，用于计算一对值的哈希值
    template<typename TFirst, typename TSecond>
    struct PairHasher{
        HashType operator()(const std::pair<TFirst, TSecond>& rPair) const{
            HashType seed = 0;
            HashCombine(seed, rPair.first);
            HashCombine(seed, rPair.second);
            return seed;
        }
    };

    // 定义了一个名为 PairComparor 的模板结构体，用于比较两个索引对（std::pair 类型的对象）是否相等
    template<typename TFirst, typename TSecond>
    struct PairComparor{
        bool operator()(const std::pair<TFirst, TSecond>& rFirst, const std::pair<TFirst, TSecond>& rSecond) const{
            return std::get<0>(rFirst) == std::get<0>(rSecond) && std::get<1>(rFirst) == std::get<1>(rSecond);
        }
    };

} // namespace Quest

namespace std{

    // 定义了一个自定义哈希函数，用于计算 std::pair<T1, T2> 类型的对象的哈希值。
    // 该哈希函数特别适用于将 std::pair 作为键存储在无序映射（如 std::unordered_map）中
    template<typename T1, typename T2>
    struct hash<std::pair<T1, T2>>{
        std::size_t operator()(const std::pair<T1, T2>& rPair) const{
            std::size_t seed = 0;
            const std::size_t h1 = std::hash<T1>()(rPair.first);
            const std::size_t h2 = std::hash<T2>()(rPair.second);
            Quest::HashCombine(seed, h1);
            Quest::HashCombine(seed, h2);
            return seed;
        }
    };

} // namespace std

#endif //QUEST_KEY_HASH_HPP