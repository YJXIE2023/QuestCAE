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

    /**
     * @brief 此方法为输入值创建一个“唯一”哈希值
     * @details 该方法来源于 Boost
     * @tparam TClassType 要进行哈希处理的类的类型
     * @param Seed 用于生成哈希值的种子
     * @param Value 需要进行哈希处理的值
     */
    template<typename TClassType>
    inline void HashCombine(HashType& seed, const TClassType& value){
        std::hash<TClassType> hasher;
        seed ^= hasher(value) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    }


    /**
     * @brief 此方法将哈希值逐步组合直到最后一个类，以生成对应的种子
     * @tparam TClassType 要进行哈希处理的类的类型
     * @param First 第一个用于比较的类
     * @param Last 最后一个用于比较的类
     * @return 生成的种子值
     */
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

    /**
     * @brief 这是一个用于两个类之间通用的键比较器
     * @tparam TClassType 要进行哈希处理的类的类型
     */
    template<typename TClassType>
    struct KeyComparorRange{
        /**
         * @brief 这是重载的 () 操作符
         * @param first 要比较的第一个类
         * @param second 要比较的第二个类
         */
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

    /**
     * @brief 这是一个通用的哈希器
     * @tparam TClassType 要进行哈希处理的类的类型
     */
    template<typename TClassType>
    struct KeyHasherRange{
        HashType operator()(const TClassType& value) const{
            return HashRange(value.begin(), value.end());
        }
    };

    /**
     * @brief 变量哈希器
     */
    struct QUEST_API(QUEST_CORE) VariableHasher{
        HashType operator()(const VariableData& rVariable) const;
    };

    /**
     * @brief 两个变量的比较器
     */
    struct QUEST_API(QUEST_CORE) VariableComparator{
        bool operator()(const VariableData& first, const VariableData& second) const;
    };

    /**
     * @brief 变量指针的哈希器
     */
    struct QUEST_API(QUEST_CORE) pVariableHasher{
        HashType operator()(const VariableData* pVariable) const;
    };

    /**
     * @brief 两个变量指针的比较器
     */
    struct QUEST_API(QUEST_CORE) pVariableComparator{
        bool operator()(const VariableData* first, const VariableData* second) const;
    };

    /**
     * @brief 索引对象的哈希器
     */
    template<typename TIndexedObject>
    struct IndexedObjectHasher{
        HashType operator()(const TIndexedObject& rIndexedObject) const{
            return rIndexedObject.Id();
        }
    };

    /**
     * @brief 索引对象的比较器
     */
    template<typename TIndexedObject>
    struct IndexedObjectComparator{
        bool operator()(const TIndexedObject& rFirst, const TIndexedObject& rSecond) const{
            return rFirst.Id() == rSecond.Id();
        }
    };

    /**
     * @brief 索引对象的指针的哈希器
     */
    template<typename TIndexedObject>
    struct IndexedObjectPointerHasher{
        HashType operator()(const TIndexedObject pIndexedObject) const{
            return pIndexedObject->Id();
        }
    };

    /**
     * @brief 索引对象的指针的比较器
     */
    template<typename TIndexedObject>
    struct IndexedObjectPointerComparator{
        bool operator()(const TIndexedObject pFirst, const TIndexedObject pSecond) const{
            return pFirst->Id() == pSecond->Id();
        }
    };

    /**
     * @brief 共享指针的哈希器
     */
    template<typename TSharedPointer>
    struct SharedPointerHasher{
        HashType operator()(const TSharedPointer& pSharedPointer) const{
            return reinterpret_cast<HashType>(pSharedPointer.get());
        }
    };

    /**
     * @brief 共享指针的比较器
     */
    template<typename TSharedPointer>
    struct SharedPointerComparator{
        bool operator()(const TSharedPointer& pFirst, const TSharedPointer& pSecond) const{
            return pFirst.get() == pSecond.get();
        }
    };

    /**
     * @brief 向量索引的哈希器
     */
    template<typename TVectorIndex>
    struct VectorIndexHasher{
        HashType operator()(const TVectorIndex& rVectorIndex) const{
            return HashRange(rVectorIndex.begin(), rVectorIndex.end());
        }
    };

    /**
     * @brief 向量索引的比较器
     */
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

    /**
     * @brief 自由度指针的哈希器
     */
    struct DofPointerHasher{
        HashType operator() (const Dof<double>::Pointer& pDof) const{
            HashType seed = 0;
            HashCombine(seed, pDof->Id());
            HashCombine(seed, (pDof->GetVariable()).Key());
            return seed;
        }
    };

    /**
     * @brief 自由度指针的比较器
     */
    struct DofPointerComparor{
        bool operator() (const Dof<double>::Pointer& pFirst, const Dof<double>::Pointer& pSecond) const{
            return pFirst->Id() == pSecond->Id() && (pFirst->GetVariable()).Key() == (pSecond->GetVariable()).Key();
        }
    };

    /**
     * @brief pair 类型的哈希器
     * @details 用于边的ID比较
     */
    template<typename TFirst, typename TSecond>
    struct PairHasher{
        HashType operator()(const std::pair<TFirst, TSecond>& rPair) const{
            HashType seed = 0;
            HashCombine(seed, rPair.first);
            HashCombine(seed, rPair.second);
            return seed;
        }
    };

    /**
     * @brief pair 类型的比较器
     * @details 用于B&S
     */
    template<typename TFirst, typename TSecond>
    struct PairComparor{
        bool operator()(const std::pair<TFirst, TSecond>& rFirst, const std::pair<TFirst, TSecond>& rSecond) const{
            return std::get<0>(rFirst) == std::get<0>(rSecond) && std::get<1>(rFirst) == std::get<1>(rSecond);
        }
    };

} // namespace Quest

namespace std{

    /**
     * @brief 这是一个用于对偶元素的哈希器
     * @details 例如，用于边的ID
     * @tparam T1 对偶元素的第一个类型
     * @tparam T2 对偶元素的第二个类型
     * @note 这是为了在无序映射中使用对偶元素作为键
     */
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