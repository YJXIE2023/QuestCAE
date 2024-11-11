/*-------------------------
封装OpenMP中归约操作的函数
-------------------------*/

#ifndef QUEST_REDUCTION_UTILITIES_HPP
#define QUEST_REDUCTION_UTILITIES_HPP

// 系统头文件
#include <tuple>
#include <limits>
#include <algorithm>
#include <mutex>

// 项目头文件
#include "includes/define.hpp"
#include "utilities/atomic_utilities.hpp"
#include "utilities/parallel_utilities.hpp"

namespace Quest{

    namespace Internals{

        template<typename TObjectType>
        struct NullInitialized{
            static TObjectType Get(){
                return TObjectType();
            }
        };


        template<typename TValueType, std::size_t ArraySize>
        struct NullInitialized<Array1d<TValueType, ArraySize>>{
            static Array1d<TValueType, ArraySize> Get(){
                Array1d<TValueType, ArraySize> result;
                std::fill_n(result.begin(), result, NullInitialized<TValueType>::Get());
                return result;
            }
        };


        template<typename TDataType, typename TReturnType = TDataType>
        class SumReduction{
            public:
                using value_type = TDataType;
                using result_type = TReturnType;

                TReturnType mValue = Internals::NullInitialized<TReturnType>::Get();

                TReturnType GetValue() const{
                    return mValue;
                }

                void LocalReduce(const TDataType& value){
                    mValue += value;
                }

                void ThreadSafeReduce(const SumReduction<TDataType, TReturnType>& other){
                    AtomicAdd(mValue, other.mValue);
                }
        };


        template<typename TDataType, typename TReturnType = TDataType>
        class SubReduction{
            public:
                using value_type = TDataType;
                using result_type = TReturnType;

                TReturnType mValue = Internals::NullInitialized<TReturnType>::Get();

                TReturnType GetValue() const{
                    return mValue;
                }

                void LocalReduce(const TDataType& value){
                    mValue -= value;
                }

                void ThreadSafeReduce(const SubReduction<TDataType, TReturnType>& other){
                    AtomicAdd(mValue, -other.mValue);
                }
        };


        template<typename TDataType, typename TReturnType = TDataType>
        class MaxReduction{
            public:
                using value_type = TDataType;
                using result_type = TReturnType;

                TReturnType mValue = std::numeric_limits<TReturnType>::lowest();

                TReturnType GetValue() const{
                    return mValue;
                }

                void LocalReduce(const TDataType& value){
                    mValue = std::max(mValue, value);
                }

                void ThreadSafeReduce(const MaxReduction<TDataType, TReturnType>& other){
                    QUEST_CRITICAL_SECTION
                    LocalReduce(other.mValue);
                }
        };


        template<typename TDataType, typename TReturnType = TDataType>
        class AbsMaxReduction{
            public:
                using value_type = TDataType;
                using result_type = TReturnType;

                TReturnType mValue = std::numeric_limits<TReturnType>::lowest();

                TReturnType GetValue() const{
                    return mValue;
                }

                void LocalReduce(const TDataType& value){
                    mValue = (std::abs(mValue) < std::abs(value)) ? value : mValue;
                }

                void ThreadSafeReduce(const AbsMaxReduction<TDataType, TReturnType>& other){
                    QUEST_CRITICAL_SECTION
                    LocalReduce(other.mValue);
                }
        };


        template<typename TDataType, typename TReturnType = TDataType>
        class MinReduction{
            public:
                using value_type = TDataType;
                using result_type = TReturnType;

                TReturnType mValue = std::numeric_limits<TReturnType>::max();

                TReturnType GetValue() const{
                    return mValue;
                }

                void LocalReduce(const TDataType& value){
                    mValue = std::min(mValue, value);
                }

                void ThreadSafeReduce(const MinReduction<TDataType, TReturnType>& other){
                    QUEST_CRITICAL_SECTION
                    LocalReduce(other.mValue);
                }
        };


