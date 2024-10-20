/*------------------------------------
为需要索引的对象提供通用的基础功能
------------------------------------*/

#ifndef QUEST_INDEXED_OBJECT_HPP
#define QUEST_INDEXED_OBJECT_HPP

// 系统头文件
#include <iostream>

// 项目头文件
#include "includes/define.hpp"
#include "includes/serializer.hpp"

namespace Quest{

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

            template<typename TObjectType>
            IndexType operator() (const TObjectType& rObject) const{
                return rObject.Id();
            }

            IndexType Id() const{
                return mId;
            }

            IndexType GetId() const{
                return mId;
            }

            virtual void SetId(IndexType NewId){
                mId = NewId;
            }

            IndexType& DepricatedIdAccess(){
                return mId;
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