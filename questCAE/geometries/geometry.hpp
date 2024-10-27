/*------------------------------------------
所有几何类的基类，负责提供几何形状的公共接口和功能
用于有限元分析中对几何对象的描述
------------------------------------------*/

#ifndef QUEST_GEOMETRY_HPP
#define QUEST_GEOMETRY_HPP

// 系统头文件
#include <typeinfo>

// 项目头文件
#include "geometries/geometry_data.hpp"
#include "geometries/point.hpp"
#include "container/pointer_vector.hpp"
#include "container/data_value_container.hpp"
#include "utilities/math_utils.hpp"
#include "IO/logger.hpp"
#include "integration/integration_info.hpp"

namespace Quest{

    template<typename TPointType>
    class Geometry{
        public:
            using Geometry = Geometry<TPointType>;

            QUEST_CLASS_POINTER_DEFINITION(Geometry);

            enum class QualityCriteria{
                INRADIUS_TO_CIRCUMRADIUS,
                AREA_TO_LENGTH,
                SHORTEST_ALTITUDE_TO_LENGTH,
                INRADIUS_TO_LONGEST_EDGE,
                SHORTEST_TO_LONGEST_EDGE,
                REGULARITY,
                VOLUME_TO_SURFACE_AREA,
                VOLUME_TO_EDGE_LENGTH,
                VOLUME_TO_AVERAGE_EDGE_LENGTH,
                VOLUME_TO_RMS_EDGE_LENGTH,
                MIN_DIHEDRAL_ANGLE,
                MAX_DIHEDRAL_ANGLE,
                MIN_SOLID_ANGLE
            };


            enum class LumpingMethods{
                ROW_SUM,
                DIAGONAL_SCALING,
                QUADRATURE_ON_NODES
            };

            using PointsArrayType = PointerVector<TPointType>;
            using IntegrationMethod = GeometryData::IntegrationMethod;
            using GeometriesArrayType = PointerVector<GeometryType>;
            using PointType = TPointType;
            using IndexType = std::size_t;
            using SizeType = std::size_t;
            using CoordinaatesArrayType = typename PointType::CoordinatesArrayType;
            using IntegrationPointType = IntegrationPoint<3>;
            using IntegrationPointsArrayType = std::vector<IntegrationPointType>;
            using IntegrationPointsContainerType = std::array<IntegrationPointsArrayType, static_cast<int>(GeometryData::IntegrationMethod::NumberOfIntegrationMethods)>;
            using ShapeFunctionsValuesContainerType = std::array<Matrix, static_cast<int>(GeometryData::IntegrationMethod::NumberOfIntegrationMethods)>;
            using ShapeFunctionsLocalGradientsContainerType = GeometryData::ShapeFunctionsLocalGradientsContainerType;
            using JacobiansType = DenseMatrix<Matrix>;
            using ShapeGunctionsGradientsType = GeometryData::ShapeFunctionsGradientsType;
            using ShapeFunctionsSecondDerivativesType = GeometryData::ShapeFunctionsSecondDerivativesType;
            using ShapeFunctionsThirdDerivativesType = GeometryData::ShapeFunctionsThirdDerivativesType;
            using NormalType = DenseVector<double>;
            using PointPointerType = typename PointType::Pointer;
            using ConstPointPointerType = const PointPointerType;
            using PointReferenceType = TPointType&;
            using ConstPointReferenceType = const TPointType&;
            using PointPointerContainerType = std::vector<PointPointerType>;
            using iterator = typename PointsArrayType::iterator;
            using const_iterator = typename PointsArrayType::const_iterator;
            using ptr_iterator = typename PointsArrayType::ptr_iterator;
            using ptr_const_iterator = typename PointsArrayType::ptr_const_iterator;
            using difference_type = typename PointsArrayType::difference_type;

            static constexpr IndexType BACKGROUND_GEOMETRY_INDEX = std::numeric_limits<IndexType>::max();

        public:
            Geometry():
                mId(GenerateSelfAssignedId()),
                mpGeometryData(&GeometryDataInstance())
            {}


            Geometry(IndexType GeometryId):mpGeometryData(&GeometryDataInstance()){
                SetId(GeometryId);
            }


            Geometry(const std::string& GeimetryName):
                mId(GenerateId(GeimetryName)),
                mpGeometryData(&GeometryDataInstance())
            {}


            Geometry(
                const PointsArrayType& ThisPoints,
                const GeometryData* pThisGeometryData = &GeometryDataInstance()
            ):mId(GenerateSelfAssignedId()),
            mpGeometryData(pThisGeometryData),
            mPoints(ThisPoints)
            {}


            Geometry(
                IndexType GeometryId,
                const PointsArrayType& ThisPoints,
                const GeometryData* pThisGeometryData = &GeometryDataInstance()
            ):mpGeometryData(pThisGeometryData),
            mPoints(ThisPoints){
                SetId(GeometryId);
            }


            Geometry(
                const std::string& GeimetryName,
                const PointsArrayType& ThisPoints,
                const GeometryData* pThisGeometryData = &GeometryDataInstance()
            ):mId(GenerateId(GeimetryName)),
            mpGeometryData(pThisGeometryData),
            mPoints(ThisPoints)
            {}
            


            Geometry(const Geometry& rOther):
                mId(rOther.mId),
                mpGeometryData(rOther.mpGeometryData),
                mPoints(rOther.mPoints),
                mData(rOther.mData)
            {}


            template<typename TOtherPointType>
            Geometry(const Geometry<TOtherPointType>& rOther):
                mId(rOther.mId),
                mpGeometryData(rOther.mpGeometryData),
                mPoints(rOther.mPoints)
            {
                mPoints = new PointsArrayType(rOther.begin(), rOther.end());
            }


            virtual ~Geometry(){}


            virtual GeometryData::QuestGeometryFamily GetGeometryFamily() const{
                return GeometryData::QuestGeometryFamily::Quest_generic_family;
            }


            virtual GeometryData::QuestGeomtryType GetGeometryType() const{
                return GeometryData::QuestGeomtryType::Quest_generic_type;
            }


            Gemetry& operator = (const Geometry& rOther){
                mpGeometryData = rOther.mpGeometryData;
                mPoints = rOther.mPoints;
                mData = rOther.mData;

                return *this;
            }


            template<typename TOtherPointType>
            Geometry& operator = (const Geometry<TOtherPointType>& rOther){
                this->clear();

                for(typename Geometry<TOtherPointType>::ptr_const_iterator it = rOther.ptr_begin(); it!= rOther.ptr_end(); ++it){
                    push_back(typename PointType::Point(new PointType(*it)));
                }

                mpGeometryData = rOther.mpGeometryData;

                return *this;
            }


            operator PointsArrayType&(){
                return mPoints;
            }


            TPointType& operator[](const SizeType& i){
                return mPoints[i];
            }


            const TPointType& operator[](const SizeType& i) const{
                return mPoints[i];
            }


            PointPointerType& operator()(const SizeType& i){
                return mPoints(i);
            }


            ConstPointPointerType operator()(const SizeType& i) const{
                return mPoints(i);
            }


            iterator begin(){
                return iterator(mPoints.begin());
            }


            const_iterator begin() const{
                return const_iterator(mPoints.begin());
            }


            iterator end(){
                return iterator(mPoints.end());
            }


            const_iterator end() const{
                return const_iterator(mPoints.end());
            }


            ptr_iterator ptr_begin(){
                return mPoints.ptr_begin();
            }


            ptr_const_iterator ptr_begin() const{
                return mPoints.ptr_begin();
            }


            ptr_iterator ptr_end(){
                return mPoints.ptr_end();
            }


            ptr_const_iterator ptr_end() const{
                return mPoints.ptr_end();
            }


            PointReferenceType front(){
                assert(!empty());
                return mPoints.front();
            }


            ConstPointReferenceType front() const{
                assert(!empty());
                return mPoints.front();
            }


            PointReferenceType back(){
                assert(!empty());
                return mPoints.back();
            }


            ConstPointReferenceType back() const{
                assert(!empty());
                return mPoints.back();
            }


            SizeType size() const{
                return mPoints.size();
            }


            SizeType PointsNumber() const{
                return this->size();
            }


            virtual SizeType PointsNumberInDirection(IndexType LocalDirectionIndex) const{
                QUEST_ERROR << "Trying to access PointsNumberInDirection from geometry base class." << std::endl;
            }


            SizeType max_size() const{
                return mPoints.max_size();
            }


            void swap(Geometry& rOther){
                mPoints.swap(rOther.mPoints);
            }


            void push_back(PointPointerType x){
                mPoints.push_back(x);
            }


            void clear(){
                mPoints.clear();
            }


            void reserve(int dim){
                mPoints.reserve(dim);
            }


            int capacity() const{
                return mPoints.capacity();
            }


            PointPointerContainerType& GetContainer(){
                return mPoints.GetContainer();
            }


            const PointPointerContainerType& GetContainer() const{
                return mPoints.GetContainer();
            }


            DataValueContainer& GetData(){
                return mData;
            }


            const DataValueContainer& GetData() const{
                return mData;
            }


            void SetData(const DataValueContainer& rData){
                mData = rData;
            }


            template<typename TDataType>
            bool Has(const Variable<TDataType>& rThisVariable) const{
                return mData.Has(rThisVariable);
            }


            template<typename TVariableType>
            void SetValue(const TVariableType& rThisVariable, typename const TVariableType::Type& rValue){
                mData.SetValue(rThisVariable, rValue);
            }


