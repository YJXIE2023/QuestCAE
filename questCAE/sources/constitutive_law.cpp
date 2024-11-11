/*-------------------------------------
constitutive_law.hpp的实现文件
-------------------------------------*/

// 项目头文件
#include "includes/constitutive_law.hpp"

namespace Quest{

    const unsigned int ConstitutiveLaw::msIndexVoigt3D6C[6][2] = { {0,0}, {1,1}, {2,2}, {0,1}, {1,2}, {0,2}};
    const unsigned int ConstitutiveLaw::msIndexVoigt2D4C[4][2] = { {0,0}, {1,1}, {2,2}, {0,1} };
    const unsigned int ConstitutiveLaw::msIndexVoigt2D3C[3][2] = { {0,0}, {1,1}, {0,1} };

    QUEST_CREATE_LOCAL_FLAG(ConstitutiveLaw, USE_ELEMENT_PROVIDED_STRAIN, 0);
    QUEST_CREATE_LOCAL_FLAG(ConstitutiveLaw, COMPUTE_STRESS,              1);
    QUEST_CREATE_LOCAL_FLAG(ConstitutiveLaw, COMPUTE_CONSTITUTIVE_TENSOR, 2);
    QUEST_CREATE_LOCAL_FLAG(ConstitutiveLaw, COMPUTE_STRAIN_ENERGY,       3);

    QUEST_CREATE_LOCAL_FLAG(ConstitutiveLaw, ISOCHORIC_TENSOR_ONLY,       4);
    QUEST_CREATE_LOCAL_FLAG(ConstitutiveLaw, VOLUMETRIC_TENSOR_ONLY,      5);

    QUEST_CREATE_LOCAL_FLAG(ConstitutiveLaw, MECHANICAL_RESPONSE_ONLY,    6);
    QUEST_CREATE_LOCAL_FLAG(ConstitutiveLaw, THERMAL_RESPONSE_ONLY,       7);
    QUEST_CREATE_LOCAL_FLAG(ConstitutiveLaw, INCREMENTAL_STRAIN_MEASURE,  8);

    QUEST_CREATE_LOCAL_FLAG(ConstitutiveLaw, INITIALIZE_MATERIAL_RESPONSE,9);
    QUEST_CREATE_LOCAL_FLAG(ConstitutiveLaw, FINALIZE_MATERIAL_RESPONSE, 10);


    QUEST_CREATE_LOCAL_FLAG(ConstitutiveLaw, FINITE_STRAINS,              1);
    QUEST_CREATE_LOCAL_FLAG(ConstitutiveLaw, INFINITESIMAL_STRAINS,       2);

    QUEST_CREATE_LOCAL_FLAG(ConstitutiveLaw, THREE_DIMENSIONAL_LAW,       3);
    QUEST_CREATE_LOCAL_FLAG(ConstitutiveLaw, PLANE_STRAIN_LAW,            4);
    QUEST_CREATE_LOCAL_FLAG(ConstitutiveLaw, PLANE_STRESS_LAW,            5);
    QUEST_CREATE_LOCAL_FLAG(ConstitutiveLaw, AXISYMMETRIC_LAW,            6);

    QUEST_CREATE_LOCAL_FLAG(ConstitutiveLaw, U_P_LAW,                     7);
    QUEST_CREATE_LOCAL_FLAG(ConstitutiveLaw, ISOTROPIC,                   8);
    QUEST_CREATE_LOCAL_FLAG(ConstitutiveLaw, ANISOTROPIC,                 9);


    ConstitutiveLaw::ConstitutiveLaw():Flags() {}


    ConstitutiveLaw::Pointer ConstitutiveLaw::Clone() const{
        QUEST_ERROR << "Called the virtual function for Clone" << std::endl;
    }


    ConstitutiveLaw::Pointer ConstitutiveLaw::Create(Quest::Parameters NewParameters) const{
        const std::string& name = NewParameters["name"].GetString();
        return QuestComponents<ConstitutiveLaw>::Get(name).Clone();
    }


    ConstitutiveLaw::Pointer ConstitutiveLaw::Create(
        Quest::Parameters NewParameters,
        const Properties& rProperties
    ) const {
        return this->Create(NewParameters);
    }


    ConstitutiveLaw::SizeType ConstitutiveLaw::WorkingSpaceDimension(){
        QUEST_ERROR << "Called the virtual function for WorkingSpaceDimension" << std::endl;
    }


    ConstitutiveLaw::SizeType ConstitutiveLaw::GetStrainSize() const{
        QUEST_ERROR << "Called the virtual function for GetStrainSize" << std::endl;
    }


    bool ConstitutiveLaw::Has(const Variable<bool>& rThisVariable){
        return false;
    }


    bool ConstitutiveLaw::Has(const Variable<int>& rThisVariable){
        return false;
    }


