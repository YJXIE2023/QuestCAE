#ifndef QUEST_GEOMETRICAL_OBJECT_HPP
#define QUEST_GEOMETRICAL_OBJECT_HPP

// 系统头文件
#include <atomic>

// 系统头文件
#include "includes/define.hpp"
#include "includes/node.hpp"
#include "container/flags.hpp"
#include "geometries/geometry.hpp"
#include "includes/indexed_object.hpp"

namespace Quest{

    /**
     * @class GeometricalObject
     * @brief 定义几何对象，是元素和条件实体的基础累
     * @details 继承自 IndexedObject，因此具有唯一的ID，同事也继承自 Flags,用于管理相关标志位
     */
    class QUEST_API(QUEST_CORE) GeometricalObject: public IndexedObject, public Flags{
        public:
            QUEST_CLASS_INTRUSIVE_POINTER_DEFINITION(GeometricalObject);

            using NodeType = Node;
            using GeometryType = Geometry<NodeType>;
            using IndexType = std::size_t;
            using result_type = std::size_t;

        public:
            /**
             * @brief 构造函数
             * @param NewId 对象的ID
             */
            explicit GeometricalObject(IndexType NewId = 0):
                IndexedObject(NewId),
                Flags(),
                mpGeometry(nullptr){
            }

            /**
             * @brief 构造函数
             * @param NewId 对象的ID
             * @param pGeometry 几何对象
             */
            GeometricalObject(IndexType NewId, GeometryType::Pointer pGeometry):
                IndexedObject(NewId),
                Flags(),
                mpGeometry(pGeometry){
            }

            /**
             * @brief 析构函数
             */
            ~GeometricalObject() override {}

            /**
             * @brief 复制构造函数
             */
            GeometricalObject(const GeometricalObject& rOther):
                IndexedObject(rOther.Id()),
                Flags(rOther),
                mpGeometry(rOther.mpGeometry){
            }

            /**
             * @brief 赋值运算符
             */
            GeometricalObject& operator = (const GeometricalObject& rOther){
                IndexedObject::operator=(rOther);
                Flags::operator=(rOther);
                return *this;
            }

            /**
             * @brief 设置具体的几何类
             */
            virtual void SetGeometry(GeometryType::Pointer pGeometry){
                mpGeometry = pGeometry;
            }

            /**
             * @brief 获取几何类指针
             */
            GeometryType::Pointer pGetGeometry(){
                return mpGeometry;
            }

            /**
             * @brief 获取几何类指针
             */
            const GeometryType::Pointer pGetGeometry() const{
                return mpGeometry;
            }

            /**
             * @brief 获取几何类
             */
            GeometryType& GetGeometry(){
                return *mpGeometry;
            }

            /**
             * @brief 获取几何类
             */
            const GeometryType& GetGeometry() const{
                return *mpGeometry;
            }

            /**
             * @brief 获取对象的标志位
             */
            Flags& GetFlags(){
                return *this;
            }

            /**
             * @brief 获取对象的标志位
             */
            const Flags& GetFlags() const{
                return *this;
            }

            /**
             * @brief 设置对象的标志位
             */
            void SetFlags(const Flags& rThisFlags){
                Flags::operator=(rThisFlags);
            }

            /**
             * @brief 返回几何对象的变量数据容器（与时间步无关）
             */
            DataValueContainer& GetData(){
                return pGetGeometry()->GetData();
            }

            /**
             * @brief 返回几何对象的变量数据容器（与时间步无关）
             */
            const DataValueContainer& GetData() const{
                return pGetGeometry()->GetData();
            }

            /**
             * @brief 设置几何对象的变量数据容器（与时间步无关）
             */
            void SetData(const DataValueContainer& rThisData){
                return GetGeometry().SetData(rThisData);
            }

            /**
             * @brief 判断几何的变量数据容器中是否有指定变量
             */
            template<typename TDataType>
            bool Has(const Variable<TDataType>& rThisVariable) const{
                return GetData().Has(rThisVariable);
            }

            /**
             * @brief 设置几何的变量数据容器中指定变量的值
             */
            template<typename TVariableType>
            void SetValue(const TVariableType& rThisVariable, const typename TVariableType::Type& rValue){
                GetData().SetValue(rThisVariable, rValue);
            }