            template<typename TVariableType>
            typename TVariableType::Type& GetValue(const TVariableType& rThisVariable){
                return mData.GetValue(rThisVariable);
            }


            template<typename TVariableType>
            const typename TVariableType::Type& GetValue(const TVariableType& rThisVariable) const{
                return mData.GetValue(rThisVariable);
            }


            virtual void Assign(const Variable<bool>& rVariable, const bool Input){}


            virtual void Assign(const Variable<int>& rVariable, const int Input){}


            virtual void Assign(const Variable<double>& rVariable, const double Input){}


            virtual void Assign(const Variable<Array1d<double,2>>& rVariable, const Array1d<double,2>& Input){}


            virtual void Assign(const Variable<Array1d<double,3>>& rVariable, const Array1d<double,3>& Input){}


            virtual void Assign(const Variable<Array1d<double,6>>& rVariable, const Array1d<double,6>& Input){}


            virtual void Assign(cosnt Variable<Vector>& rVariable, const Vector& Input){}


            virtual void Assign(const Variable<Matrix>& rVariable, const Matrix& Input){}


            virtual void Calculate(const Variable<bool>& rVariable, bool& rOutput) const{}


            virtual void Calculate(const Variable<int>& rVariable, int& rOutput) const{}


            virtual void Calculate(const Variable<double>& rVariable, double& rOutput) const{}


            virtual void Calculate(const Variable<Array1d<double,2>>& rVariable, Array1d<double,2>& rOutput) const{}


            virtual void Calculate(const Variable<Array1d<double,3>>& rVariable, Array1d<double,3>& rOutput) const{}


            virtual void Calculate(const Variable<Array1d<double,6>>& rVariable, Array1d<double,6>& rOutput) const{}


            virtual void Calculate(const Variable<Vector>& rVariable, Vector& rOutput) const{}


            virtual void Calculate(const Variable<Matrix>& rVariable, Matrix& rOutput) const{}


            inline static bool HasSameType(cosnt GeometryType& rLHS, cosnt GeometryType& rRHS){
                reutrn typeid(rLHS) == typeid(rRHS);
            }


            inline static bol HasSameType(const GeometryType* rLHS, const GeometryType* rRHS){
                return GeometryType::HasSameType(*rLHS, *rRHS);
            }


            inline static bool HasSameGeometryType(cosnt GeometryType& rLHS, cosnt GeometryType& rRHS){
                return rLHS.GetGeometryType() == rRHS.GetGeometryType();
            }


            inline static bool HasSameGeometryType(const GeometryType* rLHS, const GeometryType* rRHS){
                return GeometryType::HasSameGeometryType(*rLHS, *rRHS);
            }


            inline static bool IsSame(cosnt GeometryType& rLHS, cosnt GeometryType& rRHS){
                return GeometryType::HasSameType(rLHS, rRHS) && GeometryType::HasSameGeometryType(rLHS, rRHS);
            }


            inline static bool IsSame(const GeometryType* rLHS, const GeometryType* rRHS){
                return GeometryType::HasSameType(*rLHS, *rRHS) && GeometryType::HasSameGeometryType(*rLHS, *rRHS);
            }


            bool empty() const{
                return mPoints.empty();
            }


            virtual Pointer Create(const PointsArrayType& rThisPoints)const {
                auto p_geom = this->Create(0,rThisPoints);

                IndexType id = reinterpret_cast<IndexType>(p_geom.get());

                p_geom->SetIdSelfAssigned(id);

                p_geom->SetIdNotGeneratedFromString(id);

                p_geom->SetIdWithoutCheck(id);

                return p_geom;
            }


            virtual Pointer Create(const IndexType NewGeometruId, const PointsArrayType& rThisPoints)const{
                return Pointer( new Geometry(NewGeometruId, rThisPoints, mpGeometryData) );
            }


            Pointer Create(const std::string& rNewGeometryName, const PointsArrayType& rThisPoints)const{
                auto p_geom = this->Create(0,rThisPoints);
                p_geom->SetId(rNewGeometryName);

                return p_geom;
            }


            virtual Pointer Create(const GeometryType& rGeometry) const{
                auto p_geom = this->Create(0,rGeometry);

                IndexType id = reinterpret_cast<IndexType>(p_geom.get());

                p_geom->SetIdSelfAssigned(id);

                p_geom->SetIdNotGeneratedFromString(id);

                p_geom->SetIdWithoutCheck(id);

                return p_geom;
            }


            virtual Pointer Create(cosnt IndexType NewGeometryId, const GeometryType& rGeometry) const{
                auto p_geometry = Pointer(new Geometry(NewGeometryId, rGeometry.Points(), mpGeometryData));
                p_geometry->SetData(rGeometry.GetData());
                return p_geometry;
            }


            Pointer Create(const std::string& rNewGeometryName, const GeometryType& rGeometry) const{
                auto p_geom = this->Create(0,rGeometry);
                p_geom->SetId(rNewGeometryName);

                return p_geom;
            }


            void ClonePoints(){
                for(ptr_iterator i = this->ptr_begin(); i!= this->ptr_end(); ++i){
                    *i = typename PointType::Pointer(new PointType(**i));
                }
            }


            const GeometryData& GetGeometryData() const{
                return *mpGeometryData;
            }


            virtual void SetGeometryShapeFunctionContainer(
                const GeometryShapeFunctionContainer<GeometryData::IntegrationMethod>& rGeometryShapeFunctionContainer
            ){
                QUEST_ERROR << "Calling SetGeometryShapeFunctionContainer from base geometry class." << std::endl;
            }


            const IndexType& Id() const{
                return mId;
            }


            bool IsIdGeneratedFromString(){
                return IsIdGeneratedFromString(mId);
            }


            bool IsIdSelfAssigned(){
                return IsIdSelfAssigned(mId);
            }


            void SetId(const IndexType Id){
                QUEST_ERROR_IF(IsIdGeneratedFromString(Id) || IsIdSelfAssigned(Id))
                    << "Id: " << Id << " out of range. The Id must be loer than 2^62 = 4.61e+18. "
                    << "Geometry being recognized as generated form stding: " << IsIdGeneratedFromString(Id)
                    << ", self assigned: " << IsIdSelfAssigned(Id) << "." << std::endl;

                mId = Id;
            }


            void SetId(const std::string& rName){
                mId = GenerateId(rName);
            }


            static inline IndexType GenerateId(const std::string& rName){
                std::hash<std::string> string_hash_generator;
                auto id = string_hash_generator(rName);

                SetIdGeneratedFromString(id);

                SetIdNotSelfAssigned(id);

                return id;
            }


            virtual GeometryType& GetGeometryParent(IndexType Index) const{
                QUEST_ERROR << "Calling GetGeometryParent from base geometry class." << std::endl;
            }


            virtual void SetGeometryParent(GeometryType* pGeometryParent){
                QUEST_ERROR << "Calling SetGeometryParent from base geometry class." << std::endl;
            }


            virtual GeometryType& GetGeometryPart(const IndexType Index){
                return *pGeometryPart(Index);
            }


            virtual const GeometryType& GetGeometryPart(const IndexType Index) const{
                return *pGeometryPart(Index);
            }


            virtual typename GeometryType::Pointer pGetGeometryPart(const IndexType Index){
                QUEST_ERROR << "Calling base class 'pGetGeometryPart' method instead of derived function."
                    << " Please check the definition in the derived class." << *this << std::endl;
            }


            virtual const typename GeometryType::Pointer pGetGeometryPart(const IndexType Index) const{
                QUEST_ERROR << "Calling base class 'pGetGeometryPart' method instead of derived function."
                    << " Please check the definition in the derived class." << *this << std::endl;
            }


            virtual void SetGeometryPart(const IndexType Index, GeometryType::Pointer pGeometry){
                QUEST_ERROR << "Calling base class 'SetGeometryPart' method instead of derived function."
                    << " Please check the definition in the derived class." << *this << std::endl;
            }


            virtual IndexType AddGeometryPart(GeometryType::Pointer pGeometry){
                QUEST_ERROR << "Calling base class 'AddGeometryPart' method instead of derived function."
                    << " Please check the definition in the derived class." << *this << std::endl;
            }


            virtual void RemoveGeometryPart(GeometryType::Pointer pGeometry){
                QUEST_ERROR << "Calling base class 'RemoveGeometryPart' method instead of derived function."
                    << " Please check the definition in the derived class." << *this << std::endl;
            }


            virtual void RemoveGeometryPart(const IndexType Index){
                QUEST_ERROR << "Calling base class 'RemoveGeometryPart' method instead of derived function."
                    << " Please check the definition in the derived class." << *this << std::endl;
            }


            virtual bool HasGeometryPart(const IndexType Index) const{
                QUEST_ERROR << "Calling base class 'HasGeometryPart' method instead of derived function."
                    << " Please check the definition in the derived class." << *this << std::endl;
            }


            virtual SizeType NumberOfGeometryParts() const{
                return 0;
            }


