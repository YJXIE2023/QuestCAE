#ifndef QUEST_ELEMENT_HPP
#define QUEST_ELEMENT_HPP

// 项目头文件
#include "includes/properties.hpp"
#include "includes/process_info.hpp"
#include "includes/geometrical_object.hpp"
#include "includes/constitutive_law.hpp"
#include "includes/quest_parameters.hpp"
#include "container/global_pointers_vector.hpp"

namespace Quest{

    /**
     * @class Element
     * @brief 定义了单元的基类，包含了几何信息、本构模型、材料信息、变形信息等
     */
    class Element : public GeometricalObject{
        public:
            QUEST_CLASS_INTRUSIVE_POINTER_DEFINITION(Element);

            using ElementType = Element;
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
             * @param NewId 单元ID
             */
            explicit Element(IndexType NewId = 0):
                BaseType(NewId),
                mpProperties(nullptr){}

            /**
             * @brief 构造函数
             * @param NewId 单元ID
             * @param ThisNodes 单元的节点数组
             */
            Element(IndexType NewId, const NodesArrayType& ThisNodes):
                BaseType(NewId, GeometryType::Pointer(new GeometryType(ThisNodes))),
                mpProperties(nullptr){}

            /**
             * @brief 构造函数
             * @param NewId 单元ID
             * @param pGeometry 几何对象指针
             */
            Element(IndexType NewId, GeometryType::Pointer pGeometry):
                BaseType(NewId, pGeometry),
                mpProperties(nullptr){}

            /**
             * @brief 构造函数
             * @param NewId 单元ID
             * @param pGeometry 几何对象指针
             * @param pProperties 单元属性参数指针
             */
            Element(IndexType NewId, GeometryType::Pointer pGeometry, Properties::Pointer pProperties):
                BaseType(NewId, pGeometry),
                mpProperties(pProperties){}

            /**
             * @brief 拷贝构造函数
             */
            Element(const Element& rOther):
                BaseType(rOther),
                mpProperties(rOther.mpProperties){}

            /**
             * @brief 析构函数
             */
            ~Element() override {}

            /**
             * @brief 赋值运算符
             */
            Element& operator = (const Element& rOther){
                BaseType::operator=(rOther);
                mpProperties = rOther.mpProperties;
                return *this;
            }

            /**
             * @brief 创建一个新的单元指针
             * @param NewId 新单元的 ID
             * @param ThisNodes 新单元的节点
             * @param pProperties 分配给新单元的属性
             * @return 指向新单元的指针
             */
            virtual Pointer Create(
                IndexType NewId,
                const NodesArrayType& ThisNodes,
                Properties::Pointer pProperties
            ) const {
                QUEST_TRY

                QUEST_ERROR << "Please implement the First Create method in your derived Element" << Info() << std::endl;

                return Quest::make_intrusive<Element>(NewId, GetGeometry().Create(ThisNodes), pProperties);

                QUEST_CATCH("")
            }

            /**
             * @brief 创建一个新的单元指针
             * @param NewId 新单元的 ID
             * @param pGeometry 新单元的几何对象
             * @param pProperties 分配给新单元的属性
             * @return 指向新单元的指针
             */
            virtual Pointer Create(
                IndexType NewId,
                GeometryType::Pointer pGeometry,
                Properties::Pointer pProperties
            ) const {
                QUEST_TRY

                QUEST_ERROR << "Please implement the Second Create method in your derived Element" << Info() << std::endl;

                return Quest::make_intrusive<Element>(NewId, pGeometry, pProperties);

                QUEST_CATCH("")
            }

            /**
             * @brief 创建一个新的单元指针，并克隆之前单元的数据
             * @param NewId 新单元的 ID
             * @param ThisNodes 新单元的节点
             * @param pProperties 分配给新单元的属性
             * @return 指向新单元的指针
             */
            virtual Pointer Clone(IndexType NewId, const NodesArrayType& ThisNodes)const{
                QUEST_TRY
                QUEST_WARNING("Element") << "Call base class element Clone " << std::endl;
                Element::Pointer p_new_elem = Quest::make_intrusive<Element>(NewId, GetGeometry().Create(ThisNodes), pGetProperties());
                p_new_elem->SetData(this->GetData());
                p_new_elem->Set(Flags(*this));
                return p_new_elem;
                QUEST_CATCH("")
            }

