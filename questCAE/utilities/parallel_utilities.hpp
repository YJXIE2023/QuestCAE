/*--------------------------------
用于共享内存并行计算管理与调度的工具类
-----------------------------------*/

#ifndef QUEST_PARALLEL_UTILITIES_HPP
#define QUEST_PARALLEL_UTILITIES_HPP

// 系统头文件
#include <iostream>
#include <array>
#include <iterator>
#include <vector>
#include <tuple>
#include <cmath>
#include <limits>
#include <future>
#include <thread>
#include <mutex>
#include <type_traits>

// 外部头文件
#ifdef QUEST_SMP_OPENMP
    #include <omp.h>
#endif

// 项目头文件
#include "includes/define.hpp"
#include "includes/global_variables.hpp"
#include "includes/lock_object.hpp"

// 宏定义
#define QUEST_CRITICAL_SECTION const std::lock_guard scope_lock(ParallelUtilities::GetGlobalLock());

#define QUEST_PREPARE_CATCH_THREAD_EXCEPTION std::stringstream err_stream;

#define QUEST_CATCH_THREAD_EXCEPTION   \
} catch(Exception& e) {                \
    QUEST_CRITICAL_SECTION            \
    err_stream << "Thread #" << i << " caught exception: " << e.what(); \
} catch(std::exception& e) {          \
    QUEST_CRITICAL_SECTION            \
    err_stream << "Thread #" << i << " caught exception: " << e.what(); \
} catch(...) {                        \ 
    QUEST_CRITICAL_SECTION            \ 
    err_stream << "Thread #" << i << " caught unknown exception"; \
}

#define QUEST_CHECK_AND_THROW_THREAD_EXCEPTION \
const std::string& err_msg = err_stream.str(); \
QUEST_ERROR_IF_NOT(err_msg.empty()) << "The following errors occured in a parallel region!\n" <<err_msg << std::endl;

// 命名空间
namespace Quest{
    //---------------------------------
    // 共享内存并行计算接口
    //---------------------------------
    class QUEST_API ParallelUtilities{
        public:
            [[nodiscard]] static int GetNumThreads();

            static void SetNumThreads(const int NumThreads);

            [[nodiscard]] static int GetNumProcs();

            [[nodiscard]] static LockObject& GetGlobalLock();

        protected:

        private:
            static LockObject* mspGlobalLock;
            static int* mspNumThreads;

            ParallelUtilities() = delete;

            static int InitializeNumberOfThreads();

            static int& GetNumberOfThreads();

    };

    //---------------------------------
    // 数据分块类
    //---------------------------------
    template<typename TIterator, int MaxThreads=Globals::MaxAllowedThreads>
    class BlockPartition{
        public:
            BlockPartition(TIerator it_begin, TIerator it_end, int Nchunks = ParallelUtilities::GetNumThreads()){
                static_assert(
                    std::is_same_v<typename std::iterator_traits<TIterator>::iterator_category, std::random_access_iterator_tag>,
                    "BlockPartititon requires random access iterator to divide the input range into partitions"
                );
                QUEST_ERROR_IF(Nchunks < 1) << "Number of chunks must be > 0 (and not " << Nchunks << ")" << std::endl;

                const std::ptrdiff_t size_container = it_end-it_begin;

                if (size_container == 0){
                    mNchunks = Nchunks;
                } else {
                    mNchunks = std::min(static_cast<int>(size_container),Nchunks);
                }
                const std::ptrdiff_t block_partition_size = size_container/mNchunks;
                mBlockPartition[0] = it_begin;
                mBlockPartition[mNchunks] = it_end
                for(int i=1,i<mNchunks,i++){
                    mBlockPartition[i] = mBlockPartition[i-1]+block_partition_size;
                }
            }

            template<typename TUnaryFunction>
            inline void for_each(TUnaryFunction&& f){
                QUEST_PREPARE_CATCH_THREAD_EXCEPTION

                #pragma omp parallel for
                for(int i=0;i<mNchunks;i++){
                    QUEST_TRY
                    for(auto it=mBlockPartition[i];it!=mBlockPartition[i+1];it++){
                        f(*it);
                    }
                    QUEST_CATCH_THREAD_EXCEPTION
                }
                QUEST_CHECK_AND_THROW_THREAD_EXCEPTION
            }