            virtual Vector& LumpingFactors(Vector& rResult, const LumpingMethods LumpingMethod = LumpingMethods::ROW_SUM) const{
                const SizeType number_of_nodes = this->size();
                const SizeType local_space_dimension = this->LocalSpaceDimension();

                if(rResult.size() != number_of_nodes){
                    rResult.resize(number_of_nodes, false);
                }
                noalias(rResult) = ZeroVector(number_of_nodes);

                if(LumpingMethod == LumpingMethods::ROW_SUM){
                    const IntegrationMethod integration_method = GetDefaultIntegrationMethod();
                    const GeometryType::IntegrationPointsArrayType& r_integrations_points = this->IntegrationPoints(integration_method);
                    const Matrix& r_Ncontainer = this->ShapeFunctionsValues(integration_method);

                    Vector detJ_vector(r_integrations_points.size());
                    DeterminantOfJacobian(detJ_vector, integration_method);

                    double domain_size = 0.0;
                    for(IndexType point_number = 0; point_number < r_integrations_points.size(); ++point_number){
                        const double integration_weight = r_integrations_points[point_number].Weight() * detJ_vector[point_number];
                        const Vector& rN = row(r_Ncontainer, point_number);

                        domain_size += integration_weight;

                        for(IndexType i = 0; i < number_of_nodes; ++i){
                            rResult[i] += integration_weight * rN[i];
                        }
                    }

                    for(IndexType i = 0; i < number_of_nodes; ++i){
                        rResult[i] /= domain_size;
                    }
                } else if(LumpingMethod == LumpingMethods::DIAGONAL_SCALING){
                    IntegrationMethod integration_method = GetDefaultIntegrationMethod();
                    int j = std::min(static_cast<int>(integration_method)+1,4);
                    integration_method = static_cast<IntegrationMethod>(j);
                    const GeometryType::IntegrationPointsArrayType& r_integrations_points = this->IntegrationPoints(integration_method);
                    const Matrix& r_Ncontainer = this->ShapeFunctionsValues(integration_method);

                    Vector detJ_vector(r_integrations_points.size());
                    DeterminantOfJacobian(detJ_vector, integration_method);

                    for(IndexType point_number = 0; point_number < r_integrations_points.size(); ++point_number){
                        const double detJ = detJ_vector[point_number];
                        const double integration_weight = r_integrations_points[point_number].Weight() * detJ;
                        const Vector& rN = row(r_Ncontainer, point_number);

                        for(IndexType i = 0; i < number_of_nodes; ++i){
                            rResult[i] += std::pow(rN[i],2)*integration_weight;
                        }
                    }

                    double total_value = 0.0;
                    for(IndexType i = 0; i < number_of_nodes; ++i){
                        total_value += rResult[i];
                    }
                    for(IndexType i = 0; i < number_of_nodes; ++i){
                        rResult[i] /= total_value;
                    }
                } else if(LumpingMethod == LumpingMethods::QUADRATURE_ON_NODES){
                    const double domain_size = DomainSize();

                    Matrix local_coordinates(number_of_nodes, local_space_dimension);
                    PointsLocalCoordinates(local_coordinates);
                    Point local_point(ZeroVector(3));
                    Array1d<double,3>& r_local_coordinates = local_point.Coordinates();

                    const GeometryType::IntegrationPointsArrayType& r_integrations_points = this->IntegrationPoints(GeometryData::IntegrationMethod::GI_GAUSS_1);
                    const double weight = r_integrations_points[0].Weight()/static_cast<double>(number_of_nodes);
                    for(IndexType point_number = 0; point_number < number_of_nodes; ++point_number){
                        for(IndexType dim = 0; dim < local_space_dimension; ++dim){
                            r_local_coordinates[dim] = local_coordinates(point_number,dim);
                        }
                        const double detJ = DeterminantOfJacobian(local_point);
                        rResult[point_number] = weight*detJ/domain_size;
                    }
                }

                return rResult;
            }


            inline SizeType WorkingSpaceDimension() const{
                return mpGeometryData->WorkingSpaceDimension();
            }


            inline SizeType LocalSpaceDimension() const{
                return mpGeometryData->LocalSpaceDimension();
            }


            virtual SizeType PolynomialDegree(IndexType LocalDirectionIndex) const{
                QUEST_ERROR << "Trying to access PolynomialDegree from geometry base class." << std::endl;
            }


            virtual double Length() const {
                QUEST_ERROR << "Calling base class 'Length' method instead of derived class one. Please check the definition of derived class." << *this << std::endl;
                return 0.0;
            }


            virtual double Area() const{
                QUEST_ERROR << "Calling base class 'Area' method instead of derived class one. Please check the definition of derived class." << *this << std::endl;
                return 0.0;
            }


            virtual double Volume() const{
                QUEST_ERROR << "Calling base class 'Volume' method instead of derived class one. Please check the definition of derived class." << *this << std::endl;
                return 0.0;
            }


            virtual double DomainSize() const{
                const SizeType local_dimension = this->LocalSpaceDimension();
                if(local_dimension == 1){
                    return this->Length();
                } else if(local_dimension == 2){
                    return this->Area();
                } else if(local_dimension == 3){
                    return this->Volume();
                }
                return 0.0;
            }


            virtual double MinEdgeLength() const{
                QUEST_ERROR << "Calling base class 'MinEdgeLength' method instead of derived class one. Please check the definition of derived class." << *this << std::endl;
                return 0.0;
            }


            virtual double MaxEdgeLength() const{
                QUEST_ERROR << "Calling base class 'MaxEdgeLength' method instead of derived class one. Please check the definition of derived class." << *this << std::endl;
                return 0.0;
            }


            virtual double AverageEdgeLength() const{
                QUEST_ERROR << "Calling base class 'AverageEdgeLength' method instead of derived class one. Please check the definition of derived class." << *this << std::endl;
                return 0.0;
            }


            virtual double Circumradius() const{
                QUEST_ERROR << "Calling base class 'Circumradius' method instead of derived class one. Please check the definition of derived class." << *this << std::endl;
                return 0.0;
            }


            virtual double Inradius() const{
                QUEST_ERROR << "Calling base class 'Inradius' method instead of derived class one. Please check the definition of derived class." << *this << std::endl;
                return 0.0;
            }


            virtual bool HasIntersection(const GeometryType& ThisGeometry) const{
                QUEST_ERROR << "Calling base class 'HasIntersection' method instead of derived class one. Please check the definition of derived class." << *this << std::endl;
                return false;
            }


            virtual bool HasIntersection(const Point& rLowPoint, const Point& rHighPoint) const{
                QUEST_ERROR << "Calling base class 'HasIntersection' method instead of derived class one. Please check the definition of derived class." << *this << std::endl;
                return false;
            }


            virtual void BoundingBox(
                TPointType& rLowPoint,
                TPointType& rHighPoint
            )const{
                rHighPoint = this->GetPoint(0);
                rLowPoint = rHighPoint;
                const SizeType dim = WorkingSpaceDimension();

                for(IndexType point = 1;point < PointsNumber();point++){
                    const auto& r_point = this->GetPoint(point);
                    for(IndexType i = 0; i < dim; i++){
                        rHighPoint[i] = (r_point[i] > rHighPoint[i])? r_point[i] : rHighPoint[i];
                        rLowPoint[i] = (r_point[i] < rLowPoint[i])? r_point[i] : rLowPoint[i];
                    }
                }
            }


            virtual Point Center() const{
                const SizeType points_number = this->size();

                if(points_number == 0){
                    QUEST_ERROR << "can not compute center of a geometry of zero points" << std::endl;
                }

                Point result = (*this)[0];

                for(IndexType i = 1; i < points_number; ++i){
                    result.Coordinates() += (*this)[i];
                }

                const double temp = 1.0/double(points_number);
                result.Coordinates() *= temp;

                return result;
            }


            vitual Array1d<double,3> Normal(const CoordinatesArrayType& rPointLocalCoordinates) const{
                const SizeType local_space_dimension = this->LocalSpaceDimension();
                const SizeType dimension = this->WorkingSpaceDimension();

                QUEST_ERROR_IF(dimension == local_space_dimension) << "Remember the normal can be computed just in geometries with a local dimension: " << this->LocalSpaceDimension() << "samller than the spatial dimension: " << this->WorkingSpaceDimension() << std::endl;

                Array1d<double,3> tangent_xi = ZeroVector(3);
                Array1d<double,3> tangent_eta = ZeroVector(3);

                Matrix_j_node = ZeroMatrix(dimension,local_space_dimension);
                this->Jacobian(j_node,rPointLocalCoordinates);

                if(dimension == 2){
                    tangent_eta[2] = 1.0;
                    for(unsigned int i_dim = 0; i_dim < dimension; ++i_dim){
                        tangent_xi[i_dim] = j_node(i_dim,0);
                    }
                } else {
                    for(unsigned int i_dim = 0; i_dim < dimension; ++i_dim){
                        tengent_xi[i_dim] = j_node(i_dim,0);
                        tangent_eta[i_dim] = j_node(i_dim,1);
                    }
                }

                Array1d<double,3> normal;
                MathUtils<double>::CrossProduct(normal,tangent_xi,tangent_eta);
                return normal;
            }


            virtual Array1d<double,3> Normal(IndexType IntegrationPointIndex) const{
                return Normal(IntegrationPointIndex, mpGeometryData->DefaultIntegrationMethod());
            }