            /**
             * @brief 获取几何的变量数据容器中指定变量的值
             */
            template<typename TVariableType>
            typename TVariableType::Type& GetValue(const TVariableType& rThisVariable){
                return GetData().GetValue(rThisVariable);
            }

            /**
             * @brief 获取几何的变量数据容器中指定变量的值
             */
            template<typename TVariableType>
            const typename TVariableType::Type& GetValue(const TVariableType& rThisVariable) const{
                return GetData().GetValue(rThisVariable);
            }

            /**
             * @brief 判断几何对象是否处于激活状态
             */
            bool IsActive() const;

            /**
             * @brief 判断两个几何对象是否为同一类型
             */
            inline static bool HasSameType(const GeometricalObject& rLHS, const GeometricalObject& rRHS){
                return typeid(rLHS) == typeid(rRHS);
            }

            /**
             * @brief 判断两个几何对象是否为同一类型
             */
            inline static bool HasSameType(const GeometricalObject* pLHS, const GeometricalObject* pRHS){
                return GeometricalObject::HasSameType(*pLHS, *pRHS);
            }

            /**
             * @brief 判断两个几何对象是否有相同的几何类型
             */
            inline static bool HasSameGeometryType(const GeometricalObject& rLHS, const GeometricalObject& rRHS){
                return (rLHS.GetGeometry().GetGeometryType() == rRHS.GetGeometry().GetGeometryType());
            }

            /**
             * @brief 判断两个几何对象是否有相同的几何类型
             */
            inline static bool HasSameGeometryType(const GeometricalObject* pLHS, const GeometricalObject* pRHS){
                return GeometricalObject::HasSameGeometryType(*pLHS, *pRHS);
            }

            /**
             * @brief 判断两个几何对象是否相同
             */
            inline static bool IsSame(const GeometricalObject& rLHS, const GeometricalObject& rRHS){
                return GeometricalObject::HasSameType(rLHS, rRHS) && GeometricalObject::HasSameGeometryType(rLHS, rRHS);
            }

            /**
             * @brief 判断两个几何对象是否相同
             */
            inline static bool IsSame(const GeometricalObject* pLHS, const GeometricalObject* pRHS){
                return GeometricalObject::HasSameType(*pLHS, *pRHS) && GeometricalObject::HasSameGeometryType(*pLHS, *pRHS);
            }


            std::string Info() const override{
                std::stringstream buffer;
                buffer << "Geometrical object # " << Id();
                return buffer.str();
            }


            void PrintInfo(std::ostream& rOstream) const override{
                rOstream << this->Info();
            }


            void PrintData(std::ostream& rOstream) const override{}


            unsigned int use_count() const noexcept{
                return mReferenceCounter;
            }

        protected:

        private:
            mutable std::atomic<int> mReferenceCounter{0};


            friend void intrusive_ptr_add_ref(const GeometricalObject* x){
                x->mReferenceCounter.fetch_add(1, std::memory_order_relaxed);
            }


            friend void intrusive_ptr_release(const GeometricalObject* x){
                if(x->mReferenceCounter.fetch_sub(1, std::memory_order_release) == 1){
                    std::atomic_thread_fence(std::memory_order_acquire);
                    delete x;
                }
            }


            friend class Serializer;


            void save(Serializer& rSerializer) const override{
                QUEST_SERIALIZE_SAVE_BASE_CLASS(rSerializer, IndexedObject);
                QUEST_SERIALIZE_SAVE_BASE_CLASS(rSerializer, Flags);
                rSerializer.save("Geometry", mpGeometry);
            }


            void load(Serializer& rSerializer) override{
                QUEST_SERIALIZE_LOAD_BASE_CLASS(rSerializer, IndexedObject);
                QUEST_SERIALIZE_LOAD_BASE_CLASS(rSerializer, Flags);
                rSerializer.load("Geometry", mpGeometry);
            }

        private:
            /**
             * @brief 具体的几何类型
             * @details Geometry<NodeType>
             */
            GeometryType::Pointer mpGeometry;

    };

    inline std::istream& operator >> (std::istream& rIstream, GeometricalObject& rThis);

    inline std::ostream& operator << (std::ostream& rOstream, const GeometricalObject& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }

} // namespace Quest

#endif //QUEST_GEOMETRICAL_OBJECT_HPP