    bool ConstitutiveLaw::Has(const Variable<double>& rThisVariable){
        return false;
    }


    bool ConstitutiveLaw::Has(const Variable<Vector>& rThisVariable){
        return false;
    }


    bool ConstitutiveLaw::Has(const Variable<Matrix>& rThisVariable){
        return false;
    }


    bool ConstitutiveLaw::Has(const Variable<Array1d<double,3>>& rThisVariable){
        return false;
    }


    bool ConstitutiveLaw::Has(const Variable<Array1d<double,6>>& rThisVariable){
        return false;
    }


    bool& ConstitutiveLaw::GetValue(const Variable<bool>& rThisVariable, bool& rValue){
        return rValue;
    }


    int& ConstitutiveLaw::GetValue(const Variable<int>& rThisVariable, int& rValue){
        return rValue;
    }


    double& ConstitutiveLaw::GetValue(const Variable<double>& rThisVariable, double& rValue){
        return rValue;
    }


    Vector& ConstitutiveLaw::GetValue(const Variable<Vector>& rThisVariable, Vector& rValue){
        return rValue;
    }


    Matrix& ConstitutiveLaw::GetValue(const Variable<Matrix>& rThisVariable, Matrix& rValue){
        return rValue;
    }


    Array1d<double,3>& ConstitutiveLaw::GetValue(const Variable<Array1d<double,3>>& rThisVariable, Array1d<double,3>& rValue){
        return rValue;
    }


    Array1d<double,6>& ConstitutiveLaw::GetValue(const Variable<Array1d<double,6>>& rThisVariable, Array1d<double,6>& rValue){
        return rValue;
    }


    void ConstitutiveLaw::SetValue(
        const Variable<bool>& rThisVariable,
        const bool& Value,
        const ProcessInfo& rCurrentProcessInfo
    ){
        QUEST_ERROR << "Called the virtual function for SetValue" << std::endl;
    }


    void ConstitutiveLaw::SetValue(
        const Variable<int>& rThisVariable,
        const int& Value,
        const ProcessInfo& rCurrentProcessInfo
    ){
        QUEST_ERROR << "Called the virtual function for SetValue" << std::endl;
    }


    void ConstitutiveLaw::SetValue(
        const Variable<double>& rThisVariable,
        const double& Value,
        const ProcessInfo& rCurrentProcessInfo
    ){
        QUEST_ERROR << "Called the virtual function for SetValue" << std::endl;
    }


    void ConstitutiveLaw::SetValue(
        const Variable<Vector>& rThisVariable,
        const Vector& Value,
        const ProcessInfo& rCurrentProcessInfo
    ){
        QUEST_ERROR << "Called the virtual function for SetValue" << std::endl;
    }


    void ConstitutiveLaw::SetValue(
        const Variable<Matrix>& rThisVariable,
        const Matrix& Value,
        const ProcessInfo& rCurrentProcessInfo
    ){
        QUEST_ERROR << "Called the virtual function for SetValue" << std::endl;
    }


    void ConstitutiveLaw::SetValue(
        const Variable<Array1d<double,3>>& rThisVariable,
        const Array1d<double,3>& Value,
        const ProcessInfo& rCurrentProcessInfo
    ){
        QUEST_ERROR << "Called the virtual function for SetValue" << std::endl;
    }


    void ConstitutiveLaw::SetValue(
        const Variable<Array1d<double,6>>& rThisVariable,
        const Array1d<double,6>& Value,
        const ProcessInfo& rCurrentProcessInfo
    ){
        QUEST_ERROR << "Called the virtual function for SetValue" << std::endl;
    }


    bool& ConstitutiveLaw::CalculateValue(
        Parameters& rParameterValues,
        const Variable<bool>& rThisVariable,
        bool& rValue
    ){
        return rValue;
    }


    int& ConstitutiveLaw::CalculateValue(
        Parameters& rParameterValues,
        const Variable<int>& rThisVariable,
        int& rValue
    ){
        return rValue;
    }


    double& ConstitutiveLaw::CalculateValue(
        Parameters& rParameterValues,
        const Variable<double>& rThisVariable,
        double& rValue
    ){
        return rValue;
    }


    Vector& ConstitutiveLaw::CalculateValue(
        Parameters& rParameterValues,
        const Variable<Vector>& rThisVariable,
        Vector& rValue
    ){
        return rValue;
    }


    Matrix& ConstitutiveLaw::CalculateValue(
        Parameters& rParameterValues,
        const Variable<Matrix>& rThisVariable,
        Matrix& rValue
    ){
        return rValue;
    }


    Array1d<double,3>& ConstitutiveLaw::CalculateValue(
        Parameters& rParameterValues,
        const Variable<Array1d<double,3>>& rThisVariable,
        Array1d<double,3>& rValue
    ){
        return rValue;
    }


