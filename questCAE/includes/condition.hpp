#ifndef QUEST_CONDITION_HPP
#define QUEST_CONDITION_HPP

// 项目头文件
#include "includes/properties.hpp"
#include "includes/process_info.hpp"
#include "includes/geometrical_object.hpp"
#include "includes/quest_parameters.hpp"
#include "container/global_pointers_vector.hpp"

namespace Quest{

    /**
     * @class Condition
     * @brief 定义有限元方法（FEM）中涉及的各种条件，如边界条件、载荷条件等
     * @details 继承自 GeometricalObject，从而具备几何信息（如节点、边界和形状函数）以及对这些信息进行处理的能力
     */
    class Condition : public GeometricalObject{
        public:
            QUEST_CLASS_INTRUSIVE_POINTER_DEFINITION(Condition);

            using ConditionType = Condition;
            using BaseType = GeometricalObject;
            using NodeType = Node;
            using PropertiesType = Properties;
            using GeometryType = Geometry<NodeType>;
            using NodesArrayType = Geometry<NodeType>::PointsArrayType;
            using VectorType = Vector;
            using MatrixType = Matrix;
            using IndexType = std::size_t;
            using SizeType = std::size_t;
            using DofType = Dof<double>;
            using EquationIdVectorType = std::vector<std::size_t>;
            using DofsVectorType = std::vector<DofType::Pointer>;
            using DofsArrayType = PointerVectorSet<DofType>;
            using IntegrationMethod = GeometryData::IntegrationMethod;
            using GeometryDataType = GeometryData;

        public:
            /**
             * @brief 构造函数
             * @param NewId 新ID
             */
            explicit Condition(IndexType NewId = 0):
                BaseType(NewId),
                mpProperties(nullptr)
            {}

            /**
             * @brief 构造函数
             * @param NewId 新ID
             * @param ThisNodes 节点数组
             */
            Condition(IndexType NewId, const NodesArrayType& ThisNodes):
                BaseType(NewId, GeometryType::Pointer(new GeometryType(ThisNodes))),
                mpProperties(nullptr)
            {}

            /**
             * @brief 构造函数
             * @param NewId 新ID
             * @param pGeometry 几何对象
             */
            Condition(IndexType NewId, GeometryType::Pointer pGeometry):
                BaseType(NewId, pGeometry),
                mpProperties(nullptr)
            {}

            /**
             * @brief 构造函数
             * @param NewId 新ID
             * @param pGeometry 几何对象
             * @param pProperties 条件属性
             */
            Condition(
                IndexType NewId,
                GeometryType::Pointer pGeometry,
                PropertiesType::Pointer pProperties
            ):  BaseType(NewId, pGeometry),
                mpProperties(pProperties)
            {}

            /**
             * @brief 拷贝构造函数
             */
            Condition(const Condition& rOther):
                BaseType(rOther),
                mpProperties(rOther.mpProperties)
            {}

            /**
             * @brief 析构函数
             */
            ~Condition() override {}

            /**
             * @brief 重载赋值运算符
             */
            Condition& operator = (const Condition& rOther){
                BaseType::operator=(rOther);
                mpProperties = rOther.mpProperties;
                return *this;
            }

            /**
             * @brief 创建一个Condition实例指针
             * @param NewId 新ID
             * @param ThisNodes 节点数组
             * @param pProperties 条件属性
             */
            virtual Pointer Create(
                IndexType NewId,
                NodesArrayType const& ThisNodes,
                PropertiesType::Pointer pProperties
            ) const {
                QUEST_TRY
                QUEST_ERROR << "Please implement the First Create method in your derived Condition" << Info() << std::endl;
                return Quest::make_intrusive<Condition>(NewId, GetGeometry().Create(ThisNodes), pProperties);
                QUEST_CATCH("")
            }

            /**
             * @brief 创建一个Condition实例指针
             * @param NewId 新ID
             * @param pGeometry 几何对象
             * @param pProperties 条件属性
             */
            virtual Pointer Create(
                IndexType NewId,
                GeometryType::Pointer pGeometry,
                PropertiesType::Pointer pProperties
            ) const {
                QUEST_TRY
                QUEST_ERROR << "Please implement the Second Create method in your derived Condition" << Info() << std::endl;
                return Quest::make_intrusive<Condition>(NewId, pGeometry, pProperties);
                QUEST_CATCH("")
            }