            virtual Array1d<double,3> Normal(
                IndexType IntegrationPointIndex,
                IntegrationMethod ThisMethod
            ) const {
                const SizeType local_space_dimension = this->LocalSpaceDimension();
                const SizeType dimension = this->WorkingSpaceDimension();

                QUEST_DEBUG_ERROR_IF(dimension == local_space_dimension)
                    << "Remember the normal can be computed just in geometries with a local dimension: "
                    << this->LocalSpaceDimension() << "samller than the spatial dimension: "
                    << this->WorkingSpaceDimension() << std::endl;

                Array1d<double,3> tangent_xi = ZeroVector(3);
                Array1d<double,3> tangent_eta = ZeroVector(3);

                Matrix_j_node = ZeroMatrix(dimension,local_space_dimension);
                this->Jacobian(j_node,IntegrationPointIndex,ThisMethod);

                if(dimension == 2){
                    tangent_eta[2] = 1.0;
                    for(IndexType i_dim = 0; i_dim < dimension; ++i_dim){
                        tangent_xi[i_dim] = j_node(i_dim,0);
                    }
                } else {
                    for(IndexType i_dim = 0; i_dim < dimension; ++i_dim){
                        tangent_xi[i_dim] = j_node(i_dim,0);
                        tangent_eta[i_dim] = j_node(i_dim,1);
                    }
                }

                Array1d<double,3> normal;
                MathUtils<double>::CrossProduct(normal,tangent_xi,tangent_eta);
                return normal;
            }


            virtual Array1d<double,3> UnitNormal(const CoordinatesArrayType& rPointLocalCoordinates) const{
                Array1d<double,3> normal = Normal(rPointLocalCoordinates);
                const double norm_normal = norm_2(normal);
                if(norm_normal > std::numeric_limits<double>::epsilon()){
                    normal /= norm_normal;
                } else {
                    QUEST_ERROR << "ERROR: The normal norm is zero or almost zero. Norm. normal: " << norm_normal << std::endl;
                }

                return normal;
            }


            virtual Array1d<double,3> UnitNormal(IndexType IntegrationPointIndex) const{
                return UnitNormal(IntegrationPointIndex, mpGeometryData->DefaultIntegrationMethod());
            }


            virtual Array1d<double,3> UnitNormal(
                IndexType IntegrationPointIndex,
                IntegrationMethod ThisMethod
            ) const {
                Array1d<double,3> normal_vector = Normal(IntegrationPointIndex, ThisMethod);
                const double norm_normal = norm_2(normal_vector);
                if(norm_normal > std::numeric_limits<double>::epsilon()){
                    normal_vector /= norm_normal;
                } else {
                    QUEST_ERROR << "ERROR: The normal norm is zero or almost zero: " << norm_normal << std::endl;
                }

                return normal_vector;
            }


            double Quality(const QualityCriteria qualityCriteria) const{
                double quality = 0.0f;

                if(QualityCriteria == QualityCriteria::INRADIUS_TO_CIRCUMRADIUS){
                    quality = InradiusToCircumradiusQuality();
                } else if(QualityCriteria == QualityCriteria::AREA_TO_LENGTH){
                    quality = AreaToEdgeLengthRatio();
                } else if(qualityCriteria == QualityCriteria::SHORTEST_ALTITUDE_TO_LENGTH) {
                    quality = ShortestAltitudeToEdgeLengthRatio();
                } else if(qualityCriteria == QualityCriteria::INRADIUS_TO_LONGEST_EDGE) {
                    quality = InradiusToLongestEdgeQuality();
                } else if(qualityCriteria == QualityCriteria::SHORTEST_TO_LONGEST_EDGE) {
                    quality = ShortestToLongestEdgeQuality();
                } else if(qualityCriteria == QualityCriteria::REGULARITY) {
                    quality = RegularityQuality();
                } else if(qualityCriteria == QualityCriteria::VOLUME_TO_SURFACE_AREA) {
                    quality = VolumeToSurfaceAreaQuality();
                } else if(qualityCriteria == QualityCriteria::VOLUME_TO_EDGE_LENGTH) {
                    quality = VolumeToEdgeLengthQuality();
                } else if(qualityCriteria == QualityCriteria::VOLUME_TO_AVERAGE_EDGE_LENGTH) {
                    quality = VolumeToAverageEdgeLength();
                } else if(qualityCriteria == QualityCriteria::VOLUME_TO_RMS_EDGE_LENGTH) {
                    quality = VolumeToRMSEdgeLength();
                } else if(qualityCriteria == QualityCriteria::MIN_DIHEDRAL_ANGLE) {
                    quality = MinDihedralAngle();
                } else if (qualityCriteria == QualityCriteria::MAX_DIHEDRAL_ANGLE) {
                    quality = MaxDihedralAngle();
                } else if(qualityCriteria == QualityCriteria::MIN_SOLID_ANGLE) {
                    quality = MinSolidAngle();
                }

                return quality;
            }


            virtual inline void ComputeDihedralAngles(Vector& rDihedralAngles) const{
                QUEST_ERROR << "Called the virtual function for ComputeDihedralAngles " << *this << std::endl;
            }


            virtual inline void ComputeSolidAngles(Vector& rSolidAngles) const{
                QUEST_ERROR << "Called the virtual function for ComputeSolidAngles " << *this << std::endl;
            }


            const PointsArrayType& Points() const{
                return mPoints;
            }


            PointsArrayType& Points(){
                return mPoints;
            }


            const typename TPointType::Pointer pGetPoint(const int Index) const{
                QUEST_TRY
                return mPoints(Index);
                QUEST_CATCH(mPoints)
            }


            typename TPointType::Pointer pGetPoint(const int Index){
                QUEST_TRY
                return mPoints(Index);
                QUEST_CATCH(mPoints)
            }


            const TPointType& GetPoint(const int Index) const{
                QUEST_TRY
                return mPoints[Index];
                QUEST_CATCH(mPoints)
            }


            TPointType& GetPoint(const int Index){
                QUEST_TRY
                return mPoints[Index];
                QUEST_CATCH(mPoints)
            }


            virtual Matrix& PointsLocalCoordinates(Matrix& rResult) const{
                QUEST_ERROR << "Calling base class 'PointsLocalCoordinates' method instead of derived class one. Please check the definition of derived class." << *this << std::endl;
                return rResult;
            }


            virtual CoordinatesArrayType& PointLocalCoordinates(
                CoordinatesArrayType& rResult,
                const CoordinatesArrayType& rPoint
            ) const {
                QUEST_ERROR_IF(WorkingSpaceDimension() != LocalSpaceDimension())
                    << "ERROR: Attention, the Point Local Coordinates must be specialized for the current geometry" << std::endl;

                Matrix J = ZeroMatrix(WorkingSpaceDimension(), LocalSpaceDimension());

                rResult.clear();

                Vector DeltaXi = ZeroVector(LocalSpaceDimension());

                CoordinatesArrayType CurrentGlobalCoords(ZeroMatrix(3));

                static constexpr double MaxNormPointLocalCoordinates = 30.0;
                static constexpr std::size_t MaxIterationNumberPointLocalCoordinates = 1000;
                static constexpr double MaxTolerancePointLocalCoordinates = 1.0e-8;

                for(std::size_t k = 0; k < MaxIterationNumberPointLocalCoordinates; ++k){
                    CurrentGlobalCoords.clear();
                    DeltaXi.clear();

                    GlobalCoordinates(CurrentGlobalCoords, rResult);
                    noalias(CurrentGlobalCoords) = rPoint - CurrentGlobalCoords;
                    InverseOfJacobian(J, rResult);
                    for(unsigned int i = 0; i < WorkingSpaceDimension(); ++i){
                        for(unsigned int j = 0; j < LocalSpaceDimension(); ++j){
                            DeltaXi[i] += J(i,j)*CurrentGlobalCoords[j];
                        }
                        rResult[i] += DeltaXi[i];
                    }

                    const double norm2DXi = norm_2(DeltaXi);

                    if(norma2DXi > MaxNormPointLocalCoordinates){
                        QUEST_WARNING("Geometry") << "Computation of local coordinates failed at iterator " << k << std::endl;
                        break;
                    }

                    if(norm2DXi < MaxTolerancePointLocalCoordinates){
                        break;
                    }
                }

                return rResult;
            }


            virtual bool IsInside(
                const CoordinatesArrayType& rPointGlobalCoordinates,
                CoordiantesArrayType& rResult,
                const double Tolerance = std::numeric_limits<double>::epsilon()
            ) const {
                PointLocalCoordinates(rResult, rPointGlobalCoordinates);

                if(IsInsideLocalSpace(rResult, Tolerance) == 0){
                    return false;
                }

                return true;
            }


            virtual int IsInsideLocalSpace(
                const CoordinatesArrayType& rPointLocalCoordinates,
                const double Tolerance = std::numeric_limits<double>::epsilon()
            ) const {
                QUEST_ERROR << "Calling IsInsideLocalSpace from base class. Please check the definition of derived class." << *this << std::endl;
                return 0;
            }


            virtual void SpansLocalSpace(
                std::vector<double>& rSpans,
                IndexType LocalDirectionIndex = 0
            ) const {
                QUEST_ERROR << "Calling SpansLocalSpace from base class. Please check the definition of derived class." << *this << std::endl;
            }


            bool HasIntegrationMethod(IntegrationMethod ThisMethod) const{
                return (mpGeometryData->HasIntegrationMethod(ThisMethod));
            }


            IntegrationMethod GetDefaultIntegrationMethod() const{
                return mpGeometryData->DefaultIntegrationMethod();
            }


            virtual IntegrationInfo GetDefaultIntegrationInfo() const{
                return IntegrationInfo(LocalSpaceDimension(),GetDefaultIntegrationMethod());
            }


            virtual bool IsSymmetric() const{
                return false;
            }


