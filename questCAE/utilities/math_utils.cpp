#include "utilities/math_utils.hpp"

namespace Quest{

    template<typename TDataType>
    void MathUtils<TDataType>::Solve(
        MatrixType A,
        VectorType& rX,
        const VectorType& rB
    ){
        const SizeType size1 = A.size1();
        rX = rB;
        typedef permutation_matrix<SizeType> pmatrix;
        pmatrix pm(size1);
        int singular = lu_factorize(A, pm);
        QUEST_DEBUG_ERROR_IF(singular == 1) << "Matrix is singular: " << A << std::endl;
        lu_substitute(A, pm, rX);
    }

    
    template class MathUtils<double>;

}