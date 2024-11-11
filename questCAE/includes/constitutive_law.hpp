/*----------------------------------
所有本构关系的基类
----------------------------------*/

#ifndef QUEST_CONSTITUTIVE_LAW_HPP
#define QUEST_CONSTITUTIVE_LAW_HPP

// 项目头文件
#include "includes/define.hpp"
#include "includes/serializer.hpp"
#include "includes/variables.hpp"
#include "includes/node.hpp"
#include "includes/properties.hpp"
#include "geometries/geometry.hpp"
#include "utilities/math_utils.hpp"
#include "includes/process_info.hpp"
#include "includes/ublas_interface.hpp"
#include "includes/quest_parameters.hpp"
#include "container/data_value_container.hpp"
#include "container/flags.hpp"
#include "includes/initial_state.hpp"

namespace Quest{

    class QUEST_API(QUEST_CORE) ConstitutiveLaw: public Flags{
        public:
            enum StrainMeasure{
                StrainMeasure_Infinitesimal,
                StrainMeasure_GreenLagrange,
                StrainMeasure_Almansi,

                StrainMeasure_Hencky_Material,
                StrainMeasure_Hencky_Spatial,

                StrainMeasure_Deformation_Gradient,
                StrainMeasure_Right_CauchyGreen,
                StrainMeasure_Left_CauchyGreen,
                StrainMeasure_Velocity_Gradient
            };

            enum StressMeasure{
                StressMeasure_PK1,
                StressMeasure_PK2,
                StressMeasure_Kirchhoff,
                StressMeasure_Cauchy
            };

            using ProcessInfoType = ProcessInfo;
            using SizeType = std::size_t;
            using GeometryType = Geometry<Node >;

            using StrainVectorType = Vector;
            using StressVectorType = Vector;
            using VoigtSizeMatrixType = Matrix;
            using DeformationGradientMatrixType = Matrix;

            QUEST_CLASS_POINTER_DEFINITION(ConstitutiveLaw);

            QUEST_DEFINE_LOCAL_FLAG( USE_ELEMENT_PROVIDED_STRAIN );
            QUEST_DEFINE_LOCAL_FLAG( COMPUTE_STRESS );
            QUEST_DEFINE_LOCAL_FLAG( COMPUTE_CONSTITUTIVE_TENSOR );
            QUEST_DEFINE_LOCAL_FLAG( COMPUTE_STRAIN_ENERGY );

            QUEST_DEFINE_LOCAL_FLAG( ISOCHORIC_TENSOR_ONLY );
            QUEST_DEFINE_LOCAL_FLAG( VOLUMETRIC_TENSOR_ONLY );

            QUEST_DEFINE_LOCAL_FLAG( MECHANICAL_RESPONSE_ONLY );
            QUEST_DEFINE_LOCAL_FLAG( THERMAL_RESPONSE_ONLY );

            QUEST_DEFINE_LOCAL_FLAG( INCREMENTAL_STRAIN_MEASURE );

            QUEST_DEFINE_LOCAL_FLAG( INITIALIZE_MATERIAL_RESPONSE );
            QUEST_DEFINE_LOCAL_FLAG( FINALIZE_MATERIAL_RESPONSE );

            QUEST_DEFINE_LOCAL_FLAG( FINITE_STRAINS );
            QUEST_DEFINE_LOCAL_FLAG( INFINITESIMAL_STRAINS );

            QUEST_DEFINE_LOCAL_FLAG( THREE_DIMENSIONAL_LAW );
            QUEST_DEFINE_LOCAL_FLAG( PLANE_STRAIN_LAW );
            QUEST_DEFINE_LOCAL_FLAG( PLANE_STRESS_LAW );
            QUEST_DEFINE_LOCAL_FLAG( AXISYMMETRIC_LAW );

            QUEST_DEFINE_LOCAL_FLAG( U_P_LAW );

            QUEST_DEFINE_LOCAL_FLAG( ISOTROPIC );
            QUEST_DEFINE_LOCAL_FLAG( ANISOTROPIC );


            struct Features{

                QUEST_CLASS_POINTER_DEFINITION(Features);

                Flags mOptions;
                SizeType mStrainSize;
                SizeType mSpaceDimension;
                std::vector< StrainMeasure > mStrainMeasures;