            /**
             * @brief 确定所有单元自由度（DOFs）的单元方程 ID 向量
             * @param rResult 单元方程 ID 向量
             * @param rCurrentProcessInfo 当前的进程信息实例
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
             * @brief 确定单元自由度（DOFs）列表
             * @param ElementalDofList 自由度列表
             * @param rCurrentProcessInfo 当前的进程信息实例
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
             * @brief 确定单元的积分方法
             */
            virtual IntegrationMethod GetIntegrationMethod() const{
                return BaseType::pGetGeometry()->GetDefaultIntegrationMethod();
            }

            /**
             * @brief 获取定义自由度的变量的值
             */
            virtual void GetValuesVector(Vector& values, int Step = 0) const{
                if(values.size() != 0){
                    values.resize(0, false);
                }
            }

            /**
             * @brief 获取定义自由度的变量的时间导数值
             */
            virtual void GetFirstDerivativesVector(Vector& values, int Step = 0) const{
                if(values.size() != 0){
                    values.resize(0, false);
                }
            }

            /**
             * @brief 获取定义自由度的变量的二阶导数值
             */
            virtual void GetSecondDerivativesVector(Vector& values, int Step = 0) const{
                if(values.size() != 0){
                    values.resize(0, false);
                }
            }

            /**
             * @brief 用于初始化单元
             * @details 如果单元在进行任何计算之前需要执行任何操作，则将使用此方法初始化并设置单元变量
             */
            virtual void Initialize(const ProcessInfo& rCurrentProcessInfo){}

            /**
             * @brief 用于重置本构律参数和材料属性
             * 将使用此方法更改并重置单元变量
             */
            virtual void ResetConstitutiveLaw(){}

            /**
             * @brief 该方法在每个求解步骤开始时被调用
             */
            virtual void InitializeSolutionStep(const ProcessInfo& rCurrentProcessInfo){}

            /**
             * @brief 该方法在非线性分析的迭代过程开始时被调用
             */
            virtual void InitializeNonLinearIteration(const ProcessInfo& rCurrentProcessInfo){}

            /**
             * @brief 该方法在每个求解步骤结束时被调用
             */
            virtual void FinalizeNonLinearIteration(const ProcessInfo& rCurrentProcessInfo){}

            /**
             * @brief 该方法在每个求解步骤结束时被调用
             */
            virtual void FinaalizeSolutionStep(const ProcessInfo& rCurrentProcessInfo){}

