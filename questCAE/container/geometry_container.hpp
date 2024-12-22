/*--------------------------------------------
用来存储和管理特定几何类型的集合
以便对几何元素进行组织、管理和操作
--------------------------------------------*/

#ifndef QUEST_GEOMETRY_CONTAINER_HPP
#define QUEST_GEOMETRY_CONTAINER_HPP

// 项目头文件
#include "includes/define.hpp"
#include "container/pointer_hash_map_set.hpp"

namespace Quest{

    /**
     * @class GeometryContainer
     * @brief 用来存储和管理特定几何类型的集合，以便对几何元素进行组织、管理和操作
     * @tparam TGeometryType 几何类型
     */
    template<typename TGeometryType>
    class GeometryContainer{
        private:
            class GetGeometryId{
                public:
                    const std::size_t& operator()(const TGeometryType& rGeometry) const{
                        return rGeometry.Id();
                    } 
            };

        public:
            QUEST_CLASS_POINTER_DEFINITION(GeometryContainer);

            using IndexType = std::size_t;
            using SizeType = std::size_t;
            using GeometryPointerType = typename TGeometryType::Pointer;

            using GeometriesMapType = PointerHashMapSet<TGeometryType, std::hash<std::size_t>, GetGeometryId, GeometryPointerType>;
            using GeometryIterator = typename GeometriesMapType::iterator;
            using GeometryConstantIterator = typename GeometriesMapType::const_iterator;

        public:
            /**
             * @brief 构造函数
             */
            GeometryContainer():
                mGeometries()
            {}

            /**
             * @brief 拷贝构造函数
             */
            GeometryContainer(const GeometryContainer& rOther):
                mGeometries(rOther.mGeometries)
            {}

            /**
             * @brief 构造函数
             */
            GeometryContainer(GeometriesMapType& NewGeometries):
                mGeometries(NewGeometries)
            {}

            /**
             * @brief 析构函数
             */
            ~GeometryContainer() = default;

            /**
             * @brief 克隆函数, 返回一个新的GeometryContainer
             */
            GeometryContainer Clone(){
                typename GeometriesMapType::Pointer p_geometries(new GeometriesMapType(*mGeometries));

                return GeometryContainer(p_geometries);
            }

            /**
             * @brief 清空容器
             */
            void Clear(){
                mGeometries.clear();
            }

            /**
             * @brief 获取容器中几何元素的数量
             */
            SizeType NumberOfGeometries() const{
                return mGeometries.size();
            }

            /**
             * @brief 添加一个几何元素到容器中
             */
            GeometryIterator AddGeometry(GeometryPointerType pNewGeometry){
                auto i = mGeometries.find(pNewGeometry->Id());
                if(i == mGeometries.end()){
                    return mGeometries.insert(pNewGeometry);
                } else if (&(*i) == pNewGeometry.get()){
                    return i;
                } else {
                    QUEST_ERROR << "Attempting to add Geometry with Id: " << pNewGeometry->Id() << ", unfortunately a (different) Geometry with the same Id already exists." << std::endl;
                }
            }

            /**
             * @brief 根据Id获取一个几何元素的指针
             */
            GeometryPointerType pGetGeometry(IndexType GeometryId){
                auto i = mGeometries.find(GeometryId);
                QUEST_ERROR_IF(i == mGeometries.end()) << " geometry index not found: " << GeometryId << ".";
                return (i.base()->second);
            }

            /**
             * @brief 根据Id获取一个几何元素的指针
             */
            const GeometryPointerType pGetGeometry(IndexType GeometryId) const{
                auto i = mGeometries.find(GeometryId);
                QUEST_ERROR_IF(i == mGeometries.end()) << " geometry index not found: " << GeometryId << ".";
                return (i.base()->second);
            }

            /**
             * @brief 根据几何名称获取一个几何元素的指针
             */
            GeometryPointerType pGetGeometry(std::string GeometryName){
                auto hash_index = TGeometryType::GenerateId(GeometryName);
                auto i = mGeometries.find(hash_index);
                QUEST_ERROR_IF(i == mGeometries.end()) << " geometry name not found: " << GeometryName << ".";
                return (i.base()->second);
            }

            /**
             * @brief 根据几何名称获取一个几何元素的指针
             */
            const GeometryPointerType pGetGeometry(std::string GeometryName) const{ 
                auto hash_index = TGeometryType::GenerateId(GeometryName);
                auto i = mGeometries.find(hash_index);  
                QUEST_ERROR_IF(i == mGeometries.end()) << " geometry name not found: " << GeometryName << ".";
                return (i.base()->second);
            }

