/*-----------------------------
与openMP原子操作相关的函数
------------------------------*/

#ifndef QUEST_ATOMIC_UTILITIES_HPP
#define QUEST_ATOMIC_UTILITIES_HPP

// 系统头文件
#include <atomic>

#ifdef QUEST_SMP_OPENMP
#include <omp.h>
#endif

#if !defined(__cpp_lib_atomic_ref) && defined(QUEST_AMP_CXX11)
#include <boost/atomic/atomic_ref.hpp>
#endif

// 项目头文件
#include "includes/define.hpp"
#include "container/array_1d.hpp"

namespace Quest{

    #if defined(QUEST_SMP_CXX11)
        #if defined(__cpp_lib_atomic_ref)
            tempplate<typename T>
            using AtomicRef = std::atomic_ref<T>;
        #else
            template<typename T>
            using AtomicRef = boost::atomic_ref<T>;
        #endif
    #endif


    template<typename TDataType>
    inline void AtomicAdd(TDataType& target, const TDataType& value){
        #ifdef QUEST_SMP_OPENMP
            #pragma omp atomic
            target += value;
        #elif defined(QUEST_AMP_CXX11)
            AtomicRef<TDataType>{target} += value;
        #else
            target += value;
        #endif
    }


    template<typename TDataType, std::size_t ArraySize>
    inline void AtomicAdd(Array1d<TDataType,ArraySize>& target, const Array1d<TDataType,ArraySize>& value){
        for(std::size_t i=0; i<ArraySize; ++i){
            AtomicAdd(target[i], value[i]);
        }
    }


    template<typename TVectorType1, typename TVectorType2>
    inline void AtomicAddVector(TVectorType1& target, const TVectorType2& value){
        QUEST_DEBUG_ERROR_IF(target.size()!= value.size()) << "vector size mismatch in vector AtomicAddVector- Sizes are: " << target.size() << " for target and " << value.size() << " for value " << std::endl;

        for(std::size_t i=0; i<target.size(); ++i){
            AtomicAdd(target[i], value[i]);
        }
    }


    template<typename TMatrixType1, typename TMatrixType2>
    inline void AtomicAddMatrix(TMatrixType1& target, const TMatrixType2& value){
        QUEST_DEBUG_ERROR_IF(target.rows()!= value.rows() || target.cols()!= value.cols()) << "matrix size mismatch in matrix AtomicAddMatrix- Sizes are: " << target.rows() << "x" << target.cols() << " for target and " << value.rows() << "x" << value.cols() << " for value " << std::endl;

        for(std::size_t i=0; i<target.size1(); ++i){
            for(std::size_t j=0; j<target.size2(); ++j){
                AtomicAdd(target(i,j), value(i,j));
            }
        }
    }


    template<typename TDataType>
    inline void AtomicSub(TDataType& target, const TDataType& value){
        #ifdef QUEST_SMP_OPENMP
            #pragma omp atomic
            target -= value;
        #elif defined(QUEST_AMP_CXX11)
            AtomicRef<TDataType>{target} -= value;
        #else
            target -= value;
        #endif
    }


    template<typename TDataType, std::size_t ArraySize>
    inline void AtomicSub(Array1d<TDataType,ArraySize>& target, const Array1d<TDataType,ArraySize>& value){
        for(std::size_t i=0; i<ArraySize; ++i){
            AtomicSub(target[i], value[i]);
        }
    }


    template<typename TVectorType1, typename TVectorType2>
    inline void AtomicSubVector(TVectorType1& target, const TVectorType2& value){
        QUEST_DEBUG_ERROR_IF(target.size()!= value.size()) << "vector size mismatch in vector AtomicSubVector- Sizes are: " << target.size() << " for target and " << value.size() << " for value " << std::endl;

        for(std::size_t i=0; i<target.size(); ++i){
            AtomicSub(target[i], value[i]);
        }
    }