                Features() {}

                ~Features() {}

                void SetOptions(const Flags& rOptions){ mOptions = rOptions; }
                void SetStrainSize(const SizeType rStrainSize){ mStrainSize = rStrainSize; }
                void SetSpaceDimension(const SizeType rSpaceDimension){ mSpaceDimension = rSpaceDimension; }
                void SetStrainMeasure(const StrainMeasure& rStrainMeasure){ mStrainMeasures.push_back(rStrainMeasure); }
                void SetStrainMeasures(const std::vector< StrainMeasure > rStrainMeasures){ mStrainMeasures = rStrainMeasures; }

                const Flags& GetOptions() { return mOptions; }

                const SizeType& GetStrainSize() { return mStrainSize; }
                const SizeType& GetSpaceDimension() { return mSpaceDimension; }
                const std::vector< StrainMeasure >& GetStrainMeasures() { return mStrainMeasures; }

            }; // struct Features


            struct Parameters{
                QUEST_CLASS_POINTER_DEFINITION(Parameters);


                Parameters(){
                    mDeterminantF = 0;
                    mpStrainVector = nullptr;
                    mpStressVector = nullptr;
                    mpShapeFunctionsValues = nullptr;
                    mpShapeFunctionsDerivatives = nullptr;
                    mpDeformationGradientF = nullptr;
                    mpConstitutiveMatrix = nullptr;
                    mpCurrentProcessInfo = nullptr;
                    mpMaterialProperties = nullptr;
                    mpElementGeometry = nullptr;
                }


                Parameters(
                    const GeometryType& rElementGeometry,
                    const Properties& rMaterialProperties,
                    const ProcessInfo& rCurrentProcessInfo
                ):
                    mpCurrentProcessInfo(&rCurrentProcessInfo),
                    mpMaterialProperties(&rMaterialProperties),
                    mpElementGeometry(&rElementGeometry)
                {
                    mDeterminantF = 0;
                    mpStrainVector = nullptr;
                    mpStressVector = nullptr;
                    mpShapeFunctionsValues = nullptr;
                    mpShapeFunctionsDerivatives = nullptr;
                    mpDeformationGradientF = nullptr;
                    mpConstitutiveMatrix = nullptr;
                }


                Parameters(const Parameters& rNewwwParameters):
                    mOptions(rNewwwParameters.mOptions),
                    mDeterminantF(rNewwwParameters.mDeterminantF),
                    mpStrainVector(rNewwwParameters.mpStrainVector),
                    mpStressVector(rNewwwParameters.mpStressVector),
                    mpShapeFunctionsValues(rNewwwParameters.mpShapeFunctionsValues),
                    mpShapeFunctionsDerivatives(rNewwwParameters.mpShapeFunctionsDerivatives),
                    mpDeformationGradientF(rNewwwParameters.mpDeformationGradientF),
                    mpConstitutiveMatrix(rNewwwParameters.mpConstitutiveMatrix),
                    mpCurrentProcessInfo(rNewwwParameters.mpCurrentProcessInfo),
                    mpMaterialProperties(rNewwwParameters.mpMaterialProperties),
                    mpElementGeometry(rNewwwParameters.mpElementGeometry)
                {
                }


                ~Parameters(){}


                bool CheckAllParameters(){
                    if(CheckMechanicalVariables() && CheckShapeFunctions() && CheckInfoMaterialGeoemtry()){
                        return true;
                    } else {
                        return false;
                    }
                }


                bool CheckShapeFunctions(){
                    if(!mpShapeFunctionsValues){
                        QUEST_ERROR << "ShapeFunctionsValues NOT SET!" << std::endl;
                    }

                    if(!mpShapeFunctionsDerivatives){
                        QUEST_ERROR << "ShapeFunctionsDerivatives NOT SET!" << std::endl;
                    }

                    return true;
                }


                bool CheckInfoMaterialGeoemtry(){
                    if(!mpCurrentProcessInfo){
                        QUEST_ERROR << "CurrentProcessInfo NOT SET!" << std::endl;
                    }

                    if(!mpMaterialProperties){
                        QUEST_ERROR << "MaterialProperties NOT SET!" << std::endl;
                    }

                    if(!mpElementGeometry){
                        QUEST_ERROR << "ElementGeometry NOT SET!" << std::endl;
                    }

                    return true;
                }