            virtual GeometriesArrayType GenerateBoundariesEntities() const{
                const SizeType dimension = this->LocalSpaceDimension();
                if(dimension == 3){
                    return this->GenerateFaces();
                } else if(dimension == 2){
                    return this->GenerateEdges();
                } else {
                    return this->GeneratePoints();
                }
            }


            virtual GeometriesArrayType GeneratePoints() const{
                GeometriesArrayType points;

                const auto& p_points = this->Points();
                for(IndexType i_point = 0; i_point < p_points.size(); ++i_point){
                    PointsArrayType point_array;
                    point_array.push_back(p_points(i_point));
                    auto p_point_geometry = Kratos::make_shared<PointGeometry<TPointType>>(point_array);
                    points.push_back(p_point_geometry);
                }

                return points;
            }


            virtual SizeType EdgesNumber() const{
                QUEST_ERROR << "Calling base class EdgesNumber method instead of derived class one. Please check the definition of derived class." << *this << std::endl;
            }


            virtual GeometriesArrayType GenerateEdges() const{
                QUEST_ERROR << "Calling base class GenerateEdges method instead of derived class one. Please check the definition of derived class." << *this << std::endl;
            }


            virtual SizeType FacesNumber() const{
                QUEST_ERROR << "Calling base class FacesNumber method instead of derived class one. Please check the definition of derived class." << *this << std::endl;
            }


            virtual GeometriesArrayType GenerateFaces() const{
                QUEST_ERROR << "Calling base class GenerateFaces method instead of derived class one. Please check the definition of derived class." << *this << std::endl;
            }


            virtual void NumberNodesInFaces(DenseVector<unsigned int>& rNumberNodesInFaces) const{
                QUEST_ERROR << "Calling base class NumberNodesInFaces method instead of derived class one. Please check the definition of derived class." << *this << std::endl;
            }


            virtual void NodesInFaces(DenseMatrix<unsigned int>& rNodesInFaces) const{
                QUEST_ERROR << "Calling base class NodesInFaces method instead of derived class one. Please check the definition of derived class." << *this << std::endl;
            }


            SizeType IntegrationPointsNumber() const{
                return mpGeometryData->IntegrationPoints().size();
            }


            SizeType IntegrationPointsNumber(IntegrationMethod ThisMethod) const{
                return mpGeometryData->IntegrationPointsNumber(ThisMethod);
            }


            const IntegrationPointsArrayType& IntegrationPoints() const{
                return mpGeometryData->IntegrationPoints();
            }


            const IntegrationPointsArrayType& IntegrationPoints(IntegrationMethod ThisMethod) const{
                return mpGeometryData->IntegrationPoints(ThisMethod);
            }


            virtual void CreateIntegrationPoints(
                IntegrationPointsArrayType& rIntegrationPoints,
                IntegrationInfo& rIntegrationInfo
            ) const {
                IntegrationMethod integration_method = rIntegrationInfo.GetIntegrationMethod(0);
                for(IndexType i = 1; i < LocalSpaceDimension(); ++i){
                    QUEST_ERROR_IF(integration_method != rIntegrationInfo.GetIntegrationMethod(i))
                        << "Default creation of integration points only valid if integration method is not varying per direction." << std::endl;
                }
                rIntegrationPoints = IntegrationPoints(integration_method);
            }


            virtual void CreateQuadraturePointGeometries(
                GeometriesArrayType& rResultGeometries,
                IndexType NumberOfShapeFunctionDerivatives,
                const IntegrationPointsArrayType& rIntegrationPoints,
                IntegrationInfo& rIntegrationInfo
            ){
                QUEST_ERROR << "Calling CreateQuadraturePointGeometries from geometry base class. Please check the definition of derived class." << *this << std::endl;
            }


            virtual void CreateQuadraturePointGeometries(
                GeometriesArrayType& rResultGeometries,
                IndexType NumberOfShapeFunctionDerivatives,
                IntegrationInfo& rIntegrationInfo
            ){
                IntegrationPointsArrayType IntegrationPoints;
                CreateIntegrationPoints(IntegrationPoints, rIntegrationInfo);

                this->CreateQuadraturePointGeometries(
                    rResultGeometries,
                    NumberOfShapeFunctionDerivatives,
                    IntegrationPoints,
                    rIntegrationInfo
                );
            }


            virtual CoordinatesArrayType& GlobalCoordinates(
                CoordinatesArrayType& rResult,
                const CoordinatesArrayType& LocalCoordinates
            ) const {
                noalias(rResult) = ZeroVector(3);

                Vector N(this->size());
                ShapeFunctionsValues(N, LocalCoordinates);

                for(IndexType i = 0; i < this->size(); ++i){
                    noalias(rResult) += N[i]*(*this)[i];
                }

                return rResult;
            }


            void GlobalCoordinates(
                CoordinatesArrayType& rResult,
                IndexType IntegrationPointIndex
            ) const {
                this->GlobalCoordinates(rResult,IntegrationPointIndex,GetDefaultIntegrationMethod());
            }


            void GlobalCoordinates(
                CoordinatesArrayType& rResult,
                IndexType IntegrationPointIndex,
                const IntegrationMethod ThisMethod
            ) const {
                noalias(rResult) = ZeroVector(3);

                const Matrix& N = this->ShapeFunctionsValues(ThisMethod);

                for(IndexType i = 0; i < this->size(); ++i){
                    noalias(rReault) += N(IntegrationPointIndex,i) * (*this)[i];
                }
            }


            virtual CoordinatesArrayType& GlobalCoordinates(
                CoordinatesArrayType& rResult,
                const CoordinatesArrayType& LocalCoordinates,
                Matrix& DeltaPosition
            ) const {
                constexpr std::size_t dimension = 3;
                noalias(rResult) = ZeroVector(3);
                if(DeltaPosition.size2() != 3){
                    DeltaPosition.resize(DeltaPosition.size1(),dimension,false);
                }

                Vector N(this->size());
                ShapeFunctionsValues(N, LocalCoordinates);

                for(IndexType i = 0; i < this->size(); ++i){
                    noalias(rResult) += N[i] * ((*this)[i] + row(DeltaPosition,i));
                }

                return rResult;
            }


            virtual void GlobalSpaceDerivatives(
                std::vector<CoordinatesArrayType>& rGlobalSpaceDerivatives,
                const CoordinatesArrayType& rLocalCoordinates,
                const SizeType DerivativeOrder
            ) const {
                if(DerivativeOrder == 0){
                    if(rGlobalSpaceDerivatives.size() != 1){
                        rGlobalSpaceDerivatives.resize(1);
                    }

                    this->GlobalCoordinates(
                        rGlobalSpaceDerivatives[0],
                        rLocalCoordinates
                    );
                } else if (DerivativeOrder == 1){
                    const double local_space_dimension = LocalSpaceDimension();
                    const SizeType points_number = this->size();

                    if(rGlobalSpaceDerivatives.size() != 1+local_space_dimension){
                        rGlobalSpaceDerivatives.resize(1+local_space_dimension);
                    }

                    this->GlobalCoordinates(
                        rGlobalSpaceDerivatives[0],
                        rLocalCoordinates
                    );

                    Matrix shape_functions_gradients(points_number, local_space_dimension);
                    this->ShapeFunctionsLocalGradients(shape_functions_gradients, rLocalCoordinates);

                    for(IndexType i = 0; i < points_number; ++i){
                        const Array1d<double,3>& r_coordinates = (*this)[i].Coordinates();
                        for(IndexType k = 0; k < WorkingSpaceDimension(); ++k){
                            const double value = R_OK
                            for(IndexType m = 0; m < local_space_dimension; ++m){
                                rGlobalSpaceDerivatives[1+m][k] += shape_functions_gradients(i,m)*value;
                            }
                        }
                    }

                    return;
                } else {
                    QUEST_ERROR << "Calling GlobalShapeDerivatives from base class. Please check the definition of derived class." << *this << std::endl;
                }
            }


            virtual void GlobalSpaceDerivatives(
                std::vector<CoordinatesArrayType>& rGlobalSpaceDerivatives,
                IndexType IntegrationPointIndex,
                const SizeType DerivativeOrder
            ) const {
                if(DerivativeOrder == 0){
                    if(rGlobalSpaceDerivatives.size() != 1){
                        rGlobalSpaceDerivatives.resize(1);
                    }

                    GlobalCoordinates(
                        rGlobalSpaceDerivatives[0],
                        IntegrationPointIndex
                    );
                } else if (DerivativeOrder == 1){
                    const double local_space_dimension = LocalSpaceDimension();
                    const SizeType points_number = this->size();

                    if(rGlobalSpaceDerivatives.size() != 1+local_space_dimension){
                        rGlobalSpaceDerivatives.resize(1+local_space_dimension);
                    }

                    this->GlobalCoordinates(
                        rGlobalSpaceDerivatives[0],
                        IntegrationPointIndex
                    );

                    for(IndexType k = 0; k < local_space_dimension; ++k){
                        rGlobalSpaceDerivatives[1+k] = ZeroVector(3);
                    }

                    const Matrix& r_shape_functions_gradients_in_integration_point = this->ShapeFunctionLocalGradient(IntegrationPointIndex);

                    for(IndexType i = 0; i < points_number; ++i){
                        const Array1d<double,3>& r_coordinates = (*this)[i].Coordinates();
                        for(IndexType k = 0; k < WorkingSpaceDimension(); ++k){
                            const double value = r_coordinates[k];
                            for(IndexType m = 0; m < local_space_dimension; ++m){
                                rGlobalSpaceDerivatives[1+m][k] += r_shape_functions_gradients_in_integration_point(i,m)*value;
                            }
                        }
                    }
                } else {
                    QUEST_ERROR << "Calling GlobalShapeDerivatives from base class. Please check the definition of derived class." << *this << std::endl;
                }
            }