            /**
             * @brief 在组装过程中调用此方法，以计算所有单元对全局系统矩阵和右侧向量的贡献
             * @param rLeftHandSideMatrix 单元的左侧矩阵
             * @param rRightHandSideVector 单元的右侧向量
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void CalculateLocalSystem(
                MatrixType& rLeftHandSideMatrix,
                VectorType& rRightHandSideVector,
                const ProcessInfo& rCurrentProcessInfo
            ){
                if(rLeftHandSideMatrix.size1() != 0){
                    rLeftHandSideMatrix.resize(0, 0, false);
                }
                if(rRightHandSideVector.size() != 0){
                    rRightHandSideVector.resize(0, false);
                }
            }

            /**
             * @brief 在组装过程中调用此方法，仅计算单元的左侧矩阵
             * @param rLeftHandSideMatrix 单元的左侧矩阵
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void CalculateLeftHandSide(
                MatrixType& rLeftHandSideMatrix,
                const ProcessInfo& rCurrentProcessInfo
            ){
                if(rLeftHandSideMatrix.size1() != 0){
                    rLeftHandSideMatrix.resize(0, 0, false);
                }
            }

            /**
             * @brief 在组装过程中调用此方法，仅计算单元的右侧向量
             * @param rRightHandSideVector 单元的右侧向量
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void CalculateRightHandSide(
                VectorType& rRightHandSideVector,
                const ProcessInfo& rCurrentProcessInfo
            ){
                if(rRightHandSideVector.size() != 0){
                    rRightHandSideVector.resize(0, false);
                }
            }

            /**
             * @brief 在组装过程中调用此方法，以计算LHS和RHS的第一阶导数贡献
             * @param rLeftHandSideMatrix 单元的左侧矩阵
             * @param rRightHandSideVector 单元的右侧向量
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void CalculateFirstDerivativesContributions(
                MatrixType& rLeftHandSideMatrix,
                VectorType& rRightHandSideVector,
                const ProcessInfo& rCurrentProcessInfo
            ){
                if(rLeftHandSideMatrix.size1() != 0){
                    rLeftHandSideMatrix.resize(0, 0, false);
                }
                if(rRightHandSideVector.size() != 0){
                    rRightHandSideVector.resize(0, false);
                }
            }
            

            /**
             * @brief 在组装过程中调用此方法，以计算第一阶导数贡献的单元左侧矩阵
             * @param rLeftHandSideMatrix 单元的左侧矩阵
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void CalculateFirstDerivativesLHS(
                MatrixType& rLeftHandSideMatrix,
                const ProcessInfo& rCurrentProcessInfo
            ){
                if(rLeftHandSideMatrix.size1() != 0){
                    rLeftHandSideMatrix.resize(0, 0, false);
                }
            }

            /**
             * @brief 在组装过程中调用此方法，以计算第一阶导数贡献的单元右侧向量
             * @param rRightHandSideVector 单元的右侧向量
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void CalculateFirstDerivativesRHS(
                VectorType& rRightHandSideVector,
                const ProcessInfo& rCurrentProcessInfo
            ){
                if(rRightHandSideVector.size() != 0){
                    rRightHandSideVector.resize(0, false);
                }
            }

            /**
             * @brief 在组装过程中调用此方法，以计算LHS和RHS的第二阶导数贡献
             * @param rLeftHandSideMatrix 单元的左侧矩阵
             * @param rRightHandSideVector 单元的右侧向量
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void CalculateSecondDerivativesContributions(
                MatrixType& rLeftHandSideMatrix,
                VectorType& rRightHandSideVector,
                const ProcessInfo& rCurrentProcessInfo
            ){
                if(rLeftHandSideMatrix.size1() != 0){
                    rLeftHandSideMatrix.resize(0, 0, false);
                }
                if(rRightHandSideVector.size() != 0){
                    rRightHandSideVector.resize(0, false);
                }
            }

            /**
             * @brief 在组装过程中调用此方法，以计算第二阶导数贡献的单元左侧矩阵
             * @param rLeftHandSideMatrix 单元的左侧矩阵
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void CalculateSecondDerivativesLHS(
                MatrixType& rLeftHandSideMatrix,
                const ProcessInfo& rCurrentProcessInfo
            ){
                if(rLeftHandSideMatrix.size1() != 0){
                    rLeftHandSideMatrix.resize(0, 0, false);
                }
            }

            /**
             * @brief 在组装过程中调用此方法，以计算第二阶导数贡献的单元右侧向量
             * @param rRightHandSideVector 单元的右侧向量
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void CalculateSecondDerivativesRHS(
                VectorType& rRightHandSideVector,
                const ProcessInfo& rCurrentProcessInfo
            ){
                if(rRightHandSideVector.size() != 0){
                    rRightHandSideVector.resize(0, false);
                }
            }

            /**
             * @brief 在组装过程中调用此方法，以计算单元的质量矩阵
             * @param rMassMatrix 单元的质量矩阵
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void CalculateMassMatrix(
                Matrix& rMassMatrix,
                const ProcessInfo& rCurrentProcessInfo
            ){
                if(rMassMatrix.size1() != 0){
                    rMassMatrix.resize(0, 0, false);
                }
            }

            /**
             * @brief 在组装过程中调用此方法，以计算单元的阻尼矩阵
             * @param rDampingMatrix 单元的阻尼矩阵
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void CalculateDampingMatrix(
                Matrix& rDampingMatrix,
                const ProcessInfo& rCurrentProcessInfo
            ){
                if(rDampingMatrix.size1() != 0){
                    rDampingMatrix.resize(0, 0, false);
                }
            }

            /**
             * @brief 在构建器初始化过程中调用此方法，以计算集中质量向量
             * @param rLumpedMassVector 单元的集中质量向量
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void CalculateLumpedMassVector(
                Vector& rLumpedMassVector,
                const ProcessInfo& rCurrentProcessInfo
            ) const {
                QUEST_TRY
                QUEST_ERROR << "Calling the CalculateLumpedMassVector method of the base element. The method must be implemented in the derived element.";
                QUEST_CATCH("")
            }

            /**
             * @brief 在组装过程中调用此方法，以计算显式计算中的单元贡献。
             * NodalData 在函数内部被修改，因此“AddExplicit” 是唯一允许单元在其节点上写入的函数。
             * 调用者应确保线程安全，因此在调用此函数之前必须在策略中执行 SET/UNSETLOCK。
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void AddExplicitContribution(const ProcessInfo& rCurrentProcessInfo){}

            /**
             * @brief 该函数用于使单元组装由变量 rRHSVariable 标识的 rRHS 向量，并将其组装到变量 rDestinationVariable 上的节点。（这是双精度版本）
             * @details “AddExplicit”是唯一允许单元在其节点上写入的函数。调用者应确保线程安全，因此在调用此函数之前必须在策略中执行 SET-/UNSET-LOCK。
             * @param rRHSVector 输入变量，包含要组装的RHS向量
             * @param rRHSVariable 描述要组装的RHS向量类型的变量
             * @param rDestinationVariable 数据库中将组装 rRHSVector 的变量
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void AddExplicitContribution(
                const VectorType& rRHSVector,
                const Variable<VectorType>& rRHSVariable,
                const Variable<double>& rDestinationVariable,
                const ProcessInfo& rCurrentProcessInfo
            ){
                QUEST_ERROR << "Base element class is not able to assemble rRHS to the desired variable. destination variable is " << rDestinationVariable << std::endl;
            }

            /**
             * @brief 该函数用于使单元组装由变量 rRHSVariable 标识的 rRHS 向量，并将其组装到变量 rDestinationVariable 上的节点。（这是双精度版本）
             * @details “AddExplicit”是唯一允许单元在其节点上写入的函数。调用者应确保线程安全，因此在调用此函数之前必须在策略中执行 SET-/UNSET-LOCK。
             * @param rRHSVector 输入变量，包含要组装的RHS向量
             * @param rRHSVariable 描述要组装的RHS向量类型的变量
             * @param rDestinationVariable 数据库中将组装 rRHSVector 的变量
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void AddExplicitContribution(
                const VectorType& rRHSVector,
                const Variable<VectorType>& rRHSVariable,
                const Variable<Array1d<double, 3>>& rDestinationVariable,
                const ProcessInfo& rCurrentProcessInfo
            ){
                QUEST_ERROR << "Base element class is not able to assemble rRHS to the desired variable. destination variable is " << rDestinationVariable << std::endl;
            }

            /**
             * @brief 该函数用于使单元组装由变量 rRHSVariable 标识的 rRHS 向量，并将其组装到变量 rDestinationVariable 上的节点。（这是矩阵版本）
             * @details “AddExplicit”是唯一允许单元在其节点上写入的函数。调用者应确保线程安全，因此在调用此函数之前必须在策略中执行 SET-/UNSET-LOCK。
             * @param rRHSVector 输入变量，包含要组装的RHS向量
             * @param rRHSVariable 描述要组装的RHS向量类型的变量
             * @param rDestinationVariable 数据库中将组装 rRHSVector 的变量
             * @param rCurrentProcessInfo 当前过程信息实例
             */
            virtual void AddExplicitContribution(
                const MatrixType& rLHSMatrix,
                const Variable<MatrixType>& rLHSVariable,
                const Variable<Matrix>& rDestinationVariable,
                const ProcessInfo& rCurrentProcessInfo
            ){
                QUEST_ERROR << "Base element class is not able to assemble rLHS to the desired variable. destination variable is " << rDestinationVariable << std::endl;
            }