                bool CheckMechanicalVariables(){
                    if(mDeterminantF <= 0){
                        QUEST_ERROR << "DeterminantF NOT SET!" << std::endl;
                    }

                    if(!mpDeformationGradientF){
                        QUEST_ERROR << "DeformationGradientF NOT SET!" << std::endl;
                    }

                    if(!mpStrainVector){
                        QUEST_ERROR << "StrainVector NOT SET!" << std::endl;
                    }

                    if(!mpStressVector){
                        QUEST_ERROR << "StressVector NOT SET!" << std::endl;
                    }

                    if(!mpConstitutiveMatrix){
                        QUEST_ERROR << "ConstitutiveMatrix NOT SET!" << std::endl;
                    }

                    return true;
                }


                void Set(Flags ThisFlag) { mOptions.Set(ThisFlag); }
                void Reset(Flags ThisFlag) { mOptions.Reset(ThisFlag); }

                void SetOptions(const Flags& rOptions) { mOptions = rOptions; }
                void SetDeterminantF(const double rDeterminantF) { mDeterminantF = rDeterminantF; }

                void SetShapeFunctionsValues(const Vector& rShapeFunctionsValues) { mpShapeFunctionsValues = &rShapeFunctionsValues; }
                void SetShapeFunctionsDerivatives(const Matrix& rShapeFunctionsDerivatives) { mpShapeFunctionsDerivatives = &rShapeFunctionsDerivatives; }

                void SetDeformationGradientF(const DeformationGradientMatrixType& rDeformationGradientF) { mpDeformationGradientF = &rDeformationGradientF; }

                void SetStrainVector(StrainVectorType& rStrainVector) { mpStrainVector = &rStrainVector; }
                void SetStressVector(StressVectorType& rStressVector) { mpStressVector = &rStressVector; }
                void SetConstitutiveMatrix(VoigtSizeMatrixType& rConstitutiveMatrix) { mpConstitutiveMatrix = &rConstitutiveMatrix; }

                void SetProcessInfo(const ProcessInfo& rCurrentProcessInfo) { mpCurrentProcessInfo = &rCurrentProcessInfo; }
                void SetMaterialProperties(const Properties& rMaterialProperties) { mpMaterialProperties = &rMaterialProperties; }
                void SetElementGeometry(const GeometryType& rElementGeometry) { mpElementGeometry = &rElementGeometry; }


                Flags& GetOptions() { return mOptions; }

                const double& GetDeterminantF(){
                    QUEST_DEBUG_ERROR_IF(IsSetDeterminantF()) << "DeterminantF is not set!" << std::endl;
                    return mDeterminantF;
                }


                const Vector& GetShapeFunctionsValues(){
                    QUEST_DEBUG_ERROR_IF(IsSetShapeFunctionsValues()) << "ShapeFunctionsValues is not set!" << std::endl;
                    return *mpShapeFunctionsValues;
                }


                const Matrix& GetShapeFunctionsDerivatives(){
                    QUEST_DEBUG_ERROR_IF(IsSetShapeFunctionsDerivatives()) << "ShapeFunctionsDerivatives is not set!" << std::endl;
                    return *mpShapeFunctionsDerivatives;
                }


                const DeformationGradientMatrixType& GetDeformationGradientF(){
                    QUEST_DEBUG_ERROR_IF(IsSetDeformationGradientF()) << "DeformationGradientF is not set!" << std::endl;
                    return *mpDeformationGradientF;
                }


                StrainVectorType& GetStrainVector(){
                    QUEST_DEBUG_ERROR_IF(IsSetStrainVector()) << "StrainVector is not set!" << std::endl;
                    return *mpStrainVector;
                }


                StressVectorType& GetStressVector(){
                    QUEST_DEBUG_ERROR_IF(IsSetStressVector()) << "StressVector is not set!" << std::endl;
                    return *mpStressVector;
                }


                VoigtSizeMatrixType& GetConstitutiveMatrix(){
                    QUEST_DEBUG_ERROR_IF(IsSetConstitutiveMatrix()) << "ConstitutiveMatrix is not set!" << std::endl;
                    return *mpConstitutiveMatrix;
                }


