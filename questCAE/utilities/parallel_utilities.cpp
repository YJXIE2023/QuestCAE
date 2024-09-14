/*---------------------------------
parallel_utilities.hpp文件实现代码
----------------------------------*/

// 系统头文件
#include <algorithm>
#include <cstdlib>

// 项目头文件
#include "parallel_utilities.hpp"
#include "IO/logger.hpp"
#include "includes/lock_object.hpp"

namespace Quest{

    namespace{
        std::once_flag flag_once;
    }

    int ParallelUtilities::GetNumThreads(){
        #ifdef QUEST_SMP_OPENMP
            int nthreads = omp_get_max_threads();
            QUEST_DEBUG_ERROR_IF(nthreads <= 0) << "GetNumThreads would devolve nthreads = " << nthreads << " which is not possible" << std::endl;
            return nthreads;
        #elif defined(QUEST_SMP_CXX11)
            int nthreads = GetNumberOfThreads();
            QUEST_DEBUG_ERROR_IF(nthreads <= 0) << "GetNumThreads would devolve nthreads = " << nthreads << " which is not possible" << std::endl;
            return nthreads;
        #else
            return 1;
        #endif
    }


    void ParallelUtilities::SetNumThreads(const int NumThreads){
        QUEST_ERROR_IF(NumThreads <= 0) << "Attempting to set NumThreads to <= 0. This is not allowed" << std::endl;

        #ifdef QUEST_SMP_OPENMP
            return;
        #endif

        const int num_procs = GetNumProcs();
        QUEST_WARNING_IF("ParallelUtilities", NumThreads > num_procs) << "The number of requested threads (" << NumThreads << ") exceeds the number of available threads (" << num_procs << ")!" << std::endl;
        GetNumberOfThreads() = NumThreads;

        #ifdef QUEST_SMP_EOPNMP
            omp_set_num_threads(NumThreads);
        #endif
    }


    int ParallelUtilities::GetNumProcs(){
        #ifdef QUEST_AMP_OPENMP
            return omp_get_num_procs();
        #elif defined(QUEST_AMP_CXX11)
            int num_procs = std::thread::hardware_concurrency();
            QUEST_WARNING_IF("ParallelUtilities", num_procs <= 0) << "The number of processors cannot be determined correctly on this machine. Please check your setup carafulluy!" << std::endl;
            return std::max(1, num_procs);
        #else 
            return 1;
        #endif
    }


    int ParallelUtilities::InitializeNumberOfThreads(){
        #ifdef QUEST_SMP_OPENMP
            return 1;
        
        #else
            const char* env_quest = std::getenv("QUEST_NUM_THREADS");
            const char* env_omp = std::getenv("OMP_NUM_THREADS");

            int num_threads;

            if(env_quest){
                num_threads = std::atoi(env_quest);
            } else if(env_omp){
                num_threads = std::atoi(env_omp);
            } else {
                num_threads = std::thread::hardware_concurrency();
            }

            num_threads = std::max(1, num_threads);
        
        
            #ifdef QUEST_AMP_OPENMP
                omp_set_num_threads(num_threads);
            #endif

            return num_threads;
        #endif
    }

    LockObject& ParallelUtilities::GetGlobalLock(){
        if (!mspGlobalLock){
            std::call_once(flag_once, [](){
                static LockObject global_lock;
                mspGlobalLock = &global_lock;
            });
        }

        return *mspGlobalLock;
    }


    int& ParallelUtilities::GetNumberOfThreads(){
        if(!mspNumThreads){
            QUEST_CRITICAL_SECTION
            if(!mspNumThreads){
                static int num_threads;
                num_threads = InitializeNumberOfThreads();
                mspNumThreads = &num_threads;
            }
        }

        return *mspNumThreads;
    }

    LockObject* ParallelUtilities::mspGlobalLock = nullptr;
    int* ParallelUtilities::mspNumThreads = nullptr;

}