    Array1d<double,6>& ConstitutiveLaw::CalculateValue(
        Parameters& rParameterValues,
        const Variable<Array1d<double,6>>& rThisVariable,
        Array1d<double,6>& rValue
    ){
        return rValue;
    }


    void ConstitutiveLaw::CalculateDerivative(
        Parameters& rParameterValues,
        const Variable<double>& rFunctionVariable,
        const Variable<double>& rDerivativeVariable,
        double& rOutput
    ){
        QUEST_TRY
        
        QUEST_ERROR << "Derivative of " << rFunctionVariable.Name() << " w.r.t. " << rDerivativeVariable.Name() << " not implemented.\n";

        QUEST_CATCH("")
    }


    void ConstitutiveLaw::CalculateDerivative(
        Parameters& rParameterValues,
        const Variable<Vector>& rFunctionVariable,
        const Variable<double>& rDerivativeVariable,
        Vector& rOutput
    ){
        QUEST_TRY
        
        QUEST_ERROR << "Derivative of " << rFunctionVariable.Name() << " w.r.t. " << rDerivativeVariable.Name() << " not implemented.\n";

        QUEST_CATCH("")
    }


    void ConstitutiveLaw::CalculateDerivative(
        Parameters& rParameterValues,
        const Variable<Matrix>& rFunctionVariable,
        const Variable<double>& rDerivativeVariable,
        Matrix& rOutput
    ){
        QUEST_TRY
        
        QUEST_ERROR << "Derivative of " << rFunctionVariable.Name() << " w.r.t. " << rDerivativeVariable.Name() << " not implemented.\n";

        QUEST_CATCH("")
    }


    void ConstitutiveLaw::CalculateDerivative(
        Parameters& rParameterValues,
        const Variable<Array1d<double,3>>& rFunctionVariable,
        const Variable<double>& rDerivativeVariable,
        Array1d<double,3>& rOutput
    ){
        QUEST_TRY
        
        QUEST_ERROR << "Derivative of " << rFunctionVariable.Name() << " w.r.t. " << rDerivativeVariable.Name() << " not implemented.\n";

        QUEST_CATCH("")
    }


    bool ConstitutiveLaw::ValidateInput(const Properties& rMaterialProperties){
        return false;
    }


    ConstitutiveLaw::StrainMeasure ConstitutiveLaw::GetStrainMeasure(){
        return StrainMeasure_Infinitesimal;
    }


    ConstitutiveLaw::StressMeasure ConstitutiveLaw::GetStressMeasure(){
        return StressMeasure_PK1;
    }


    bool ConstitutiveLaw::IsIncremental(){
        return false;
    }


    void ConstitutiveLaw::InitializeMaterial(
        const Properties& rMaterialProperties,
        const GeometryType& rElementGeometry,
        const Vector& rShapeFunctionsValues
    ){}


    void ConstitutiveLaw::CalculateMaterialResponse(
        Parameters& rValues,
        const StressMeasure& rStressMeasure
    ){
        switch(rStressMeasure){
            case StressMeasure_PK1:
                CalculateMaterialResponsePK1(rValues);
                break;
            case StressMeasure_PK2:
                CalculateMaterialResponsePK2(rValues);
                break;
            case StressMeasure_Kirchhoff:
                CalculateMaterialResponseKirchhoff(rValues);
                break;
            case StressMeasure_Cauchy:
                CalculateMaterialResponseCauchy(rValues);
                break;
            default:
                QUEST_ERROR << " Stress Measure not Defined " << rStressMeasure << std::endl;
                break;
        }
    }


    void ConstitutiveLaw::CalculateMaterialResponsePK1(Parameters& rValues){
        QUEST_ERROR << "Called the virtual function for CalculateMaterialResponsePK1" << std::endl;
    }


    void ConstitutiveLaw::CalculateMaterialResponsePK2(Parameters& rValues){
        QUEST_ERROR << "Called the virtual function for CalculateMaterialResponsePK2" << std::endl;
    }


    void ConstitutiveLaw::CalculateMaterialResponseKirchhoff(Parameters& rValues){
        QUEST_ERROR << "Called the virtual function for CalculateMaterialResponseKirchhoff" << std::endl;
    }


    void ConstitutiveLaw::CalculateMaterialResponseCauchy(Parameters& rValues){
        QUEST_ERROR << "Called the virtual function for CalculateMaterialResponseCauchy" << std::endl;
    }


    void ConstitutiveLaw::CalculateStressResponse(Parameters& rValues, Vector& rInternalVariables){
        QUEST_ERROR << "Called the virtual function for CalculateStressResponse" << std::endl;
    }