            /**
             * @brief 计算通常与积分点相关的单元变量
             * 输出给出在积分点上的值，描述了该单元的状态
             */
            virtual void Calculate(
                const Variable<double>& rVariable,
                double& rOutput,
                const ProcessInfo& rCurrentProcessInfo
            ){}
            

            /**
             * @brief 计算通常与积分点相关的单元变量
             * 输出给出在积分点上的值，描述了该单元的状态
             */
            virtual void Calculate(
                const Variable<Array1d<double, 3>>& rVariable,
                Array1d<double, 3>& rOutput,
                const ProcessInfo& rCurrentProcessInfo
            ){}

            /**
             * @brief 计算通常与积分点相关的单元变量
             * 输出给出在积分点上的值，描述了该单元的状态
             */
            virtual void Calculate(
                const Variable<Vector>& rVariable,
                Vector& rOutput,
                const ProcessInfo& rCurrentProcessInfo
            ){}

            /**
             * @brief 计算通常与积分点相关的单元变量
             * 输出给出在积分点上的值，描述了该单元的状态
             */
            virtual void Calculate(
                const Variable<Matrix>& rVariable,
                Matrix& rOutput,
                const ProcessInfo& rCurrentProcessInfo
            ){}

            /**
             * @brief 积分点计算变量
             * @details 这提供了访问在每个积分点上由本构定律计算的变量的能力。
             * 这些函数期望指定的变量类型的 std::vector 值，该向量包含每个积分点的一个值！
             */
            virtual void CalculateOnIntegrationPoints(
                const Variable<bool>& rVariable,
                std::vector<bool>& rOutput,
                const ProcessInfo& rCurrentProcessInfo
            ){}
            