                const ProcessInfo& GetCurrentProcessInfo(){
                    QUEST_DEBUG_ERROR_IF(IsSetProcessInfo()) << "ProcessInfo is not set!" << std::endl;
                    return *mpCurrentProcessInfo;
                }


                const Properties& GetMaterialProperties(){
                    QUEST_DEBUG_ERROR_IF(IsSetMaterialProperties()) << "MaterialProperties is not set!" << std::endl;
                    return *mpMaterialProperties;
                }


                const GeometryType& GetElementGeometry(){
                    QUEST_DEBUG_ERROR_IF(IsSetElementGeometry()) << "ElementGeometry is not set!" << std::endl;
                    return *mpElementGeometry;
                }


                double& GetDeterminantF(double& rDeterminantF){rDeterminantF = mDeterminantF; return rDeterminantF;}
                StrainVectorType& GetStrainVector(StrainVectorType& rStrainVector){rStrainVector = *mpStrainVector; return rStrainVector;}
                StressVectorType& GetStressVector(StressVectorType& rStressVector){rStressVector = *mpStressVector; return rStressVector;}
                VoigtSizeMatrixType& GetConstitutiveMatrix(VoigtSizeMatrixType& rConstitutiveMatrix){rConstitutiveMatrix = *mpConstitutiveMatrix; return rConstitutiveMatrix;}
                DeformationGradientMatrixType& GetDeformationGradientF(DeformationGradientMatrixType& rDeformationGradientF){rDeformationGradientF = *mpDeformationGradientF; return rDeformationGradientF;}


                bool IsSetDeterminantF() { return (mDeterminantF > 0.0); }
                bool IsSetShapeFunctionsValues() { return (mpShapeFunctionsValues != nullptr); }
                bool IsSetShapeFunctionsDerivatives() { return (mpShapeFunctionsDerivatives != nullptr); }
                bool IsSetDeformationGradientF() { return (mpDeformationGradientF != nullptr); }

                bool IsSetStrainVector() { return (mpStrainVector != nullptr); }
                bool IsSetStressVector() { return (mpStressVector != nullptr); }

                bool IsSetConstitutiveMatrix() { return (mpConstitutiveMatrix != nullptr); }

                bool IsSetProcessInfo() { return (mpCurrentProcessInfo != nullptr); }
                bool IsSetMaterialProperties() { return (mpMaterialProperties != nullptr); }
                bool IsSetElementGeometry() { return (mpElementGeometry != nullptr); }

                private:
                    Flags mOptions;
                    double mDeterminantF;

                    StrainVectorType* mpStrainVector;
                    StressVectorType* mpStressVector;

                    const Vector* mpShapeFunctionsValues;
                    const Matrix* mpShapeFunctionsDerivatives;

                    const DeformationGradientMatrixType* mpDeformationGradientF;
                    VoigtSizeMatrixType* mpConstitutiveMatrix;

                    const ProcessInfo* mpCurrentProcessInfo;
                    const Properties* mpMaterialProperties;
                    const GeometryType* mpElementGeometry;

            }; // struct Parameters

        public:
            ConstitutiveLaw();


            ~ConstitutiveLaw() override {};


            virtual ConstitutiveLaw::Pointer Clone() const;


            virtual Pointer Create(Quest::Parameters NewParameters) const;


            virtual Pointer Create(Quest::Parameters NewParameters, const Properties& rProperties) const;


            virtual SizeType WorkingSpaceDimension();


            virtual SizeType GetStrainSize() const;


            void SetInitialState(InitialState::Pointer pInitialState){
                mpInitialState = pInitialState;
            }


            InitialState::Pointer pGetInitialState(){
                return mpInitialState;
            }


            InitialState& GetInitialState(){
                return *mpInitialState;
            }


            bool HasInitialState() const{
                return (mpInitialState != nullptr);
            }


            template<typename TVectorType>
            void AddInitialStressVectorContribution(TVectorType& rStressVector){
                if(this->HasInitialState()){
                    const auto& r_initial_state = this->GetInitialState();
                    noalias(rStressVector) += r_initial_state.GetInitialStressVector();
                }
            }