            /**
            * @brief 创建一个新的条件指针，并克隆之前条件的数据
            * @param NewId 新条件的ID
            * @param ThisNodes 新条件的节点
            * @param pProperties 分配给新条件的属性
            * @return 指向新条件的指针
            */
            virtual Pointer Clone(IndexType NewId, const NodesArrayType& ThisNodes) const {
                QUEST_TRY
                QUEST_WARNING("Condition") << " Call base claass condition Clone " << std::endl;
                Condition::Pointer p_new_cond = Quest::make_intrusive<Condition>(NewId, GetGeometry().Create(ThisNodes), pGetProperties());
                p_new_cond->SetData(this->GetData());
                p_new_cond->Set(Flags(*this));
                return p_new_cond;
                QUEST_CATCH("")
            }

            /**
             * 该函数确定所有条件自由度 (DOFs) 的条件方程 ID 向量
             * @param rResult 条件方程 ID 向量
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void EquationIdVector(
                EquationIdVectorType& rResult,
                const ProcessInfo& rCurrentProcessInfo
            ) const {
                if(rResult.size() != 0){
                    rResult.resize(0);
                }
            }

            /**
             * 确定条件的自由度 (DOFs) 列表
             * @param ConditionDofList 自由度列表
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void GetDofList(
                DofsVectorType& rElementalDofList,
                const ProcessInfo& rCurrentProcessInfo
            ) const {
                if(rElementalDofList.size() != 0){  
                    rElementalDofList.resize(0);
                }
            }

            /**
             * @brief 获取默认积分方法
             */
            virtual IntegrationMethod GetIntegrationMethod() const {
                return pGetGeometry()->GetDefaultIntegrationMethod();
            }

            /**
             * @brief 获取定义自由度的变量值
             */
            virtual void GetValuesVector(Vector& rValues, int Step = 0) const {
                if(rValues.size() != 0){
                    rValues.resize(0);
                }
            }

            /**
             * @brief 该函数用于获取自由度的一阶时间导数
             */
            virtual void GetFirstDerivativesVector(Vector& rValues, int Step = 0) const {
                if(rValues.size() != 0){
                    rValues.resize(0);
                }
            }

            /**
             * @brief 该函数用于获取自由度的二阶时间导数
             */
            virtual void GetSecondDerivativesVector(Vector& rValues, int Step = 0) const {
                if(rValues.size() != 0){
                    rValues.resize(0);
                }
            }

            /**
             * @brief 用于初始化条件
             */
            virtual void Initialize(const ProcessInfo& rCurrentProcessInfo) {}

            /**
             * @brief 用于重置本构参数和材料属性
             */
            virtual void ResetConstitutiveLaw() {}

            /**
             * @brief 初始化求解步信息
             */
            virtual void InitializeSolutionStep(const ProcessInfo& rCurrentProcessInfo) {}

            /**
             * @brief 初始化非线性迭代信息
             */
            virtual void InitializeNonLinearIteration(const ProcessInfo& rCurrentProcessInfo) {}

            /**
             * @brief 非线性分析迭代后期处理
             */
            virtual void FinalizeNonLinearIteration(const ProcessInfo& rCurrentProcessInfo) {}

            /**
             * @brief 求解步后期处理
             */
            virtual void FinalizeSolutionStep(const ProcessInfo& rCurrentProcessInfo) {}