            /**
             * @brief 积分点计算变量
             * @details 这提供了访问在每个积分点上由本构定律计算的变量的能力。
             * 这些函数期望指定的变量类型的 std::vector 值，该向量包含每个积分点的一个值！
             */
            virtual void CalculateOnIntegrationPoints(
                const Variable<int>& rVariable,
                std::vector<int>& rOutput,
                const ProcessInfo& rCurrentProcessInfo
            ){}
            

            /**
             * @brief 积分点计算变量
             * @details 这提供了访问在每个积分点上由本构定律计算的变量的能力。
             * 这些函数期望指定的变量类型的 std::vector 值，该向量包含每个积分点的一个值！
             */
            virtual void CalculateOnIntegrationPoints(
                const Variable<double>& rVariable,
                std::vector<double>& rOutput,
                const ProcessInfo& rCurrentProcessInfo
            ){}
            

            /**
             * @brief 积分点计算变量
             * @details 这提供了访问在每个积分点上由本构定律计算的变量的能力。
             * 这些函数期望指定的变量类型的 std::vector 值，该向量包含每个积分点的一个值！
             */
            virtual void CalculateOnIntegrationPoints(
                const Variable<Array1d<double, 3>>& rVariable,
                std::vector<Array1d<double, 3>>& rOutput,
                const ProcessInfo& rCurrentProcessInfo
            ){}

            /**
             * @brief 积分点计算变量
             * @details 这提供了访问在每个积分点上由本构定律计算的变量的能力。
             * 这些函数期望指定的变量类型的 std::vector 值，该向量包含每个积分点的一个值！
             */
            virtual void CalculateOnIntegrationPoints(
                const Variable<Array1d<double, 4>>& rVariable,
                std::vector<Array1d<double, 4>>& rOutput,
                const ProcessInfo& rCurrentProcessInfo
            ){}

            /**
             * @brief 积分点计算变量
             * @details 这提供了访问在每个积分点上由本构定律计算的变量的能力。
             * 这些函数期望指定的变量类型的 std::vector 值，该向量包含每个积分点的一个值！
             */
            virtual void CalculateOnIntegrationPoints(
                const Variable<Array1d<double, 6>>& rVariable,
                std::vector<Array1d<double, 6>>& rOutput,
                const ProcessInfo& rCurrentProcessInfo
            ){}

            /**
             * @brief 积分点计算变量
             * @details 这提供了访问在每个积分点上由本构定律计算的变量的能力。
             * 这些函数期望指定的变量类型的 std::vector 值，该向量包含每个积分点的一个值！
             */
            virtual void CalculateOnIntegrationPoints(
                const Variable<Array1d<double, 9>>& rVariable,
                std::vector<Array1d<double, 9>>& rOutput,
                const ProcessInfo& rCurrentProcessInfo
            ){}

            /**
             * @brief 积分点计算变量
             * @details 这提供了访问在每个积分点上由本构定律计算的变量的能力。
             * 这些函数期望指定的变量类型的 std::vector 值，该向量包含每个积分点的一个值！
             */
            virtual void CalculateOnIntegrationPoints(
                const Variable<Vector>& rVariable,
                std::vector<Vector>& rOutput,
                const ProcessInfo& rCurrentProcessInfo
            ){}

            /**
             * @brief 积分点计算变量
             * @details 这提供了访问在每个积分点上由本构定律计算的变量的能力。
             * 这些函数期望指定的变量类型的 std::vector 值，该向量包含每个积分点的一个值！
             */
            virtual void CalculateOnIntegrationPoints(
                const Variable<Matrix>& rVariable,
                std::vector<Matrix>& rOutput,
                const ProcessInfo& rCurrentProcessInfo
            ){}

            /**
             * @brief 积分点计算变量
             * @details 这提供了访问在每个积分点上由本构定律计算的变量的能力。
             * 这些函数期望指定的变量类型的 std::vector 值，该向量包含每个积分点的一个值！
             */
            virtual void CalculateOnIntegrationPoints(
                const Variable<ConstitutiveLaw::Pointer>& rVariable,
                std::vector<ConstitutiveLaw::Pointer>& rValues,
                const ProcessInfo& rCurrentProcessInfo
            ){}

            /**
             * @brief 访问积分点上的变量。
             * @details 这提供了访问在每个积分点上由本构定律存储的变量的能力。
             * 这些函数期望指定的变量类型的 std::vector 值，该向量包含每个积分点的一个值！
             */
            virtual void SetValuesOnIntegrationPoints(
                const Variable<bool>& rVariable,
                const std::vector<bool>& rValues,
                const ProcessInfo& rCurrentProcessInfo
            ){}
            