    void ConstitutiveLaw::InitializeMaterialResponse(Parameters& rValues, const StressMeasure& rStressMeasure){
        switch(rStressMeasure){
            case StressMeasure_PK1:
                InitializeMaterialResponsePK1(rValues);
                break;
            case StressMeasure_PK2:
                InitializeMaterialResponsePK2(rValues);
                break;
            case StressMeasure_Kirchhoff:
                InitializeMaterialResponseKirchhoff(rValues);
                break;
            case StressMeasure_Cauchy:
                InitializeMaterialResponseCauchy(rValues);
                break;
            default:
                QUEST_ERROR << " Stress Measure not Defined " << rStressMeasure << std::endl;
                break;
        }
    }


    void ConstitutiveLaw::InitializeMaterialResponsePK1(Parameters& rValues){
        QUEST_ERROR_IF(this->RequiresInitializeMatrialResponse()) << "Calling virtual function for InitializeMaterialResponsePK1" << std::endl;
    }


    void ConstitutiveLaw::InitializeMaterialResponsePK2(Parameters& rValues){
        QUEST_ERROR_IF(this->RequiresInitializeMatrialResponse()) << "Calling virtual function for InitializeMaterialResponsePK2" << std::endl;
    }


    void ConstitutiveLaw::InitializeMaterialResponseKirchhoff(Parameters& rValues){
        QUEST_ERROR_IF(this->RequiresInitializeMatrialResponse()) << "Calling virtual function for InitializeMaterialResponseKirchhoff" << std::endl;
    }


    void ConstitutiveLaw::InitializeMaterialResponseCauchy(Parameters& rValues){
        QUEST_ERROR_IF(this->RequiresInitializeMatrialResponse()) << "Calling virtual function for InitializeMaterialResponseCauchy" << std::endl;
    }


    void ConstitutiveLaw::FinalizeMaterialResponse(Parameters& rValues, const StressMeasure& rStressMeasure){
        switch(rStressMeasure){
            case StressMeasure_PK1:
                FinalizeMaterialResponsePK1(rValues);
                break;
            case StressMeasure_PK2:
                FinalizeMaterialResponsePK2(rValues);
                break;
            case StressMeasure_Kirchhoff:
                FinalizeMaterialResponseKirchhoff(rValues);
                break;
            case StressMeasure_Cauchy:
                FinalizeMaterialResponseCauchy(rValues);
                break;
            default:
                QUEST_ERROR << " Stress Measure not Defined " << rStressMeasure << std::endl;
                break;
        }
    }


    void ConstitutiveLaw::FinalizeMaterialResponsePK1(Parameters& rValues){
        QUEST_ERROR_IF(this->RequiresFinalizeMatrialResponse()) << "Calling virtual function for FinalizeMaterialResponsePK1" << std::endl;
    }


    void ConstitutiveLaw::FinalizeMaterialResponsePK2(Parameters& rValues){
        QUEST_ERROR_IF(this->RequiresFinalizeMatrialResponse()) << "Calling virtual function for FinalizeMaterialResponsePK2" << std::endl;
    }


    void ConstitutiveLaw::FinalizeMaterialResponseKirchhoff(Parameters& rValues){
        QUEST_ERROR_IF(this->RequiresFinalizeMatrialResponse()) << "Calling virtual function for FinalizeMaterialResponseKirchhoff" << std::endl;
    }


    void ConstitutiveLaw::FinalizeMaterialResponseCauchy(Parameters& rValues){
        QUEST_ERROR_IF(this->RequiresFinalizeMatrialResponse()) << "Calling virtual function for FinalizeMaterialResponseCauchy" << std::endl;
    }


    void ConstitutiveLaw::ResetMaterial(
        const Properties& rMaterialProperties,
        const GeometryType& rElementGeometry,
        const Vector& rShapeFunctionsValues
    ){
        QUEST_ERROR << "Called the virtual function for ResetMaterial" << std::endl;
    }


