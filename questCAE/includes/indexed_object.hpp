#ifndef QUEST_INDEXED_OBJECT_HPP
#define QUEST_INDEXED_OBJECT_HPP

// 系统头文件
#include <iostream>

// 项目头文件
#include "includes/define.hpp"
#include "includes/serializer.hpp"

namespace Quest{
    /**
     * @class IndexedObject
     * @brief 具有索引的随想
     * @details 可被用作任何需要被索引的类的基类
     */
    class IndexedObject{
        public:
            QUEST_CLASS_POINTER_DEFINITION(IndexedObject);

            using IndexType = std::size_t;
            using result_size = std::size_t;


            explicit IndexedObject(IndexType NewId = 0): mId(NewId){};

            virtual ~IndexedObject() {}

            IndexedObject(const IndexedObject& rOther): mId(rOther.mId){};

            IndexedObject& operator=(const IndexedObject& rOther){
                mId = rOther.mId;
                return *this;
            }

            /**
             * @brief 函数调用运算符重载
             * @details 返回对象的索引号
             * @param rObject 对象
             */
            template<typename TObjectType>
            IndexType operator() (const TObjectType& rObject) const{
                return rObject.Id();
            }

            /**
             * @brief 获取此对象的索引号
             */
            IndexType Id() const{
                return mId;
            }

            /**
             * @brief 获取此对象的索引号
             */
            IndexType GetId() const{
                return mId;
            }

            /**
             * @brief 设置此对象的索引号
             * @param NewId 新的索引号
             */
            virtual void SetId(IndexType NewId){
                mId = NewId;
            }

            virtual std::string Info() const{
                std::stringstream buffer;
                buffer << "indexed object # " << mId;
                return buffer.str();
            }

            virtual void PrintInfo(std::ostream& rOstream) const{
                rOstream << Info();
            }

            virtual void PrintData(std::ostream& rOstream) const{}

        protected:

        private:
            friend class Serializer;

            virtual void save(Serializer& rSerializer) const{
                rSerializer.save("Id", mId);
            }

            virtual void load(Serializer& rSerializer){
                rSerializer.load("Id", mId);
            }

        private:
            /**
             * @brief 索引号
             */
            IndexType mId;

    };

    inline std::istream& operator >> (std::istream& rIstream, IndexedObject& rThis);

    inline std::ostream& operator << (std::ostream& rOstream, const IndexedObject& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }

} // namespace Quest

#endif //QUEST_INDEXED_OBJECT_HPP