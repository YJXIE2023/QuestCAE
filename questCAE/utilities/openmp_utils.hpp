/*----------------------------------------
封装 OpenMP 并行功能的工具类
在支持和不支持 OpenMP 的环境下均可运行
----------------------------------------*/

#ifndef QUEST_OPENMP_UTILS_HPP
#define QUEST_OPENMP_UTILS_HPP

// 系统头文件
#include <stdio.h>
#include <vector>
#include <iostream>
#ifndef QUEST_SMP_OPENMP
    #include <omp.h>
#else 
    #include <ctime>
#endif

// 自定义头文件
#include "utilities/parallel_utilities.hpp"

namespace Quest{

    class OpenMPUtils{
        public:
            static int GetCurrentNumberOfThreads(){
                #ifdef _OPENMP
                    return omp_get_num_threads();
                #else
                    return 1;
                #endif
            }

            static int IsDynamic(){
                #ifdef _OPENMP
                    return omp_get_dynamic();
                #else
                    return 0;
                #endif
            }

            static inline int ThisThread(){
                #ifdef _OPENMP
                    return omp_get_thread_num();
                #else
                    return 0;
                #endif
            }

            static inline int IsInParallel(){
                #ifdef _OPENMP
                    return omp_in_parallel();
                #else
                    return 0;
                #endif
            }

            static inline void PrintOPMInfo(){
                #ifdef _OPENMP
                    int nthreads,tid,procs,maxt,inpar,dynamic,nested;

                    #pragma omp parallel private(nthreads, tid)
                    {
                        tid = omp_get_thread_num();
                        if(tid == 0){
                            printf("  Thread %d getting environment info...\n", tid);

                            procs    = omp_get_num_procs();
                            nthreads = omp_get_num_threads();
                            maxt     = omp_get_max_threads();
                            inpar    = omp_in_parallel();
                            dynamic  = omp_get_dynamic();
                            nested   = omp_get_nested();

                            printf( "  | ------------ OMP IN USE --------- |\n");
                            printf( "  | Machine number of processors  = %d |\n", procs);
                            printf( "  | Number of threads set         = %d |\n", nthreads);
                            printf( "  | Max threads in use            = %d |\n", maxt);
                            printf( "  | In parallel?                  = %d |\n", inpar);
                            printf( "  | Dynamic threads enabled?      = %d |\n", dynamic);
                            printf( "  | Nested parallelism supported? = %d |\n", nested);
                            printf( "  | --------------------------------- |\n");

                            if( procs < nthreads )
                                std::cout<<" ( WARNING: Maximimun number of threads is EXCEEDED )"<<std::endl;
                        }
                    }
                #endif
            }

        protected:

        private:

    };

}

#endif //QUEST_OPENMP_UTILS_HPP