    Vector& ConstitutiveLaw::TransformStrains(
        Vector& rStrainVector,
        const Matrix& rF,
        StrainMeasure rStrainInitial,
        StrainMeasure rStrainFinal
    ){
        switch(rStrainInitial){
            case StrainMeasure_GreenLagrange:
                switch(rStrainFinal){
                    case StrainMeasure_GreenLagrange:
                        break;
                    case StrainMeasure_Almansi:
                    {
                        Matrix StrainMatrix = MathUtils<double>::StrainVectorToTensor( rStrainVector );
                        CoVariantPushForward (StrainMatrix,rF);
                        rStrainVector = MathUtils<double>::StrainTensorToVector( StrainMatrix, rStrainVector.size() );
                        break;
                    }
                    case StrainMeasure_Hencky_Material:
                        QUEST_ERROR << "Hency strain has no transformation coded" << std::endl;
                        break;
                    case StrainMeasure_Hencky_Spatial:
                        QUEST_ERROR << "Hency strain has no transformation coded" << std::endl;
                        break;
                    default:
                        QUEST_ERROR << "FINAL STRAIN NOT DEFINED in StrainTransformation" << std::endl;
                        break;
                }
                break;

            case StrainMeasure_Almansi:
                switch(rStrainFinal){
                    case StrainMeasure_GreenLagrange:
                    {
                        Matrix StrainMatrix = MathUtils<double>::StrainVectorToTensor(rStrainVector);
                        CoVariantPullBack(StrainMatrix, rF);
                        rStrainVector = MathUtils<double>::StrainTensorToVector(StrainMatrix, rStrainVector.size());
                        break;
                    }
                    case StrainMeasure_Almansi:
                        break;
                    case StrainMeasure_Hencky_Material:
                        QUEST_ERROR << "Hency strain has no transformation coded" << std::endl;
                        break;
                    case StrainMeasure_Hencky_Spatial:
                        QUEST_ERROR << "Hency strain has no transformation coded" << std::endl;
                        break;
                    default:
                        QUEST_ERROR << "FINAL STRAIN NOT DEFINED in StrainTransformation" << std::endl;
                        break;
                }
                break;

            case StrainMeasure_Hencky_Material:
                QUEST_ERROR << "Hency strain has no transformation coded" << std::endl;
                break;

            case StrainMeasure_Hencky_Spatial:
                QUEST_ERROR << "Hency strain has no transformation coded" << std::endl;
                break;

            default:
                QUEST_ERROR << "Measure of strain NOT DEFINED in Strains Transformation" << std::endl;
                break;
        }
    }


    Matrix& ConstitutiveLaw::TransformStresses(
        Matrix& rStressMatrix,
        const Matrix& rF,
        const double& rdetF,
        StressMeasure rStrainInitial,
        StressMeasure rStrainFinal
    ){
        Vector StressVector;

        StressVector = MathUtils<double>::StressTensorToVector(rStressMatrix);

        StressVector = TransformStresses(StressVector, rF, rdetF, rStrainInitial, rStrainFinal);

        rStressMatrix = MathUtils<double>::StressVectorToTensor(StressVector);

        return rStressMatrix;
    }


    Vector& ConstitutiveLaw::TransformStresses(
        Vector& rStressVector,
        const Matrix& rF,
        const double& rdetF,
        StressMeasure rStrainInitial,
        StressMeasure rStrainFinal
    ){
        switch(rStrainInitial){
            case StressMeasure_PK1:
                TransformPK1Stresses(rStressVector, rF, rdetF, rStrainFinal);
                break;
            case StressMeasure_PK2:
                TransformPK2Stresses(rStressVector, rF, rdetF, rStrainFinal);
                break;
            case StressMeasure_Kirchhoff:
                TransformKirchhoffStresses(rStressVector, rF, rdetF, rStrainFinal);
                break;
            case StressMeasure_Cauchy:
                TransformCauchyStresses(rStressVector, rF, rdetF, rStrainFinal);
                break;
            default:
                QUEST_ERROR << "INITIAL STRESS NOT DEFINED in StressTransformation" << rStrainInitial << std::endl;
                break;
        }
        return rStressVector;
    }


    Vector& ConstitutiveLaw::TransformPK1Stresses(
        Vector& rStressVector,
        const Matrix& rF,
        const double& rdetF,
        StressMeasure rStressFinal
    ){
        unsigned int size = rF.size1();

        switch(rStressFinal){
            case StressMeasure_PK1:
                break;
            case StressMeasure_PK2:
            {
                Matrix StressMatrix = MathUtils<double>::StressVectorToTensor(rStressVector);
                Matrix InvF(size, size);
                double J;
                MathUtils<double>::InvertMatrix(rF, InvF, J);

                StressMatrix = prod(InvF, StressMatrix);

                rStressVector = MathUtils<double>::StressTensorToVector(StressMatrix, rStressVector.size());
                break;
            }
            case StressMeasure_Kirchhoff:
            {
                Matrix StressMatrix = MathUtils<double>::StressVectorToTensor(rStressVector);
                Matrix InvF(size, size);
                double J;
                MathUtils<double>::InvertMatrix(rF, InvF, J);

                StressMatrix = prod(InvF, StressMatrix);

                ContraVariantPushForward(StressMatrix, rF);

                rStressVector = MathUtils<double>::StressTensorToVector(StressMatrix, rStressVector.size());
                break;
            }
            case StressMeasure_Cauchy:
            {
                Matrix StressMatrix = MathUtils<double>::StressVectorToTensor(rStressVector);
                Matrix InvF(size, size);
                double J;
                MathUtils<double>::InvertMatrix(rF, InvF, J);

                StressMatrix = prod(InvF, StressMatrix);

                ContraVariantPushForward(StressMatrix, rF);

                StressMatrix /= J;

                rStressVector = MathUtils<double>::StressTensorToVector(StressMatrix, rStressVector.size());
                break;
            }
            default:
                QUEST_ERROR << "FINAL STRESS NOT DEFINED in StressTransformation" << std::endl;
                break;
        }

        return rStressVector;
    }