            template<typename TVectorType>
            void AddInitialStrainVectorContribution(TVectorType& rStrainVector){
                if(this->HasInitialState()){
                    const auto& r_initial_state = this->GetInitialState();
                    noalias(rStrainVector) -= r_initial_state.GetInitialStrainVector();
                }
            }


            template<typename TMatrixType>
            void AddInitialDeformationGradientMatrixContribution(TMatrixType& rF){
                if(this->HasInitialState()){
                    const auto& r_initial_state = this->GetInitialState();
                    rF = Prod(r_initial_state.GetInitialDeformationGradientMatrix(), rF);
                }
            }


            virtual bool Has(const Variable<bool>& rThisVariable);


            virtual bool Has(const Variable<int>& rThisVariable);


            virtual bool Has(const Variable<double>& rThisVariable);


            virtual bool Has(const Variable<Vector>& rThisVariable);


            virtual bool Has(const Variable<Matrix>& rThisVariable);


            virtual bool Has(const Variable<Array1d<double, 3>>& rThisVariable);


            virtual bool Has(const Variable<Array1d<double, 6>>& rThisVariable);


            virtual bool& GetValue(const Variable<bool>& rThisVariable, bool& rValue);


            virtual int& GetValue(const Variable<int>& rThisVariable, int& rValue);


            virtual double& GetValue(const Variable<double>& rThisVariable, double& rValue);


            virtual Vector& GetValue(const Variable<Vector>& rThisVariable, Vector& rValue);


            virtual Matrix& GetValue(const Variable<Matrix>& rThisVariable, Matrix& rValue);


            virtual Array1d<double, 3>& GetValue(const Variable<Array1d<double, 3>>& rThisVariable, Array1d<double, 3>& rValue);


            virtual Array1d<double, 6>& GetValue(const Variable<Array1d<double, 6>>& rThisVariable, Array1d<double, 6>& rValue);


            virtual void SetValue(
                const Variable<bool>& rThisVariable,
                const bool& Value,
                const ProcessInfo& rCurrentProcessInfo
            );


            virtual void SetValue(
                const Variable<int>& rThisVariable,
                const int& Value,
                const ProcessInfo& rCurrentProcessInfo
            );


            virtual void SetValue(
                const Variable<double>& rThisVariable,
                const double& Value,
                const ProcessInfo& rCurrentProcessInfo
            );


            virtual void SetValue(
                const Variable<Vector>& rThisVariable,
                const Vector& Value,
                const ProcessInfo& rCurrentProcessInfo
            );


            virtual void SetValue(
                const Variable<Matrix>& rThisVariable,
                const Matrix& Value,
                const ProcessInfo& rCurrentProcessInfo
            );


            virtual void SetValue(
                const Variable<Array1d<double, 3>>& rThisVariable,
                const Array1d<double, 3>& Value,
                const ProcessInfo& rCurrentProcessInfo
            );


            virtual void SetValue(
                const Variable<Array1d<double, 6>>& rThisVariable,
                const Array1d<double, 6>& Value,
                const ProcessInfo& rCurrentProcessInfo
            );


            virtual bool& CalculateValue(
                Parameters& rParameterValues,
                const Variable<bool>& rThisVariable,
                bool& rValue
            );


            virtual int& CalculateValue(
                Parameters& rParameterValues,
                const Variable<int>& rThisVariable,
                int& rValue
            );


            virtual double& CalculateValue(
                Parameters& rParameterValues,
                const Variable<double>& rThisVariable,
                double& rValue
            );


            virtual Vector& CalculateValue(
                Parameters& rParameterValues,
                const Variable<Vector>& rThisVariable,
                Vector& rValue
            );


            virtual Matrix& CalculateValue(
                Parameters& rParameterValues,
                const Variable<Matrix>& rThisVariable,
                Matrix& rValue
            );


            virtual Array1d<double, 3>& CalculateValue(
                Parameters& rParameterValues,
                const Variable<Array1d<double, 3>>& rThisVariable,
                Array1d<double, 3>& rValue
            );


            virtual Array1d<double, 6>& CalculateValue(
                Parameters& rParameterValues,
                const Variable<Array1d<double, 6>>& rThisVariable,
                Array1d<double, 6>& rValue
            );


            virtual void CalculateDerivative(
                Parameters& rParameterValues,
                const Variable<double>& rFunctionVariable,
                const Variable<double>& rDerivativeVariable,
                double& rOutput
            );