        template<typename TDataType, typename TReturnType = TDataType>
        class AbsMinReduction{
            public:
                using value_type = TDataType;
                using result_type = TReturnType;

                TReturnType mValue = std::numeric_limits<TReturnType>::max();

                TReturnType GetValue() const{
                    return mValue;
                }

                void LocalReduce(const TDataType& value){
                    mValue = (std::abs(mValue) > std::abs(value)) ? value : mValue;
                }

                void ThreadSafeReduce(const AbsMinReduction<TDataType, TReturnType>& other){
                    QUEST_CRITICAL_SECTION
                    LocalReduce(other.mValue);
                }
        };


        template<typename TDataType, typename TReturnType = std::vector<TDataType>>
        class AccumReduction{
            public:
                using value_type = TDataType;
                using result_type = TReturnType;

                TReturnType mValue = TReturnType();

                TReturnType GetValue() const{
                    return mValue;
                }

                void LocalReduce(const TDataType& value){
                    mValue.insert(mValue.end(), value);
                }

                void ThreadSafeReduce(const AccumReduction<TDataType, TReturnType>& other){
                    QUEST_CRITICAL_SECTION
                    std::copy(other.mValue.begin(), other.mValue.end(), std::inserter(mValue, mValue.end()));
                }
        };


        template<typename TDataType, typename TReturnType = std::vector<TDataType>>
        class FilteredAccumReduction : public AccumReduction<TDataType, TReturnType>{
            public:
                void LocalReduce(const std::pair<bool, TDataType> ValuePair){
                    if(ValuePair.first){
                        this->mValue.push_back(ValuePair.second);
                    }
                }
        };


        template<typename MapType>
        class MapReduction{
            public:
                using value_type = MapType;
                using return_type = MapType;

                return_type mValue;

                return_type GetValue() const{
                    return mValue;
                }

                void LocalReduce(const MapType& value){
                    mValue.emplace(value);
                }

                void ThreadSafeReduce(MapReduction<MapType>& other){
                    QUEST_CRITICAL_SECTION
                    mValue.merge(other.mValue);
                }
        };


        template<typename... Reducer>
        class CombinedReduction{
            public:
                using value_type = std::tuple<typename Reducer::value_type...>;
                using return_type = std::tuple<typename Reducer::return_type...>;

                std::tuple<Reducer...> mChild;
            
            public:
                CombinedReduction() {}


                return_type GetValue(){
                    return_type return_value;
                    fill_value<0>(return_value);
                    return return_value;
                }


                template<int I, typename T>
                typename std::enable_if<(I < sizeof...(Reducer)), void>::type fill_value(T& value){
                    std::get<I>(value) = std::get<I>(mChild).GetValue();
                    fill_value<I+1>(value);
                }


                template<int I, typename T>
                typename std::enable_if<(I == sizeof...(Reducer)), void>::type fill_value(T& value){}


                template<typename... T>
                void LocalReduce(const std::tuple<T...>&&v){
                    reduce_value<0>(v);
                }


                void ThreadSafeReduce(const CombinedReduction &other){
                    reduce_global<0>(other);
                }

            privete:
                template<int I, typename T>
                typename std::enable_if<I < (sizeof...(Reducer)), void>::type reduce_local(T&& value){
                    std::get<I>(mChild).LocalReduce(std::get<I>(value));
                    redece_local<I+1>(std::forward<T>(value));
                };


                template<int I, typename T>
                typename std::enable_if<I == (sizeof...(Reducer)), void>::type reduce_local(T&& value){};


                template<int I>
                typename std::enable_if<I < (sizeof...(Reducer)), void>::type reduce_global(const CombinedReduction& other){
                    std::get<I>(mChild).ThreadSafeReduce(std::get<I>(other.mChild));
                    reduce_global<I+1>(other);
                }


                template<int I>
                typename std::enable_if<I == (sizeof...(Reducer)), void>::type reduce_global(const CombinedReduction& other){}

        };

    } // namespace Internals

} // namespace Quest

#endif //QUEST_REDUCTION_UTILITIES_HPP