    Vector& ConstitutiveLaw::TransformPK2Stresses(
        Vector& rStressVector,
        const Matrix& rF,
        const double& rdetF,
        StressMeasure rStressFinal
    ){
        switch(rStressFinal){
            case StressMeasure_PK1:
            {
                Matrix StressMatrix = MathUtils<double>::StressVectorToTensor(rStressVector);   

                StressMatrix = prod(rF, StressMatrix);

                rStressVector = MathUtils<double>::StressTensorToVector(StressMatrix, rStressVector.size());

                break;
            }
            case StressMeasure_PK2:
                break;
            case StressMeasure_Kirchhoff:
            {
                Matrix StressMatrix = MathUtils<double>::StressVectorToTensor(rStressVector);   

                ContraVariantPushForward(StressMatrix, rF);

                rStressVector = MathUtils<double>::StressTensorToVector(StressMatrix, rStressVector.size());

                break;
            }
            case StressMeasure_Cauchy:
            {
                Matrix StressMatrix = MathUtils<double>::StressVectorToTensor(rStressVector);   

                ContraVariantPushForward(StressMatrix, rF);

                if(rdetF!=0){
                    StressMatrix /= rdetF;
                }

                rStressVector = MathUtils<double>::StressTensorToVector(StressMatrix, rStressVector.size());

                break;
            }
            default:
                QUEST_ERROR << "FINAL STRESS NOT DEFINED in StressTransformation" << std::endl;
                break;
        }
        return rStressVector;
    }


    Vector& ConstitutiveLaw::TransformKirchhoffStresses(
        Vector& rStressVector,
        const Matrix& rF,
        const double& rdetF,
        StressMeasure rStressFinal
    ){
        switch(rStressFinal){
            case StressMeasure_PK1:
            {
                Matrix StressMatrix = MathUtils<double>::StressVectorToTensor(rStressVector);   

                ContraVariantPullBack(StressMatrix, rF);

                StressMatrix = prod(rF, StressMatrix);

                rStressVector = MathUtils<double>::StressTensorToVector(StressMatrix, rStressVector.size());

                break;
            }
            case StressMeasure_PK2:
            {
                Matrix StressMatrix = MathUtils<double>::StressVectorToTensor(rStressVector);   

                ContraVariantPullBack(StressMatrix, rF);

                rStressVector = MathUtils<double>::StressTensorToVector(StressMatrix, rStressVector.size());

                break;
            }
            case StressMeasure_Kirchhoff:
                break;
            case StressMeasure_Cauchy:
            {
                if(rdetF!=0){
                    rStressVector /= rdetF;
                }
                break;
            }
            default:
                QUEST_ERROR << "FINAL STRESS NOT DEFINED in StressTransformation" << std::endl;
                break;
        }
        return rStressVector;
    }


    Vector& ConstitutiveLaw::TransformCauchyStresses(
        Vector& rStressVector,
        const Matrix& rF,
        const double& rdetF,
        StressMeasure rStressFinal
    ){
        switch(rStressFinal){
            case StressMeasure_PK1:
            {
                rStressVector *= rdetF;
                
                Matrix StressMatrix = MathUtils<double>::StressVectorToTensor(rStressVector);   

                ContraVariantPullBack(StressMatrix, rF);

                StressMatrix = prod(rF, StressMatrix);

                rStressVector = MathUtils<double>::StressTensorToVector(StressMatrix, rStressVector.size());

                break;
            }
            case StressMeasure_PK2:
            {
                rStressVector *= rdetF;
                
                Matrix StressMatrix = MathUtils<double>::StressVectorToTensor(rStressVector);   

                ContraVariantPullBack(StressMatrix, rF);

                rStressVector = MathUtils<double>::StressTensorToVector(StressMatrix, rStressVector.size());

                break;
            }
            case StressMeasure_Kirchhoff:
                rStressVector *= rdetF;
                break;
            case StressMeasure_Cauchy:
                break;
            default:
                QUEST_ERROR << "FINAL STRESS NOT DEFINED in StressTransformation" << std::endl;
                break;
        }
        return rStressVector;
    }


    void ConstitutiveLaw::PullBackConstitutiveMatrix(
        Matrix& rConstitutiveMatrix,
        const Matrix& rF
    ){
        Matrix OriginalConstitutiveMatrix = rConstitutiveMatrix;

        rConstitutiveMatrix.clear();

        Matrix InverseF(3,3);
        double detF = 0;
        MathUtils<double>::InvertMatrix(rF, InverseF, detF);

        ConstitutiveMatrixTransformation(rConstitutiveMatrix, OriginalConstitutiveMatrix, InverseF);
    }