            virtual void CalculateDerivative(
                Parameters& rParameterValues,
                const Variable<Vector>& rFunctionVariable,
                const Variable<double>& rDerivativeVariable,
                Vector& rOutput
            );


            virtual void CalculateDerivative(
                Parameters& rParameterValues,
                const Variable<Matrix>& rFunctionVariable,
                const Variable<double>& rDerivativeVariable,
                Matrix& rOutput
            );


            virtual void CalculateDerivative(
                Parameters& rParameterValues,
                const Variable<Array1d<double, 3>>& rFunctionVariable,
                const Variable<double>& rDerivativeVariable,
                Array1d<double, 3>& rOutput
            );


            virtual bool ValidateInput(const Properties& rMaterialProperties);


            virtual StrainMeasure GetStrainMeasure();


            virtual StressMeasure GetStressMeasure();


            virtual bool IsIncremental();


            virtual void InitializeMaterial(
                const Properties& rMaterialProperties,
                const GeometryType& rElementGeometry,
                const Vector& rShapeFunctionsValues
            );


            void CalculateMaterialResponse(Parameters& rValues, const StressMeasure& rStressMeasure);


            virtual void CalculateMaterialResponsePK1(Parameters& rValues);


            virtual void CalculateMaterialResponsePK2(Parameters& rValues);


            virtual void CalculateMaterialResponseKirchhoff(Parameters& rValues);


            virtual void CalculateMaterialResponseCauchy(Parameters& rValues);


            virtual bool RequiresInitializeMatrialResponse(){
                return true;
            }


            virtual void CalculateStressResponse(Parameters& rValues, Vector& rInternalVariables);


            void InitializeMaterialResponse(Parameters& rValues, const StressMeasure& rStressMeasure);


            virtual void InitializeMaterialResponsePK1(Parameters& rValues);


            virtual void InitializeMaterialResponsePK2(Parameters& rValues);


            virtual void InitializeMaterialResponseKirchhoff(Parameters& rValues);


            virtual void InitializeMaterialResponseCauchy(Parameters& rValues);


            virtual bool RequiresFinalizeMatrialResponse(){
                return true;
            }


            void FinalizeMaterialResponse(Parameters& rValues, const StressMeasure& rStressMeasure);


            virtual void FinalizeMaterialResponsePK1(Parameters& rValues);


            virtual void FinalizeMaterialResponsePK2(Parameters& rValues);


            virtual void FinalizeMaterialResponseKirchhoff(Parameters& rValues);


            virtual void FinalizeMaterialResponseCauchy(Parameters& rValues);


            virtual void ResetMaterial(
                const Properties& rMaterialProperties,
                const GeometryType& rElementGeometry,
                const Vector& rShapeFunctionsValues
            );


            virtual Vector& TransformStrains(
                Vector& rStrainVector,
                const Matrix& rF,
                StrainMeasure rStrainInitial,
                StrainMeasure rStrainFinal
            );


            virtual Matrix& TransformStresses(
                Matrix& rStressMatrix,
                const Matrix& rF,
                const double& rdetF,
                StressMeasure rStressInitial,
                StressMeasure rStressFinal
            );


            virtual Vector& TransformStresses(
                Vector& rStressVector,
                const Matrix& rF,
                const double& rdetF,
                StressMeasure rStressInitial,
                StressMeasure rStressFinal
            );


            Vector& TransformPK1Stresses(
                Vector& rStressVector,
                const Matrix& rF,
                const double& rdetF,
                StressMeasure rStressFinal
            );


            Vector& TransformPK2Stresses(
                Vector& rStressVector,
                const Matrix& rF,
                const double& rdetF,
                StressMeasure rStressFinal
            );


            Vector& TransformKirchhoffStresses(
                Vector& rStressVector,
                const Matrix& rF,
                const double& rdetF,
                StressMeasure rStressFinal
            );


            Vector& TransformCauchyStresses(
                Vector& rStressVector,
                const Matrix& rF,
                const double& rdetF,
                StressMeasure rStressFinal
            );


            void PullBackConstitutiveMatrix(
                Matrix& rConstitutiveMatrix,
                const Matrix& rF
            );


