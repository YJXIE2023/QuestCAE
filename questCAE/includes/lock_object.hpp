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

    /**
     * @class LockObject
     * @brief 封装互斥锁的类
     * @details 封装互斥锁的类，提供常用的锁操作，包括锁定、解锁、尝试锁定等。
     */
    class LockObject{
        public:
            /**
             * @brief 默认构造函数
             */
            LockObject() noexcept{
                #ifdef QUEST_SMP_OPENMP
                    omp_init_lock(&mLock);
                #endif
            }


            /**
             * @brief 复制构造函数
             */
            LockObject(const LockObject& rOther) = delete;


            /**
             * @brief 析构函数
             */
            ~LockObject() noexcept{
                #ifdef QUEST_SMP_OPENMP
                    omp_destroy_lock(&mLock);
                #endif
            }


            /**
             * @brief 赋值运算符
             */
            LockObject& operator=(const LockObject& rOther) = delete;

            inline void lock() const{
                #ifdef QUEST_SMP_CXX11
                    mLock.lock();
                #elif QUEST_SMP_OPENMP
                    omp_set_lock(&mLock);
                #endif
            }


            /**
             * @brief 解锁
             */
            inline void unlock() const{
                #ifdef QUEST_SMP_CXX11
                    mLock.unlock();
                #elif QUEST_SMP_OPENMP
                    omp_unset_lock(&mLock);
                #endif
            }


            /**
             * @brief 尝试锁定
             */
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
            /**
             * @brief 互斥锁
             * @details 使用不同的锁实现，包括std::mutex和omp_lock_t。
             */
            #ifdef QUEST_SMP_CXX11
                mutable std::mutex mLock;
            #elif QUEST_SMP_OPENMP
                mutable omp_lock_t mLock;
            #endif
    };

} // namespace Quest

#endif // LOCK_OBJECT_HPP