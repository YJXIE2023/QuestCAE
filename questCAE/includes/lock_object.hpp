/*----------------------------------------
封装互斥锁的类
-----------------------------------------*/

#ifndef QUEST_LOCK_OBJECT_HPP
#define QUEST_LOCK_OBJECT_HPP

// 系统头文件
#include <mutex>

// 外部头文件
#ifdef QUEST_SMP_OPENMP
    #include <omp.h>
#endif

// 项目头文件
#include "includes/define.hpp"

namespace Quest{
    class LockObject{
        public:
            LockObject() noexcept{
                #ifdef QUEST_SMP_OPENMP
                    omp_init_lock(&mLock);
                #endif
            }

            LockObject(const LockObject& rOther) = delete;

            ~LockObject() noexcept{
                #ifdef QUEST_SMP_OPENMP
                    omp_destroy_lock(&mLock);
                #endif
            }

            LockObject& operator=(const LockObject& rOther) = delete;

            inline void lock() const{
                #ifdef QUEST_SMP_CXX11
                    mLock.lock();
                #elif QUEST_SMP_OPENMP
                    omp_set_lock(&mLock);
                #endif
            }

            inline void unlock() const{
                #ifdef QUEST_SMP_CXX11
                    mLock.unlock();
                #elif QUEST_SMP_OPENMP
                    omp_unset_lock(&mLock);
                #endif
            }

            inline bool try_lock() const{
                #ifdef QUEST_SMP_CXX11
                    return mLock.try_lock();
                #elif QUEST_SMP_OPENMP
                    return omp_test_lock(&mLock);
                #endif
                return true;
            }

        protected:

        private:
            #ifdef QUEST_SMP_CXX11
                mutable std::mutex mLock;
            #elif QUEST_SMP_OPENMP
                mutable omp_lock_t mLock;
            #endif
    };

} // namespace Quest

#endif // LOCK_OBJECT_HPP