            /**
             * 在组装过程中调用，用于计算条件对全局系统矩阵和右端项的所有贡献
             * @param rLeftHandSideMatrix 条件左端矩阵
             * @param rRightHandSideVector 条件右端向量
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void CalculateLocalSystem(
                MatrixType& rLeftHandSideMatrix,
                VectorType& rRightHandSideVector,
                const ProcessInfo& rCurrentProcessInfo
            ){
                if(rLeftHandSideMatrix.size1() != 0){
                    rLeftHandSideMatrix.resize(0,0,false);
                }
                if(rRightHandSideVector.size() != 0){
                    rRightHandSideVector.resize(0,false);
                }
            }

            /**
             * 在组装过程中调用，仅用于计算条件左端矩阵
             * @param rLeftHandSideMatrix 条件左端矩阵
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void CalculateLeftHandSide(
                MatrixType& rLeftHandSideMatrix,
                const ProcessInfo& rCurrentProcessInfo
            ){
                if(rLeftHandSideMatrix.size1() != 0){
                    rLeftHandSideMatrix.resize(0,0,false);
                }
            }

            /**
             * 在组装过程中调用，仅用于计算条件右端矩阵
             * @param rLeftHandSideMatrix 条件右端矩阵
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void CalculateRightHandSide(
                VectorType& rRightHandSideVector,
                const ProcessInfo& rCurrentProcessInfo
            ){
                if(rRightHandSideVector.size() != 0){
                    rRightHandSideVector.resize(0,false);
                }
            }

            /**
             * 在组装过程中调用，用于计算左端矩阵（LHS）和右端向量（RHS）的一阶导数贡献
             * @param rLeftHandSideMatrix 条件左端矩阵
             * @param rRightHandSideVector 条件右端向量
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void CalculateFirstDerivativesContributions(
                MatrixType& rLeftHandSideMatrix,
                VectorType& rRightHandSideVector,
                const ProcessInfo& rCurrentProcessInfo
            ){
                if(rLeftHandSideMatrix.size1() != 0){
                    rLeftHandSideMatrix.resize(0,0,false);
                }
                if(rRightHandSideVector.size() != 0){
                    rRightHandSideVector.resize(0,false);
                }
            }

            /**
             * 在组装过程中调用，用于计算条件左端矩阵的一阶导数贡献
             * @param rLeftHandSideMatrix 条件左端矩阵
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void CalculateFirstDerivativesLHS(
                MatrixType& rLeftHandSideMatrix,
                const ProcessInfo& rCurrentProcessInfo
            ){
                if(rLeftHandSideMatrix.size1() != 0){
                    rLeftHandSideMatrix.resize(0,0,false);
                }
            }

            /**
             * 在组装过程中调用，用于计算条件右端向量的一阶导数贡献
             * @param rRightHandSideVector 条件右端向量
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void CalculateFirstDerivativesRHS(
                VectorType& rRightHandSideVector,
                const ProcessInfo& rCurrentProcessInfo
            ){
                if(rRightHandSideVector.size() != 0){
                    rRightHandSideVector.resize(0,false);
                }
            }

            /**
             * 在组装过程中调用，用于计算条件左端矩阵（LHS）和右端向量（RHS）的二阶导数贡献
             * @param rLeftHandSideMatrix 条件左端矩阵
             * @param rRightHandSideVector 条件右端向量
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void CalculateSecondDerivativesContributions(
                MatrixType& rLeftHandSideMatrix,
                VectorType& rRightHandSideVector,
                const ProcessInfo& rCurrentProcessInfo
            ){
                if(rLeftHandSideMatrix.size1() != 0){
                    rLeftHandSideMatrix.resize(0,0,false);
                }
                if(rRightHandSideVector.size() != 0){
                    rRightHandSideVector.resize(0,false);
                }
            }

            /**
             * 在组装过程中调用，用于计算条件左端矩阵的二阶导数贡献
             * @param rLeftHandSideMatrix 条件左端矩阵
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void CalculateSecondDerivativesLHS(
                MatrixType& rLeftHandSideMatrix,
                const ProcessInfo& rCurrentProcessInfo
            ){
                if(rLeftHandSideMatrix.size1() != 0){
                    rLeftHandSideMatrix.resize(0,0,false);
                }
            }

            /**
             * 在组装过程中调用，用于计算条件右端向量的二阶导数贡献
             * @param rRightHandSideVector 条件右端向量
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void CalculateSecondDerivativesRHS(
                VectorType& rRightHandSideVector,
                const ProcessInfo& rCurrentProcessInfo
            ){
                if(rRightHandSideVector.size() != 0){
                    rRightHandSideVector.resize(0,false);
                }
            }

            /**
             * 在组装过程中调用，用于计算条件质量矩阵
             * @param rMassMatrix 条件质量矩阵
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void CalculateMassMatrix(
                MatrixType& rMassMatrix,
                const ProcessInfo& rCurrentProcessInfo
            ){
                if(rMassMatrix.size1() != 0){
                    rMassMatrix.resize(0,0,false);
                }
            }

            /**
             * 在组装过程中调用，用于计算条件阻尼矩阵
             * @param rDampingMatrix 条件阻尼矩阵
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void CalculateDampingMatrix(
                MatrixType& rDampingMatrix,
                const ProcessInfo& rCurrentProcessInfo
            ){
                if(rDampingMatrix.size1() != 0){
                    rDampingMatrix.resize(0,0,false);
                }
            }

            /**
             * @brief 在组装过程中调用，用于计算条件在显式计算中的贡献。
             * 节点数据在函数内部被修改，因此
             * "AddExplicit" 是唯一允许条件在其节点上写入的函数。
             * 调用者需要确保线程安全，因此
             * 在调用此函数之前，必须在策略中执行 SET/UNSETLOCK 操作。
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void AddExplicitContribution(const ProcessInfo& rCurrentProcessInfo) {}

            /**
             * @brief 该函数旨在使条件将 rRHS 向量（由变量 rRHSVariable 标识）组装到节点上，并将其组装到变量 rDestinationVariable 中。（这是双精度版本）
             * @details "AddExplicit" 是唯一允许条件在其节点上写入的函数。调用者需要确保线程安全，因此在调用此函数之前，必须在策略中执行 SET-/UNSET-LOCK 操作。
             * @param rRHSVector 输入变量，包含要组装的 RHS 向量
             * @param rRHSVariable 描述要组装的 RHS 向量类型的变量
             * @param rDestinationVariable 数据库中将组装 rRHSVector 的变量
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void AddExplicitContribution(
                const VectorType& rRHSVector,
                const Variable<VectorType>& rRHSVariable,
                const Variable<double>& rDestinationVariable,
                const ProcessInfo& rCurrentProcessInfo
            ){
                QUEST_ERROR << "Base condition class is not able to assemble rRHS to the desired variable. destination variable is " << rDestinationVariable << std::endl;
            }

            /**
             * @brief 该函数旨在使条件将 rRHS 向量（由变量 rRHSVariable 标识）组装到节点上，并将其组装到变量 rDestinationVariable 中。（这是向量版本）
             * @details "AddExplicit" 是唯一允许条件在其节点上写入的函数。调用者需要确保线程安全，因此在调用此函数之前，必须在策略中执行 SET-/UNSET-LOCK 操作。
             * @param rRHSVector 输入变量，包含要组装的 RHS 向量
             * @param rRHSVariable 描述要组装的 RHS 向量类型的变量
             * @param rDestinationVariable 数据库中将组装 rRHSVector 的变量
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void AddExplicitContribution(
                const VectorType& rRHSVector,
                const Variable<VectorType>& rRHSVariable,
                const Variable<Array1d<double, 3>>& rDestinationVariable,
                const ProcessInfo& rCurrentProcessInfo
            ){
                QUEST_ERROR << "Base condition class is not able to assemble rRHS to the desired variable. destination variable is " << rDestinationVariable << std::endl;
            }

            /**
             * @brief 该函数旨在使条件将 rRHS 向量（由变量 rRHSVariable 标识）组装到节点上，并将其组装到变量 rDestinationVariable 中。（这是矩阵版本）
             * @details "AddExplicit" 是唯一允许条件在其节点上写入的函数。调用者需要确保线程安全，因此在调用此函数之前，必须在策略中执行 SET-/UNSET-LOCK 操作。
             * @param rRHSVector 输入变量，包含要组装的 RHS 向量
             * @param rRHSVariable 描述要组装的 RHS 向量类型的变量
             * @param rDestinationVariable 数据库中将组装 rRHSVector 的变量
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void AddExplicitContribution(
                const MatrixType& rLHSMatrix,
                const Variable<MatrixType>& rLHSVariable,
                const Variable<double>& rDestinationVariable,
                const ProcessInfo& rCurrentProcessInfo
            ){
                QUEST_ERROR << "Base condition class is not able to assemble rLHS to the desired variable. destination variable is " << rDestinationVariable << std::endl;
            }

            /**
             * @brief 计算通常与积分点相关联的条件变量
             */
            virtual void Calculate(
                const Variable<double>& rVariable,
                double& rOutput,
                const ProcessInfo& rCurrentProcessInfo
            ){}