            /**
             * @brief 访问积分点上的变量。
             * @details 这提供了访问在每个积分点上由本构定律存储的变量的能力。
             * 这些函数期望指定的变量类型的 std::vector 值，该向量包含每个积分点的一个值！
             */
            virtual void SetValuesOnIntegrationPoints(
                const Variable<int>& rVariable,
                const std::vector<int>& rValues,
                const ProcessInfo& rCurrentProcessInfo
            ){}

            /**
             * @brief 访问积分点上的变量。
             * @details 这提供了访问在每个积分点上由本构定律存储的变量的能力。
             * 这些函数期望指定的变量类型的 std::vector 值，该向量包含每个积分点的一个值！
             */
            virtual void SetValuesOnIntegrationPoints(
                const Variable<double>& rVariable,
                const std::vector<double>& rValues,
                const ProcessInfo& rCurrentProcessInfo
            ){}

            /**
             * @brief 访问积分点上的变量。
             * @details 这提供了访问在每个积分点上由本构定律存储的变量的能力。
             * 这些函数期望指定的变量类型的 std::vector 值，该向量包含每个积分点的一个值！
             */
            virtual void SetValuesOnIntegrationPoints(
                const Variable<Array1d<double, 3>>& rVariable,
                const std::vector<Array1d<double, 3>>& rValues,
                const ProcessInfo& rCurrentProcessInfo
            ){}

            /**
             * @brief 访问积分点上的变量。
             * @details 这提供了访问在每个积分点上由本构定律存储的变量的能力。
             * 这些函数期望指定的变量类型的 std::vector 值，该向量包含每个积分点的一个值！
             */
            virtual void SetValuesOnIntegrationPoints(
                const Variable<Array1d<double, 4>>& rVariable,
                const std::vector<Array1d<double, 4>>& rValues,
                const ProcessInfo& rCurrentProcessInfo
            ){}

            /**
             * @brief 访问积分点上的变量。
             * @details 这提供了访问在每个积分点上由本构定律存储的变量的能力。
             * 这些函数期望指定的变量类型的 std::vector 值，该向量包含每个积分点的一个值！
             */
            virtual void SetValuesOnIntegrationPoints(
                const Variable<Array1d<double, 6>>& rVariable,
                const std::vector<Array1d<double, 6>>& rValues,
                const ProcessInfo& rCurrentProcessInfo
            ){}

            /**
             * @brief 访问积分点上的变量。
             * @details 这提供了访问在每个积分点上由本构定律存储的变量的能力。
             * 这些函数期望指定的变量类型的 std::vector 值，该向量包含每个积分点的一个值！
             */
            virtual void SetValuesOnIntegrationPoints(
                const Variable<Array1d<double, 9>>& rVariable,
                const std::vector<Array1d<double, 9>>& rValues,
                const ProcessInfo& rCurrentProcessInfo
            ){}

            /**
             * @brief 访问积分点上的变量。
             * @details 这提供了访问在每个积分点上由本构定律存储的变量的能力。
             * 这些函数期望指定的变量类型的 std::vector 值，该向量包含每个积分点的一个值！
             */
            virtual void SetValuesOnIntegrationPoints(
                const Variable<Vector>& rVariable,
                const std::vector<Vector>& rValues,
                const ProcessInfo& rCurrentProcessInfo
            ){}

            /**
             * @brief 访问积分点上的变量。
             * @details 这提供了访问在每个积分点上由本构定律存储的变量的能力。
             * 这些函数期望指定的变量类型的 std::vector 值，该向量包含每个积分点的一个值！
             */
            virtual void SetValuesOnIntegrationPoints(
                const Variable<Matrix>& rVariable,
                const std::vector<Matrix>& rValues,
                const ProcessInfo& rCurrentProcessInfo
            ){}

            /**
             * @brief 访问积分点上的变量。
             * @details 这提供了访问在每个积分点上由本构定律存储的变量的能力。
             * 这些函数期望指定的变量类型的 std::vector 值，该向量包含每个积分点的一个值！
             */
            virtual void SetValuesOnIntegrationPoints(
                const Variable<ConstitutiveLaw::Pointer>& rVariable,
                const std::vector<ConstitutiveLaw::Pointer>& rValues,
                const ProcessInfo& rCurrentProcessInfo
            ){}