            virtual int ProjectionPointLocalToLocalSpace(
                const CoordinatesArrayType& rPointGlobalCoordinates,
                CoordinatesArrayType& rProjectionPointLocalCoordinates,
                const double Tolerance = std::numeric_limits<double>::epsilon()
            ) const {
                QUEST_ERROR << "Calling ProjectionPointLocalToLocalSpace from base class. Please check the definition of derived class." << *this << std::endl;
            }


            virtual int ProjectionPointGlobalToLocalSpace(
                const CoordinatesArrayType& rPointGlobalCoordinates,
                CoordinatesArrayType& rProjectionPointLocalCoordinates,
                const double Tolerance = std::numeric_limits<double>::epsilon()
            ) const {
                QUEST_ERROR << "Calling ProjectionPointGlobalToLocalSpace from base class. Please check the definition of derived class." << *this << std::endl;
            }


            virtual int ClosestPointLocalToLocalSpace(
                const CoordinatesArrayType& rPointLocalCoordinates,
                CoordinatesArrayType& rClosestPointLocalCoordinates,
                const double Tolerance = std::numeric_limits<double>::epsilon()
            ) const {
                const int projection_result = ProjectionPointLocalToLocalSpace(
                    rPointLocalCoordinates,
                    rClosestPointLocalCoordinates,
                    Tolerance
                );

                if(projection_result == 1){
                    return IsInsideLocalSpace(rClosestPointLocalCoordinates, Tolerance);
                } else {
                    return -1;
                }
            }


            virtual int ClosestPointGlobalToLocalSpace(
                const CoordinatesArrayType& rPointGlobalCoordinates,
                CoordinatesArrayType& rClosestPointLocalCoordinates,
                const double Tolerance = std::numeric_limits<double>::epsilon()
            ) const {
                const int projection_result = ProjectionPointGlobalToLocalSpace(
                    rPointGlobalCoordinates,
                    rClosestPointLocalCoordinates,
                    Tolerance
                );

                if(projection_result == 1){
                    return IsInsideLocalSpace(rClosestPointLocalCoordinates, Tolerance);
                } else {
                    return -1;
                }
            }


            virtual double CalculateDistance(
                const CoordinatesArrayType& rPointGlobalCoordinates,
                const double Tolerance = std::numeric_limits<double>::epsilon()
            ) const {
                CoordinatesArrayType local_coordinates(ZeroVector(3));
                if(ClosestPointGlobalToLocalSpace(rPointGlobalCoordinates, local_coordinates, Tolerance) < 1){
                    return std::numeric_limits<double>::max();
                }

                CoordinatesArrayType global_coordinates(ZeroVector(3));
                this->GlobalCoordinates(global_coordinates, local_coordinates);

                return norm_2(rPointGlobalCoordinates - global_coordinates);
            }


            JacobiansType& Jacobian(JacobiansType& rResult) const{
                Jacobian(rResult, mpGeometryData->DefaultIntegrationMethod());
                return rResult;
            }


            virtual JacobiansType& Jacobian(
                JacobiansType& rResult,
                IntegrationMethod ThisMethod
            ) const {
                if(rResult.size() != this->IntegrationPointsNumber(ThisMethod)){
                    rResult.resize(this->IntegrationPointsNumber(ThisMethod),false);
                }

                for(unsigned int pnt = 0; pnt < this->IntegrationPointsNumber(ThisMethod); ++pnt){
                    this->Jacobian(rResult[pnt], pnt, ThisMethod);
                }

                return rResult;
            }


            virtual JacobiansType& Jacobian(
                JacobiansType& rResult,
                IntegrationMethod ThisMethod,
                Matrix& DeltaPosition
            ) const {
                if(rResult.size() != this->IntegrationPointsNumber(ThisMethod)){
                    rResult.resize(this->IntegrationPointsNumber(ThisMethod),false);
                }

                for(unsigned int pnt = 0; pnt < this->IntegrationPointsNumber(ThisMethod); ++pnt){
                    this->Jacobian(rResult[pnt], pnt, ThisMethod, DeltaPosition);
                }

                return rResult;
            }


            Matrix& Jacobian(Matrix& rResult, IndexType IntegrationPointIndex) const{
                Jacobian(rResult, IntegrationPointIndex, mpGeometryData->DefaultIntegrationMethod());
                return rResult;
            }


            virtual Matrix& Jacobian(
                Matrix& rResult,
                IndexType IntegrationPointIndex,
                IntegrationMethod ThisMethod
            ) const {
                const SizeType working_space_dimension = this->WorkingSpaceDimension();
                const SizeType local_space_dimension = this->LocalSpaceDimension();

                if(rResult.size1() != working_space_dimension || rResult.size2() != local_space_dimension){
                    rResult.resize(working_space_dimension,local_space_dimension,false);
                }

                const Matrix& r_shape_functions_gradient_in_integration_point = ShapeFunctionsLocalGradients(ThisMethod)[IntegrationPointIndex];

                rResult.clear();
                const SizeType points_number = this->PointsNumber();
                for(IndexType i = 0; i < points_number; ++i){
                    const Array1d<double,3>& r_coordinates = (*this)[i].Coordinates();
                    for(IndexType k = 0; k < working_space_dimension; ++k){
                        const double value = r_coordinates[k];
                        for(IndexType m = 0; m < local_space_dimension; ++m){
                            rResult(k,m) += r_shape_functions_gradient_in_integration_point(i,m)*value;
                        }
                    }
                }

                return rResult;
            }


            virtual Matrix& Jacobian(
                Matrix& rResult,
                IndexType IntegrationPointIndex,
                IntegrationMethod ThisMethod,
                const Matrix& rDeltaPosition
            ) const {
                const SizeType working_space_dimension = this->WorkingSpaceDimension();
                const SizeType local_space_dimension = this->LocalSpaceDimension();

                if(rResult.size1() != working_space_dimension || rResult.size2() != local_space_dimension){
                    rResult.resize(working_space_dimension,local_space_dimension,false);
                }

                const Matrix& r_shape_functions_gradient_in_integration_point = ShapeFunctionsLocalGradients(ThisMethod)[IntegrationPointIndex];

                rResult.clear();
                const SizeType points_number = this->PointsNumber();
                for(IndexType i = 0; i < points_number; ++i){
                    const Array1d<double,3>& r_coordinates = (*this)[i].Coordinates();
                    for(IndexType k = 0; k < working_space_dimension; ++k){
                        const double value = r_coordinates[k] - rDeltaPosition(i,k);
                        for(IndexType m = 0; m < local_space_dimension; ++m){
                            rResult(k,m) += r_shape_functions_gradient_in_integration_point(i,m)*value;
                        }
                    }
                }

                return rResult;
            }


            virtual Matrix& Jacobian(Matrix& rResult, const CoordinatesArrayType& rCoordinates) const{
                const SizeType working_space_dimension = this->WorkingSpaceDimension();
                const SizeType local_space_dimension = this->LocalSpaceDimension();
                const SizeType points_number = this->PointsNumber();

                if(rResult.size1() != working_space_dimension || rResult.size2() != local_space_dimension){
                    rResult.resize(working_space_dimension,local_space_dimension,false);
                }

                Matrix shape_functions_gradients(points_number, local_space_dimension);
                ShapeFunctionsLocalGradients(shape_functions_gradients, rCoordinates);

                rResult.clear();
                for(IndexType i = 0; i < points_number; ++i){
                    const Array1d<double,3>& r_coordinates = (*this)[i].Coordinates();
                    for(IndexType k = 0; k < working_space_dimension; ++k){
                        const double value = r_coordinates[k];
                        for(IndexType m = 0; m < local_space_dimension; ++m){
                            rResult(k,m) += value * shape_functions_gradients(i,m);
                        }
                    }
                }

                return rResult;
            }


            virtual Matrix& Jacobian(
                Matrix& rResult,
                const CoordinatesArrayType& rCoordinates,
                Matrix& rDeltaPosition
            ) const {
                const SizeType working_space_dimension = this->WorkingSpaceDimension();
                const SizeType local_space_dimension = this->LocalSpaceDimension();
                const SizeType points_number = this->PointsNumber();

                if(rResult.size1() != working_space_dimension || rResult.size2() != local_space_dimension){
                    rResult.resize(working_space_dimension,local_space_dimension,false);
                }

                Matrix shape_functions_gradients(points_number, local_space_dimension);
                ShapeFunctionsLocalGradients(shape_functions_gradients, rCoordinates);

                rResult.clear();
                for(IndexType i = 0; i < points_number; ++i){
                    const Array1d<double,3>& r_coordinates = (*this)[i].Coordinates();
                    for(IndexType k = 0; k < working_space_dimension; ++k){
                        const double value = r_coordinates[k] - rDeltaPosition(i,k);
                        for(IndexType m = 0; m < local_space_dimension; ++m){
                            rResult(k,m) += value * shape_functions_gradients(i,m);
                        }
                    }
                }

                return rResult;
            }


            Vector& DeterminantOfJacobian(Vector& rResult) const{
                DeterminantOfJacobian(rResult, mpGeometryData->DefaultIntegrationMethod()); 
                return rResult;
            }