            /**
             * @brief 计算通常与积分点相关联的条件变量
             */
            virtual void Calculate(
                const Variable<Array1d<double, 3>>& rVariable,
                Array1d<double, 3>& rOutput,
                const ProcessInfo& rCurrentProcessInfo
            ){}

            /**
             * @brief 计算通常与积分点相关联的条件变量
             */
            virtual void Calculate(
                const Variable<Vector>& rVariable,
                Vector& rOutput,
                const ProcessInfo& rCurrentProcessInfo
            ){}

            /**
             * @brief 计算通常与积分点相关联的条件变量
             */
            virtual void Calculate(
                const Variable<Matrix>& rVariable,
                Matrix& rOutput,
                const ProcessInfo& rCurrentProcessInfo
            ){}

            /**
             * @brief 在积分点上计算变量。
             * 这提供了访问在每个积分点上计算的本构定律中的变量的途径。
             */
            virtual void CalculateOnIntegrationPoints(
                const Variable<bool>& rVariable,
                std::vector<bool>& rValues,
                const ProcessInfo& rCurrentProcessInfo
            ){}

            /**
             * @brief 在积分点上计算变量。
             * 这提供了访问在每个积分点上计算的本构定律中的变量的途径。
             */
            virtual void CalculateOnIntegrationPoints(
                const Variable<int>& rVariable,
                std::vector<int>& rValues,
                const ProcessInfo& rCurrentProcessInfo
            ){}