            template<typename TReducer,typename TUnaryFunction>
            [[nodiscard]] inline typename TReducer::return_type for_each(TUnaryFunction&& f){
                QUEST_PREPARE_CATCH_THREAD_EXCEPTION

                TReducer global_reducer;
                #pragma omp parallel for
                for(int i=0;i<mNchunks;i++){
                    QUEST_TRY
                    TReducer local_reducer;
                    for(auto it=mBlockPartition[i];it!=mBlockPartition[i+1];it++){
                        local_reducer.LocalReduce(f(*it));
                    }
                    global_reducer.ThreadSafeReduce(local_reducer);
                    QUEST_CATCH_THREAD_EXCEPTION
                }
                QUEST_CHECK_AND_THROW_THREAD_EXCEPTION

                return global_reducer.GetValue();
            }

            template<typename TThreadLocalStorage,typename TFunction>
            inline void for_each(const TThreadLocalStorage& rThreadLocalStoragePrototype, TFunction&& f){
                static_assert(std::is_copy_constructible<TThreadLocalStorage>::value, "ThreadLocalStorage must be copy constructible");

                QUEST_PREPARE_CATCH_THREAD_EXCEPTION

                #pragma omp parallel
                {
                    TThreadLocalStorage thread_loacl_storage(rThreadLocalStoragePrototype);

                    #pragma omp for
                    for(int i=0;i<mNchunks;++i){
                        QUEST_TRY
                        for (auto it = mBlockPartition[i]; it!= mBlockPartition[i+1]; ++it){
                            f(*it, thread_loacl_storage);
                        }
                        QUEST_CATCH_THREAD_EXCEPTION 
                    }
                }
                QUEST_CHECK_AND_THROW_THREAD_EXCEPTION
            }

            template<typename TReducer,typename TThreadLocalStorage,typename TFunction>
            [[nodiscard]] inline typename TReducer::return_type for_each(const TThreadLocalStorage& rThreadLocalStoragePrototype, TFunction&& f){
                static_assert(std::is_copy_constructible<TThreadLocalStorage>::value, "ThreadLocalStorage must be copy constructible");

                QUEST_PREPARE_CATCH_THREAD_EXCEPTION

                TReducer global_reducer;
                #pragma omp parallel
                {
                    TThreadLocalStorage thread_loacl_storage(rThreadLocalStoragePrototype);

                    #pragma omp for
                    for(int i=0;i<mNchunks;++i){
                        QUEST_TRY
                        TReducer local_reducer;
                        for (auto it = mBlockPartition[i]; it!= mBlockPartition[i+1]; ++it){
                            local_reducer.LocalReduce(f(*it, thread_loacl_storage));
                        }
                        global_reducer.ThreadSafeReduce(local_reducer);
                        QUEST_CATCH_THREAD_EXCEPTION 
                    }
                }
                QUEST_CHECK_AND_THROW_THREAD_EXCEPTION

                return global_reducer.GetValue();
            }

        protected:

        private:
            int mNchunks;
            std::array<TIterator,MaxThreads> mBlockPartition;

    };


    template<typename TIterator, typename TFunction>
    std::enable_if_t<std::is_base_of_v<std::input_iterator_tag,typename std::iterator_traits<TIterator>::iterator_category>,void>
    block_for_each(TIterator it_begin, TIterator it_end, TFunction&& f){
        BlockPartition<TIterator>(it_begin, it_end).for_each(std::forward<TFunction>(f));
    }


    template<typename TReduction, typename TIterator, typename TFunction,
            std::enable_if_t<std::is_base_of_v<std::input_iterator_tag, typename std::iterator_traits<TIterator>::iterator_Category>,bool> = true>
    [[nodiscard]] typename TReduction::return_type block_for_each(TIterator it_begin, TIterator it_end, TFunction&& f){
        return BlockPartition<TIterator>(it_begin, it_end).template for_each<TReduction>(std::forward<TFunction>(std::forward<TFunction>(f)));
    }