            /**
             * @brief 在输入的完整性和与问题选项以及选择的本构定律的兼容性上进行检查
             * 它设计为只调用一次，通常在计算开始时调用，以验证输入中没有遗漏的内容或者没有发现常见错误。
             * @param rCurrentProcessInfo 包含当前进程信息的实例
             */
            virtual int Check(const ProcessInfo& rCurrentProcessInfo) const{
                QUEST_TRY

                QUEST_ERROR_IF(this->Id() < 1) << "Element found with Id " << this->Id() << std::endl;

                const double domain_size = BaseType::GetGeometry().DomainSize();
                QUEST_ERROR_IF(domain_size <= 0.0) << "Element " << this->Id() << " has non-positive size " << domain_size << std::endl;

                BaseType::GetGeometry().Check();

                return 0;

                QUEST_CATCH("")
            }

            /**
             * @brief 该方法在组装过程中被调用，用于计算单元的质量矩阵
             * @param rMassMatrix 单元的质量矩阵
             * @param rCurrentProcessInfo 当前的过程信息实例
             */
            virtual void MassMatrix(
                MatrixType& rMassMatrix,
                const ProcessInfo& rCurrentProcessInfo
            ){
                if(rMassMatrix.size1() != 0){
                    rMassMatrix.resize(0, 0, false);
                }
            }

            /**
             * @brief 将质量矩阵乘以给定的因子后添加到左侧矩阵 (LHS)。
             * @param rLeftHandSideMatrix 单元的左侧矩阵 (LHS)
             * @param coeff 给定的因子
             * @param rCurrentProcessInfo 当前的过程信息实例
             */
            virtual void AddMassMatrix(
                MatrixType& rLeftHandSideMatrix,
                double coeff,
                const ProcessInfo& rCurrentProcessInfo
            ){}

            /**
             * @brief 在组装过程中调用此方法，用于计算单元的阻尼矩阵。
             * @param rDampMatrix 单元的阻尼矩阵
             * @param rCurrentProcessInfo 当前的过程信息实例
             */
            virtual void DampMatrix(
                MatrixType& rDampMatrix,
                const ProcessInfo& rCurrentProcessInfo
            ){
                if(rDampMatrix.size1() != 0){
                    rDampMatrix.resize(0, 0, false);
                }
            }
            

            /**
             * @brief 将惯性力添加到右端项（RHS）--> 执行残差 = 静态残差 - 系数 * M * 加速度
             * @param rCurrentProcessInfo 当前的过程信息实例
             */
            virtual void AddInertiaForces(
                VectorType& rRightHandSideVector,
                double coeff,
                const ProcessInfo& rCurrentProcessInfo
            ){}
            

            /**
             * @brief 计算阻尼矩阵并将速度贡献添加到右端项（RHS）
             * @param rDampingMatrix 速度比例的“阻尼”矩阵
             * @param rRightHandSideVector 单元的右端项向量
             * @param rCurrentProcessInfo 当前的过程信息实例
             */
            virtual void CalculateLocalVelocityContribution(
                MatrixType& rDampingMatrix,
                VectorType& rRightHandSideVector,
                const ProcessInfo& rCurrentProcessInfo
            ){
                if(rDampingMatrix.size1() != 0){
                    rDampingMatrix.resize(0, 0, false);
                }
            }

            /**
             * @brief 计算单元残差相对于设计变量的转置梯度
             */
            virtual void CalculateSensitivityMatrix(
                const Variable<double>& rDesignVariable,
                Matrix& rOutput,
                const ProcessInfo& rCurrentProcessInfo
            ){
                if(rOutput.size1() != 0){
                    rOutput.resize(0, 0, false);
                }
            }

            /**
             * @brief 计算单元残差相对于设计变量的转置梯度
             */
            virtual void CalculateSensitivityMatrix(
                const Variable<Array1d<double, 3>>& rDesignVariable,
                Matrix& rOutput,
                const ProcessInfo& rCurrentProcessInfo
            ){
                if(rOutput.size1() != 0){
                    rOutput.resize(0, 0, false);
                }
            }
            
            /**
             * @brief 获取单元的属性参数指针
             */
            PropertiesType::Pointer pGetProperties(){
                return mpProperties;
            }

            /**
             * @brief 获取单元的属性参数指针
             */
            const PropertiesType::Pointer pGetProperties() const{
                return mpProperties;
            }

            /**
             * @brief 获取单元的属性参数
             */
            PropertiesType& GetProperties(){
                QUEST_DEBUG_ERROR_IF(mpProperties == nullptr)
                    << "Tryining to get the properties of " << Info()
                    << ", which are uninitialized." << std::endl;
                return *mpProperties;
            }