            /**
             * @brief 在积分点上计算变量。
             * 这提供了访问在每个积分点上计算的本构定律中的变量的途径。
             */
            virtual void CalculateOnIntegrationPoints(
                const Variable<double>& rVariable,
                std::vector<double>& rValues,
                const ProcessInfo& rCurrentProcessInfo
            ){}

            /**
             * @brief 在积分点上计算变量。
             * 这提供了访问在每个积分点上计算的本构定律中的变量的途径。
             */
            virtual void CalculateOnIntegrationPoints(
                const Variable<Array1d<double, 3>>& rVariable,
                std::vector<Array1d<double, 3>>& rValues,
                const ProcessInfo& rCurrentProcessInfo
            ){}

            /**
             * @brief 在积分点上计算变量。
             * 这提供了访问在每个积分点上计算的本构定律中的变量的途径。
             */
            virtual void CalculateOnIntegrationPoints(
                const Variable<Array1d<double, 4>>& rVariable,
                std::vector<Array1d<double, 4>>& rValues,
                const ProcessInfo& rCurrentProcessInfo
            ){}

            /**
             * @brief 在积分点上计算变量。
             * 这提供了访问在每个积分点上计算的本构定律中的变量的途径。
             */
            virtual void CalculateOnIntegrationPoints(
                const Variable<Array1d<double, 6>>& rVariable,
                std::vector<Array1d<double, 6>>& rValues,
                const ProcessInfo& rCurrentProcessInfo
            ){}

            /**
             * @brief 在积分点上计算变量。
             * 这提供了访问在每个积分点上计算的本构定律中的变量的途径。
             */
            virtual void CalculateOnIntegrationPoints(
                const Variable<Array1d<double, 9>>& rVariable,
                std::vector<Array1d<double, 9>>& rValues,
                const ProcessInfo& rCurrentProcessInfo
            ){}

            /**
             * @brief 在积分点上计算变量。
             * 这提供了访问在每个积分点上计算的本构定律中的变量的途径。
             */
            virtual void CalculateOnIntegrationPoints(
                const Variable<Vector>& rVariable,
                std::vector<Vector>& rValues,
                const ProcessInfo& rCurrentProcessInfo
            ){}

            /**
             * @brief 在积分点上计算变量。
             * 这提供了访问在每个积分点上计算的本构定律中的变量的途径。
             */
            virtual void CalculateOnIntegrationPoints(
                const Variable<Matrix>& rVariable,
                std::vector<Matrix>& rValues,
                const ProcessInfo& rCurrentProcessInfo
            ){}

            /**
             * @brief 设置积分点上变量的值
             * 这提供了访问在每个积分点上设置的本构定律中的变量的途径
             */
            virtual void SetValuesOnIntegrationPoints(
                const Variable<bool>& rVariable,
                const std::vector<bool>& rValues,
                const ProcessInfo& rCurrentProcessInfo
            ){}