    template<typename TIterator, typename TTLS, typename TFunction,
            std::enable_if_t<std::is_base_of_v<std::input_iterator_tag, typename std::iterator_traits<TIterator>::iterator_category>,bool> = true>
    void block_for_each(TIterator it_begin, TIterator it_end, const TTLS& rTLS, TFunction&& f){
        BlockPartition<TIterator>(it_begin, it_end).for_each(rTLS, std::forward<TFunction>(f));
    }


    template<typename TReduction, typename TIterator, typename TTLS, typename TFunction,
            std::enable_if_t<std::is_base_of_v<std::input_iterator_tag, typename std::iterator_traits<TIterator>::iterator_category>,bool> = true>
    [[nodiscard]] typename TReduction::return_type block_for_each(TIterator it_begin, TIterator it_end, const TTLS& rTLS, TFunction&& f){
        return BlockPartition<TIterator>(it_begin, it_end).template for_each<TReduction>(rTLS, std::forward<TFunction>(f));
    }


    template<typename TContainerType,typename TFunctionType,
            std::enable_if_t<!std::is_same_v<std::iterator_traits<typename decltype(std::declval<std::remove_cv_t<TContainerType>>().begin())::value_type>,void>,bool> = true>
    void block_for_each(TContainerType&& v,TFunctionType&& f){
        block_for_each(v.begin(),v.end(),std::forward<TFunctionType>(f));
    }


    template<typename TReduction,typename TContainerType,typename TFunctionType,
            std::enable_if_t<!std::is_same_v<std::iterator_traits<typename decltype(std::declval<std::remove_cv_t<TContainerType>>().begin())::value_type>,void>,bool> = true>
    [[nodiscard]] typename TReduction::return_type block_for_each(TContainerType&& v,TFunctionType&& f){
        return block_for_each<TReduction>(v.begin(),v.end(),std::forward<TFunctionType>(f));
    }


    template<typename TContainerType,typename TTLS,typename TFunctionType,
            std::enable_if_t<!std::is_same_v<std::iterator_traits<typename decltype(std::declval<std::remove_cv_t<TContainerType>>().begin())::value_type>,void>,bool> = true>
    void block_for_each(TContainerType&& v,const TTLS& rTLS,TFunctionType&& f){
        block_for_each(v.begin(),v.end(),rTLS,std::forward<TFunctionType>(f));
    }


    template<typename TReduction,typename TContainerType,typename TTLS,typename TFunctionType,
            std::enable_if_t<!std::is_same_v<std::iterator_traits<typename decltype(std::declval<std::remove_cv_t<TContainerType>>().begin())::value_type>,void>,bool> = true>
    [[nodiscard]] typename TReduction::return_type block_for_each(TContainerType&& v,const TTLS& rTLS,TFunctionType&& f){    
        return block_for_each<TReduction>(v.begin(),v.end(),rTLS,std::forward<TFunctionType>(f));
    }


    // 线程分区情况下在容器上进行索引
    template<typename TIndexType = std::size_t, int TMaxThreads = Globals::MaxAllowedThreads>
    class IndexPartition{
        public:
            IndexPartition(TIndexType Size, int Nchunks = ParallelUtilities::GetNumThreads()){
                QUEST_ERROR_IF(Nchunks < 1) << "Number of chunks must be > 0 (and not " << Nchunks << ")" << std::endl;

                if(Size == 0){
                    mNchunks = Nchunks;
                } else {
                    mNchunks = std::min(static_cast<int>(Size),Nchunks);
                }

                const int block_partition_size = Size/mNchunks;
                mBlockPartition[0] = 0;
                mBlockPartition[mNchunks] = Size;
                for(int i=1;i<mNchunks;i++){
                    mBlockPartition[i] = mBlockPartition[i-1]+block_partition_size;
                }
            }

            template<typename TUnaryFunction>
            inline void for_pure_c11(TUnaryFunction&& f){
                std::vector<std::future<void>> runners(mNchunks);
                const auto& partition = mBlockPartition;
                for(int i=0;i<mNchunks;++i){
                    runners[i] = std::async(std::launch::async,[&partition,i,&f](){
                        for(auto k=partition[i];k<partition[i+1];++k){  
                            f(k);
                        }
                    });
                }

                for(int i=0;i<mNchunks;++i){
                    try{
                        runners[i].get();
                    }
                    catch(Exception& e){
                        QUEST_ERROR << "Thread #" << i << " caught exception: " << e.what() << std::endl;
                    } catch(std::exception& e){
                        QUEST_ERROR << "Thread #" << i << " caught exception: " << e.what() << std::endl;
                    } catch(...){ 
                        QUEST_ERROR << std::endl << "unknown error" << std::endl;
                    }
                }
            }

