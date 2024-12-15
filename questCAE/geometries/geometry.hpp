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

    /**
     * @class Geometry
     * @brief 所有几何类的基类，负责提供几何形状的公共接口和功能，还包含指向各个点的指针数组
     * 以及在所有积分点上计算的形函数值的引用与形函数的梯度值
     * @tparam TPointType 点的类型
     */
    template<typename TPointType>
    class Geometry{
        public:
            using GeometryType = Geometry<TPointType>;

            QUEST_CLASS_POINTER_DEFINITION(Geometry);

            /**
             * @brief 定义了几何体质量的评判标准
             */
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

            /**
             * @brief 质量矩阵集成方法
             * @details 行求和法、对角线缩放法、积分点法
             */
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
            using CoordinatesArrayType = typename PointType::CoordinatesArrayType;
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
            

            /**
             * @brief 复制构造函数
             * @details 不会复制点，而是与原始几何体共享相同的点
             */
            Geometry(const Geometry& rOther):
                mId(rOther.mId),
                mpGeometryData(rOther.mpGeometryData),
                mPoints(rOther.mPoints),
                mData(rOther.mData)
            {}

            /**
             * @brief 带有TOtherPointType的复制构造函数
             */
            template<typename TOtherPointType>
            Geometry(const Geometry<TOtherPointType>& rOther):
                mId(rOther.mId),
                mpGeometryData(rOther.mpGeometryData),
                mPoints(rOther.mPoints)
            {
                mPoints = new PointsArrayType(rOther.begin(), rOther.end());
            }


            virtual ~Geometry(){}

            /**
             * @brief 获取几何体默认的类型（大类，如点、线、面等）
             */
            virtual GeometryData::QuestGeometryFamily GetGeometryFamily() const{
                return GeometryData::QuestGeometryFamily::Quest_generic_family;
            }

            /**
             * @brief 获取几何体默认的类型（小类，如二节点线、三节点线等）
             */
            virtual GeometryData::QuestGeomtryType GetGeometryType() const{
                return GeometryData::QuestGeomtryType::Quest_generic_type;
            }

            /**
             * @brief 赋值运算符重载
             */
            Gemetry& operator = (const Geometry& rOther){
                mpGeometryData = rOther.mpGeometryData;
                mPoints = rOther.mPoints;
                mData = rOther.mData;

                return *this;
            }

            /**
             * @brief 赋值运算符重载,带TOtherPointType
             */
            template<typename TOtherPointType>
            Geometry& operator = (const Geometry<TOtherPointType>& rOther){
                this->clear();

                for(typename Geometry<TOtherPointType>::ptr_const_iterator it = rOther.ptr_begin(); it!= rOther.ptr_end(); ++it){
                    push_back(typename PointType::Point(new PointType(*it)));
                }

                mpGeometryData = rOther.mpGeometryData;

                return *this;
            }

            /**
             * @brief 函数调用运算符重载，返回几何体的节点指针数组
             */
            operator PointsArrayType&(){
                return mPoints;
            }

            /**
             * @brief 下标访问运算符重载，返回第i个节点的指针
             */
            TPointType& operator[](const SizeType& i){
                return mPoints[i];
            }

            /**
             * @brief 下标访问运算符重载，返回第i个节点的指针
             */
            const TPointType& operator[](const SizeType& i) const{
                return mPoints[i];
            }

            /**
             * @brief 函数调用运算符重载，返回节点指针
             */
            PointPointerType& operator()(const SizeType& i){
                return mPoints(i);
            }

            /**
             * @brief 函数调用运算符重载，返回节点指针
             */
            ConstPointPointerType operator()(const SizeType& i) const{
                return mPoints(i);
            }

            /**
             * @brief 返回指向第一个节点的迭代器
             */
            iterator begin(){
                return iterator(mPoints.begin());
            }

            /**
             * @brief 返回指向第一个节点的迭代器
             */
            const_iterator begin() const{
                return const_iterator(mPoints.begin());
            }

            /**
             * @brief 返回指向最后一个节点的迭代器
             */
            iterator end(){
                return iterator(mPoints.end());
            }

            /**
             * @brief 返回指向最后一个节点的迭代器
             */
            const_iterator end() const{
                return const_iterator(mPoints.end());
            }

            /**
             * @brief 返回指向第一个节点指针的迭代器
             */
            ptr_iterator ptr_begin(){
                return mPoints.ptr_begin();
            }

            /**
             * @brief 返回指向第一个节点指针的迭代器
             */
            ptr_const_iterator ptr_begin() const{
                return mPoints.ptr_begin();
            }

            /**
             * @brief 返回指向最后一个节点指针的迭代器
             */
            ptr_iterator ptr_end(){
                return mPoints.ptr_end();
            }

            /**
             * @brief 返回指向最后一个节点指针的迭代器
             */
            ptr_const_iterator ptr_end() const{
                return mPoints.ptr_end();
            }

            /**
             * @brief 返回第一个节点的指针
             */
            PointReferenceType front(){
                assert(!empty());
                return mPoints.front();
            }

            /**
             * @brief 返回第一个节点的指针
             */
            ConstPointReferenceType front() const{
                assert(!empty());
                return mPoints.front();
            }

            /**
             * @brief 返回最后一个节点的指针
             */
            PointReferenceType back(){
                assert(!empty());
                return mPoints.back();
            }

            /**
             * @brief 返回最后一个节点的指针
             */
            ConstPointReferenceType back() const{
                assert(!empty());
                return mPoints.back();
            }

            /**
             * @brief 节点数量
             */
            SizeType size() const{
                return mPoints.size();
            }

            /**
             * @brief 节点数量
             */
            SizeType PointsNumber() const{
                return this->size();
            }

            /**
             * @brief 返回某个局部方向的节点数量
             * @param LocalDirectionIndex 局部方向索引
             */
            virtual SizeType PointsNumberInDirection(IndexType LocalDirectionIndex) const{
                QUEST_ERROR << "Trying to access PointsNumberInDirection from geometry base class." << std::endl;
            }

            /**
             * @brief 最多能容纳的节点数量
             */
            SizeType max_size() const{
                return mPoints.max_size();
            }

            /**
             * @brief 和另一个几何体交换节点
             * @param rOther 另一个几何体
             */
            void swap(Geometry& rOther){
                mPoints.swap(rOther.mPoints);
            }

            /**
             * @brief 添加节点
             * @param pNewPoint 新节点指针
             */
            void push_back(PointPointerType x){
                mPoints.push_back(x);
            }

            /**
             * @brief 清除所有节点
             */
            void clear(){
                mPoints.clear();
            }

            /**
             * @brief 重新设置节点容器大小
             * @param dim 新的容器大小
             */
            void reserve(int dim){
                mPoints.reserve(dim);
            }

            /**
             * @brief 节点数组的容量
             */
            int capacity() const{
                return mPoints.capacity();
            }

            /**
             * @brief 返回节点存储容器对象
             */
            PointPointerContainerType& GetContainer(){
                return mPoints.GetContainer();
            }

            /**
             * @brief 返回节点存储容器对象
             */
            const PointPointerContainerType& GetContainer() const{
                return mPoints.GetContainer();
            }

            /**
             * @brief 返回单元变量数据容器（与时间步无关变量）
             */
            DataValueContainer& GetData(){
                return mData;
            }

            /**
             * @brief 返回单元变量数据容器（与时间步无关变量）
             */
            const DataValueContainer& GetData() const{
                return mData;
            }

            /**
             * @brief 设置单元变量数据容器（与时间步无关变量）
             */
            void SetData(const DataValueContainer& rData){
                mData = rData;
            }

            /**
             * @brief 检查是否有某个变量
             * @param rThisVariable 变量
             */
            template<typename TDataType>
            bool Has(const Variable<TDataType>& rThisVariable) const{
                return mData.Has(rThisVariable);
            }

            /**
             * @brief 设置某个变量的值
             * @param rThisVariable 变量
             * @param rValue 值
             */
            template<typename TVariableType>
            void SetValue(const TVariableType& rThisVariable, typename const TVariableType::Type& rValue){
                mData.SetValue(rThisVariable, rValue);
            }

            /**
             * @brief 获取某个变量的值
             * @param rThisVariable 变量
             */
            template<typename TVariableType>
            typename TVariableType::Type& GetValue(const TVariableType& rThisVariable){
                return mData.GetValue(rThisVariable);
            }

            /**
             * @brief 获取某个变量的值
             * @param rThisVariable 变量
             */
            template<typename TVariableType>
            const typename TVariableType::Type& GetValue(const TVariableType& rThisVariable) const{
                return mData.GetValue(rThisVariable);
            }

            /**
             * @brief 为变量赋值
             * @param rVariable 变量
             * @param Input 输入值
             */
            virtual void Assign(const Variable<bool>& rVariable, const bool Input){}

            /**
             * @brief 为变量赋值
             * @param rVariable 变量
             * @param Input 输入值
             */
            virtual void Assign(const Variable<int>& rVariable, const int Input){}

            /**
             * @brief 为变量赋值
             * @param rVariable 变量
             * @param Input 输入值
             */
            virtual void Assign(const Variable<double>& rVariable, const double Input){}

            /**
             * @brief 为变量赋值
             * @param rVariable 变量
             * @param Input 输入值
             */
            virtual void Assign(const Variable<Array1d<double,2>>& rVariable, const Array1d<double,2>& Input){}

            /**
             * @brief 为变量赋值
             * @param rVariable 变量
             * @param Input 输入值
             */
            virtual void Assign(const Variable<Array1d<double,3>>& rVariable, const Array1d<double,3>& Input){}

            /**
             * @brief 为变量赋值
             * @param rVariable 变量
             * @param Input 输入值
             */
            virtual void Assign(const Variable<Array1d<double,6>>& rVariable, const Array1d<double,6>& Input){}

            /**
             * @brief 为变量赋值
             * @param rVariable 变量
             * @param Input 输入值
             */
            virtual void Assign(cosnt Variable<Vector>& rVariable, const Vector& Input){}

            /**
             * @brief 为变量赋值
             * @param rVariable 变量
             * @param Input 输入值
             */
            virtual void Assign(const Variable<Matrix>& rVariable, const Matrix& Input){}

            /**
             * @brief 根据变量计算、获取或提取某个特定的值
             * @param rVariable 变量
             * @param rOutput 输出值
             */
            virtual void Calculate(const Variable<bool>& rVariable, bool& rOutput) const{}

            /**
             * @brief 根据变量计算、获取或提取某个特定的值
             * @param rVariable 变量
             * @param rOutput 输出值
             */
            virtual void Calculate(const Variable<int>& rVariable, int& rOutput) const{}

            /**
             * @brief 根据变量计算、获取或提取某个特定的值
             * @param rVariable 变量
             * @param rOutput 输出值
             */
            virtual void Calculate(const Variable<double>& rVariable, double& rOutput) const{}

            /**
             * @brief 根据变量计算、获取或提取某个特定的值
             * @param rVariable 变量
             * @param rOutput 输出值
             */
            virtual void Calculate(const Variable<Array1d<double,2>>& rVariable, Array1d<double,2>& rOutput) const{}

            /**
             * @brief 根据变量计算、获取或提取某个特定的值
             * @param rVariable 变量
             * @param rOutput 输出值
             */
            virtual void Calculate(const Variable<Array1d<double,3>>& rVariable, Array1d<double,3>& rOutput) const{}

            /**
             * @brief 根据变量计算、获取或提取某个特定的值
             * @param rVariable 变量
             * @param rOutput 输出值
             */
            virtual void Calculate(const Variable<Array1d<double,6>>& rVariable, Array1d<double,6>& rOutput) const{}

            /**
             * @brief 根据变量计算、获取或提取某个特定的值
             * @param rVariable 变量
             * @param rOutput 输出值
             */
            virtual void Calculate(const Variable<Vector>& rVariable, Vector& rOutput) const{}

            /**
             * @brief 根据变量计算、获取或提取某个特定的值
             * @param rVariable 变量
             * @param rOutput 输出值
             */
            virtual void Calculate(const Variable<Matrix>& rVariable, Matrix& rOutput) const{}

            /**
             * @brief 检查两个几何对象的类类型是否相同（模板参数是否相同）
             */
            inline static bool HasSameType(const GeometryType& rLHS, const GeometryType& rRHS){
                reutrn typeid(rLHS) == typeid(rRHS);
            }

            /**
             * @brief 检查两个几何对象的类类型是否相同（模板参数是否相同）
             */
            inline static bool HasSameType(const GeometryType* rLHS, const GeometryType* rRHS){
                return GeometryType::HasSameType(*rLHS, *rRHS);
            }

            /**
             * @brief 检查两个几何对象的类型是否相同（如都为二节点线）
             */
            inline static bool HasSameGeometryType(const GeometryType& rLHS, const GeometryType& rRHS){
                return rLHS.GetGeometryType() == rRHS.GetGeometryType();
            }

            /**
             * @brief 检查两个几何对象的类型是否相同（如都为二节点线）
             */
            inline static bool HasSameGeometryType(const GeometryType* rLHS, const GeometryType* rRHS){
                return GeometryType::HasSameGeometryType(*rLHS, *rRHS);
            }

            /**
             * @brief 检查两个几何对象是否相同（对象类型信息相同且属于同一类几何单元时两个对象相同）
             */
            inline static bool IsSame(cosnt GeometryType& rLHS, cosnt GeometryType& rRHS){
                return GeometryType::HasSameType(rLHS, rRHS) && GeometryType::HasSameGeometryType(rLHS, rRHS);
            }

            /**
             * @brief 检查两个几何对象是否相同（对象类型信息相同且属于同一类几何单元时两个对象相同）
             */
            inline static bool IsSame(const GeometryType* rLHS, const GeometryType* rRHS){
                return GeometryType::HasSameType(*rLHS, *rRHS) && GeometryType::HasSameGeometryType(*rLHS, *rRHS);
            }

            /**
             * @brief 判断是否为空（无节点）
             */
            bool empty() const{
                return mPoints.empty();
            }

            /**
             * @brief 创建一个新的几何体指针
             * @brief rThisPoints 节点数组
             */
            virtual Pointer Create(const PointsArrayType& rThisPoints)const {
                auto p_geom = this->Create(0,rThisPoints);

                IndexType id = reinterpret_cast<IndexType>(p_geom.get());

                p_geom->SetIdSelfAssigned(id);

                p_geom->SetIdNotGeneratedFromString(id);

                p_geom->SetIdWithoutCheck(id);

                return p_geom;
            }

            /**
             * @brief 创建一个新的几何体指针
             * @brief NewGeometryId 新几何体的Id
             * @brief rThisPoints 节点数组
             */
            virtual Pointer Create(const IndexType NewGeometryId, const PointsArrayType& rThisPoints)const{
                return Pointer( new Geometry(NewGeometryId, rThisPoints, mpGeometryData) );
            }

            /**
             * @brief 创建一个新的几何体指针
             * @brief rNewGeometryName 新几何体的名称
             * @brief rThisPoints 节点数组
             */
            Pointer Create(const std::string& rNewGeometryName, const PointsArrayType& rThisPoints)const{
                auto p_geom = this->Create(0,rThisPoints);
                p_geom->SetId(rNewGeometryName);

                return p_geom;
            }

            /**
             * @brief 创建一个新的几何体指针
             * @brief rGeometry 已存在的几何体对象的引用
             */
            virtual Pointer Create(const GeometryType& rGeometry) const{
                auto p_geom = this->Create(0,rGeometry);

                IndexType id = reinterpret_cast<IndexType>(p_geom.get());

                p_geom->SetIdSelfAssigned(id);

                p_geom->SetIdNotGeneratedFromString(id);

                p_geom->SetIdWithoutCheck(id);

                return p_geom;
            }

            /**
             * @brief 创建一个新的几何体指针
             * @brief NewGeometryId 新几何体的Id
             * @brief rGeometry 已存在的几何体对象的引用
             */
            virtual Pointer Create(cosnt IndexType NewGeometryId, const GeometryType& rGeometry) const{
                auto p_geometry = Pointer(new Geometry(NewGeometryId, rGeometry.Points(), mpGeometryData));
                p_geometry->SetData(rGeometry.GetData());
                return p_geometry;
            }

            /**
             * @brief 创建一个新的几何体指针
             * @brief rNewGeometryName 新几何体的名称
             * @brief rGeometry 已存在的几何体对象的引用
             */
            Pointer Create(const std::string& rNewGeometryName, const GeometryType& rGeometry) const{
                auto p_geom = this->Create(0,rGeometry);
                p_geom->SetId(rNewGeometryName);

                return p_geom;
            }

            /**
             * @brief 创建其所有点的副本，并用这些点替换掉原有的点
             */
            void ClonePoints(){
                for(ptr_iterator i = this->ptr_begin(); i!= this->ptr_end(); ++i){
                    *i = typename PointType::Pointer(new PointType(**i));
                }
            }

            /**
             * @brief 返回几何体数据容器
             */
            const GeometryData& GetGeometryData() const{
                return *mpGeometryData;
            }

            /**
             * @brief 设置形函数数据容器
             */
            virtual void SetGeometryShapeFunctionContainer(
                const GeometryShapeFunctionContainer<GeometryData::IntegrationMethod>& rGeometryShapeFunctionContainer
            ){
                QUEST_ERROR << "Calling SetGeometryShapeFunctionContainer from base geometry class." << std::endl;
            }

            /**
             * @brief 返回几何体的序号
             */
            const IndexType& Id() const{
                return mId;
            }

            /**
             * @brief 判断几何体的序号是否为生成自字符串
             */
            bool IsIdGeneratedFromString(){
                return IsIdGeneratedFromString(mId);
            }

            /**
             * @brief 判断ID是否由其自身生成
             */
            bool IsIdSelfAssigned(){
                return IsIdSelfAssigned(mId);
            }

            /**
             * @brief 设置几何体的序号
             * @param Id 新的序号
             */
            void SetId(const IndexType Id){
                QUEST_ERROR_IF(IsIdGeneratedFromString(Id) || IsIdSelfAssigned(Id))
                    << "Id: " << Id << " out of range. The Id must be loer than 2^62 = 4.61e+18. "
                    << "Geometry being recognized as generated form stding: " << IsIdGeneratedFromString(Id)
                    << ", self assigned: " << IsIdSelfAssigned(Id) << "." << std::endl;

                mId = Id;
            }

            /**
             * @brief 基于这个几何体的名称生成一个新的序号
             * @param rName 几何体的名称
             */
            void SetId(const std::string& rName){
                mId = GenerateId(rName);
            }

            /**
             * @brief 获取对应字符串名称的哈希ID
             */
            static inline IndexType GenerateId(const std::string& rName){
                std::hash<std::string> string_hash_generator;
                auto id = string_hash_generator(rName);

                SetIdGeneratedFromString(id);

                SetIdNotSelfAssigned(id);

                return id;
            }

            /**
             * @param 访问父几何体（一些几何体需要和其他几何体相关联，如积分点）
             * @param Index 这个几何体的对象
             */
            virtual GeometryType& GetGeometryParent(IndexType Index) const{
                QUEST_ERROR << "Calling GetGeometryParent from base geometry class." << std::endl;
            }

            /**
             * @brief 设置父几何体（一些几何体需要和其他几何体相关联，如积分点）
             * @param pGeometryParent 父几何体的指针
             */
            virtual void SetGeometryParent(GeometryType* pGeometryParent){
                QUEST_ERROR << "Calling SetGeometryParent from base geometry class." << std::endl;
            }

            /**
             * @brief 对于复合几何体，根据给定的索引返回对应的几何对象
             * @param Index 几何体的索引
             */
            virtual GeometryType& GetGeometryPart(const IndexType Index){
                return *pGeometryPart(Index);
            }

            /**
             * @brief 对于复合几何体，根据给定的索引返回对应的几何对象
             * @param Index 几何体的索引
             */
            virtual const GeometryType& GetGeometryPart(const IndexType Index) const{
                return *pGeometryPart(Index);
            }

            /**
             * @brief 对于复合几何体，根据给定的索引返回对应的几何对象
             * @param Index 几何体的索引
             */
            virtual typename GeometryType::Pointer pGetGeometryPart(const IndexType Index){
                QUEST_ERROR << "Calling base class 'pGetGeometryPart' method instead of derived function."
                    << " Please check the definition in the derived class." << *this << std::endl;
            }

            /**
             * @brief 对于复合几何体，根据给定的索引返回对应的几何对象
             * @param Index 几何体的索引
             */
            virtual const typename GeometryType::Pointer pGetGeometryPart(const IndexType Index) const{
                QUEST_ERROR << "Calling base class 'pGetGeometryPart' method instead of derived function."
                    << " Please check the definition in the derived class." << *this << std::endl;
            }

            /**
             * @brief 允许交换特定的几何体
             * @param Index 几何体的索引。0表示主几何体，1表示从属几何体
             * @param pGeometry 要添加的新的几何体
             */
            virtual void SetGeometryPart(const IndexType Index, GeometryType::Pointer pGeometry){
                QUEST_ERROR << "Calling base class 'SetGeometryPart' method instead of derived function."
                    << " Please check the definition in the derived class." << *this << std::endl;
            }

            /**
             * @brief 往复合体重添加一个几何体
             * @param pGeometry 要添加的新的几何体
             */
            virtual IndexType AddGeometryPart(GeometryType::Pointer pGeometry){
                QUEST_ERROR << "Calling base class 'AddGeometryPart' method instead of derived function."
                    << " Please check the definition in the derived class." << *this << std::endl;
            }

            /**
             * @brief 移除一个几何体部件
             * @param pGeometry 要移除的几何体
             */
            virtual void RemoveGeometryPart(GeometryType::Pointer pGeometry){
                QUEST_ERROR << "Calling base class 'RemoveGeometryPart' method instead of derived function."
                    << " Please check the definition in the derived class." << *this << std::endl;
            }

            /**
             * @brief 移除一个几何体部件
             * @param pGeometry 要移除的几何体的索引
             */
            virtual void RemoveGeometryPart(const IndexType Index){
                QUEST_ERROR << "Calling base class 'RemoveGeometryPart' method instead of derived function."
                    << " Please check the definition in the derived class." << *this << std::endl;
            }

            /**
             * @brief 用于检查某个带索引的对象是否存在与该几何体的几何部分中
             * @param Index 几何部分的索引，在派生类中此索引的使用方式可能不同
             */
            virtual bool HasGeometryPart(const IndexType Index) const{
                QUEST_ERROR << "Calling base class 'HasGeometryPart' method instead of derived function."
                    << " Please check the definition in the derived class." << *this << std::endl;
            }

            /**
             * @brief 这个几何容器中拥有的几何体部件数量
             */
            virtual SizeType NumberOfGeometryParts() const{
                return 0;
            }

            /**
             * @brief 计算用于构建集总质量矩阵的集总因子
             * @param rResult 用于存储集总因子的向量
             * @param LumpingMethod 集总方法
             */
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

            /**
             * @brief 返回整体坐标系的维度
             */
            inline SizeType WorkingSpaceDimension() const{
                return mpGeometryData->WorkingSpaceDimension();
            }

            /**
             * @brief 返回局部坐标系的维度
             */
            inline SizeType LocalSpaceDimension() const{
                return mpGeometryData->LocalSpaceDimension();
            }

            /**
             * @brief 返回几何体在某一方向上的多项式阶数
             * @param LocalDirectionIndex 局部方向的索引
             */
            virtual SizeType PolynomialDegree(IndexType LocalDirectionIndex) const{
                QUEST_ERROR << "Trying to access PolynomialDegree from geometry base class." << std::endl;
            }

            /**
             * @brief 返回几何体长度
             */
            virtual double Length() const {
                QUEST_ERROR << "Calling base class 'Length' method instead of derived class one. Please check the definition of derived class." << *this << std::endl;
                return 0.0;
            }

            /**
             * @brief 返回几何体的面积
             */
            virtual double Area() const{
                QUEST_ERROR << "Calling base class 'Area' method instead of derived class one. Please check the definition of derived class." << *this << std::endl;
                return 0.0;
            }

            /**
             * @brief 返回几何体的体积
             */
            virtual double Volume() const{
                QUEST_ERROR << "Calling base class 'Volume' method instead of derived class one. Please check the definition of derived class." << *this << std::endl;
                return 0.0;
            }

            /**
             * @brief 此方法根据几何体的维度计算并返回其长度、面积或体积
             */
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

            /**
             * @brief 返回几何体中最短的边的长度
             */
            virtual double MinEdgeLength() const{
                QUEST_ERROR << "Calling base class 'MinEdgeLength' method instead of derived class one. Please check the definition of derived class." << *this << std::endl;
                return 0.0;
            }

            /**
             * @brief 返回几何体中最长的边的长度
             */
            virtual double MaxEdgeLength() const{
                QUEST_ERROR << "Calling base class 'MaxEdgeLength' method instead of derived class one. Please check the definition of derived class." << *this << std::endl;
                return 0.0;
            }

            /**
             * @brief 返回所有边的平均长度
             */
            virtual double AverageEdgeLength() const{
                QUEST_ERROR << "Calling base class 'AverageEdgeLength' method instead of derived class one. Please check the definition of derived class." << *this << std::endl;
                return 0.0;
            }

            /**
             * @brief 计算几何体外接圆的半径
             */
            virtual double Circumradius() const{
                QUEST_ERROR << "Calling base class 'Circumradius' method instead of derived class one. Please check the definition of derived class." << *this << std::endl;
                return 0.0;
            }

            /**
             * @brief 计算几何体的内切圆半径
             */
            virtual double Inradius() const{
                QUEST_ERROR << "Calling base class 'Inradius' method instead of derived class one. Please check the definition of derived class." << *this << std::endl;
                return 0.0;
            }

            /**
             * @brief 判断是否与另一个几何体相交
             * @param 另一个几何体的引用
             */
            virtual bool HasIntersection(const GeometryType& ThisGeometry) const{
                QUEST_ERROR << "Calling base class 'HasIntersection' method instead of derived class one. Please check the definition of derived class." << *this << std::endl;
                return false;
            }

            /**
             * @brief 测试几何体是否与一个盒子相交
             * @param 盒子的下界和上界
             */
            virtual bool HasIntersection(const Point& rLowPoint, const Point& rHighPoint) const{
                QUEST_ERROR << "Calling base class 'HasIntersection' method instead of derived class one. Please check the definition of derived class." << *this << std::endl;
                return false;
            }

            /**
             * @brief 计算几何体的边界框
             * @param rLowPoint 用于存储下界的点
             * @param rHighPoint 用于存储上界的点
             */
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

            /**
             * @brief 计算几何体的中心点
             */
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

            /**
             * @brief 计算与局部点对应的几何体法线向量
             * @param rPointLocalCoordinates 局部坐标点
             */
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

            /**
             * @brief 返回给定积分点处与相应几何体法线垂直的向量，使用默认的积分方法
             */
            virtual Array1d<double,3> Normal(IndexType IntegrationPointIndex) const{
                return Normal(IntegrationPointIndex, mpGeometryData->DefaultIntegrationMethod());
            }

            /**
             * @brief 返回给定积分点处与相应几何体法线垂直的向量
             * @param IntegrationPointIndex 积分点的索引
             * @param ThisMethod 积分方法
             */
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

            /**
             * @brief 计算给定局部点的单位法线
             * @param rPointLocalCoordinates 参考局部坐标，指定计算单位法线的点
             */
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

            /**
             * @brief 返回给定积分点处的单位法向量，使用默认的积分方法
             * @param 积分点在内部积分点列表中的索引
             */
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

            /**
             * @brief 计算几何体的质量，质量为1.0表示网格质量高，0.0表示退化单元，-1.0表示倒转单元
             * @details 退化单元表示单元几何形状变形或尺寸过小，倒转单元是单元的节点顺序错误导致几何体朝向错误
             */
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

            /**
             * @brief 计算几何体的二面角（两个相邻平面之间的角度）
             */
            virtual inline void ComputeDihedralAngles(Vector& rDihedralAngles) const{
                QUEST_ERROR << "Called the virtual function for ComputeDihedralAngles " << *this << std::endl;
            }

            /**
             * @brief 计算几何体的立体角（描述三维空间中一个表面区域的角度大小的度量）
             */
            virtual inline void ComputeSolidAngles(Vector& rSolidAngles) const{
                QUEST_ERROR << "Called the virtual function for ComputeSolidAngles " << *this << std::endl;
            }

            /**
             * @brief 获取存储几何体节点的数组
             */
            const PointsArrayType& Points() const{
                return mPoints;
            }

            /**
             * @brief 获取存储几何体节点的数组
             */
            PointsArrayType& Points(){
                return mPoints;
            }

            /**
             * @brief 获取几何体中某个节点指针
             */
            const typename TPointType::Pointer pGetPoint(const int Index) const{
                QUEST_TRY
                return mPoints(Index);
                QUEST_CATCH(mPoints)
            }

            /**
             * @brief 获取几何体中某个节点指针
             */
            typename TPointType::Pointer pGetPoint(const int Index){
                QUEST_TRY
                return mPoints(Index);
                QUEST_CATCH(mPoints)
            }

            /**
             * @brief 获取几何体中某个节点
             */
            const TPointType& GetPoint(const int Index) const{
                QUEST_TRY
                return mPoints[Index];
                QUEST_CATCH(mPoints)
            }

            /**
             * @brief 获取几何体中某个节点
             */
            TPointType& GetPoint(const int Index){
                QUEST_TRY
                return mPoints[Index];
                QUEST_CATCH(mPoints)
            }

            /**
             * @brief 返回所有点的局部坐标矩阵
             * @param rResult 用于存储局部坐标系的矩阵
             * @return 当前几何体所有点的坐标
             */
            virtual Matrix& PointsLocalCoordinates(Matrix& rResult) const{
                QUEST_ERROR << "Calling base class 'PointsLocalCoordinates' method instead of derived class one. Please check the definition of derived class." << *this << std::endl;
                return rResult;
            }

            /**
             * @brief 返回给定任意点的局部坐标
             * @param rResult 包含该点局部坐标的向量
             * @param rPoint 改点的全局坐标
             * @return 包含该点局部坐标的向量
             */
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

            /**
             * @brief 检查给定的全局坐标点是否在几何体的边界内
             * @param rPointGlobalCoordinates 全局坐标点
             * @param rResult 该点的局部坐标
             * @param Tolerance 容差
             */
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

            /**
             * @brief 检查给定的局部坐标点是否在几何体的边界内
             * @param rPointLocalCoordinates 局部坐标点
             * @param Tolerance 容差
             */
            virtual int IsInsideLocalSpace(
                const CoordinatesArrayType& rPointLocalCoordinates,
                const double Tolerance = std::numeric_limits<double>::epsilon()
            ) const {
                QUEST_ERROR << "Calling IsInsideLocalSpace from base class. Please check the definition of derived class." << *this << std::endl;
                return 0;
            }

            /**
             * @brief 提供几何体在局部坐标系中的跨度，方向由LocalDirectionIndex指定
             * @param rSpans 用于存储跨度的向量
             * @param LocalDirectionIndex 局部坐标系的方向索引，对于曲线总是0
             */
            virtual void SpansLocalSpace(
                std::vector<double>& rSpans,
                IndexType LocalDirectionIndex = 0
            ) const {
                QUEST_ERROR << "Calling SpansLocalSpace from base class. Please check the definition of derived class." << *this << std::endl;
            }

            /**
             * @brief 判断是否有该积分方法
             * @param ThisMethod 积分方法
             */
            bool HasIntegrationMethod(IntegrationMethod ThisMethod) const{
                return (mpGeometryData->HasIntegrationMethod(ThisMethod));
            }

            /**
             * @brief 获取默认积分方法
             */
            IntegrationMethod GetDefaultIntegrationMethod() const{
                return mpGeometryData->DefaultIntegrationMethod();
            }

            /**
             * @brief 获取默认的积分信息
             */
            virtual IntegrationInfo GetDefaultIntegrationInfo() const{
                return IntegrationInfo(LocalSpaceDimension(),GetDefaultIntegrationMethod());
            }

            /**
             * @brief 判断几何体是否对称
             */
            virtual bool IsSymmetric() const{
                return false;
            }

            /**
             * @brief 返回该几何体的所有边界实体
             */
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

            /**
             * @brief 返回该几何体的所有点
             */
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

            /**
             * @brief 获取几何体中边的数量
             */
            virtual SizeType EdgesNumber() const{
                QUEST_ERROR << "Calling base class EdgesNumber method instead of derived class one. Please check the definition of derived class." << *this << std::endl;
            }

            /**
             * @brief 返回该几何体的所有边
             */
            virtual GeometriesArrayType GenerateEdges() const{
                QUEST_ERROR << "Calling base class GenerateEdges method instead of derived class one. Please check the definition of derived class." << *this << std::endl;
            }

            /**
             * @brief 获取几何体中面的数量
             */
            virtual SizeType FacesNumber() const{
                QUEST_ERROR << "Calling base class FacesNumber method instead of derived class one. Please check the definition of derived class." << *this << std::endl;
            }

            /**
             * @brief 返回该几何体的所有面
             */
            virtual GeometriesArrayType GenerateFaces() const{
                QUEST_ERROR << "Calling base class GenerateFaces method instead of derived class one. Please check the definition of derived class." << *this << std::endl;
            }

            /**
             * @brief 获取每个面中节点的数量
             */
            virtual void NumberNodesInFaces(DenseVector<unsigned int>& rNumberNodesInFaces) const{
                QUEST_ERROR << "Calling base class NumberNodesInFaces method instead of derived class one. Please check the definition of derived class." << *this << std::endl;
            }

            /**
             * @brief 获取每个面中节点的索引
             */
            virtual void NodesInFaces(DenseMatrix<unsigned int>& rNodesInFaces) const{
                QUEST_ERROR << "Calling base class NodesInFaces method instead of derived class one. Please check the definition of derived class." << *this << std::endl;
            }

            /**
             * @brief 获取积分点数量
             */
            SizeType IntegrationPointsNumber() const{
                return mpGeometryData->IntegrationPoints().size();
            }

            /**
             * @brief 获取指定积分方法的积分点数量
             */
            SizeType IntegrationPointsNumber(IntegrationMethod ThisMethod) const{
                return mpGeometryData->IntegrationPointsNumber(ThisMethod);
            }

            /**
             * @brief 获取所有积分点
             */
            const IntegrationPointsArrayType& IntegrationPoints() const{
                return mpGeometryData->IntegrationPoints();
            }

            /**
             * @brief 获取指定积分方法的所有积分点
             */
            const IntegrationPointsArrayType& IntegrationPoints(IntegrationMethod ThisMethod) const{
                return mpGeometryData->IntegrationPoints(ThisMethod);
            }

            /**
             * @brief 根据其积分规则创建积分点
             */
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

            /**
             * @brief 根据积分点列表创建积分点几何体的列表
             * @param rResultGeometries 用于存储积分点几何体的列表
             * @param rIntegrationPoints 积分点列表
             * @param NumberOfShapeFunctionDerivatives 积分点几何体处形函数导数的数量
             * @param rIntegrationInfo 积分信息
             */
            virtual void CreateQuadraturePointGeometries(
                GeometriesArrayType& rResultGeometries,
                IndexType NumberOfShapeFunctionDerivatives,
                const IntegrationPointsArrayType& rIntegrationPoints,
                IntegrationInfo& rIntegrationInfo
            ){
                QUEST_ERROR << "Calling CreateQuadraturePointGeometries from geometry base class. Please check the definition of derived class." << *this << std::endl;
            }

            /**
             * @brief 根据积分点列表创建积分点几何体的列表
             * @param rResultGeometries 用于存储积分点几何体的列表
             * @param NumberOfShapeFunctionDerivatives 积分点几何体处形函数导数的数量
             * @param rIntegrationInfo 积分信息
             */
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

            /**
             * @brief 获取给定局部坐标对应的全局坐标
             * @param rResult 用于存储全局坐标的向量
             * @param LocalCoordinates 局部坐标
             */
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

            /**
             * @brief 获取对应积分点的全局坐标
             * @param rResult 用于存储全局坐标的向量
             * @param IntegrationPointIndex 积分点索引
             */
            void GlobalCoordinates(
                CoordinatesArrayType& rResult,
                IndexType IntegrationPointIndex
            ) const {
                this->GlobalCoordinates(rResult,IntegrationPointIndex,GetDefaultIntegrationMethod());
            }

            /**
             * @brief 获取对应积分点的全局坐标
             * @param rResult 用于存储全局坐标的向量
             * @param IntegrationPointIndex 积分点索引
             * @param ThisMethod 积分方法
             */
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

            /** 
             * @brief 该方法提供对应于提供的局部坐标的全局坐标，同时考虑了坐标的增量
             * @param rResult 包含对应于提供的局部坐标的全局坐标的数组
             * @param LocalCoordinates 提供的局部坐标
             * @param DeltaPosition 考虑的位移增量
             * @return 包含对应于提供的局部坐标的全局坐标的数组
             */
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

            /**
             * @brief 将局部坐标下的几何体信息（如形函数的导数）映射到全局坐标系，并计算指定阶数的导数
             * @param rGlobalSpaceDerivatives 用于存储全局坐标系下的导数的向量
             * @param rLocalCoordinates 局部坐标
             * @param DerivativeOrder 导数阶数
             */
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

            /**
             * @brief 基于积分点的坐标来计算全局坐标及其导数
             * @param rGlobalSpaceDerivatives 存储计算结果的向量
             * @param IntegrationPointIndex 积分点索引
             * @param DerivativeOrder 导数阶数
             */
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

            /**
             * @brief 将一个点投影到几何体局部空间中
             * @details 此函数用于将一个给定的局部坐标点投影到几何体上，或者根据提供的初始猜测找到最接近的点。
             * 该函数处理的是局部坐标系内的投影。如果提供的点不在几何体上，它将找到最接近的点。
             * 这个函数可能会使用数学投影算法（如牛顿-拉夫森方法）
             * @param rPointGlobalCoordinates 要投影的点的全局坐标
             * @param rProjectionPointGlobalCoordinates 用于存储投影点的局部坐标，存储初始猜测
             * @param Tolerance 容差
             * @return 1表示找到了最接近的点，0表示没有找到最接近的点
             */
            virtual int ProjectionPointLocalToLocalSpace(
                const CoordinatesArrayType& rPointGlobalCoordinates,
                CoordinatesArrayType& rProjectionPointLocalCoordinates,
                const double Tolerance = std::numeric_limits<double>::epsilon()
            ) const {
                QUEST_ERROR << "Calling ProjectionPointLocalToLocalSpace from base class. Please check the definition of derived class." << *this << std::endl;
            }

            /**
             * @brief 将一个点从全局空间坐标投影到几何体局部空间中
             * @details 此函数用于将一个给定的全局坐标点投影到几何体的局部坐标系中。
             * 处理投影过程中的初始猜测，并尝试找到与几何体上的最近点。在计算过程中使用投影算法
             * @param rPointGlobalCoordinates 要投影的点的全局坐标
             * @param rProjectionPointGlobalCoordinates 用于存储投影点的局部坐标，存储初始猜测
             * @param Tolerance 容差
             * @return 1表示找到了最接近的点，0表示没有找到最接近的点
             */
            virtual int ProjectionPointGlobalToLocalSpace(
                const CoordinatesArrayType& rPointGlobalCoordinates,
                CoordinatesArrayType& rProjectionPointLocalCoordinates,
                const double Tolerance = std::numeric_limits<double>::epsilon()
            ) const {
                QUEST_ERROR << "Calling ProjectionPointGlobalToLocalSpace from base class. Please check the definition of derived class." << *this << std::endl;
            }

            /**
             * @brief 计算最接近点的投影
             * 该方法计算一个点在局部坐标系中的最接近点投影
             * @param rPointLocalCoordinates 输入的局部坐标
             * @param rClosestPointLocalCoordinates 最接近点的局部坐标。应初始化为初始猜测
             * @param Tolerance 允许的正交误差
             * @return int -1 -> 失败
             *             0 -> 在外部
             *             1 -> 在内部
             *             2 -> 在边界上
             */
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

            /**
             * @brief 计算最接近点的投影
             * 该方法计算一个点在全局坐标系中的最接近点投影
             * @param rPointLocalCoordinates 输入的全局坐标
             * @param rClosestPointLocalCoordinates 最接近点的局部坐标。应初始化为初始猜测
             * @param Tolerance 允许的正交误差
             * @return int -1 -> 失败
             *             0 -> 在外部
             *             1 -> 在内部
             *             2 -> 在边界上
             */
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

            /**
             * @brief 计算一个点与该几何体的最接近点之间的距离
             *        如果投影失败，将返回 double::max。
             * @param rPointGlobalCoordinates 要查找最接近点的目标点。
             * @param Tolerance 允许的正交误差。
             * @return 与几何体的距离。
             *         正数 -> 在几何体外（适用于2D和固体）
             *         0     -> 在几何体上/内。
             */
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

            /**
             * @brief 默认积分方法的雅可比矩阵
             */
            JacobiansType& Jacobian(JacobiansType& rResult) const{
                Jacobian(rResult, mpGeometryData->DefaultIntegrationMethod());
                return rResult;
            }

            /**
             * @brief 给定积分方法的雅可比矩阵
             * @param rResult 用于存储雅可比矩阵的矩阵
             * @param ThisMethod 积分方法
             */
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

            /**
             * @brief 在给定积分方法的所有积分点上计算雅可比矩阵
             * @param rResult 用于存储雅可比矩阵的矩阵
             * @param ThisMethod 积分方法
             * @param DeltaPosition 位移增量
             */
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

            /**
             * @brief 计算默认积分方法中特定积分点的雅可比矩阵
             * @param IntegrationPointIndex 积分点索引
             */
            Matrix& Jacobian(Matrix& rResult, IndexType IntegrationPointIndex) const{
                Jacobian(rResult, IntegrationPointIndex, mpGeometryData->DefaultIntegrationMethod());
                return rResult;
            }

            /**
             * @brief 计算给定积分方法中特定积分点的雅可比矩阵
             * @param IntegrationPointIndex 积分点索引
             * @param ThisMethod 积分方法
             */
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

            /**
             * @brief 给定积分方法中特定积分点的雅可比矩阵
             * @param IntegrationPointIndex 积分点索引
             * @param ThisMethod 积分方法
             * @param rDeltaPosition 位移增量
             */
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

            /**
             * @brief 计算给定点的雅可比矩阵
             * @param rCoordinates 给定点的坐标
             */
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

            /**
             * @brief 计算给定点的雅可比矩阵
             * @param rCoordinates 给定点的坐标
             * @param rDeltaPosition 位移增量
             */
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

            /**
             * @brief 默认积分方法的雅可比行列式
             */
            Vector& DeterminantOfJacobian(Vector& rResult) const{
                DeterminantOfJacobian(rResult, mpGeometryData->DefaultIntegrationMethod()); 
                return rResult;
            }

            /**
             * @brief 给定积分方法的雅可比行列式
             * @param ThisMethod 积分方法
             */
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

            /**
             * @brief 计算指定积分点的雅可比行列式
             */
            double DeterminantOfJacobian(IndexType IntegrationPointIndex) const{
                return DeterminantOfJacobian(IntegrationPointIndex, mpGeometryData->DefaultIntegrationMethod());
            }

            /**
             * @brief 计算指定积分方法的特定积分点的雅可比行列式
             */
            virtual double DeterminantOfJacobian(IndexType IntegrationPointIndex, IntegrationMethod ThisMethod) const{
                Matrix J(this->WorkingSpaceDimension(), this->LocalSpaceDimension());
                this->Jacobian(J, IntegrationPointIndex, ThisMethod);
                return MathUtils<double>::GeneralizedDet(J);
            }

            /**
             * @brief 计算给定点的雅可比行列式
             * @param rPoint 给定点的坐标
             */
            virtual double DeterminantOfJacobian(const CoordinatesArrayType& rPoint) const{
                Matrix J(this->WorkingSpaceDimension(), this->LocalSpaceDimension());
                this->Jacobian(J, rPoint);
                return MathUtils<double>::GeneralizedDet(J);
            }

            /**
             * @brief 默认积分方法的雅可比矩阵的逆
             */
            JacobiansType& InverseOfJacobian(JacobiansType& rResult) const{
                InverseOfJacobian(rResult, mpGeometryData->DefaultIntegrationMethod());
                return rResult;
            }

            /**
             * @brief 给定积分方法的雅可比矩阵的逆
             */
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

            /**
             * @brief 计算指定积分点的雅可比矩阵的逆
             */
            Matrix& InverseOfJacobian(Matrix& rResult, IndexType IntegrationPointIndex) const{
                InverseOfJacobian(rResult, IntegrationPointIndex, mpGeometryData->DefaultIntegrationMethod());
                return rResult;
            }

            /**
             * @brief 计算指定积分方法的特定积分点的雅可比矩阵的逆
             */
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

            /**
             * @brief 计算给定点的雅可比矩阵的逆
             */
            virtual Matrix& InverseOfJacobian(Matrix& rResult, const CoordinatesArrayType& rCoordinates) const{
                Jacobian(rResult, rCoordinates);

                double detJ;
                Matrix Jinv(this->LocalSpaceDimension(), this->WorkingSpaceDimension());

                MathUtils<double>::GeneralizedInvertMatrix(rResult, Jinv, detJ);
                noalias(rResult) = Jinv;

                return rResult;
            }

            /**
             * @brief 获取单元形函数在所有积分点的值
             */
            const Matrix& ShapeFunctionsValues() const{
                return mpGeometryData->ShapeFunctionsValues();
            }

            /**
             * @brief 获取单元形函数在指定点的的值
             */
            virtual Vector& ShapeFunctionsValues(Vector& rResult, const CoordinatesArrayType& rCoordinates) const{
                QUEST_ERROR << "Calling ShapeFunctionsValues from base class. Please check the definition of derived class." << *this << std::endl;
                return rResult;
            }

            /**
             * @brief 获取单元形函数在指定积分方法的所有积分点的值
             */
            const Matrix& ShapeFunctionsValues(IntegrationMethod ThisMethod) const{
                return mpGeometryData->ShapeFunctionsValues(ThisMethod);
            }

            /**
             * @brief 获取指定形函数分量在指定积分点的值
             */
            double ShapeFunctionValue(IndexType IntegrationPointIndex, IndexType ShapeFunctionIndex) const{
                return mpGeometryData->ShapeFunctionValue(IntegrationPointIndex, ShapeFunctionIndex);
            }

            /**
             * @brief 获取单元形函数分量在指定积分方法的特定积分点的值
             */
            double ShapeFunctionValue(
                IndexType IntegrationPointIndex,
                IndexType ShapeFunctionIndex,
                IntegrationMethod ThisMethod
            ) const {
                return mpGeometryData->ShapeFunctionValue(IntegrationPointIndex, ShapeFunctionIndex, ThisMethod);
            }

            /**
             * @brief 获取单元形函数分量在指定点的值
             */
            virtual double ShapeFunctionValue(IndexType ShapeFunctionIndex, const CoordinatesArrayType& rCoordinates) const{
                QUEST_ERROR << "Calling ShapeFunctionValue from base class. Please check the definition of derived class." << *this << std::endl;

                return 0.0;
            }

            /**
             * @brief 获取所有积分点的形函数梯度值
             */
            const ShapeFunctionsGradientsType& ShapeFunctionsLocalGradients() const{
                return mpGeometryData->ShapeFunctionsLocalGradients();
            }

            /**
             * @brief 获取指定积分方法的所有积分点的形函数梯度值
             */
            const ShapeFunctionsGradientsType& ShapeFunctionsLocalGradients(IntegrationMethod ThisMethod) const{
                return mpGeometryData->ShapeFunctionsLocalGradients(ThisMethod);
            }

            /**
             * @brief 获取指定积分点的形函数梯度值
             */
            const Matrix& ShapeFunctionLocalGradient(IndexType IntegrationPointIndex) const{
                return mpGeometryData->ShapeFunctionLocalGradient(IntegrationPointIndex);
            }

            /**
             * @brief 获取指定积分方法的特定积分点的形函数梯度值
             */
            const Matrix& ShapeFunctionLocalGradient(IndexType IntegrationPointIndex, IntegrationMethod ThisMethod) const{
                return mpGeometryData->ShapeFunctionLocalGradient(IntegrationPointIndex, ThisMethod);
            }

            /**
             * @brief 获取指定形函数分量在指定点的形函数梯度值
             */
            const Matrix& ShapeFunctionLocalGradient(
                IndexType IntegrationPointIndex,
                IndexType ShapeFunctionIndex,
                IntegrationMethod ThisMethod
            ) const {
                return mpGeometryData->ShapeFunctionLocalGradient(IntegrationPointIndex, ShapeFunctionIndex, ThisMethod);
            }

            /**
             * @brief 获取指定点的形函数梯度值
             */
            virtual Matrix& ShapeFunctionsLocalGradients(Matrix& rResult, const CoordinatesArrayType& rPoint) const{
                QUEST_ERROR << "Calling ShapeFunctionsLocalGradients from base class. Please check the definition of derived class." << *this << std::endl;
                return rResult;
            }

            /**
             * @brief 获取指定积分方法的特定点的指定阶数的形函数导数值
             */
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

            /**
             * @brief 获取默认积分方法的特定点的指定阶数的形函数导数值
             */
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

            /**
             * @brief 获取点的二阶形函数导数值
             */
            virtual ShapeFunctionsSecondDerivativesType& ShapeFunctionsSecondDerivatives(
                ShapeFunctionsSecondDerivativesType& rResult,
                const CoordinatesArrayType& rPoint
            ) const {
                QUEST_ERROR << "Calling ShapeFunctionsSecondDerivatives from base class. Please check the definition of derived class." << *this << std::endl;
                return rResult;
            }

            /**
             * @brief 获取点的三阶形函数导数值
             */
            virtual ShapeFunctionsThirdDerivativesType& ShapeFunctionsThirdDerivatives(
                ShapeFunctionsThirdDerivativesType& rResult,
                const CoordinatesArrayType& rPoint
            ) const {
                QUEST_ERROR << "Calling ShapeFunctionsThirdDerivatives from base class. Please check the definition of derived class." << *this << std::endl;
                return rResult;
            }

            /**
             * @brief 计算默认积分方法的积分点的形函数导数值
             */
            void ShapeFunctionsIntegrationPointsGradients(ShapeFunctionsGradientsType& rResult) const{
                ShapeFunctionsIntegrationPointsGradients(rResult, mpGeometryData->DefaultIntegrationMethod());
            }

            /**
             * @brief 计算给定积分方法的积分点的形函数导数值
             */
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

            /**
             * @brief 计算给定积分方法的积分点的形函数导数值和雅可比行列式
             */
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

            /**
             * 
             */
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

            /**
             * @brief 检查一个Id是否是由字符串生成的
             * @details Id的最高位为1表示由字符串生成，0表示由其他方式生成
             */
            static inline bool IsIdGeneratedFromString(IndexType Id){
                return Id&(IndexType(1) << (sizeof(IndexType)*8-1));
            }

            /**
             * @brief 将ID的最高位设置为1，表示由字符串生成
             */
            static inline void SetIdGeneratedFromString(IndexType& Id){
                Id|=(IndexType(1) << (sizeof(IndexType)*8-1));
            }

            /**
             * @brief 将ID的最高位设置为0，表示不是由字符串生成
             */
            static inline void SetIdNotGeneratedFromString(IndexType& Id){
                Id&=~(IndexType(1) << (sizeof(IndexType)*8-1));
            }

            /**
             * @brief 检查一个Id是否是由自身分配的
             * @details Is的倒数第二位为1表示由自身分配，0表示由其他方式分配
             */
            static inline bool IsIdSelfAssigned(IndexType Id){
                return Id&(IndexType(1) << (sizeof(IndexType)*8-2));
            }


            static inline void SetIdSelfAssigned(IndexType& Id){
                Id|=(IndexType(1) << (sizeof(IndexType)*8-2));
            }

            /**
             * @brief 将ID的倒数第二位设置为0，表示未自分配
             */
            static inline void SetIdNotSelfAssigned(IndexType& Id){
                Id&=~(IndexType(1) << (sizeof(IndexType)*8-2));
            }

            /**
             * @brief 直接设置ID，不进行任何检查
             */
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
            /**
             * @brief 单元编号
             */
            IndexType mId;

            /**
             * @brief 单元数据
             * @details 包含维度信息，积分点形函数值，形函数梯度等信息
             */
            const GeometryData* mpGeometryData;

            /**
             * @brief 默认维度信息
             * @details 用于构建GeometryData实例
             */
            static const GeometryDimension msGeometryDimension;

            /**
             * @brief 存储单元节点指针的数组
             * @details PointerVector<TPointType> mPoints;
             */
            PointsArrayType mPoints;

            /**
             * @brief 存储变量（与时间步无关）数据的容器
             */
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