            /**
             * @brief 设置积分点上变量的值
             * 这提供了访问在每个积分点上设置的本构定律中的变量的途径
             */
            virtual void SetValuesOnIntegrationPoints(
                const Variable<int>& rVariable,
                const std::vector<int>& rValues,
                const ProcessInfo& rCurrentProcessInfo
            ){}

            /**
             * @brief 设置积分点上变量的值
             * 这提供了访问在每个积分点上设置的本构定律中的变量的途径
             */
            virtual void SetValuesOnIntegrationPoints(
                const Variable<double>& rVariable,
                const std::vector<double>& rValues,
                const ProcessInfo& rCurrentProcessInfo
            ){}

            /**
             * @brief 设置积分点上变量的值
             * 这提供了访问在每个积分点上设置的本构定律中的变量的途径
             */
            virtual void SetValuesOnIntegrationPoints(
                const Variable<Array1d<double, 3>>& rVariable,
                const std::vector<Array1d<double, 3>>& rValues,
                const ProcessInfo& rCurrentProcessInfo
            ){}

            /**
             * @brief 设置积分点上变量的值
             * 这提供了访问在每个积分点上设置的本构定律中的变量的途径
             */
            virtual void SetValuesOnIntegrationPoints(
                const Variable<Array1d<double, 4>>& rVariable,
                const std::vector<Array1d<double, 4>>& rValues,
                const ProcessInfo& rCurrentProcessInfo
            ){}

            /**
             * @brief 设置积分点上变量的值
             * 这提供了访问在每个积分点上设置的本构定律中的变量的途径
             */
            virtual void SetValuesOnIntegrationPoints(
                const Variable<Array1d<double, 6>>& rVariable,
                const std::vector<Array1d<double, 6>>& rValues,
                const ProcessInfo& rCurrentProcessInfo
            ){}

            /**
             * @brief 设置积分点上变量的值
             * 这提供了访问在每个积分点上设置的本构定律中的变量的途径
             */
            virtual void SetValuesOnIntegrationPoints(
                const Variable<Array1d<double, 9>>& rVariable,
                const std::vector<Array1d<double, 9>>& rValues,
                const ProcessInfo& rCurrentProcessInfo
            ){}

            /**
             * @brief 设置积分点上变量的值
             * 这提供了访问在每个积分点上设置的本构定律中的变量的途径
             */
            virtual void SetValuesOnIntegrationPoints(
                const Variable<Vector>& rVariable,
                const std::vector<Vector>& rValues,
                const ProcessInfo& rCurrentProcessInfo
            ){}

            /**
             * @brief 设置积分点上变量的值
             * 这提供了访问在每个积分点上设置的本构定律中的变量的途径
             */
            virtual void SetValuesOnIntegrationPoints(
                const Variable<Matrix>& rVariable,
                const std::vector<Matrix>& rValues,
                const ProcessInfo& rCurrentProcessInfo
            ){}