            /**
             * @brief 根据Id获取一个几何元素的引用
             */
            TGeometryType& GetGeometry(IndexType GeometryId){
                return *pGetGeometry(GeometryId);
            }

            /**
             * @brief 根据Id获取一个几何元素的引用
             */
            const TGeometryType& GetGeometry(IndexType GeometryId) const{
                return *pGetGeometry(GeometryId);
            }

            /**
             * @brief 根据几何名称获取一个几何元素的引用
             */
            TGeometryType& GetGeometry(std::string GeometryName){
                return *pGetGeometry(GeometryName);
            }

            /**
             * @brief 根据几何名称获取一个几何元素的引用
             */
            const TGeometryType& GetGeometry(std::string GeometryName) const{
                return *pGetGeometry(GeometryName);
            }

            /**
             * @brief 根据Id移除一个几何元素
             */
            void RemoveGeometry(IndexType GeometryId){
                mGeometries.erase(GeometryId);
            }

            /**
             * @brief 根据几何名称移除一个几何元素
             */
            void RemoveGeometry(std::string GeometryName){
                auto hash_index = TGeometryType::GenerateId(GeometryName);
                mGeometries.erase(hash_index);
            }

            /**
             * @brief 判断容器中是否有某个Id的几何元素
             */
            bool HasGeometry(IndexType GeometryId) const{
                return mGeometries.find(GeometryId) != mGeometries.end();
            }

            /**
             * @brief 判断容器中是否有某个几何名称的几何元素
             */
            bool HasGeometry(std::string GeometryName) const{
                auto hash_index = TGeometryType::GenerateId(GeometryName);
                return mGeometries.find(hash_index) != mGeometries.end();
            }

            /**
             * @brief 获取容器中第一个几何元素的容器自定义迭代器
             */
            GeometryIterator GeometriesBegin(){
                return mGeometries.begin();
            }

            /**
             * @brief 获取容器中第一个几何元素的容器自定义迭代器
             */
            GeometryConstantIterator GeometriesBegin() const{
                return mGeometries.begin();
            }

            /**
             * @brief 获取容器中最后一个几何元素的容器自定义迭代器
             */
            GeometryIterator GeometriesEnd(){
                return mGeometries.end();
            }

            /**
             * @brief 获取容器中最后一个几何元素的容器自定义迭代器
             */
            GeometryConstantIterator GeometriesEnd() const{
                return mGeometries.end();
            }

            /**
             * @brief 返回容器对象
             */
            GeometriesMapType& Geometries(){
                return mGeometries;
            }

            /**
             * @brief 返回容器对象
             */
            const GeometriesMapType& Geometries() const{
                return mGeometries;
            }


            std::string Info() const{
                std::stringstream buffer;
                buffer << "GeometryContainer";
                return buffer.str();
            }


            void PrintInfo(std::ostream& rOstream) const{
                rOstream << Info();
            }


            void PrintData(std::ostream& rOstream) const{
                rOstream << "Number of Geometries: " << mGeometries.size() << std::endl;
            }


            virtual void PrintInfo(std::ostream& rOstream, const std::string& PrefixString) const{
                rOstream << PrefixString << Info();
            }


            virtual void PrintData(std::ostream& rOstream, const std::string& PrefixString) const{
                rOstream << PrefixString << "Number of Geometries: " << mGeometries.size() << std::endl;
            }

        protected:

        private:
            friend class Serializer;


            void save(Serializer& rSerializer) const{
                rSerializer.save("Geometries", mGeometries);
            }


            void load(Serializer& rSerializer){
                rSerializer.load("Geometries", mGeometries);
            }


            GeometryContainer& operator=(const GeometryContainer& rOther){
                mGeometries = rOther.mGeometries;
                return *this;
            }

        private:
            /**
             * @brief 存储和管理特定几何类型的容器
             * @details PointerHashMapSet<TGeometryType, std::hash<std::size_t>, GetGeometryId, GeometryPointerType>
             */
            GeometriesMapType mGeometries;

    };


    template<class TGeometryType>
    inline std::istream& operator >> (std::istream& rIstream, GeometryContainer<TGeometryType>& rThis);


    template<class TGeometryType>
    inline std::ostream& operator << (std::ostream& rOstream, const GeometryContainer<TGeometryType>& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }

} // namespace Quest

#endif //QUEST_GEOMETRY_CONTAINER_HPP