            void PushForwardConstitutiveMatrix(
                Matrix& rConstitutiveMatrix,
                const Matrix& rF
            );


            virtual void GetLawFeatures(Features& rFeatures);


            virtual int Check(
                const Properties& rMaterialProperties,
                const GeometryType& rElementGeometry,
                const ProcessInfo& rCurrentProcessInfo
            ) const;


            virtual void CalculateCauchyStresses(
                Vector& Cauchy_StressVector,
                const Matrix& F,
                const Vector& PK2_StressVector,
                const Vector& GreenLagrangeStrainVector
            );


            inline static bool HasSameType(const ConstitutiveLaw& rLHS, const ConstitutiveLaw& rRHS){
                return typeid(rLHS) == typeid(rRHS);
            }


            inline static bool HasSameType(const ConstitutiveLaw* rLHS, const ConstitutiveLaw* rRHS){
                return ConstitutiveLaw::HasSameType(*rLHS, *rRHS);
            }


            std::string Info() const override{
                std::stringstream buffer;
                buffer << "ConstitutiveLaw";
                return buffer.str();
            }


            void PrintInfo(std::ostream& rOstream) const override{
                rOstream << Info();
            }


            void PrintData(std::ostream& rOstream) const override{
                rOstream << "ConstitutiveLaw has no data";
            }

        protected:
            static const unsigned int msIndexVoigt3D6C[6][2];
            static const unsigned int msIndexVoigt2D4C[4][2];
            static const unsigned int msIndexVoigt2D3C[3][2];


            void ContraVariantPushForward(Matrix& rMatrix, const Matrix& rF);


            void ContraVariantPullBack(Matrix& rMatrix, const Matrix& rF);


            void CoVariantPushForward(Matrix& rMatrix, const Matrix& rF);


            void CoVariantPullBack(Matrix& rMatrix, const Matrix& rF);


            void ConstitutiveMatrixTransformation(
                Matrix& rConstitutiveMatrix,
                const Matrix& rOriginalConstitutiveMatrix,
                const Matrix& rF
            );


            double& TransformConstitutiveComponent(
                double& rCabcd,
                const Matrix& rConstitutiveMatrix,
                const Matrix& rF,
                const unsigned int& a,
                const unsigned int& b,
                const unsigned int& c,
                const unsigned int& d
            );


            double& GetConstitutiveComponent(
                double& rCabcd,
                const Matrix& rConstitutiveMatrix,
                const unsigned int& a,
                const unsigned int& b,
                const unsigned int& c,
                const unsigned int& d
            );

        private:
            friend class Serializer;


            void save(Serializer& rSerializer) const override{
                QUEST_SERIALIZE_SAVE_BASE_CLASS(rSerializer, Flags);
                rSerializer.save("InitialState", mpInitialState);
            }


            void load(Serializer& rSerializer) override{
                QUEST_SERIALIZE_LOAD_BASE_CLASS(rSerializer, Flags);
                rSerializer.load("InitialState", mpInitialState);
            }

        private:
            InitialState::Pointer mpInitialState = nullptr;

    };

    inline std::istream& operator >> (std::istream& rIstream, ConstitutiveLaw& rThis);

    inline std::ostream& operator << (std::ostream& rOstream, const ConstitutiveLaw& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }


    QUEST_API_EXTERN template class QUEST_API(QUEST_CORE) QuestComponents<ConstitutiveLaw>;
    QUEST_API_EXTERN template class QUEST_API(QUEST_CORE) QuestComponents<Variable<ConstitutiveLaw::Pointer>>;

    void QUEST_API(QUEST_CORE) AddQuestComponent(const std::string& Name, const ConstitutiveLaw& ThisComponent);
    void QUEST_API(QUEST_CORE) AddQuestComponent(const std::string& Name, const Variable<ConstitutiveLaw::Pointer>& ThisComponent);


    #undef QUEST_EXPORT_MACRO
    #define QUEST_EXPORT_MACRO QUEST_API

    QUEST_DEFINE_VARIABLE(ConstitutiveLaw::Pointer, CONSTITUTIVE_LAW)

    #undef QUEST_EXPORT_MACRO
    #define QUEST_EXPORT_MACRO QUEST_API_EXTERN

} // namespace Quest

#endif //QUEST_CONSTITUTIVE_LAW_HPP