/*---------------------------------
initial_state.hpp头文件实现代码
---------------------------------*/

// 项目头文件
#include "includes/initial_state.hpp"

namespace Quest{

    InitialState::InitialState(const SizeType Dimension):
        mReferenceCount(0)
    {
        const SizeType voigt_size = (Dimension == 3) ? 6 : 3;
        mInitialStressVector.resize(voigt_size, false);
        mInitialStrainVector.resize(voigt_size, false);
        mInitialDeformationGradientMatrix.resize(Dimension, Dimension, false);

        noalias(mInitialStressVector) = ZeroVector(voigt_size);
        noalias(mInitialStrainVector) = ZeroVector(voigt_size);
        noalias(mInitialDeformationGradientMatrix) = ZeroMatrix(Dimension, Dimension);
    }


    InitialState::InitialState(
        const Vector& InitialStrainVector,
        const Vector& InitialStressVector,
        const Matrix& InitialDeformationGradientMatrix
    ) : mReferenceCount(0){
        const SizeType voigt_size = InitialStrainVector.size();
        const SizeType dimension = InitialDeformationGradientMatrix.size1();

        QUEST_ERROR_IF(voigt_size <= 0 || dimension <= 0) << "The impose vector or matrix is null..." << std::endl;

        mInitialStressVector.resize(voigt_size, false); 
        mInitialStrainVector.resize(voigt_size, false);
        mInitialDeformationGradientMatrix.resize(dimension, dimension, false);

        noalias(mInitialStressVector) = InitialStressVector;
        noalias(mInitialStrainVector) = InitialStrainVector;
        noalias(mInitialDeformationGradientMatrix) = InitialDeformationGradientMatrix;
    }


    InitialState::InitialState(
        const Vector& rImposingEntity,
        const InitialImposingType InitialImposition
    ) : mReferenceCount(0){
        const SizeType voigt_size = rImposingEntity.size();
        const SizeType dimension = (voigt_size == 6) ? 3 : 2;

        mInitialStressVector.resize(voigt_size, false);
        mInitialStrainVector.resize(voigt_size, false);
        mInitialDeformationGradientMatrix.resize(dimension, dimension, false);

        noalias(mInitialStressVector) = ZeroVector(voigt_size);
        noalias(mInitialStrainVector) = ZeroVector(voigt_size);
        noalias(mInitialDeformationGradientMatrix) = ZeroMatrix(dimension, dimension);

        if(InitialImposition == InitialImposingType::STRAIN_ONLY){
            noalias(mInitialStrainVector) = rImposingEntity;
        } else if (InitialImposition == InitialImposingType::STRESS_ONLY){
            noalias(mInitialStressVector) = rImposingEntity;
        }
    }


    InitialState::InitialState(
        const Vector& rInitialStrainVector,
        const Vector& rInitialStressVector
    ) : mReferenceCount(0){
        const SizeType voigt_size_1 = rInitialStrainVector.size();
        const SizeType voigt_size_2 = rInitialStressVector.size();
        const SizeType dimension = (voigt_size_1 == 6) ? 3 : 2;
        QUEST_ERROR_IF(voigt_size_1 <= 0 || voigt_size_2 <= 0) << "The impose vector is null..." << std::endl;

        mInitialStressVector.resize(voigt_size_1, false);
        mInitialStrainVector.resize(voigt_size_1, false);
        mInitialDeformationGradientMatrix.resize(dimension, dimension, false);

        noalias(mInitialStressVector) = rInitialStressVector;
        noalias(mInitialStrainVector) = rInitialStrainVector;
        noalias(mInitialDeformationGradientMatrix) = ZeroMatrix(dimension, dimension);
    }


    InitialState::InitialState(
        const Matrix& rInitialDeformationGradientMatrix
    ) : mReferenceCount(0){
        const SizeType dimension = rInitialDeformationGradientMatrix.size1();
        const SizeType voigt_size = (dimension == 3) ? 6 : 3;
        QUEST_ERROR_IF(dimension <= 0) << "The impose matrix is null..." << std::endl;

        mInitialDeformationGradientMatrix.resize(dimension, dimension, false);
        noalias(mInitialDeformationGradientMatrix) = rInitialDeformationGradientMatrix;

        mInitialStressVector.resize(voigt_size, false);
        mInitialStrainVector.resize(voigt_size, false);
        noalias(mInitialStressVector) = ZeroVector(voigt_size); 
        noalias(mInitialStrainVector) = ZeroVector(voigt_size);
    }

    
    void InitialState::SetInitialStrainVector(const Vector& rInitialStrainVector){
        const SizeType voigt_size = rInitialStrainVector.size();
        QUEST_ERROR_IF(voigt_size <= 0) << "The impose vector is null..." << std::endl;

        mInitialStrainVector.resize(voigt_size, false);
        noalias(mInitialStrainVector) = rInitialStrainVector;
    }


    void InitialState::SetInitialStressVector(const Vector& rInitialStressVector){
        const SizeType voigt_size = rInitialStressVector.size();
        QUEST_ERROR_IF(voigt_size <= 0) << "The impose vector is null..." << std::endl;

        mInitialStressVector.resize(voigt_size, false);
        noalias(mInitialStressVector) = rInitialStressVector;
    }


    void InitialState::SetInitialDeformationGradientMatrix(const Matrix& rInitialDeformationGradientMatrix){
        const SizeType dimension = rInitialDeformationGradientMatrix.size1();
        QUEST_ERROR_IF(dimension <= 0) << "The impose matrix is null..." << std::endl;

        mInitialDeformationGradientMatrix.resize(dimension, dimension, false);
        noalias(mInitialDeformationGradientMatrix) = rInitialDeformationGradientMatrix;
    }


    const Vector& InitialState::GetInitialStrainVector() const{
        return mInitialStrainVector;
    }


    const Vector& InitialState::GetInitialStressVector() const{
        return mInitialStressVector;
    }


    const Matrix& InitialState::GetInitialDeformationGradientMatrix() const{
        return mInitialDeformationGradientMatrix;
    }

}