            /**
             * 该方法用于执行输入完整性检查，以及与问题选项和所选本构定律的兼容性检查。
             * 设计上该方法应仅调用一次（或者说，不频繁调用），通常是在计算开始时调用，以验证输入是否完整，或者是否存在常见的错误。
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual int Check(const ProcessInfo& rCurrentProcessInfo) const {
                QUEST_TRY
                
                QUEST_ERROR_IF(this->Id() < 1) << "Condition found with Id " << this->Id() << std::endl;

                const double domain_size = this->GetGeometry().DomainSize();
                QUEST_ERROR_IF(domain_size <= 0.0) << "Condition " << this->Id() << " has negative size " << domain_size << std::endl;

                GetGeometry().Check();

                return 0;
                QUEST_CATCH("")
            }

            /**
             * @brief 在组装过程中调用此方法，以计算条件质量矩阵
             * @param rMassMatrix 条件质量矩阵
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void MassMatrix(MatrixType& rMassMatrix, const ProcessInfo& rCurrentProcessInfo){
                if(rMassMatrix.size1() != 0){
                    rMassMatrix.resize(0,0,false);
                }
            }

            /**
             * @brief 将按给定因子缩放的质量矩阵添加到 LHS
             * @param rLeftHandSideMatrix 条件 LHS 矩阵
             * @param coeff 给定因子
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void AddMassMatrix(
                MatrixType& rLeftHandSideMatrix,
                double coeff,
                const ProcessInfo& rCurrentProcessInfo
            ){}
            
            /**
             * @brief 在组装过程中调用此方法，以计算条件阻尼矩阵
             * @param rDampMatrix 条件阻尼矩阵
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void DampMatrix(MatrixType& rDampMatrix, const ProcessInfo& rCurrentProcessInfo){
                if(rDampMatrix.size1() != 0){
                    rDampMatrix.resize(0,0,false);
                }
            }

            /**
             * @brief 将惯性力添加到 RHS --> 执行残差 = 静态残差 - coeff*M*加速度
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void AddInertiaForces(
                VectorType& rRightHandSideVector,
                double coeff,
                const ProcessInfo& rCurrentProcessInfo
            ){}
            

            /**
             * @brief 计算阻尼矩阵并将速度贡献添加到 RHS
             * @param rDampingMatrix 与速度成比例的“阻尼”矩阵
             * @param rRightHandSideVector 条件右侧矩阵
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void CalculateLocalVelocityContribution(
                MatrixType& rDampingMatrix,
                VectorType& rRightHandSideVector,
                const ProcessInfo& rCurrentProcessInfo
            ){
                if(rDampingMatrix.size1() != 0){
                    rDampingMatrix.resize(0,0,false);
                }
            }

            /**
             * @brief 计算条件残差关于设计变量的转置梯度
             */
            virtual void CalculateSensitivityMatrix(
                const Variable<double>& rDesignVariable,
                Matrix& rOutput,
                const ProcessInfo& rCurrentProcessInfo
            ){
                if(rOutput.size1() != 0){
                    rOutput.resize(0,0,false);
                }
            }

            /**
             * @brief 计算条件残差关于设计变量的转置梯度
             */
            virtual void CalculateSensitivityMatrix(
                const Variable<Array1d<double, 3>>& rDesignVariable,
                Matrix& rOutput,
                const ProcessInfo& rCurrentProcessInfo
            ){
                if(rOutput.size1() != 0){
                    rOutput.resize(0,0,false);
                }
            }

            /**
             * @brief 获取条件属性的指针
             */
            PropertiesType::Pointer pGetProperties(){
                return mpProperties;
            }

            /**
             * @brief 获取条件属性的指针
             */
            const PropertiesType::Pointer pGetProperties() const{
                return mpProperties;
            }

            /**
             * @brief 获取条件属性引用
             */
            PropertiesType& GetProperties(){
                QUEST_DEBUG_ERROR_IF(mpProperties == nullptr)
                    << "Tryining to get the properties of " << Info()
                    << ", which are uninitialized." << std::endl;
                return *mpProperties;
            }

            /**
             * @brief 获取条件属性引用
             */
            const PropertiesType& GetProperties() const{
                QUEST_DEBUG_ERROR_IF(mpProperties == nullptr)
                    << "Tryining to get the properties of " << Info()
                    << ", which are uninitialized." << std::endl;
                return *mpProperties;
            }

            /**
             * @brief 设置条件属性的指针
             */
            void SetProperties(PropertiesType::Pointer pProperties){
                mpProperties = pProperties;
            }

            /**
             * @brief 检查条件是否具有属性
             */
            bool HasProperties() const{
                return mpProperties != nullptr;
            }

