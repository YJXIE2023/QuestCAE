/*---------------------------------------------------------------------------
Element 类是所有单元（elements）的基类
从 GeometricalObject 继承，具有 GeometricalObject 的基本属性（如几何信息和标识符）
---------------------------------------------------------------------------*/

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
            explicit Element(IndexType NewId = 0):
                BaseType(NewId),
                mpProperties(nullptr){}


            Element(IndexType NewId, const NodesArrayType& ThisNodes):
                BaseType(NewId, GeometryType::Pointer(new GeometryType(ThisNodes))),
                mpProperties(nullptr){}


            Element(IndexType NewId, GeometryType::Pointer pGeometry):
                BaseType(NewId, pGeometry),
                mpProperties(nullptr){}


            Element(IndexType NewId, GeometryType::Pointer pGeometry, Properties::Pointer pProperties):
                BaseType(NewId, pGeometry),
                mpProperties(pProperties){}


            Element(const Element& rOther):
                BaseType(rOther),
                mpProperties(rOther.mpProperties){}


            ~Element() override {}


            Element& operator = (const Element& rOther){
                BaseType::operator=(rOther);
                mpProperties = rOther.mpProperties;
                return *this;
            }


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


            virtual Pointer Clone(IndexType NewId, const NodesArrayType& ThisNodes)const{
                QUEST_TRY
                QUEST_WARNING("Element") << "Call base class element Clone " << std::endl;
                Element::Pointer p_new_elem = Quest::make_intrusive<Element>(NewId, GetGeometry().Create(ThisNodes), pGetProperties());
                p_new_elem->SetData(this->GetData());
                p_new_elem->Set(Flags(*this));
                return p_new_elem;
                QUEST_CATCH("")
            }


            virtual void EquationIdVector(
                EquationIdVectorType& rResult,
                const ProcessInfo& rCurrentProcessInfo
            ) const {
                if(rResult.size() != 0){
                    rResult.resize(0);
                }
            }


            virtual void GetDofList(
                DofsVectorType& rElementalDofList,
                const ProcessInfo& rCurrentProcessInfo
            ) const {
                if(rElementalDofList.size() != 0){
                    rElementalDofList.resize(0);
                }
            }
            

            virtual IntegrationMethod GetIntegrationMethod() const{
                return BaseType::pGetGeometry()->GetDefaultIntegrationMethod();
            }


            virtual void GetValuesVector(Vector& values, int Step = 0) const{
                if(values.size() != 0){
                    values.resize(0, false);
                }
            }


            virtual void GetFirstDerivativesVector(Vector& values, int Step = 0) const{
                if(values.size() != 0){
                    values.resize(0, false);
                }
            }


            virtual void GetSecondDerivativesVector(Vector& values, int Step = 0) const{
                if(values.size() != 0){
                    values.resize(0, false);
                }
            }


            virtual void Initialize(const ProcessInfo& rCurrentProcessInfo){}


            virtual void ResetConstitutiveLaw(){}


            virtual void InitializeSolutionStep(const ProcessInfo& rCurrentProcessInfo){}


            virtual void InitializeNonLinearIteration(const ProcessInfo& rCurrentProcessInfo){}


            virtual void FinalizeNonLinearIteration(const ProcessInfo& rCurrentProcessInfo){}


            virtual void FinaalizeSolutionStep(const ProcessInfo& rCurrentProcessInfo){}


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


            virtual void CalculateLeftHandSide(
                MatrixType& rLeftHandSideMatrix,
                const ProcessInfo& rCurrentProcessInfo
            ){
                if(rLeftHandSideMatrix.size1() != 0){
                    rLeftHandSideMatrix.resize(0, 0, false);
                }
            }


            virtual void CalculateRightHandSide(
                VectorType& rRightHandSideVector,
                const ProcessInfo& rCurrentProcessInfo
            ){
                if(rRightHandSideVector.size() != 0){
                    rRightHandSideVector.resize(0, false);
                }
            }


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
            


            virtual void CalculateFirstDerivativesLHS(
                MatrixType& rLeftHandSideMatrix,
                const ProcessInfo& rCurrentProcessInfo
            ){
                if(rLeftHandSideMatrix.size1() != 0){
                    rLeftHandSideMatrix.resize(0, 0, false);
                }
            }


            virtual void CalculateFirstDerivativesRHS(
                VectorType& rRightHandSideVector,
                const ProcessInfo& rCurrentProcessInfo
            ){
                if(rRightHandSideVector.size() != 0){
                    rRightHandSideVector.resize(0, false);
                }
            }


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


            virtual void CalculateSecondDerivativesLHS(
                MatrixType& rLeftHandSideMatrix,
                const ProcessInfo& rCurrentProcessInfo
            ){
                if(rLeftHandSideMatrix.size1() != 0){
                    rLeftHandSideMatrix.resize(0, 0, false);
                }
            }


            virtual void CalculateSecondDerivativesRHS(
                VectorType& rRightHandSideVector,
                const ProcessInfo& rCurrentProcessInfo
            ){
                if(rRightHandSideVector.size() != 0){
                    rRightHandSideVector.resize(0, false);
                }
            }


            virtual void CalculateMassMatrix(
                Matrix& rMassMatrix,
                const ProcessInfo& rCurrentProcessInfo
            ){
                if(rMassMatrix.size1() != 0){
                    rMassMatrix.resize(0, 0, false);
                }
            }


            virtual void CalculateDampingMatrix(
                Matrix& rDampingMatrix,
                const ProcessInfo& rCurrentProcessInfo
            ){
                if(rDampingMatrix.size1() != 0){
                    rDampingMatrix.resize(0, 0, false);
                }
            }


            virtual void CalculateLumpedMassVector(
                Vector& rLumpedMassVector,
                const ProcessInfo& rCurrentProcessInfo
            ) const {
                QUEST_TRY
                QUEST_ERROR << "Calling the CalculateLumpedMassVector method of the base element. The method must be implemented in the derived element.";
                QUEST_CATCH("")
            }


            virtual void AddExplicitContribution(const ProcessInfo& rCurrentProcessInfo){}


            virtual void AddExplicitContribution(
                const VectorType& rRHSVector,
                const Variable<VectorType>& rRHSVariable,
                const Variable<double>& rDestinationVariable,
                const ProcessInfo& rCurrentProcessInfo
            ){
                QUEST_ERROR << "Base element class is not able to assemble rRHS to the desired variable. destination variable is " << rDestinationVariable << std::endl;
            }


            virtual void AddExplicitContribution(
                const VectorType& rRHSVector,
                const Variable<VectorType>& rRHSVariable,
                const Variable<Array1d<double, 3>>& rDestinationVariable,
                const ProcessInfo& rCurrentProcessInfo
            ){
                QUEST_ERROR << "Base element class is not able to assemble rRHS to the desired variable. destination variable is " << rDestinationVariable << std::endl;
            }


            virtual void AddExplicitContribution(
                const MatrixType& rLHSMatrix,
                const Variable<MatrixType>& rLHSVariable,
                const Variable<Matrix>& rDestinationVariable,
                const ProcessInfo& rCurrentProcessInfo
            ){
                QUEST_ERROR << "Base element class is not able to assemble rLHS to the desired variable. destination variable is " << rDestinationVariable << std::endl;
            }


            virtual void Calculate(
                const Variable<double>& rVariable,
                double& rOutput,
                const ProcessInfo& rCurrentProcessInfo
            ){}
            


            virtual void Calculate(
                const Variable<Array1d<double, 3>>& rVariable,
                Array1d<double, 3>& rOutput,
                const ProcessInfo& rCurrentProcessInfo
            ){}


            virtual void Calculate(
                const Variable<Vector>& rVariable,
                Vector& rOutput,
                const ProcessInfo& rCurrentProcessInfo
            ){}


            virtual void Calculate(
                const Variable<Matrix>& rVariable,
                Matrix& rOutput,
                const ProcessInfo& rCurrentProcessInfo
            ){}


            virtual void CalculateOnIntegrationPoints(
                const Variable<bool>& rVariable,
                std::vector<bool>& rOutput,
                const ProcessInfo& rCurrentProcessInfo
            ){}
            


            virtual void CalculateOnIntegrationPoints(
                const Variable<int>& rVariable,
                std::vector<int>& rOutput,
                const ProcessInfo& rCurrentProcessInfo
            ){}
            


            virtual void CalculateOnIntegrationPoints(
                const Variable<double>& rVariable,
                std::vector<double>& rOutput,
                const ProcessInfo& rCurrentProcessInfo
            ){}
            


            virtual void CalculateOnIntegrationPoints(
                const Variable<Array1d<double, 3>>& rVariable,
                std::vector<Array1d<double, 3>>& rOutput,
                const ProcessInfo& rCurrentProcessInfo
            ){}


            virtual void CalculateOnIntegrationPoints(
                const Variable<Array1d<double, 4>>& rVariable,
                std::vector<Array1d<double, 4>>& rOutput,
                const ProcessInfo& rCurrentProcessInfo
            ){}


            virtual void CalculateOnIntegrationPoints(
                const Variable<Array1d<double, 6>>& rVariable,
                std::vector<Array1d<double, 6>>& rOutput,
                const ProcessInfo& rCurrentProcessInfo
            ){}


            virtual void CalculateOnIntegrationPoints(
                const Variable<Array1d<double, 9>>& rVariable,
                std::vector<Array1d<double, 9>>& rOutput,
                const ProcessInfo& rCurrentProcessInfo
            ){}


            virtual void CalculateOnIntegrationPoints(
                const Variable<Vector>& rVariable,
                std::vector<Vector>& rOutput,
                const ProcessInfo& rCurrentProcessInfo
            ){}


            virtual void CalculateOnIntegrationPoints(
                const Variable<Matrix>& rVariable,
                std::vector<Matrix>& rOutput,
                const ProcessInfo& rCurrentProcessInfo
            ){}


            virtual void GetValueOnIntegrationPoints(
                const Variable<ConstitutiveLaw::Pointer>& rVariable,
                std::vector<ConstitutiveLaw::Pointer>& rValues,
                const ProcessInfo& rCurrentProcessInfo
            ){}


            virtual void SetValuesOnIntegrationPoints(
                const Variable<bool>& rVariable,
                const std::vector<bool>& rValues,
                const ProcessInfo& rCurrentProcessInfo
            ){}
            


            virtual void SetValuesOnIntegrationPoints(
                const Variable<int>& rVariable,
                const std::vector<int>& rValues,
                const ProcessInfo& rCurrentProcessInfo
            ){}


            virtual void SetValuesOnIntegrationPoints(
                const Variable<double>& rVariable,
                const std::vector<double>& rValues,
                const ProcessInfo& rCurrentProcessInfo
            ){}


            virtual void SetValuesOnIntegrationPoints(
                const Variable<Array1d<double, 3>>& rVariable,
                const std::vector<Array1d<double, 3>>& rValues,
                const ProcessInfo& rCurrentProcessInfo
            ){}


            virtual void SetValuesOnIntegrationPoints(
                const Variable<Array1d<double, 4>>& rVariable,
                const std::vector<Array1d<double, 4>>& rValues,
                const ProcessInfo& rCurrentProcessInfo
            ){}


            virtual void SetValuesOnIntegrationPoints(
                const Variable<Array1d<double, 6>>& rVariable,
                const std::vector<Array1d<double, 6>>& rValues,
                const ProcessInfo& rCurrentProcessInfo
            ){}


            virtual void SetValuesOnIntegrationPoints(
                const Variable<Array1d<double, 9>>& rVariable,
                const std::vector<Array1d<double, 9>>& rValues,
                const ProcessInfo& rCurrentProcessInfo
            ){}


            virtual void SetValuesOnIntegrationPoints(
                const Variable<Vector>& rVariable,
                const std::vector<Vector>& rValues,
                const ProcessInfo& rCurrentProcessInfo
            ){}


            virtual void SetValuesOnIntegrationPoints(
                const Variable<Matrix>& rVariable,
                const std::vector<Matrix>& rValues,
                const ProcessInfo& rCurrentProcessInfo
            ){}


            virtual void GetValueOnIntegrationPoints(
                const Variable<ConstitutiveLaw::Pointer>& rVariable,
                const std::vector<ConstitutiveLaw::Pointer>& rValues,
                const ProcessInfo& rCurrentProcessInfo
            ){}


            virtual int Check(const ProcessInfo& rCurrentProcessInfo) const{
                QUEST_TRY

                QUEST_ERROR_IF(this->Id() < 1) << "Element found with Id " << this->Id() << std::endl;

                const double domain_size = BaseType::GetGeometry().DomainSize();
                QUEST_ERROR_IF(domain_size <= 0.0) << "Element " << this->Id() << " has non-positive size " << domain_size << std::endl;

                BaseType::GetGeometry().Check();

                return 0;

                QUEST_CATCH("")
            }


            virtual void MassMatrix(
                MatrixType& rMassMatrix,
                const ProcessInfo& rCurrentProcessInfo
            ){
                if(rMassMatrix.size1() != 0){
                    rMassMatrix.resize(0, 0, false);
                }
            }


            virtual void AddMassMatrix(
                MatrixType& rLeftHandSideMatrix,
                double coeff,
                const ProcessInfo& rCurrentProcessInfo
            ){}


            virtual void DampMatrix(
                MatrixType& rDampMatrix,
                const ProcessInfo& rCurrentProcessInfo
            ){
                if(rDampMatrix.size1() != 0){
                    rDampMatrix.resize(0, 0, false);
                }
            }
            


            virtual void AddInertiaForces(
                VectorType& rRightHandSideVector,
                double coeff,
                const ProcessInfo& rCurrentProcessInfo
            ){}
            


            virtual void CalculateLocalVelocityContribution(
                MatrixType& rDampingMatrix,
                VectorType& rRightHandSideVector,
                const ProcessInfo& rCurrentProcessInfo
            ){
                if(rDampingMatrix.size1() != 0){
                    rDampingMatrix.resize(0, 0, false);
                }
            }


            virtual void CalculateSensitivityMatrix(
                const Variable<double>& rDesignVariable,
                Matrix& rOutput,
                const ProcessInfo& rCurrentProcessInfo
            ){
                if(rOutput.size1() != 0){
                    rOutput.resize(0, 0, false);
                }
            }


            virtual void CalculateSensitivityMatrix(
                const Variable<Array1d<double, 3>>& rDesignVariable,
                Matrix& rOutput,
                const ProcessInfo& rCurrentProcessInfo
            ){
                if(rOutput.size1() != 0){
                    rOutput.resize(0, 0, false);
                }
            }
            

            PropertiesType::Pointer pGetProperties(){
                return mpProperties;
            }


            const PropertiesType::Pointer pGetProperties() const{
                return mpProperties;
            }


            PropertiesType& GetProperties(){
                QUEST_DEBUG_ERROR_IF(mpProperties == nullptr)
                    << "Tryining to get the properties of " << Info()
                    << ", which are uninitialized." << std::endl;
                return *mpProperties;
            }


            const PropertiesType& GetProperties() const{
                QUEST_DEBUG_ERROR_IF(mpProperties == nullptr)
                    << "Tryining to get the properties of " << Info()
                    << ", which are uninitialized." << std::endl;
                return *mpProperties;
            }


            void SetProperties(PropertiesType::Pointer pProperties){
                mpProperties = pProperties;
            }


            bool HasProperties() const{
                return mpProperties != nullptr;
            }


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