            virtual Vector& DeterminantOfJacobian(Vector& rResult, IntegrationMethod ThisMethod) const{
                if(rResult.size() != this->IntegrationPointsNumber(ThisMethod)){
                    rResult.resize(this->IntegrationPointsNumber(ThisMethod),false);
                }

                Matrix J(this->WorkingSpaceDimension(), this->LocalSpaceDimension());
                for(unsigned int pnt = 0; pnt < this->IntegrationPointsNumber(ThisMethod); ++pnt){
                    this->Jacobian(J, pnt, ThisMethod);
                    rResult[pnt] = MathUtils<double>::GeneralizedDet(J);
                }

                return rResult;
            }


            double DeterminantOfJacobian(IndexType IntegrationPointIndex) const{
                return DeterminantOfJacobian(IntegrationPointIndex, mpGeometryData->DefaultIntegrationMethod());
            }


            virtual double DeterminantOfJacobian(IndexType IntegrationPointIndex, IntegrationMethod ThisMethod) const{
                Matrix J(this->WorkingSpaceDimension(), this->LocalSpaceDimension());
                this->Jacobian(J, IntegrationPointIndex, ThisMethod);
                return MathUtils<double>::GeneralizedDet(J);
            }


            virtual double DeterminantOfJacobian(const CoordinatesArrayType& rPoint) const{
                Matrix J(this->WorkingSpaceDimension(), this->LocalSpaceDimension());
                this->Jacobian(J, rPoint);
                return MathUtils<double>::GeneralizedDet(J);
            }


            JacobiansType& InverseOfJacobian(JacobiansType& rResult) const{
                InverseOfJacobian(rResult, mpGeometryData->DefaultIntegrationMethod());
                return rResult;
            }


            virtual JacobiansType& InverseOfJacobian(JocabiansType& rResult, IntegrationMethod ThisMethod) const{
                Jacobian(rResult, ThisMethod);

                double detJ;
                Matrix Jinv(this->LocalSpaceDimension(), this->WorkingSpaceDimension());
                for(unsigned int pnt = 0; pnt < this->IntegrationPointsNumber(ThisMethod); ++pnt){
                    MathUtils<double>::GeneralizedInvertMatrix(rResult[pnt], Jinv, detJ);
                    noalias(rResult[pnt]) = Jinv;
                }
                return rResult;
            }


            Matrix& InverseOfJacobian(Matrix& rResult, IndexType IntegrationPointIndex) const{
                InverseOfJacobian(rResult, IntegrationPointIndex, mpGeometryData->DefaultIntegrationMethod());
                return rResult;
            }


            virtual Matrix& InverseOfJacobian(
                Matrix& rResult,
                IndexType IntegrationPointIndex,
                IntegrationMethod ThisMethod
            ) const {
                Jacobian(rResult, IntegrationPointIndex, ThisMethod);

                double detJ;
                Matrix Jinv(this->LocalSpaceDimension(), this->WorkingSpaceDimension());

                MathUtils<double>::GeneralizedInvertMatrix(rResult, Jinv, detJ);
                noalias(rResult) = Jinv;

                return rResult;
            }


            virtual Matrix& InverseOfJacobian(Matrix& rResult, const CoordinatesArrayType& rCoordinates) const{
                Jacobian(rResult, rCoordinates);

                double detJ;
                Matrix Jinv(this->LocalSpaceDimension(), this->WorkingSpaceDimension());

                MathUtils<double>::GeneralizedInvertMatrix(rResult, Jinv, detJ);
                noalias(rResult) = Jinv;

                return rResult;
            }


            const Matrix& ShapeFunctionsValues() const{
                return mpGeometryData->ShapeFunctionsValues();
            }


            virtual Vector& ShapeFunctionsValues(Vector& rResult, const CoordinatesArrayType& rCoordinates) const{
                QUEST_ERROR << "Calling ShapeFunctionsValues from base class. Please check the definition of derived class." << *this << std::endl;
                return rResult;
            }


            const Matrix& ShapeFunctionsValues(IntegrationMethod ThisMethod) const{
                return mpGeometryData->ShapeFunctionsValues(ThisMethod);
            }


            double ShapeFunctionValue(IndexType IntegrationPointIndex, IndexType ShapeFunctionIndex) const{
                return mpGeometryData->ShapeFunctionValue(IntegrationPointIndex, ShapeFunctionIndex);
            }


            double ShapeFunctionValue(
                IndexType IntegrationPointIndex,
                IndexType ShapeFunctionIndex,
                IntegrationMethod ThisMethod
            ) const {
                return mpGeometryData->ShapeFunctionValue(IntegrationPointIndex, ShapeFunctionIndex, ThisMethod);
            }


            virtual double ShapeFunctionValue(IndexType ShapeFunctionIndex, const CoordinatesArrayType& rCoordinates) const{
                QUEST_ERROR << "Calling ShapeFunctionValue from base class. Please check the definition of derived class." << *this << std::endl;

                return 0.0;
            }


            const ShapeFunctionsGradientsType& ShapeFunctionsLocalGradients() const{
                return mpGeometryData->ShapeFunctionsLocalGradients();
            }


            const ShapeFunctionsGradientsType& ShapeFunctionsLocalGradients(IntegrationMethod ThisMethod) const{
                return mpGeometryData->ShapeFunctionsLocalGradients(ThisMethod);
            }


            const Matrix& ShapeFunctionLocalGradient(IndexType IntegrationPointIndex) const{
                return mpGeometryData->ShapeFunctionLocalGradient(IntegrationPointIndex);
            }


            const Matrix& ShapeFunctionLocalGradient(IndexType IntegrationPointIndex, IntegrationMethod ThisMethod) const{
                return mpGeometryData->ShapeFunctionLocalGradient(IntegrationPointIndex, ThisMethod);
            }


            const Matrix& ShapeFunctionLocalGradient(
                IndexType IntegrationPointIndex,
                IndexType ShapeFunctionIndex,
                IntegrationMethod ThisMethod
            ) const {
                return mpGeometryData->ShapeFunctionLocalGradient(IntegrationPointIndex, ShapeFunctionIndex, ThisMethod);
            }


            virtual Matrix& ShapeFunctionsLocalGradients(Matrix& rResult, const CoordinatesArrayType& rPoint) const{
                QUEST_ERROR << "Calling ShapeFunctionsLocalGradients from base class. Please check the definition of derived class." << *this << std::endl;
                return rResult;
            }


            const Matrix& ShapeFunctionDerivatives(
                IndexType DerivativeOrderIndex,
                IndexType IntegrationPointIndex,
                IntegrationMethod ThisMethod
            ) const {
                return mpGeometryData->ShapeFunctionDerivatives(
                    DerivativeOrderIndex,
                    IntegrationPointIndex,
                    ThisMethod
                );
            }


            const Matrix& ShapeFunctionDerivatives(
                IndexType DerivativeOrderIndex,
                IndexType IntegrationPointIndex
            ) const {
                return mpGeometryData->ShapeFunctionDerivatives(
                    DerivativeOrderIndex,
                    IntegrationPointIndex,
                    GetDefaultIntegrationMethod()
                );
            }


            virtual ShapeFunctionsSecondDerivativesType& ShapeFunctionsSecondDerivatives(
                ShapeFunctionsSecondDerivativesType& rResult,
                const CoordinatesArrayType& rPoint
            ) const {
                QUEST_ERROR << "Calling ShapeFunctionsSecondDerivatives from base class. Please check the definition of derived class." << *this << std::endl;
                return rResult;
            }


            virtual ShapeFunctionsThirdDerivativesType& ShapeFunctionsThirdDerivatives(
                ShapeFunctionsThirdDerivativesType& rResult,
                const CoordinatesArrayType& rPoint
            ) const {
                QUEST_ERROR << "Calling ShapeFunctionsThirdDerivatives from base class. Please check the definition of derived class." << *this << std::endl;
                return rResult;
            }

            
            void ShapeFunctionsIntegrationPointsGradients(ShapeFunctionsGradientsType& rResult) const{
                ShapeFunctionsIntegrationPointsGradients(rResult, mpGeometryData->DefaultIntegrationMethod());
            }


            virtual void ShapeFunctionsIntegrationPointsGradients(
                ShapeFunctionsGradientsType& rResult,
                IntegrationMethod ThisMethod
            ) const {
                QUEST_ERROR_IF_NOT(this->WorkingSpaceDimension() == this->LocalSpaceDimension())
                    << "\'ShapeFunctionsIntegrationPointsGradients\' is not defined for current geometry type as only defined in the local space." << std::endl;

                const unsigned int integration_points_number = this->IntegrationPointsNumber(ThisMethod);

                if(integration_points_number == 0){
                    QUEST_ERROR << "This integration method is not supported" << *this << std::endl;
                }

                if(rResult.size() != integration_points_number){
                    rResult.resize(this->IntegrationPointsNumber(ThisMethod),false);
                }

                const ShapeFunctionsGradientsType& DN_De = ShapeFunctionsLocalGradients(ThisMethod);

                Matrix Jinv(this->LocalSpaceDimension(), this->WorkingSpaceDimension());
                for(unsigned int pnt = 0; pnt < integration_points_number; ++pnt){
                    if(rResult[pnt].size1() != (*this).size() || rResult[pnt].size2() != this->LocalSpaceDimension()){
                        rResult[pnt].resize((*this).size(), this->LocalSpaceDimension(), false);
                    }
                    this->InverseOfJacobian(Jinv, pnt, ThisMethod);
                    noalias(rResult[pnt]) = prod(DN_De[pnt], Jinv);
                }
            }