            /**
             * @brief 此方法提供单元的规格/要求
             * @details 可用于增强求解器和分析
             * {
                "time_integration"           : [],                                   // 注：选项为 static, implicit, explicit
                "framework"                  : "eulerian",                           // 注：选项为 eulerian, lagrangian, ALE
                "symmetric_lhs"              : true,                                 // 注：选项为 true/false
                "positive_definite_lhs"      : false,                                // 注：选项为 true/false
                "output"                     : {                                     // 注：与输出兼容的值
                        "gauss_point"            : ["INTEGRATION_WEIGTH"],
                        "nodal_historical"       : ["DISPLACEMENT"],
                        "nodal_non_historical"   : [],
                        "entity"                 : []
                },
                "required_variables"         : ["DISPLACEMENT"],                     // 注：填写所需变量
                "required_dofs"              : ["DISPLACEMENT_X", "DISPLACEMENT_Y"], // 注：填写所需的自由度
                "flags_used"                 : ["BOUNDARY", "ACTIVE"],               // 注：填写使用的标志
                "compatible_geometries"      : ["Triangle2D3"],                      // 注：兼容几何体。选项为 "Point2D", "Point3D", "Sphere3D1", "Line2D2", "Line2D3", "Line3D2", "Line3D3", "Triangle2D3", "Triangle2D6", "Triangle3D3", "Triangle3D6", "Quadrilateral2D4", "Quadrilateral2D8", "Quadrilateral2D9", "Quadrilateral3D4", "Quadrilateral3D8", "Quadrilateral3D9", "Tetrahedra3D4" , "Tetrahedra3D10" , "Prism3D6" , "Prism3D15" , "Hexahedra3D8" , "Hexahedra3D20" , "Hexahedra3D27"
                "element_integrates_in_time" : true,                                 // 注：选项为 true/false
                "compatible_constitutive_laws": {
                    "type"         : ["PlaneStress","PlaneStrain"],                  // 注：兼容的本构类型。选项为 "PlaneStress", "PlaneStrain", "3D"
                    "dimension"   : ["2D", "2D"],                                    // 注：维度列表。选项为 "2D", "3D", "2DAxysimm"
                    "strain_size" : [3,3]                                            // 注：应变尺寸列表
                    },
                "documentation"              : "This is a condition"                 // 注：实体的文档说明
                }
            * @return specifications 所需的规格/要求
            */
            virtual const Parameters GetSpecifications() const
            {
                const Parameters specifications = Parameters(R"({
                    "time_integration"           : [],
                    "framework"                  : "lagrangian",
                    "symmetric_lhs"              : false,
                    "positive_definite_lhs"      : false,
                    "output"                     : {
                        "gauss_point"            : [],
                        "nodal_historical"       : [],
                        "nodal_non_historical"   : [],
                        "entity"                 : []
                    },
                    "required_variables"         : [],
                    "required_dofs"              : [],
                    "flags_used"                 : [],
                    "compatible_geometries"      : [],
                    "element_integrates_in_time" : true,
                    "compatible_constitutive_laws": {
                        "type"        : [],
                        "dimension"   : [],
                        "strain_size" : []
                    },
                    "required_polynomial_degree_of_geometry" : -1,
                    "documentation"   : "This is the base condition"

                })");
                return specifications;
            }


            std::string Info() const override{
                std::stringstream buffer;
                buffer << "Condition #" << this->Id();
                return buffer.str();
            }


            void PrintInfo(std::ostream& rOstream) const override{
                rOstream << this->Info();
            }


            void PrintData(std::ostream& rOstream) const override{
                pGetGeometry()->PrintData(rOstream);
            }

        protected:

        private:
            friend class Serializer;


            void save(Serializer& rSerializer) const override{
                QUEST_SERIALIZE_SAVE_BASE_CLASS(rSerializer, GeometricalObject);
                rSerializer.save("Properties", mpProperties);
            }


            void load(Serializer& rSerializer) override{
                QUEST_SERIALIZE_LOAD_BASE_CLASS(rSerializer, GeometricalObject);
                rSerializer.load("Properties", mpProperties);
            }

        private:
            /**
             * @brief 条件属性的指针
             */
            Properties::Pointer mpProperties;

    };

    inline std::istream& operator >> (std::istream& rIstream, Condition& rThis);

    inline std::ostream& operator << (std::ostream& rOstream, const Condition& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << " : " << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }


    QUEST_API_EXTERN template class QUEST_API(QUEST_CORE) QuestComponents<Condition>;

    void QUEST_API(QUEST_CORE) AddQuestComponent(const std::string& Name, const Condition& ThisComponent);

    #undef QUEST_EXPORT_MACRO
    #define QUEST_EXPORT_MACRO QUEST_API

    QUEST_DEFINE_VARIABLE(GlobalPointersVector<Condition>, NEIGHBOUR_CONDITIONS)

    #undef QUEST_EXPORT_MACRO
    #define QUEST_EXPORT_MACRO QUEST_NO_EXPORT

} // namespace Quest

#endif //QUEST_CONDITION_HPP