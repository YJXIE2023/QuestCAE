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

    /**
     * @class ConstitutiveLaw
     * @brief 本构模型的基类，定义了本构模型的接口
     */
    class QUEST_API(QUEST_CORE) ConstitutiveLaw: public Flags{
        public:
            enum StrainMeasure{
                StrainMeasure_Infinitesimal,    // Infinitesimal应变度量
                StrainMeasure_GreenLagrange,    // GreenLagrange应变度量
                StrainMeasure_Almansi,          // Almansi应变度量

                StrainMeasure_Hencky_Material,  // 亨基应变度量（材料参考配置）
                StrainMeasure_Hencky_Spatial,   // 亨基应变度量（当前配置）

                StrainMeasure_Deformation_Gradient,   // 变形梯度
                StrainMeasure_Right_CauchyGreen,    // 右卡CauchyGreen应变度量
                StrainMeasure_Left_CauchyGreen,     // 左卡CauchyGreen应变度量
                StrainMeasure_Velocity_Gradient      // 速度梯度
            };

            enum StressMeasure{
                StressMeasure_PK1,    // 第一PK应力度量
                StressMeasure_PK2,        // 第二PK应力度量
                StressMeasure_Kirchhoff,        // Kirchhoff应力
                StressMeasure_Cauchy    // 柯西应力
            };

            using ProcessInfoType = ProcessInfo;
            using SizeType = std::size_t;
            using GeometryType = Geometry<Node >;

            using StrainVectorType = Vector;
            using StressVectorType = Vector;
            using VoigtSizeMatrixType = Matrix;
            using DeformationGradientMatrixType = Matrix;

            QUEST_CLASS_POINTER_DEFINITION(ConstitutiveLaw);

            QUEST_DEFINE_LOCAL_FLAG( USE_ELEMENT_PROVIDED_STRAIN );     // 是否使用单元提供的应变
            QUEST_DEFINE_LOCAL_FLAG( COMPUTE_STRESS );                  // 是否计算应力
            QUEST_DEFINE_LOCAL_FLAG( COMPUTE_CONSTITUTIVE_TENSOR );     // 是否计算本构张量
            QUEST_DEFINE_LOCAL_FLAG( COMPUTE_STRAIN_ENERGY );           // 是否计算应变能

            QUEST_DEFINE_LOCAL_FLAG( ISOCHORIC_TENSOR_ONLY );           // 是否仅计算等体积张量
            QUEST_DEFINE_LOCAL_FLAG( VOLUMETRIC_TENSOR_ONLY );          // 是否仅计算体积张量

            QUEST_DEFINE_LOCAL_FLAG( MECHANICAL_RESPONSE_ONLY );        // 是否考虑力学响应
            QUEST_DEFINE_LOCAL_FLAG( THERMAL_RESPONSE_ONLY );           // 是否考虑热响应

            QUEST_DEFINE_LOCAL_FLAG( INCREMENTAL_STRAIN_MEASURE );      // 是否采用增量应变度量

            QUEST_DEFINE_LOCAL_FLAG( INITIALIZE_MATERIAL_RESPONSE );    // 是否初始化材料响应
            QUEST_DEFINE_LOCAL_FLAG( FINALIZE_MATERIAL_RESPONSE );      // 是否完成材料响应

            QUEST_DEFINE_LOCAL_FLAG( FINITE_STRAINS );                  // 是都支持有限应变分析  
            QUEST_DEFINE_LOCAL_FLAG( INFINITESIMAL_STRAINS );           // 是否假设小变形分析

            QUEST_DEFINE_LOCAL_FLAG( THREE_DIMENSIONAL_LAW );           // 是否为三维本构模型   
            QUEST_DEFINE_LOCAL_FLAG( PLANE_STRAIN_LAW );                // 是否诶平面应变模型
            QUEST_DEFINE_LOCAL_FLAG( PLANE_STRESS_LAW );                // 是否为平面应力模型
            QUEST_DEFINE_LOCAL_FLAG( AXISYMMETRIC_LAW );                // 是否为轴对称模型

            QUEST_DEFINE_LOCAL_FLAG( U_P_LAW );                         // 是否为U-P模型

            QUEST_DEFINE_LOCAL_FLAG( ISOTROPIC );                       // 是否为各向同性模型   
            QUEST_DEFINE_LOCAL_FLAG( ANISOTROPIC );                     // 是否为各向异性模型


            struct Features{

                QUEST_CLASS_POINTER_DEFINITION(Features);

                /**
                 * @brief 本构定律的标志
                 */
                Flags mOptions;

                /**
                 * @brief 应变向量的大小
                 */
                SizeType mStrainSize;

                /**
                 * @brief 空间维度
                 */
                SizeType mSpaceDimension;

                /**
                 * @brief 列举了当前本构定律支持的应变测量方法，用于判断与单元之间的兼容性
                 */
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

            /**
             * @struct Parameters
             * @brief 用于单元将参数传递到本构模型中
             */
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
                    /**
                     * @brief 标志
                     */
                    Flags mOptions;

                    /**
                     * @brief 当前变形梯度的行列式的副本
                     */
                    double mDeterminantF;

                    /**
                     * @brief 指向当前应变（总应变）的指针（输入数据）
                     */
                    StrainVectorType* mpStrainVector;

                    /**
                     * @brief 指向当前应力的指针
                     */
                    StressVectorType* mpStressVector;

                    /**
                     * @brief 指向当前积分点处形函数值的指针（输入数据）
                     */
                    const Vector* mpShapeFunctionsValues;

                    /**
                     * @brief 指向当前积分点处形函数导数值的指针（输入数据）
                     */
                    const Matrix* mpShapeFunctionsDerivatives;

                    /**
                     * @brief 指向当前变形梯度的指针
                     */
                    const DeformationGradientMatrixType* mpDeformationGradientF;

                    /**
                     * @brief 指向材料切线矩阵的指针
                     */
                    VoigtSizeMatrixType* mpConstitutiveMatrix;

                    /**
                     * @brief 指向当前 ProcessInfo 实例的指针（输入数据）
                     */
                    const ProcessInfo* mpCurrentProcessInfo;

                    /**
                     * @brief 指向材料属性对象的指针（输入数据）
                     */
                    const Properties* mpMaterialProperties;

                    /**
                     * @brief 指向单元几何形状的指针（输入数据）
                     */
                    const GeometryType* mpElementGeometry;

            }; // struct Parameters

        public:
            /**
             * @brief 构造函数
             */
            ConstitutiveLaw();

            /**
             * @brief 析构函数
             */
            ~ConstitutiveLaw() override {};

            /**
             * @brief 克隆函数
             */
            virtual ConstitutiveLaw::Pointer Clone() const;

            /**
             * @brief 创建一个新的本构模型指针
             * @param NewParameters 新本构模型的配置参数
             * @return 指向新本构模型的指针
             */
            virtual Pointer Create(Quest::Parameters NewParameters) const;

            /**
             * @brief 创建一个新的本构模型指针（包含材料属性的版本）
             * @param NewParameters 新本构模型的配置参数
             * @param rProperties 材料的属性
             * @return 指向新本构模型的指针
             */
            virtual Pointer Create(Quest::Parameters NewParameters, const Properties& rProperties) const;

            /**
             * @brief 获取全局维度
             */
            virtual SizeType WorkingSpaceDimension();

            /**
             * @return 当前本构模型应变向量的大小
             */
            virtual SizeType GetStrainSize() const;

            /**
             * @brief 设置初始状态
             * @param pInitialState 初始状态的指针
             */
            void SetInitialState(InitialState::Pointer pInitialState){
                mpInitialState = pInitialState;
            }

            /**
             * @brief 获取初始状态指针
             */
            InitialState::Pointer pGetInitialState(){
                return mpInitialState;
            }

            /**
             * @brief 获取初始状态
             */
            InitialState& GetInitialState(){
                return *mpInitialState;
            }

            /**
             * @brief 判断是否有初始状态
             */
            bool HasInitialState() const{
                return (mpInitialState != nullptr);
            }

            /**
             * @brief 如果在 InitialState 中定义了初始应力向量，则将其添加
             */
            template<typename TVectorType>
            void AddInitialStressVectorContribution(TVectorType& rStressVector){
                if(this->HasInitialState()){
                    const auto& r_initial_state = this->GetInitialState();
                    noalias(rStressVector) += r_initial_state.GetInitialStressVector();
                }
            }

            /**
             * @brief 如果在 InitialState 中定义了初始应变向量，则将其添加
             */
            template<typename TVectorType>
            void AddInitialStrainVectorContribution(TVectorType& rStrainVector){
                if(this->HasInitialState()){
                    const auto& r_initial_state = this->GetInitialState();
                    noalias(rStrainVector) -= r_initial_state.GetInitialStrainVector();
                }
            }

            /**
             * @brief 如果在 InitialState 中定义了初始应变梯度矩阵，则将其添加
             */
            template<typename TMatrixType>
            void AddInitialDeformationGradientMatrixContribution(TMatrixType& rF){
                if(this->HasInitialState()){
                    const auto& r_initial_state = this->GetInitialState();
                    rF = Prod(r_initial_state.GetInitialDeformationGradientMatrix(), rF);
                }
            }

            /**
             * @brief 返回该本构关系是否具有指定的变量（布尔值）
             */
            virtual bool Has(const Variable<bool>& rThisVariable);

            /**
             * @brief 返回该本构关系是否具有指定的变量（整数值）
             */
            virtual bool Has(const Variable<int>& rThisVariable);

            /**
             * @brief 返回该本构关系是否具有指定的变量（双精度值）
             */
            virtual bool Has(const Variable<double>& rThisVariable);

            /**
             * @brief 返回该本构关系是否具有指定的变量（向量值）
             */
            virtual bool Has(const Variable<Vector>& rThisVariable);

            /**
             * @brief 返回该本构关系是否具有指定的变量（矩阵值）
             */
            virtual bool Has(const Variable<Matrix>& rThisVariable);

            /**
             * @brief 返回该本构关系是否具有指定的变量（3维数组值）
             */
            virtual bool Has(const Variable<Array1d<double, 3>>& rThisVariable);

            /**
             * @brief 返回该本构关系是否具有指定的变量（6维数组值）
             */
            virtual bool Has(const Variable<Array1d<double, 6>>& rThisVariable);

            /**
             * @brief 获取该本构关系的变量的值（布尔值）
             */
            virtual bool& GetValue(const Variable<bool>& rThisVariable, bool& rValue);

            /**
             * @brief 获取该本构关系的变量的值（整数值）
             */
            virtual int& GetValue(const Variable<int>& rThisVariable, int& rValue);

            /**
             * @brief 获取该本构关系的变量的值（双精度值）
             */
            virtual double& GetValue(const Variable<double>& rThisVariable, double& rValue);

            /**
             * @brief 获取该本构关系的变量的值（向量值）
             */
            virtual Vector& GetValue(const Variable<Vector>& rThisVariable, Vector& rValue);

            /**
             * @brief 获取该本构关系的变量的值（矩阵值）
             */
            virtual Matrix& GetValue(const Variable<Matrix>& rThisVariable, Matrix& rValue);

            /**
             * @brief 获取该本构关系的变量的值（3维数组值）
             */
            virtual Array1d<double, 3>& GetValue(const Variable<Array1d<double, 3>>& rThisVariable, Array1d<double, 3>& rValue);

            /**
             * @brief 获取该本构关系的变量的值（6维数组值）
             */
            virtual Array1d<double, 6>& GetValue(const Variable<Array1d<double, 6>>& rThisVariable, Array1d<double, 6>& rValue);

            /**
             * @brief 设置该本构关系的变量的值（布尔值）
             */
            virtual void SetValue(
                const Variable<bool>& rThisVariable,
                const bool& Value,
                const ProcessInfo& rCurrentProcessInfo
            );

            /**
             * @brief 设置该本构关系的变量的值（整数值）
             */
            virtual void SetValue(
                const Variable<int>& rThisVariable,
                const int& Value,
                const ProcessInfo& rCurrentProcessInfo
            );

            /**
             * @brief 设置该本构关系的变量的值（双精度值）
             */
            virtual void SetValue(
                const Variable<double>& rThisVariable,
                const double& Value,
                const ProcessInfo& rCurrentProcessInfo
            );

            /**
             * @brief 设置该本构关系的变量的值（向量值）
             */
            virtual void SetValue(
                const Variable<Vector>& rThisVariable,
                const Vector& Value,
                const ProcessInfo& rCurrentProcessInfo
            );

            /**
             * @brief 设置该本构关系的变量的值（矩阵值）
             */
            virtual void SetValue(
                const Variable<Matrix>& rThisVariable,
                const Matrix& Value,
                const ProcessInfo& rCurrentProcessInfo
            );

            /**
             * @brief 设置该本构关系的变量的值（3维数组值）
             */
            virtual void SetValue(
                const Variable<Array1d<double, 3>>& rThisVariable,
                const Array1d<double, 3>& Value,
                const ProcessInfo& rCurrentProcessInfo
            );

            /**
             * @brief 设置该本构关系的变量的值（6维数组值）
             */
            virtual void SetValue(
                const Variable<Array1d<double, 6>>& rThisVariable,
                const Array1d<double, 6>& Value,
                const ProcessInfo& rCurrentProcessInfo
            );

            /**
             * @brief 计算指定变量的值（布尔类型）
             * @param rParameterValues 本构关系计算所需的参数
             * @param rThisVariable 要返回的变量
             * @param rValue 返回值的引用
             * @param rValue 输出：指定变量的值
             */
            virtual bool& CalculateValue(
                Parameters& rParameterValues,
                const Variable<bool>& rThisVariable,
                bool& rValue
            );

            /**
             * @brief 计算指定变量的值（整数型）
             * @param rParameterValues 本构关系计算所需的参数
             * @param rThisVariable 要返回的变量
             * @param rValue 返回值的引用
             * @param rValue 输出：指定变量的值
             */
            virtual int& CalculateValue(
                Parameters& rParameterValues,
                const Variable<int>& rThisVariable,
                int& rValue
            );

            /**
             * @brief 计算指定变量的值（双精度型）
             * @param rParameterValues 本构关系计算所需的参数
             * @param rThisVariable 要返回的变量
             * @param rValue 返回值的引用
             * @param rValue 输出：指定变量的值
             */
            virtual double& CalculateValue(
                Parameters& rParameterValues,
                const Variable<double>& rThisVariable,
                double& rValue
            );

            /**
             * @brief 计算指定变量的值（向量型）
             * @param rParameterValues 本构关系计算所需的参数
             * @param rThisVariable 要返回的变量
             * @param rValue 返回值的引用
             * @param rValue 输出：指定变量的值
             */
            virtual Vector& CalculateValue(
                Parameters& rParameterValues,
                const Variable<Vector>& rThisVariable,
                Vector& rValue
            );

            /**
             * @brief 计算指定变量的值（矩阵型）
             * @param rParameterValues 本构关系计算所需的参数
             * @param rThisVariable 要返回的变量
             * @param rValue 返回值的引用
             * @param rValue 输出：指定变量的值
             */
            virtual Matrix& CalculateValue(
                Parameters& rParameterValues,
                const Variable<Matrix>& rThisVariable,
                Matrix& rValue
            );

            /**
             * @brief 计算指定变量的值（3维数组型）
             * @param rParameterValues 本构关系计算所需的参数
             * @param rThisVariable 要返回的变量
             * @param rValue 返回值的引用
             * @param rValue 输出：指定变量的值
             */
            virtual Array1d<double, 3>& CalculateValue(
                Parameters& rParameterValues,
                const Variable<Array1d<double, 3>>& rThisVariable,
                Array1d<double, 3>& rValue
            );

            /**
             * @brief 计算指定变量的值（6维数组型）
             * @param rParameterValues 本构关系计算所需的参数
             * @param rThisVariable 要返回的变量
             * @param rValue 返回值的引用
             * @param rValue 输出：指定变量的值
             */
            virtual Array1d<double, 6>& CalculateValue(
                Parameters& rParameterValues,
                const Variable<Array1d<double, 6>>& rThisVariable,
                Array1d<double, 6>& rValue
            );

            /**
             * @brief 计算给定函数的导数(双精度型)
             *
             * 此方法用于计算向量函数（由 rFunctionVariable 表示）相对于 rDerivativeVariable 的导数，
             * 并将结果存储在 rOutput 中。rDerivativeVariable 仅代表高斯点的标量变量。
             *
             * @param rParameterValues      导数计算的输入参数
             * @param rFunctionVariable     用于标识需要计算导数的函数的变量
             * @param rDerivativeVariable   标量导数变量
             * @param rOutput               输出结果，类型与 rFunctionVariable 相同
             */
            virtual void CalculateDerivative(
                Parameters& rParameterValues,
                const Variable<double>& rFunctionVariable,
                const Variable<double>& rDerivativeVariable,
                double& rOutput
            );

            /**
             * @brief 计算给定函数的导数(向量型)
             *
             * 此方法用于计算向量函数（由 rFunctionVariable 表示）相对于 rDerivativeVariable 的导数，
             * 并将结果存储在 rOutput 中。rDerivativeVariable 仅代表高斯点的标量变量。
             *
             * @param rParameterValues      导数计算的输入参数
             * @param rFunctionVariable     用于标识需要计算导数的函数的变量
             * @param rDerivativeVariable   标量导数变量
             * @param rOutput               输出结果，类型与 rFunctionVariable 相同
             */
            virtual void CalculateDerivative(
                Parameters& rParameterValues,
                const Variable<Vector>& rFunctionVariable,
                const Variable<double>& rDerivativeVariable,
                Vector& rOutput
            );

            /**
             * @brief 计算给定函数的导数(矩阵型)
             *
             * 此方法用于计算向量函数（由 rFunctionVariable 表示）相对于 rDerivativeVariable 的导数，
             * 并将结果存储在 rOutput 中。rDerivativeVariable 仅代表高斯点的标量变量。
             *
             * @param rParameterValues      导数计算的输入参数
             * @param rFunctionVariable     用于标识需要计算导数的函数的变量
             * @param rDerivativeVariable   标量导数变量
             * @param rOutput               输出结果，类型与 rFunctionVariable 相同
             */
            virtual void CalculateDerivative(
                Parameters& rParameterValues,
                const Variable<Matrix>& rFunctionVariable,
                const Variable<double>& rDerivativeVariable,
                Matrix& rOutput
            );

            /**
             * @brief 计算给定函数的导数(3维数组型)
             *
             * 此方法用于计算向量函数（由 rFunctionVariable 表示）相对于 rDerivativeVariable 的导数，
             * 并将结果存储在 rOutput 中。rDerivativeVariable 仅代表高斯点的标量变量。
             *
             * @param rParameterValues      导数计算的输入参数
             * @param rFunctionVariable     用于标识需要计算导数的函数的变量
             * @param rDerivativeVariable   标量导数变量
             * @param rOutput               输出结果，类型与 rFunctionVariable 相同
             */
            virtual void CalculateDerivative(
                Parameters& rParameterValues,
                const Variable<Array1d<double, 3>>& rFunctionVariable,
                const Variable<double>& rDerivativeVariable,
                Array1d<double, 3>& rOutput
            );

            /**
             * @brief 用于检查提供的材料参数是否符合当前本构模型的要求
             * @param rMaterialProperties 当前需要验证的材料属性
             */
            virtual bool ValidateInput(const Properties& rMaterialProperties);

            /**
             * @brief 返回该本构模型的期望应变测量方式（默认是线性应变）
             * @return 期望的应变测量方式
             */
            virtual StrainMeasure GetStrainMeasure();

            /**
             * @brief 返回该本构模型的期望应力测量方式（默认是1阶Piola-Kirchhoff应力，使用Voigt符号表示）
             * @return 期望的应变测量方式
             */
            virtual StressMeasure GetStressMeasure();

            /**
             * @brief 返回该本构模型是否采用增量应变/应力形式
             * 注意：默认情况下，所有本构模型应采用总应变形式。
             */
            virtual bool IsIncremental();

            /**
             * @brief 该函数在计算开始时调用（例如，从InitializeElement函数），以初始化本构模型的所有相关属性
             * @param rMaterialProperties 当前元素的属性实例
             * @param rElementGeometry 当前元素的几何形状
             * @param rShapeFunctionsValues 当前积分点的形状函数值
             */
            virtual void InitializeMaterial(
                const Properties& rMaterialProperties,
                const GeometryType& rElementGeometry,
                const Vector& rShapeFunctionsValues
            );

            /**
             * @brief 计算材料响应，包括应力和本构张量
             */
            void CalculateMaterialResponse(Parameters& rValues, const StressMeasure& rStressMeasure);

            /**
             * @brief 计算材料响应，包括1阶Piola-Kirchhoff应力和本构张量
             */
            virtual void CalculateMaterialResponsePK1(Parameters& rValues);

            /**
             * @brief 计算材料响应，包括2阶Piola-Kirchhoff应力和本构张量
             */
            virtual void CalculateMaterialResponsePK2(Parameters& rValues);

            /**
             * @brief 计算材料响应，包括Kirchhoff应力和本构张量
             */
            virtual void CalculateMaterialResponseKirchhoff(Parameters& rValues);

            /**
             * @brief 计算材料响应，包括Cauchy应力和本构张量
             */
            virtual void CalculateMaterialResponseCauchy(Parameters& rValues);

            /**
             * @brief 如果本构模型需要初始化材料响应，在InitializeSolutionStep中由元素调用
             */
            virtual bool RequiresInitializeMatrialResponse(){
                return true;
            }

            /**
             * @brief 计算材料响应，返回Cauchy应力和本构张量，同时返回内部变量
             */
            virtual void CalculateStressResponse(Parameters& rValues, Vector& rInternalVariables);

            /**
             * @brief 初始化材料响应，在InitializeSolutionStep中由单元调用
             */
            void InitializeMaterialResponse(Parameters& rValues, const StressMeasure& rStressMeasure);

            /**
             * @brief 初始化材料响应，以1阶Piola-Kirchhoff应力表示
             */
            virtual void InitializeMaterialResponsePK1(Parameters& rValues);

            /**
             * @brief 初始化材料响应，以2阶Piola-Kirchhoff应力表示
             */
            virtual void InitializeMaterialResponsePK2(Parameters& rValues);

            /**
             * @brief 初始化材料响应，以Kirchhoff应力表示
             */
            virtual void InitializeMaterialResponseKirchhoff(Parameters& rValues);

            /**
             * @brief 初始化材料响应，以Cauchy应力表示
             */
            virtual void InitializeMaterialResponseCauchy(Parameters& rValues);

            /**
             * @brief 如果本构模型需要结束材料响应，在FinalizeSolutionStep中由单元调用
             */
            virtual bool RequiresFinalizeMatrialResponse(){
                return true;
            }

            /**
             * @brief 结束材料响应，在FinalizeSolutionStep中由单元调用
             */
            void FinalizeMaterialResponse(Parameters& rValues, const StressMeasure& rStressMeasure);

            /**
             * @brief 以1阶Piola-Kirchhoff应力为基础结束材料响应
             */
            virtual void FinalizeMaterialResponsePK1(Parameters& rValues);

            /**
             * @brief 以2阶Piola-Kirchhoff应力为基础结束材料响应
             */
            virtual void FinalizeMaterialResponsePK2(Parameters& rValues);

            /**
             * @brief 以Kirchhoff应力为基础结束材料响应
             */
            virtual void FinalizeMaterialResponseKirchhoff(Parameters& rValues);

            /**
             * @brief 以Cauchy应力为基础结束材料响应
             */
            virtual void FinalizeMaterialResponseCauchy(Parameters& rValues);

            /**
             * @brief 这个方法可以用来重置本构模型的所有内部变量
             * （例如，如果一个模型需要重置到其参考状态）
             * @param rMaterialProperties 当前元素的属性实例
             * @param rElementGeometry 当前元素的几何信息
             * @param rShapeFunctionsValues 当前积分点的形状函数值
             * @param 当前的 ProcessInfo 实例
             */
            virtual void ResetMaterial(
                const Properties& rMaterialProperties,
                const GeometryType& rElementGeometry,
                const Vector& rShapeFunctionsValues
            );

            /**
             * @brief 用于转换应变向量的方法：
             * @param rStrainVector 应变张量矩阵，其应力测量将被更改
             * @param rF 配置之间的变形梯度矩阵
             * @param rdetF 配置之间变形梯度矩阵的行列式
             * @param rStrainInitial 给定的 rStrainVector 的应力测量
             * @param rStrainFinal 返回的 rStrainVector 的应力测量
             */
            virtual Vector& TransformStrains(
                Vector& rStrainVector,
                const Matrix& rF,
                StrainMeasure rStrainInitial,
                StrainMeasure rStrainFinal
            );

            /**
             * @brief 用于转换应力矩阵的方法：
             * @param rStressMatrix 应力张量矩阵，其应力测量将被更改
             * @param rF 配置之间的变形梯度矩阵
             * @param rdetF 配置之间变形梯度矩阵的行列式
             * @param rStressInitial 给定的 rStressMatrix 的应力测量
             * @param rStressFinal 返回的 rStressMatrix 的应力测量
             */
            virtual Matrix& TransformStresses(
                Matrix& rStressMatrix,
                const Matrix& rF,
                const double& rdetF,
                StressMeasure rStressInitial,
                StressMeasure rStressFinal
            );

            /**
             * @brief 用于转换应力向量的方法：
             * @param rStressVector 应力张量矩阵，其应力测量将被更改
             * @param rF 配置之间的变形梯度矩阵
             * @param rdetF 配置之间变形梯度矩阵的行列式
             * @param rStressInitial 给定的 rStressVector 的应力测量
             * @param rStressFinal 返回的 rStressVector 的应力测量
             */
            virtual Vector& TransformStresses(
                Vector& rStressVector,
                const Matrix& rF,
                const double& rdetF,
                StressMeasure rStressInitial,
                StressMeasure rStressFinal
            );

            /**
             * @brief 用于转换应力向量的方法，专门针对初始应力测量 PK1：
             * @param rStressVector 应力张量矩阵，其应力测量将被更改
             * @param rF 配置之间的变形梯度矩阵
             * @param rdetF 配置之间变形梯度矩阵的行列式
             * @param rStressFinal 返回的 rStressVector 的应力测量
             */
            Vector& TransformPK1Stresses(
                Vector& rStressVector,
                const Matrix& rF,
                const double& rdetF,
                StressMeasure rStressFinal
            );

            /**
             * @brief 用于转换应力向量的方法，专门针对初始应力测量 PK2：
             * @param rStressVector 应力张量矩阵，其应力测量将被更改
             * @param rF 配置之间的变形梯度矩阵
             * @param rdetF 配置之间变形梯度矩阵的行列式
             * @param rStressFinal 返回的 rStressVector 的应力测量
             */
            Vector& TransformPK2Stresses(
                Vector& rStressVector,
                const Matrix& rF,
                const double& rdetF,
                StressMeasure rStressFinal
            );

            /**
             * @brief 用于转换应力向量的方法，专门针对初始应力测量 Kirchhoff：
             * @param rStressVector 应力张量矩阵，其应力测量将被更改
             * @param rF 配置之间的变形梯度矩阵
             * @param rdetF 配置之间变形梯度矩阵的行列式
             * @param rStressFinal 返回的 rStressVector 的应力测量
             */
            Vector& TransformKirchhoffStresses(
                Vector& rStressVector,
                const Matrix& rF,
                const double& rdetF,
                StressMeasure rStressFinal
            );

            /**
             * @brief 用于转换应力向量的方法，专门针对初始应力测量 Cauchy：
             * @param rStressVector 应力张量矩阵，其应力测量将被更改
             * @param rF 配置之间的变形梯度矩阵
             * @param rdetF 配置之间变形梯度矩阵的行列式
             * @param rStressFinal 返回的 rStressVector 的应力测量
             */
            Vector& TransformCauchyStresses(
                Vector& rStressVector,
                const Matrix& rF,
                const double& rdetF,
                StressMeasure rStressFinal
            );

            /**
             * @brief 用于计算应变张量矩阵的方法,执行本构矩阵的pull-back操作
             * @param rConstitutiveMatrix 本构矩阵
             * @param rF 配置之间的变形梯度矩阵
             */
            void PullBackConstitutiveMatrix(
                Matrix& rConstitutiveMatrix,
                const Matrix& rF
            );

            /**
             * @brief 用于计算应变张量矩阵的方法,执行本构矩阵的push-forward操作
             * @param rConstitutiveMatrix 本构矩阵
             * @param rF 配置之间的变形梯度矩阵
             */
            void PushForwardConstitutiveMatrix(
                Matrix& rConstitutiveMatrix,
                const Matrix& rF
            );

            /**
             * @brief 此函数设计为一次性调用，用于检查与元素的兼容性
             * @param rFeatures
             */
            virtual void GetLawFeatures(Features& rFeatures);

            /**
             * @brief 此函数设计为一次性调用，在提供的输入上以执行所有必要的检查。检查可能是“昂贵的”，因为此函数旨在捕获用户错误。
             * @param rMaterialProperties
             * @param rElementGeometry
             * @param rCurrentProcessInfo
             */
            virtual int Check(
                const Properties& rMaterialProperties,
                const GeometryType& rElementGeometry,
                const ProcessInfo& rCurrentProcessInfo
            ) const;

            /**
             * @brief 计算Cauchy应力张量
             * @param Cauchy_StressVector 输出：Cauchy应力张量
             * @param F 变形梯度矩阵
             * @param PK2_StressVector 输入：2阶Piola-Kirchhoff应力张量
             * @param GreenLagrangeStrainVector 输入：Green-Lagrange应变张量
             */
            virtual void CalculateCauchyStresses(
                Vector& Cauchy_StressVector,
                const Matrix& F,
                const Vector& PK2_StressVector,
                const Vector& GreenLagrangeStrainVector
            );

            /**
             * @brief 判断类的类型是否相同
             */
            inline static bool HasSameType(const ConstitutiveLaw& rLHS, const ConstitutiveLaw& rRHS){
                return typeid(rLHS) == typeid(rRHS);
            }

            /**
             * @brief 判断类的类型是否相同
             */
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

            /**
             * @brief 此方法执行两个张量之间的逆协变推送，即从 2nd Piola-Kirchhoff 应力到 Kirchhoff 应力
             */
            void ContraVariantPushForward(Matrix& rMatrix, const Matrix& rF);

            /**
             * @brief 此方法执行两个张量之间的逆协变拉回，即从 Kirchhoff 应力到 2nd Piola-Kirchhoff 应力
             */
            void ContraVariantPullBack(Matrix& rMatrix, const Matrix& rF);

            /**
             * @brief 此方法执行两个张量之间的协变推送，即从 Green-Lagrange 应变到 Almansi 应变
             */
            void CoVariantPushForward(Matrix& rMatrix, const Matrix& rF);

            /**
             * @brief 此方法执行两个张量之间的逆协变拉回，即从 Almansi 应变到 Green-Lagrange 应变
             */
            void CoVariantPullBack(Matrix& rMatrix, const Matrix& rF);

            /**
             * @brief 此方法在两个本构矩阵之间执行拉回或推送操作
             */
            void ConstitutiveMatrixTransformation(
                Matrix& rConstitutiveMatrix,
                const Matrix& rOriginalConstitutiveMatrix,
                const Matrix& rF
            );

            /**
             * @brief 此方法在两个本构张量分量之间执行拉回或推送操作
             */
            double& TransformConstitutiveComponent(
                double& rCabcd,
                const Matrix& rConstitutiveMatrix,
                const Matrix& rF,
                const unsigned int& a,
                const unsigned int& b,
                const unsigned int& c,
                const unsigned int& d
            );

            /**
             * @brief 此方法从以 Voigt 表示法提供的本构矩阵中获取本构张量分量
             */
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
            /**
             * @brief 初始状态
             */
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