    template<typename TMatrixType1, typename TMatrixType2>
    inline void AtomicSubMatrix(TMatrixType1& target, const TMatrixType2& value){
        QUEST_DEBUG_ERROR_IF(target.rows()!= value.rows() || target.cols()!= value.cols()) << "matrix size mismatch in matrix AtomicSubMatrix- Sizes are: " << target.rows() << "x" << target.cols() << " for target and " << value.rows() << "x" << value.cols() << " for value " << std::endl;

        for(std::size_t i=0; i<target.size1(); ++i){
            for(std::size_t j=0; j<target.size2(); ++j){
                AtomicSub(target(i,j), value(i,j));
            }
        }
    }


    template<typename TDataType>
    inline void AtomicMult(TDataType& target, const TDataType& value){
        #ifdef QUEST_SMP_OPENMP
            #pragma omp atomic
            target *= value;
        #elif defined(QUEST_AMP_CXX11)
            AtomicRef<TDataType> at_ref{target};
            at_ref = at_ref * value;
        #else
            target *= value;
        #endif
    }


    template<typename TDataType, std::size_t ArraySize>
    inline void AtomicMult(Array1d<TDataType,ArraySize>& target, const Array1d<TDataType,ArraySize>& value){
        for(std::size_t i=0; i<ArraySize; ++i){
            AtomicMult(target[i], value[i]);
        }
    }


    template<typename TVectorType1, typename TVectorType2>
    inline void AtomicMultVector(TVectorType1& target, const TVectorType2& value){
        QUEST_DEBUG_ERROR_IF(target.size()!= value.size()) << "vector size mismatch in vector AtomicMultVector- Sizes are: " << target.size() << " for target and " << value.size() << " for value " << std::endl;

        for(std::size_t i=0; i<target.size(); ++i){
            AtomicMult(target[i], value[i]);
        }
    }


    template<typename TMatrixType1, typename TMatrixType2>
    inline void AtomicMultMatrix(TMatrixType1& target, const TMatrixType2& value){  
        QUEST_DEBUG_ERROR_IF(target.rows()!= value.rows() || target.cols()!= value.cols()) << "matrix size mismatch in matrix AtomicMultMatrix- Sizes are: " << target.rows() << "x" << target.cols() << " for target and " << value.rows() << "x" << value.cols() << " for value " << std::endl;   

        for(std::size_t i=0; i<target.size1(); ++i){
            for(std::size_t j=0; j<target.size2(); ++j){
                AtomicMult(target(i,j), value(i,j));
            }
        }
    }


    template<typename TDataType>
    inline void AtomicDiv(TDataType& target, const TDataType& value){
        AtomicMult(target, 1.0/value);
    }


    template<typename TDataType, std::size_t ArraySize>
    inline void AtomicDiv(Array1d<TDataType,ArraySize>& target, const Array1d<TDataType,ArraySize>& value){
        for(std::size_t i=0; i<ArraySize; ++i){
            AtomicDiv(target[i], value[i]);
        }
    }


    template<typename TVectorType1, typename TVectorType2>
    inline void AtomicDivVector(TVectorType1& target, const TVectorType2& value){
        QUEST_DEBUG_ERROR_IF(target.size()!= value.size()) << "vector size mismatch in vector AtomicDivVector- Sizes are: " << target.size() << " for target and " << value.size() << " for value " << std::endl;

        for(std::size_t i=0; i<target.size(); ++i){
            AtomicDiv(target[i], value[i]);
        }
    }


    template<typename TMatrixType1, typename TMatrixType2>
    inline void AtomicDivMatrix(TMatrixType1& target, const TMatrixType2& value){
        QUEST_DEBUG_ERROR_IF(target.rows()!= value.rows() || target.cols()!= value.cols()) << "matrix size mismatch in matrix AtomicDivMatrix- Sizes are: " << target.rows() << "x" << target.cols() << " for target and " << value.rows() << "x" << value.cols() << " for value " << std::endl;

        for(std::size_t i=0; i<target.size1(); ++i){
            for(std::size_t j=0; j<target.size2(); ++j){
                AtomicDiv(target(i,j), value(i,j));
            }
        }
    }

} // namespace Quest

#endif //QUEST_ATOMIC_UTILITIES_HPP