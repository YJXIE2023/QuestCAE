/*-------------------------------------------
用于定义几何对象
作为元素（element）和条件（condition）实体的基类
-------------------------------------------*/

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

    class QUEST_API(QUEST_CORE) GeometricalObject: public IndexedObject, public Flags{
        public:
            QUEST_CLASS_INTRUSIVE_POINTER_DEFINITION(GeometricalObject);

            using NodeType = Node;
            using GeometryType = Geometry<NodeType>;
            using IndexType = std::size_t;
            using result_type = std::size_t;

        public:
            explicit GeometricalObject(IndexType NewId = 0):
                IndexedObject(NewId),
                Flags(),
                mpGeometry(nullptr){
            }


            GeometricalObject(IndexType NewId, GeometryType::Pointer pGeometry):
                IndexedObject(NewId),
                Flags(),
                mpGeometry(pGeometry){
            }


            ~GeometricalObject() override {}


            GeometricalObject(const GeometricalObject& rOther):
                IndexedObject(rOther.Id()),
                Flags(rOther),
                mpGeometry(rOther.mpGeometry){
            }


            GeometricalObject& operator = (const GeometricalObject& rOther){
                IndexedObject::operator=(rOther);
                Flags::operator=(rOther);
                return *this;
            }


            virtual void SetGeometry(GeometryType::Pointer pGeometry){
                mpGeometry = pGeometry;
            }


            GeometryType::Pointer pGetGeometry(){
                return mpGeometry;
            }


            const GeometryType::Pointer pGetGeometry() const{
                return mpGeometry;
            }


            GeometryType& GetGeometry(){
                return *mpGeometry;
            }


            const GeometryType& GetGeometry() const{
                return *mpGeometry;
            }


            Flags& GetFlags(){
                return *this;
            }


            const Flags& GetFlags() const{
                return *this;
            }


            void SetFlags(const Flags& rThisFlags){
                Flags::operator=(rThisFlags);
            }


            DataValueContainer& GetData(){
                return pGetGeometry()->GetData();
            }


            const DataValueContainer& GetData() const{
                return pGetGeometry()->GetData();
            }


            void SetData(const DataValueContainer& rThisData){
                return GetGeometry().SetData(rThisData);
            }


            template<typename TDataType>
            bool Has(const Variable<TDataType>& rThisVariable) const{
                return GetData().Has(rThisVariable);
            }


            template<typename TVariableType>
            void SetValue(const TVariableType& rThisVariable, const typename TVariableType::Type& rValue){
                GetData().SetValue(rThisVariable, rValue);
            }


            template<typename TVariableType>
            typename TVariableType::Type& GetValue(const TVariableType& rThisVariable){
                return GetData().GetValue(rThisVariable);
            }


            template<typename TVariableType>
            const typename TVariableType::Type& GetValue(const TVariableType& rThisVariable) const{
                return GetData().GetValue(rThisVariable);
            }


            bool IsActive() const;


            inline static bool HasSameType(const GeometricalObject& rLHS, const GeometricalObject& rRHS){
                return typeid(rLHS) == typeid(rRHS);
            }


            inline static bool HasSameType(const GeometricalObject* pLHS, const GeometricalObject* pRHS){
                return GeometricalObject::HasSameType(*pLHS, *pRHS);
            }


            inline static bool HasSameGeometryType(const GeometricalObject& rLHS, const GeometricalObject& rRHS){
                return (rLHS.GetGeometry().GetGeometryType() == rRHS.GetGeometry().GetGeometryType());
            }


            inline static bool HasSameGeometryType(const GeometricalObject* pLHS, const GeometricalObject* pRHS){
                return GeometricalObject::HasSameGeometryType(*pLHS, *pRHS);
            }


            inline static bool IsSame(const GeometricalObject& rLHS, const GeometricalObject& rRHS){
                return GeometricalObject::HasSameType(rLHS, rRHS) && GeometricalObject::HasSameGeometryType(rLHS, rRHS);
            }


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