    void ConstitutiveLaw::PushForwardConstitutiveMatrix(
        Matrix& rConstitutiveMatrix,
        const Matrix& rF
    ){
        Matrix OriginalConstitutiveMatrix = rConstitutiveMatrix;

        rConstitutiveMatrix.clear();

        ConstitutiveMatrixTransformation(rConstitutiveMatrix, OriginalConstitutiveMatrix, rF);
    }


    void ConstitutiveLaw::GetLawFeatures(Features& rFeatures) {
        QUEST_ERROR << "Calling virtual function for GerConstitutiveLawFeatures" << std::endl;
    }


    int ConstitutiveLaw::Check(
        const Properties& rMaterialProperties,
        const GeometryType& rElementGeometry,
        const ProcessInfo& rCurrentProcessInfo
    ) const {
        QUEST_TRY
        return 0;
        QUEST_CATCH("")
    }


    void ConstitutiveLaw::ContraVariantPushForward(
        Matrix& rMatrix,
        const Matrix& rF
    ){
        unsigned int size = rF.size1();
        Matrix temp(size, size);

        noalias(temp) = prod(rF, rMatrix);
        noalias(rMatrix) = prod(temp, trans(rF));
    }


    void ConstitutiveLaw::ContraVariantPullBack(
        Matrix& rMatrix,
        const Matrix& rF
    ){
        unsigned int size = rF.size1();
        Matrix InvF(size, size);
        double J;
        MathUtils<double>::InvertMatrix(rF, InvF, J);

        Matrix temp(size, size);

        noalias(temp) = prod(InvF, rMatrix);
        noalias(rMatrix) = prod(temp, trans(InvF));
    }


    void ConstitutiveLaw::CoVariantPushForward(
        Matrix& rMatrix,
        const Matrix& rF
    ){
        unsigned int size = rF.size1();
        Matrix InvF(size, size);
        double J;
        MathUtils<double>::InvertMatrix(rF, InvF, J);

        Matrix temp(size, size);

        noalias(temp) = prod(trans(InvF), rMatrix);
        noalias(rMatrix) = prod(temp, InvF);
    }


    void ConstitutiveLaw::CoVariantPullBack(
        Matrix& rMatrix,
        const Matrix& rF
    ){
        unsigned int size = rF.size1();
        Matrix temp(size, size);

        noalias(temp) = prod(trans(rF), rMatrix);
        noalias(rMatrix) = prod(temp, rF);
    }


    void ConstitutiveLaw::ConstitutiveMatrixTransformation(
        Matrix& rConstitutiveMatrix,
        const Matrix& rOriginalConstitutiveMatrix,
        const Matrix& rF
    ){
        unsigned int size = rOriginalConstitutiveMatrix.size1();
        if(size == 6){
            for(unsigned int i=0; i<6; ++i){
                for(unsigned int j=0; j<6; ++j){
                    rConstitutiveMatrix(i,j) = TransformConstitutiveComponent(rConstitutiveMatrix(i,j), rOriginalConstitutiveMatrix, rF,
                        this->msIndexVoigt3D6C[i][0], this->msIndexVoigt3D6C[i][1], this->msIndexVoigt3D6C[j][0],this->msIndexVoigt3D6C[j][1]);
                }
            }
        }
        else if(size == 4){
            for(unsigned int i=0; i<4; ++i){
                for(unsigned int j=0; j<4; ++j){
                    rConstitutiveMatrix(i,j) = TransformConstitutiveComponent(rConstitutiveMatrix(i,j), rOriginalConstitutiveMatrix, rF,
                        this->msIndexVoigt2D4C[i][0], this->msIndexVoigt2D4C[i][1], this->msIndexVoigt2D4C[j][0],this->msIndexVoigt2D4C[j][1]);
                }
            }
        }
        else if(size == 3){
            for(unsigned int i=0; i<3; ++i){
                for(unsigned int j=0; j<3; ++j){
                    rConstitutiveMatrix(i,j) = TransformConstitutiveComponent(rConstitutiveMatrix(i,j), rOriginalConstitutiveMatrix, rF,
                        this->msIndexVoigt2D3C[i][0], this->msIndexVoigt2D3C[i][1], this->msIndexVoigt2D3C[j][0],this->msIndexVoigt2D3C[j][1]);
                }
            }
        }
    }