            template<typename TUnaryFunction>
            inline void for_each(TUnaryFunction&& f){
                QUEST_PREPARE_CATCH_THREAD_EXCEPTION

                #pragma omp parallel for
                for(int i=0;i<mNchunks;i++){
                    QUEST_TRY
                    for(auto k=mBlockPartition[i];k<mBlockPartition[i+1];++k){  
                        f(k);
                    }
                    QUEST_CATCH_THREAD_EXCEPTION
                }
                QUEST_CHECK_AND_THROW_THREAD_EXCEPTION
            }

            template<typename TReducer,typename TUnaryFunction>
            [[nodiscard]] inline typename TReducer::return_type for_each(TUnaryFunction&& f){
                QUEST_PREPARE_CATCH_THREAD_EXCEPTION

                TReducer global_reducer;
                #pragma omp parallel for
                for(int i=0;i<mNchunks;i++){
                    QUEST_TRY
                    TReducer local_reducer;
                    for(auto k=mBlockPartition[i];k<mBlockPartition[i+1];++k){  
                        local_reducer.LocalReduce(f(k));
                    }
                    global_reducer.ThreadSafeReduce(local_reducer);
                    QUEST_CATCH_THREAD_EXCEPTION
                }
                QUEST_CHECK_AND_THROW_THREAD_EXCEPTION

                return global_reducer.GetValue();
            }

            template<typename TThreadLocalStorage,typename TFunction>
            inline void for_each(const TThreadLocalStorage& rThreadLocalStoragePrototype, TFunction&& f){
                static_assert(std::is_copy_constructible<TThreadLocalStorage>::value, "ThreadLocalStorage must be copy constructible");

                QUEST_PREPARE_CATCH_THREAD_EXCEPTION

                #pragma omp parallel
                {
                    TThreadLocalStorage thread_loacl_storage(rThreadLocalStoragePrototype);

                    #pragma omp for
                    for(int i=0;i<mNchunks;++i){
                        QUEST_TRY
                        for (auto k = mBlockPartition[i]; k < mBlockPartition[i+1]; ++k){
                            f(k, thread_loacl_storage);
                        }
                        QUEST_CATCH_THREAD_EXCEPTION 
                    }
                }
                QUEST_CHECK_AND_THROW_THREAD_EXCEPTION
            }

            template<typename TReducer,typename TThreadLocalStorage,typename TFunction>
            [[nodiscard]] inline typename TReducer::return_type for_each(const TThreadLocalStorage& rThreadLocalStoragePrototype, TFunction&& f){
                static_assert(std::is_copy_constructible<TThreadLocalStorage>::value, "ThreadLocalStorage must be copy constructible");

                QUEST_PREPARE_CATCH_THREAD_EXCEPTION

                TReducer global_reducer;
                #pragma omp parallel
                {
                    TThreadLocalStorage thread_loacl_storage(rThreadLocalStoragePrototype);

                    #pragma omp for
                    for(int i=0;i<mNchunks;++i){
                        QUEST_TRY
                        TReducer local_reducer;
                        for (auto k = mBlockPartition[i]; k < mBlockPartition[i+1]; ++k){
                            local_reducer.LocalReduce(f(k, thread_loacl_storage));
                        }
                        global_reducer.ThreadSafeReduce(local_reducer);
                        QUEST_CATCH_THREAD_EXCEPTION 
                    }
                }
                QUEST_CHECK_AND_THROW_THREAD_EXCEPTION

                return global_reducer.GetValue();
            }

        protected:

        private:
            int mNchunks;
            std::array<TIndexType,TMaxThreads> mBlockPartition;

    };

} // namespace Quest

#undef QUEST_PREPARE_CATCH_THREAD_EXCEPTION
#undef QUEST_CATCH_THREAD_EXCEPTION
#undef QUEST_CHECK_AND_THROW_THREAD_EXCEPTION

#endif // PARALLEL_UTILITIES_HPP