            virtual void ShapeFunctionsIntegrationPointsGradients(
                ShapeFunctionsGradientsType& rResult,
                Vector& rDeterminantsOfJacobian,
                IntegrationMethod ThisMethod
            ) const {
                QUEST_ERROR_IF_NOT(this->WorkingSpaceDimension() == this->LocalSpaceDimension())
                    << "\'ShapeFunctionsIntegrationPointsGradients\' is not defined for current geometry type as gradients are only defined in the local space." << std::endl;

                const unsigned int integration_points_number = this->IntegrationPointsNumber(ThisMethod);

                if(integration_points_number == 0){
                    QUEST_ERROR << "This integration method is not supported" << *this << std::endl;
                }

                if(rResult.size() != integration_points_number){
                    rResult.resize(this->IntegrationPointsNumber(ThisMethod),false);
                }

                if(rDeterminantsOfJacobian.size() != integration_points_number){
                    rDeterminantsOfJacobian.resize(this->IntegrationPointsNumber(ThisMethod),false);
                }

                const ShapeFunctionsGradientsType& DN_De = ShapeFunctionsLocalGradients(ThisMethod);

                Matrix J(this->WorkingSpaceDimension(), this->LocalSpaceDimension());
                Matrix Jinv(this->LocalSpaceDimension(), this->WorkingSpaceDimension());
                double DetJ;
                for(unsigned int pnt = 0; pnt < integration_points_number; ++pnt){
                    if(rResult[pnt].size1() != (*this).size() || rResult[pnt].size2() != this->LocalSpaceDimension()){
                        rResult[pnt].resize((*this).size(), this->LocalSpaceDimension(), false);
                    }
                    this->Jacobian(J, pnt, ThisMethod);
                    MathUtils<double>::GeneralizedInvertMatrix(J, Jinv, DetJ);
                    noalias(rResult[pnt]) = prod(DN_De[pnt], Jinv);
                    rDeterminantsOfJacobian[pnt] = DetJ;
                }
            }


            virtual int Check() const{
                QUEST_TRY

                return 0;

                QUEST_CATCH("")
            }


            virtual std::string Info() const{
                std::stringstream buffer;
                buffer << "Geometry # "
                    << std::to_string(mId) << ": "
                    << LocalSpaceDimension() << "-dimensional geometry in "
                    << WorkingSpaceDimension() << "D space";
                
                return buffer.str();
            }


            virtual std::string Name() const{
                std::string geometryName = "BaseGeometry";
                QUEST_ERROR << "Base geometry does not have a name." << std::endl;
                return geometryName;
            }


            virtual void PrintInfo(std::ostream& rOstream) const{
                rOstream << Info();
            }


            virtual void PrintName(std::ostream& rOstream) const{
                rOstream << Name() << std::endl;
            }


            virtual void PrintData(std::ostream& rOstream) const{
                if(mpGeometryData){
                    mpGeometryData->PrintData(rOstream);
                }

                rOstream << std::endl;
                rOstream << std::endl;

                for(unsigned int i = 0; i < this->size(); ++i){
                    rOstream << "\tPoint " << i+1 << "\t : ";
                    if(mPoints(i) != nullptr){
                        mPoints[i].PrintData(rOstream);
                    } else {
                        rOstream << "point is empty (nullptr)." << std::endl;
                    }
                    rOstream << std::endl;
                }

                if (AllPointsAreValid()){
                    rOstream << "\tCenter\t : ";
                    Center().PrintData(rOstream);
                }

                rOstream << std::endl;
                rOstream << std::endl;
            }

        protected:
            void SetGeometryData(const GeometryData* pGeometryData){
                mpGeometryData = pGeometryData;
            }


            virtual double InradiusToCircumradiusQuality() const{
                QUEST_ERROR << "Calling InradiusToCircumradiusQuality from base class. Please check the definition of derived class." << *this << std::endl;
                return 0.0;
            }


            virtual double AreaToEdgeLengthRatio() const{
                QUEST_ERROR << "Calling AreaToEdgeLengthRatio from base class. Please check the definition of derived class." << *this << std::endl;
                return 0.0;
            }


            virtual double ShortestAltitudeToEdgeLengthRatio() const{
                QUEST_ERROR << "Calling ShortestAltitudeToEdgeLengthRatio from base class. Please check the definition of derived class." << *this << std::endl;
                return 0.0;
            }


            virtual double InradiusToLongestEdgeQuality() const{
                QUEST_ERROR << "Calling InradiusToLongestEdgeQuality from base class. Please check the definition of derived class." << *this << std::endl;
                return 0.0;
            }


            virtual double ShortestToLongestEdgeQuality() const{
                QUEST_ERROR << "Calling ShortestToLongestEdgeQuality from base class. Please check the definition of derived class." << *this << std::endl;
                return 0.0;
            }


            virtual double RegularityQuality() const{
                QUEST_ERROR << "Calling RegularityQuality from base class. Please check the definition of derived class." << *this << std::endl;
                return 0.0;
            }


            virtual double VolumeToSurfaceAreaQuality() const{
                QUEST_ERROR << "Calling VolumeToSurfaceAreaQuality from base class. Please check the definition of derived class." << *this << std::endl;
                return 0.0;
            }


            virtual double VolumeToEdgeLengthQuality() const{
                QUEST_ERROR << "Calling VolumeToEdgeLengthQuality from base class. Please check the definition of derived class." << *this << std::endl;
                return 0.0;
            }


            virtual double VolumeToAverageEdgeLength() const{
                QUEST_ERROR << "Calling VolumeToAverageEdgeLength from base class. Please check the definition of derived class." << *this << std::endl;
                return 0.0;
            }


            virtual double VolumeToRMSEdgeLength() const{
                QUEST_ERROR << "Calling VolumeToRMSEdgeLength from base class. Please check the definition of derived class." << *this << std::endl;
                return 0.0;
            }


            virtual double MinDihedralAngle() const{
                QUEST_ERROR << "Calling MinDihedralAngle from base class. Please check the definition of derived class." << *this << std::endl;
                return 0.0;
            }


            virtual double MaxDihedralAngle() const{
                QUEST_ERROR << "Calling MaxDihedralAngle from base class. Please check the definition of derived class." << *this << std::endl;
                return 0.0;
            }


            virtual double MinSolidAngle() const{
                QUEST_ERROR << "Calling MinSolidAngle from base class. Please check the definition of derived class." << *this << std::endl;
                return 0.0;
            }


            bool AllPointsAreValid() const{
                return std::none_of(mPoints.begin(), mPoints.end(), [](const auto& pPoint){ return pPoint == nullptr; });
            }

        private:
            IndexType GenerateSelfAssignedId() const{
                IndexType id = reinterpret_cast<IndexType>(this);

                SetIdSelfAssigned(id);

                SetIdNotGeneratedFromString(id);

                return id;
            }


            static inline bool IsIdGeneratedFromString(IndexType Id){
                return Id&(IndexType(1) << (sizeof(IndexType)*8-1));
            }


            static inline void SetIdGeneratedFromString(IndexType& Id){
                Id|=(IndexType(1) << (sizeof(IndexType)*8-1));
            }


            static inline void SetIdNotGeneratedFromString(IndexType& Id){
                Id&=~(IndexType(1) << (sizeof(IndexType)*8-1));
            }


            static inline bool IsIdSelfAssigned(IndexType Id){
                return Id&(IndexType(1) << (sizeof(IndexType)*8-2));
            }


            static inline void SetIdSelfAssigned(IndexType& Id){
                Id|=(IndexType(1) << (sizeof(IndexType)*8-2));
            }


            static inline void SetIdNot SelfAssigned(IndexType& Id){
                Id&=~(IndexType(1) << (sizeof(IndexType)*8-2));
            }


            void SetIdWithoutCheck(const IndexType Id){
                mId = Id;
            }


            static const GeometryData& GeometryDataInstance(){
                IntegrationPointsContainerType integration_points = {};
                ShapeFunctionsValuesContainerType shape_functions_values = {};
                ShapeFunctionsLocalGradientsContainerType shape_functions_local_gradients = {};
                static GeometryData s_geometry_data(
                    &msGeometryDimension,
                    GeometryData::IntegrationMethod::GI_GAUSS_1,
                    integration_points,
                    shape_functions_values,
                    shape_functions_local_gradients
                )

                return s_geometry_data;
            }

            template<typename TOtherPointType> friend class Geometry;
            friend class Serializer;

            virtual void save(Serializer& rSerializer) const{
                rSerializer.save("Id",mId);
                rSerializer.save("Points",mPoints);
                rSerializer.save("Data",mData);
            }

            virtual void load(Serializer& rSerializer){
                rSerializer.load("Id",mId);
                rSerializer.load("Points",mPoints);
                rSerializer.load("Data",mData);
            }

        private:
            IndexType mId;
            const GeometryData* mpGeometryData;
            static const GeometryDimension msGeometryDimension;
            PointsArrayType mPoints;
            DataValueContainer mData;

    };

    template<typename TPointType>
    inline std::istream& operator >> (std::istream& rIstream, Geometry<TPointType>& rThis);

    template<typename TPointType>
    inline std::ostream& operator << (std::ostream& rOstream, const Geometry<TPointType>& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }

    template<typename TPointType>
    const GeometryDimension Geometry<TPointType>::msGeometryDimension(3,3);

} // namespace Quest

#endif //QUEST_GEOMETRY_HPP