            /**
             * @brief 获取单元的属性参数
             */
            const PropertiesType& GetProperties() const{
                QUEST_DEBUG_ERROR_IF(mpProperties == nullptr)
                    << "Tryining to get the properties of " << Info()
                    << ", which are uninitialized." << std::endl;
                return *mpProperties;
            }

            /**
             * @brief 设置单元的属性参数
             */
            void SetProperties(PropertiesType::Pointer pProperties){
                mpProperties = pProperties;
            }

            /**
             * @brief 确定单元是否具有属性参数
             */
            bool HasProperties() const{
                return mpProperties != nullptr;
            }

            /**
             * @brief 此方法提供单元的规格/要求
             * @details 这可以用于增强求解器和分析。以下是一个示例：
             * {
                    "time_integration"           : [],                                   // 注意：选项为静态、隐式、显式
                    "framework"                  : "eulerian",                           // 注意：选项为eulerian、lagrangian、ALE
                    "symmetric_lhs"              : true,                                 // 注意：选项为true/false
                    "positive_definite_lhs"      : false,                                // 注意：选项为true/false
                    "output"                     : {                                     // 注意：兼容的输出值
                            "gauss_point"            : ["INTEGRATION_WEIGTH"],
                            "nodal_historical"       : ["DISPLACEMENT"],
                            "nodal_non_historical"   : [],
                            "entity"                 : []
                    },
                    "required_variables"         : ["DISPLACEMENT"],                     // 注意：填充所需的变量
                    "required_dofs"              : ["DISPLACEMENT_X", "DISPLACEMENT_Y"], // 注意：填充所需的自由度
                    "flags_used"                 : ["BOUNDARY", "ACTIVE"],               // 注意：填充所使用的标志
                    "compatible_geometries"      : ["Triangle2D3"],                      // 注意：兼容的几何体。选项有"Point2D"、"Point3D"、"Sphere3D1"、"Line2D2"、"Line2D3"、"Line3D2"、"Line3D3"、"Triangle2D3"、"Triangle2D6"、"Triangle3D3"、"Triangle3D6"、"Quadrilateral2D4"、"Quadrilateral2D8"、"Quadrilateral2D9"、"Quadrilateral3D4"、"Quadrilateral3D8"、"Quadrilateral3D9"、"Tetrahedra3D4" 、"Tetrahedra3D10" 、"Prism3D6" 、"Prism3D15" 、"Hexahedra3D8" 、"Hexahedra3D20" 、"Hexahedra3D27"
                    "element_integrates_in_time" : true,                                 // 注意：选项为true/false
                    "compatible_constitutive_laws": {
                        "type"         : ["PlaneStress","PlaneStrain"],                  // 注意：兼容的CL类型列表。选项有"PlaneStress"、"PlaneStrain"、"3D"
                        "dimension"   : ["2D", "2D"],                                    // 注意：维度列表。选项有"2D"、"3D"、"2DAxysimm"
                        "strain_size" : [3,3]                                            // 注意：应变大小的列表
                        },
                    "documentation"              : "这是一个单元"                          // 注意：实体的文档
                    }
            * @return specifications 所需的规格/要求
            */
            virtual const Parameters GetSpecifications() const{
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
                    "documentation"   : "This is the base element"

                })");
                return specifications;
            }


            std::string Info() const override{
                std::stringstream buffer;
                buffer << "Element #" << this->Id();
                return buffer.str();
            }


            void PrintInfo(std::ostream& rOstream) const override{
                rOstream << this->Info();
            }


            void PrintData(std::ostream& rOstream) const override{
                BaseType::pGetGeometry()->PrintData(rOstream);
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
             * @brief 存储与单元相关的属性参数
             */
            Properties::Pointer mpProperties;

    };

    inline std::istream& operator >> (std::istream& rIstream, Element& rThis);

    inline std::ostream& operator << (std::ostream& rOstream, const Element& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }

    QUEST_API_EXTERN template class QUEST_API(QUEST_CORE) QuestComponents<Element>;

    void QUEST_API(QUEST_CORE) AddQuestComponent(const std::string& Name, const Element& ThisComponent);

    #undef QUEST_EXPORT_MACRO
    #define QUEST_EXPORT_MACRO QUEST_API

    QUEST_DEFINE_VARIABLE(GlobalPointersVector< Element >, NEIGHBOUR_ELEMENTS)

    #undef QUEST_EXPORT_MACRO
    #define QUEST_EXPORT_MACRO QUEST_API_EXTERN

} // namespace Quest

#endif //QUEST_ELEMENT_HPP