    double& ConstitutiveLaw::TransformConstitutiveComponent(
        double& rCabcd,
        const Matrix& rConstitutiveMatrix,
        const Matrix& rF,
        const unsigned int&a,
        const unsigned int&b,
        const unsigned int&c,
        const unsigned int&d
    ){
        rCabcd = 0;
        double Cijkl = 0;

        unsigned int dimension = rF.size1();

        for(unsigned int j=0; j<dimension; ++j){
            for(unsigned int l=0; l<dimension; ++l){
                for(unsigned int k=0; k<dimension; ++k){
                    for(unsigned int i=0; i<dimension; ++i){
                        rCabcd += rF(a,i)*rF(b,j)*rF(c,k)*rF(d,l)*GetConstitutiveComponent(Cijkl, rConstitutiveMatrix, i,j,k,l);
                    }
                }
            }
        }
        return rCabcd;
    }


    double& ConstitutiveLaw::GetConstitutiveComponent(
        double& rCabcd,
        const Matrix& rConstitutiveMatrix,
        const unsigned int&a,
        const unsigned int&b,
        const unsigned int&c,
        const unsigned int&d
    ){
        unsigned int k=0, l=0;

        unsigned int size = rConstitutiveMatrix.size1();

        if(size == 3){
            for(unsigned int i=0; i<3; ++i){
                if(a==b){
                    if(this->msIndexVoigt2D3C[i][0]==a && this->msIndexVoigt2D3C[i][1]==b){
                        k = i;
                        break;
                    }
                } else {
                    if((this->msIndexVoigt2D3C[i][0]==a && this->msIndexVoigt2D3C[i][1]==b) ||
                        (this->msIndexVoigt2D3C[i][1]==a && this->msIndexVoigt2D3C[i][0]==b)){
                            k = i;
                            break;
                    }
                }
            }
            for(unsigned int i=0; i<3; ++i){
                if(c==d){
                    if(this->msIndexVoigt2D3C[i][0]==c && this->msIndexVoigt2D3C[i][1]==d){
                        l = i;
                        break;
                    }
                } else {
                    if((this->msIndexVoigt2D3C[i][0]==c && this->msIndexVoigt2D3C[i][1]==d) ||
                        (this->msIndexVoigt2D3C[i][1]==c && this->msIndexVoigt2D3C[i][0]==d)){
                            l = i;
                            break;
                    }
                }
            }
        } else if(size == 4){
            for(unsigned int i=0; i<4; ++i){
                if(a==b){
                    if(this->msIndexVoigt2D4C[i][0]==a && this->msIndexVoigt2D4C[i][1]==b){
                        k = i;
                        break;
                    }
                } else {
                    if((this->msIndexVoigt2D4C[i][0]==a && this->msIndexVoigt2D4C[i][1]==b) ||
                        (this->msIndexVoigt2D4C[i][1]==a && this->msIndexVoigt2D4C[i][0]==b)){
                            k = i;
                            break;
                    }
                }
            }
            for(unsigned int i=0; i<4; ++i){
                if(c==d){
                    if(this->msIndexVoigt2D4C[i][0]==c && this->msIndexVoigt2D4C[i][1]==d){ 
                        l = i;
                        break;
                    }
                } else {
                    if((this->msIndexVoigt2D4C[i][0]==c && this->msIndexVoigt2D4C[i][1]==d) ||
                        (this->msIndexVoigt2D4C[i][1]==c && this->msIndexVoigt2D4C[i][0]==d)){
                            l = i;
                            break;
                    }
                }
            }
        } else if(size == 6){
            for(unsigned int i=0; i<6; ++i){
                if(a==b){
                    if(this->msIndexVoigt3D6C[i][0]==a && this->msIndexVoigt3D6C[i][1]==b){
                        k = i;
                        break;
                    }
                } else {
                    if((this->msIndexVoigt3D6C[i][0]==a && this->msIndexVoigt3D6C[i][1]==b) ||
                        (this->msIndexVoigt3D6C[i][1]==a && this->msIndexVoigt3D6C[i][0]==b)){
                            k = i;
                            break;
                    }
                }
            }
            for(unsigned int i=0; i<6; ++i){
                if(c==d){
                    if(this->msIndexVoigt3D6C[i][0]==c && this->msIndexVoigt3D6C[i][1]==d){
                        l = i;
                        break;
                    }
                } else {
                    if((this->msIndexVoigt3D6C[i][0]==c && this->msIndexVoigt3D6C[i][1]==d) ||
                        (this->msIndexVoigt3D6C[i][1]==c && this->msIndexVoigt3D6C[i][0]==d)){
                            l = i;
                            break;
                    }
                }
            }
        }

        rCabcd = rConstitutiveMatrix(k,l);

        return rCabcd;
    }


    void ConstitutiveLaw::CalculateCauchyStresses(
        Vector& Cauchy_StressVector,
        const Matrix& F,
        const Vector& PK2_StressVector,
        const Vector& GreenLagrangeStrainVector
    ){}

} // namespace Quest