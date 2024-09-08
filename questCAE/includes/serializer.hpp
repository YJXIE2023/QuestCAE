/*----------------------------------
提供序列化和反序列化的接口
------------------------------------*/

#ifndef SERIALIZER_HPP
#define SERIALIZER_HPP

// 系统头文件
#include <map>
#include <set>
#include <string>
#include <cstring>
#include <sstream>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <unordered_set>

// 项目头文件
#include "includes/define.hpp"
#include "IO/logger.hpp"
#include "container/flags.hpp"
#include "container/array_1d.hpp"
#include "container/weak_pointer_vector.hpp"

// 宏定义
#define QUEST_SERIALIZATION_DIRECT_LOAD(type)                     \
    void load(const std::string& rTag, type& rValue){             \
        load_trace_point(rTag);                                   \
        read(rValue);                                             \
    }                                                             \
    void load(const std::string& rTag, const type& rValue){       \
        load_trace_point(rTag);                                   \
        read(const_cast<type&>(rValue));                          \
    }                                                             \
    void load_base(const std::string& rTag, type& rValue){        \
        load_trace_point(rTag);                                   \
        read(rValue);                                             \
    }                                                             

#define QUEST_SERIALIZATION_DIRECT_SAVE(type)                     \
    void save(const std::string& rTag, const type& rValue){       \
        save_trace_point(rTag);                                   \
        write(rValue);                                            \
    }                                                             \
    void save_base(const std::string& rTag, const type& rValue){  \
        save_trace_point(rTag);                                   \
        write(rValue);                                            \
    }                                       

#define QUEST_SERIALIZATION_DIRECT_CREATE(type)                   \
    void* creat(const std::string& rTag, type* prototype){        \
        type* p_new = new type;                                   \
        load(rTag, *p_new);                                       \
        return p_new;                                             \
    }                                                             

#define QUEST_SERIALIZER_MODE_BINARY                              \
    if(!mTrace) {                                                 

#define QUEST_SERIALIZER_MODE_ASCII                                \
    } else {

#define QUEST_SERIALIZER_MODE_END                                 \
    }


namespace Quest{
    
    class ModelPart;
    class VariableData;
    template<typename TDataType> class Variable;

    class QUEST_API Serializer{
        public:
            enum PointerType{SP_INVALID_POINTER, SP_BASE_CLASS_POINTER, SP_DERIVED_CLASS_POINTER};
            enum TraceType{SERIALIZER_NO_TRACE=0, SERIALIZER_TRACE_ERROR=1, SERIALIZER_TRACE_ALL=2};

            QUEST_CLASS_POINTER_DEFINITION(Serializer);

            QUEST_DEFINE_LOCAL_FLAG( MPI );
            QUEST_DEFINE_LOCAL_FLAG( SHALLOW_GLOBAL_POINTER_SERIALIZATION );

            typedef std::size_t SizeType;
            typedef void* (*ObjectFactoryType)();
            typedef std::map<void*, void*> LoadedPointerContainerType;
            typedef std::map<std::string, ObjectFactoryType> RegisteredObjectsContainerType;
            typedef std::map<std::string, std::string> RegisteredObjectsNameContianerType;
            typedef std::set<const void*> SavedPointerContainerType;
            typedef std::iostream BufferType;

            explicit Serializer(BufferType*pBuffer,const TraceType& rTrace=SERIALIZER_NO_TRACE):
                mpBuffer(pBuffer),mTrace(rTrace),mNumberOfLines(0){}

            virtual ~Serializer(){
                delete mpBuffer;
            }

            void SetLoadState();

            TraceType GetTraceType() const{
                return mTrace;
            }

            void SetBuffer(BufferType* pBuffer){
                mpBuffer = pBuffer;
            }

            template<typename TDataType>
            static void* Create(){
                return new TDataType;
            }

            template<typename TDataType>
            static void Register(const std::string& rName,const TDataType& pPrototype){
                msRegisteredObjects.insert(RegisteredObjectsContainerType::value_type(rName,Create<TDataType>));
                msRegisteredObjectsName.insert(RegisteredObjectsNameContianerType::value_type(typeid(TDataType).name(),rName));
            }

            template<typename TDataType>
            void load(const std::string& rTag, TDataType& rObject){
                load_trace_point(rTag);
                rObject.load(*this);
            }

            template<typename TDataType>
            void load(const std::string& rTag, Quest::shared_ptr<TDataType>& pValue){
                PointerType pointer_type = SP_INVALID_POINTER;
                void* p_pointer;
                read(pointer_type);

                if(pointer_type != SP_INVALID_POINTER){
                    read(p_pointer);
                    LoadedPointerContainerType::iterator i_pointer = mLoadedPointers.find(p_pointer);
                    if(i_pointer == mLoadedPointers.end()){
                        if(pointer_type == SP_BASE_CLASS_POINTER){
                            if(!pValue){
                                pVlaue = Quest::shared_ptr<TDataType>(new TDataType);
                            }
                        } else if(pointer_type == SP_DERIVED_CLASS_POINTER){
                            std::string object_name;
                            read(object_name);
                            typename RegisteredObjectsContainerType::iterator i_prototype = msRegisteredObjects.find(object_name);

                            QUEST_ERROR_IF(i_prototype == msRegisteredObjects.end())
                                <<"There is no object registered in Quest with name : "
                                << object_name << std::endl;

                            if(!pValue){
                                pValue = Quest::shared_ptr<TDataType>(static_cast<TDataType*>((i_prototype->second)()));
                            }
                        }

                        mLoadedPointers[p_pointer] = &pValue;
                        load(rTag,*pValue);
                    } else {
                        pValue = *static_cast<Quest::shared_ptr<TDataType>*>(i_pointer->second);
                    }
                }
            }

            template<typename TDataType>
            void load(const std::string& rTag, Quest::intrusive_ptr<TDataType>& pValue){
                PointerType pointer_type = SP_INVALID_POINTER;
                void* p_pointer;
                read(pointer_type);

                if(pointer_type != SP_INVALID_POINTER){
                    read(p_pointer);                    
                    LoadedPointerContainerType::iterator i_pointer = mLoadedPointers.find(p_pointer);
                    if(i_pointer == mLoadedPointers.end()){
                        if(pointer_type == SP_BASE_CLASS_POINTER){
                            if(!pValue){
                                pValue = Quest::intrusive_ptr<TDataType>(new TDataType);
                            }
                        } else if(pointer_type == SP_DERIVED_CLASS_POINTER){
                            std::string object_name;
                            read(object_name);
                            typename RegisteredObjectsContainerType::iterator i_prototype = msRegisteredObjects.find(object_name);

                            QUEST_ERROR_IF(i_prototype == msRegisteredObjects.end())
                                <<"There is no object registered in Quest with name : "
                                << object_name << std::endl;

                            if(!pValue){
                                pValue = Quest::intrusive_ptr<TDataType>(static_cast<TDataType*>((i_prototype->second)()));
                            }
                        }

                        mLoadedPointers[p_pointer] = &pValue;
                        load(rTag,*pValue);
                    } else {
                        pValue = *static_cast<Quest::intrusive_ptr<TDataType>*>(i_pointer->second);
                    }
                }
            }

            template<typename TDataType>
            void load(const std::string& rTag, Quest::unique_ptr<TDataType>& pValue){
                PointerType pointer_type = SP_INVALID_POINTER;
                void* p_pointer;
                read(pointer_type);

                if(pointer_type != SP_INVALID_POINTER){
                    read(p_pointer);                    
                    LoadedPointerContainerType::iterator i_pointer = mLoadedPointers.find(p_pointer);
                    if(i_pointer == mLoadedPointers.end()){
                        if(pointer_type == SP_BASE_CLASS_POINTER){
                            if(!pValue){
                                pValue = Quest::unique_ptr<TDataType>(new TDataType);
                            }
                        } else if(pointer_type == SP_DERIVED_CLASS_POINTER){
                            std::string object_name;
                            read(object_name);
                            typename RegisteredObjectsContainerType::iterator i_prototype = msRegisteredObjects.find(object_name);

                            QUEST_ERROR_IF(i_prototype == msRegisteredObjects.end())
                                <<"There is no object registered in Quest with name : "
                                << object_name << std::endl;

                            if(!pValue){
                                pValue = Quest::unique_ptr<TDataType>(static_cast<TDataType*>((i_prototype->second)()));
                            }
                        }

                        mLoadedPointers[p_pointer] = &pValue;
                        load(rTag,*pValue);
                    } else {
                        pValue = std::move(Quest::unique_ptr<TDataType>(static_cast<TDataType*>(i_pointer->second)));
                    }
                }
            }

            template<typename TDataType>
            void load(const std::string & rTag, TDataType* &pValue){
                PointerType pointer_type = SP_INVALID_POINTER;
                void* p_pointer;
                read(pointer_type);

                if(pointer_type != SP_INVALID_POINTER){
                    read(p_pointer);                    
                    LoadedPointerContainerType::iterator i_pointer = mLoadedPointers.find(p_pointer);
                    if(i_pointer == mLoadedPointers.end()){
                        if(pointer_type == SP_BASE_CLASS_POINTER){
                            if(!pValue){
                                pValue = new TDataType;
                            }
                        } else if(pointer_type == SP_DERIVED_CLASS_POINTER){
                            std::string object_name;
                            read(object_name);
                            typename RegisteredObjectsContainerType::iterator i_prototype = msRegisteredObjects.find(object_name);

                            QUEST_ERROR_IF(i_prototype == msRegisteredObjects.end())
                                <<"There is no object registered in Quest with name : "
                                << object_name << std::endl;

                            if(!pValue){
                                pValue = static_cast<TDataType*>((i_prototype->second)());
                            }
                        }

                        mLoadedPointers[p_pointer] = pValue;
                        load(rTag,*pValue);
                    } else {
                        pValue = *static_cast<TDataType**>(i_pointer->second);
                    }
                }
            }

            void load(const std::string& rTag, ModelPart*& pValue);

            void load(const std::string& rTag, Quest::unique_ptr<ModelPart>& pValue);

            void load(const std::string& rTag, Quest::shared_ptr<ModelPart>& pVlaue);

            template<typename TDataType>
            void load(const std::string& rTag, const Variable<TDataType>* pVariable){
                load_trace_point(rTag);
                std::string name;
                read(name);
            }

            template<typename TDataType, std::size_t TDataSize>
            void load(const std::string& rTag, std::array<TDataType, TDataSize>& rObject){
                load_trace_point(rTag);
                for(SizeType i=0;i<TDataSize;i++){
                    load("E",rObject[i]);
                }
            }

            template<typename TDataType>
            void load(const std::string& rTag, std::vector<TDataType>& rObject){
                load_trace_point(rTag);
                SizeType size;

                load("size",size);

                rObject.resize(size);
                for(SizeType i=0; i<size; i++)
                    load("E",rObject[i]);
            }

            template<typename TDataType>
            void load(const std::string& rTag, DenseVector<TDataType>& rObject){
                load_trace_point(rTag);
                SizeType size;

                load("size",size);

                rObject.resize(size,false);

                for(SizeType i=0; i<size; i++)
                    load("E",rObject[i]);
            }
            
            template<typename TKeyType, typename TDataType>
            void load(const std::string& rTag, std::map<TKeyType, TDataType>& rObject){
                load_associative_container(rTag, rObject);
            }

            template<typename TKeyType, typename TDataType>
            void load(const std::string& rTag, std::unordered_map<TKeyType, TDataType>& rObject){
                load_associative_container(rTag, rObject);
            }

            template<typename TDataType>
            void load(const std::string& rTag, std::set<TDataType>& rObject){
                load_associative_container(rTag, rObject);
            }

            template<typename TDataType>
            void load(const std::string& rTag, std::unordered_set<TDataType>& rObject){
                load_associative_container(rTag, rObject);
            }

            template<typename TDataType, std::size_t TDimension>
            void load(const std::string& rTag, Array1d<TDataType,TDimension>& rObject){
                load_trace_point(rTag);
                for(SizeType i=0;i<TDimension;i++)
                    load("E",rObject[i]);
            }

            template<typename TFirstType,typename TSecondType>
            void load(const std::string& rTag, std::pair<TFirstType,TSecondType>& rObject){
                load_trace_point(rTag);
                load("first",rObject.first);
                load("second",rObject.second);
            }

            template<typename TDataType, std::size_t TDimension>
            void load(const std::string& rTag, BoundedVector<TDataType,TDimension>& rObject){
                load_trace_point(rTag);
                for(SizeType i=0;i<TDimension;i++)
                    load("E",rObject[i]);
            }

            template<typename TDataType, std::size_t TDimension1, std::size_t TDimension2>
            void load(const std::string& rTag, BoundedMatrix<TDataType,TDimension1,TDimension2>& rObject){
                load_trace_point(rTag);
                for(SizeType i=0;i<TDimension1;i++){
                    for(SizeType j=0;j<TDimension2;j++){
                        load("E",rObject(i,j));
                    }
                }
            }

            QUEST_SERIALIZATION_DIRECT_LOAD(bool)
            QUEST_SERIALIZATION_DIRECT_LOAD(int)
            QUEST_SERIALIZATION_DIRECT_LOAD(long)
            QUEST_SERIALIZATION_DIRECT_LOAD(double)
            QUEST_SERIALIZATION_DIRECT_LOAD(unsigned long)
            QUEST_SERIALIZATION_DIRECT_LOAD(unsigned int)
            QUEST_SERIALIZATION_DIRECT_LOAD(std::string)
            QUEST_SERIALIZATION_DIRECT_LOAD(Matrix)
            QUEST_SERIALIZATION_DIRECT_LOAD(long long)

            #ifdef __WIN64
                QUEST_SERIALIZATION_DIRECT_LOAD(std::size_t)
            #endif
            QUEST_SERIALIZATION_DIRECT_LOAD(std::complex<double>)

            template<typename TDataType, std::size_t TDataSize>
            void save(const std::string& rTag, const std::array<TDataType, TDataSize>& rObject){
                save_trace_point(rTag);
                for(SizeType i=0;i<TDataSize;i++)
                    save("E",rObject[i]);
            }

            template<typename TDateType>
            void save(const std::string& rTag, const std::vector<TDateType>& rObject){
                save_trace_point(rTag);
                SizeType size = rObject.size();
                save("size",size);

                using SaveType = std::conditional_t<std::is_same_v<typename std::decay<TDataType>::type, bool>,bool,const TDataType&>;

                for(SizeType i=0; i<size; i++)
                    save("E",SaveType(rObject[i]));
            }

            template<typename TDataType>
            void save(const std::string& rTag, const DenseVector<TDataType>& rObject){
                save_trace_point(rTag);
                SizeType size = rObject.size();
                save("size",size);

                for(SizeType i=0; i<size; i++)
                    save("E",rObject[i]);
            }

            template<typename TDataType, std::size_t TDimension>
            void save(const std::string& rTag, const Array1d<TDataType,TDimension>& rObject){
                save_trace_point(rTag);
                for(SizeType i=0;i<TDimension;i++)
                    save("E",rObject[i]);
            }

            template<typename TKeyType, typename TDataType>
            void save(const std::string& rTag, const std::map<TKeyType, TDataType>& rObject){
                save_associative_container(rTag, rObject);
            }

            template<typename TKeyType, typename TDataType>
            void save(const std::string& rTag, const std::unordered_map<TKeyType, TDataType>& rObject){
                save_associative_container(rTag, rObject);
            }

            template<typename TDataType>
            void save(const std::string& rTag, const std::set<TDataType>& rObject){
                save_associative_container(rTag, rObject);
            }   

            template<typename TDataType>
            void save(const std::string& rTag, const std::unordered_set<TDataType>& rObject){
                save_associative_container(rTag, rObject);
            }

            template<typename TDataType>
            void save(const std::string& rTag, const TDataType& rObject){
                save_trace_point(rTag);
                rObject.save(*this);
            }

            template<typename TDataType>
            void save(const std::string& rTag, const Variable<TDataType>* pVariable){
                save_trace_point(rTag);
                write(pVariable->Name());
            }

            template<typename TDataType>
            void save(const std::string& rTag, Quest::shared_ptr<TDataType>& pValue){
                save(rTag,pValue.get());
            }

            template<typename TDataType>
            void save(const std::string& rTag, Quest::intrusive_ptr<TDataType>& pValue){
                save(rTag,pValue.get());
            }

            template<typename TDataType>
            void save(const std::string& rTag, Quest::unique_ptr<TDataType>& pValue){
                save(rTag,pValue.get());
            }

            template<typename TDataType>
            void save(const std::string& rTag, const TDataType* pValue){
                if(pValue){
                    if(IsDerived(pValue))
                        write(SP_DERIVED_CLASS_POINTER);
                    else
                        write(SP_BASE_CLASS_POINTER);

                    SavePointer(rTag,pValue);
                } else {
                    write(SP_INVALID_POINTER);
                }
            }

            template<typename TDataType>
            bool IsDerived(TDataType* pValue){
                if(strcmp(typeid(TDataType).name(),typeid(*pValue).name())!=0)
                    return true;
                else
                    return false;
            }

            template<typename TDataType>
            void save(const std::string& rTag, TDataType* pValue){
                if(pValue){
                    if(IsDerived(pValue))
                        write(SP_DERIVED_CLASS_POINTER);
                    else
                        write(SP_BASE_CLASS_POINTER);

                    SavePointer(rTag,pValue);
                } else {
                    write(SP_INVALID_POINTER);
                }
            }

            template<typename TDataType>
            void save(const std::string& rTag, Quest::shared_ptr<const TDataType> pValue){
                save(rTag,pValue.get());
            }

            void save(const std::string& rTag, const char* pValue){
                save_trace_point(rTag);
                write(std::string(pValue));
            }

            template<typename TFirstType,typename TSecondType>
            void save(const std::string& rTag, const std::pair<TFirstType,TSecondType>& rObject){
                save_trace_point(rTag);
                save("first",rObject.first);
                save("second",rObject.second);
            }

            template<typename TDataType, std::size_t TDimension>
            void save(const std::string& rTag, const BoundedVector<TDataType,TDimension>& rObject){
                save_trace_point(rTag);
                for(SizeType i=0;i<TDimension;i++)
                    save("E",rObject[i]);
            }

            template<typename TDataType, std::size_t TDimension1, std::size_t TDimension2>
            void save(const std::string& rTag, const BoundedMatrix<TDataType,TDimension1,TDimension2>& rObject){
                save_trace_point(rTag);
                for(SizeType i=0;i<TDimension1;i++){
                    for(SizeType j=0;j<TDimension2;j++){
                        save("E",rObject(i,j));
                    }
                }
            }

            QUEST_SERIALIZATION_DIRECT_SAVE(bool)
            QUEST_SERIALIZATION_DIRECT_SAVE(int)
            QUEST_SERIALIZATION_DIRECT_SAVE(long)
            QUEST_SERIALIZATION_DIRECT_SAVE(double)
            QUEST_SERIALIZATION_DIRECT_SAVE(unsigned long)
            QUEST_SERIALIZATION_DIRECT_SAVE(unsigned int)
            QUEST_SERIALIZATION_DIRECT_SAVE(std::string)
            QUEST_SERIALIZATION_DIRECT_SAVE(Matrix)
            QUEST_SERIALIZATION_DIRECT_SAVE(long long)

            #ifdef __WIN64
                QUEST_SERIALIZATION_DIRECT_SAVE(std::size_t)
            #endif
            QUEST_SERIALIZATION_DIRECT_SAVE(std::complex<double>)

            template<typename TDataType>
            void load_base(const std::string& rTag,TDataType& rObject){
                load_trace_point(rTag);
                rObject.TDataType::load(*this);
            }

            template<typename TDataType>
            void load_base(const std::string& rTag,std::vector<TDataType>& rObject){
                load_trace_point(rTag);
                load(rTag,rObject);
            }

            template<typename TDataType>
            void load_base(const std::string& rTag,DenseVector<TDataType>& rObject){
                load_trace_point(rTag);
                load(rTag,rObject);
            }

            template<typename TDataType, std::size_t TDimension>
            void load_base(const std::string& rTag,Array1d<TDataType,TDimension>& rObject){
                load_trace_point(rTag);
                load(rTag,rObject);
            }

            template<typename TDataType>
            void load_base(const std::string& rTag,const TDataType& rObject){
                save_trace_point(rTag);
                save(rTag,rObject);
            }

            template<typename TDataType>
            void save_base(const std::string& rTag,const std::vector<TDataType>& rObject){
                save_trace_point(rTag);
                save(rTag,rObject);
            }

            template<typename TDataType>
            void save_base(const std::string& rTag, const DenseVector<TDataType>& rObject){
                save_trace_point(rTag);
                save(rTag,rObject);
            }

            template<typename TDataType, std::size_t TDimension>
            void save_base(const std::string& rTag,const Array1d<TDataType,TDimension>& rObject){
                save_trace_point(rTag);
                save(rTag,rObject);
            }

            void save_trace_point(const std::string& rTag){
                if (mTrace){
                    write(rTag);
                }
            }

            bool load_trace_point(const std::string& rTag){
                if(mTrace == SERIALIZER_TRACE_ERROR){
                    std::string read_tag;
                    read(read_tag);
                    if(read_tag!= rTag){
                        return true;
                    } else{
                        std::stringstream buffer;
                        buffer << "In line "<< mNumberOfLines;
                        buffer << " the trace tag is not the expected one:" << std::endl;
                        buffer << "    Tag found : " << read_tag << std::endl;
                        buffer << "    Tag given : " << rTag << std::endl;
                        QUEST_ERROR << buffer.str() << std::endl;
                    }
                } else if(mTrace == SERIALIZER_TRACE_ALL){
                    std::string read_tag;
                    read(read_tag);
                    if(read_tag!= rTag){
                        QUEST_INFO("Serializer") << "In line " << mNumberOfLines << " loading " << rTag << " as expected" << std::endl;
                        return true;
                    } else {
                        std::stringstream buffer;
                        buffer << "In line "<< mNumberOfLines;
                        buffer << " the trace tag is not the expected one:" << std::endl;
                        buffer << "    Tag found : " << read_tag << std::endl;
                        buffer << "    Tag given : " << rTag << std::endl;
                        QUEST_ERROR << buffer.str() << std::endl;
                    }
                }
                return false;
            }

            BufferType* pGetBuffer(){
                return mpBuffer;
            }

            template<typename TDataType>
            void AddToSavedPointers(const TDataType& pValue){
                mSavedPointers.insert(pValue);
            }

            void RedirectLoadingPointer(void* pStoredPointer, void* pAllocatedPosition){
                mLoadedPointers[pStoredPointer] = pAllocatedPosition;
            }

            static RegisteredObjectsContainerType& GetRegisteredObjects(){
                return msRegisteredObjects;
            }

            static RegisteredObjectsNameContianerType& GetRegisteredObjectsName(){
                return msRegisteredObjectsName;
            }

            void Set(const Flags ThisFlag){
                mFlags.Set(ThisFlag);
            }

            bool Is(const Flags& rOtherFlag) const{
                return mFlags.Is(rOtherFlag);
            }

            virtual std::string Info() const{
                return "Serializer";
            }

            virtual void PrintInfo(std::ostream& rOStream) const{}
            
            virtual void PrintData(std::ostream& rOStream) const{}

        protected:

        private:
            static RegisteredObjectsContainerType msRegisteredObjects;
            static RegisteredObjectsNameContianerType msRegisteredObjectsName;

            Flags mFlags;

            BufferType* mpBuffer;
            TraceType mTrace;
            SizeType mNumberOfLines;

            SavedPointerContainerType mSavedPointers;
            LoadedPointerContainerType mLoadedPointers;

            template<typename TDataType>
            void SavePointer(const std::string& rTag,const TDataType* pValue){
                write(pValue);
                if (mSavedPointers.find(pValue) == mSavedPointers.end()){
                    mSavedPointers.insert(pValue);
                    if (IsDerived(pValue)){
                        typename RegisteredObjectsContainerType::iterator i_name = msRegisteredObjects.find(typeid(*pValue).name());

                        if(i_name!= msRegisteredObjects.end()){
                            QUEST_ERROR << "There is no object regisered in Quest with type id : "
                                        << typeid(*pValue).name() << std::endl;
                        } else {
                            write(i_name->second);
                        }
                    }

                    save(rTag,*pValue);
                }
            }

            VariableData* GetVariableData(const std::string& VariableName){}

            template<typename TMapType>
            void load_associative_container(const std::string& rTag, TMapType& rObject){
                load_trace_point(rTag);
                SizeType size;

                load("size",size);

                for(SizeType i=0; i<size; i++){
                    typename TMapType::value_type temp;
                    load("E",temp);
                    rObject.insert(temp);
                }
            }

            template<typename TMapType> 
            void save_associative_container(const std::string& rTag, const TMapType& rObject){
                save_trace_point(rTag);
                SizeType size = rObject.size();

                save("size",size);

                for(auto& i:rObject)
                    save("E",i);
            }

            void read(PointerType& rValue){
                QUEST_SERIALIZER_MODE_BINARY

                int temp;
                mpBuffer->read((char*)(&temp),sizeof(PointerType));

                QUEST_SERIALIZER_MODE_ASCII

                int temp;
                *mpBuffer >> temp;
                rValue = (PointerType)temp;
                mNumberOfLines++;

                QUEST_SERIALIZER_MODE_END
            }

            void write(const PointerType& rValue){
                QUEST_SERIALIZER_MODE_BINARY

                int ptr = (int)rValue;
                const char* data = reinterpret_cast<const char*>(&ptr);
                mpBuffer->write(data,sizeof(PointerType));

                QUEST_SERIALIZER_MODE_ASCII

                *mpBuffer << (int)rValue << std::endl;

                QUEST_SERIALIZER_MODE_END
            }

            void read(std::string& rValue){
                QUEST_SERIALIZER_MODE_BINARY

                SizeType size;
                mpBuffer -> read((char*)(&size),sizeof(SizeType));
                rValue.resize(size);
                if (size>0){
                    mpBuffer->read(&rValue[0],size);
                }

                QUEST_SERIALIZER_MODE_ASCII

                std::getline(*mpBuffer,rValue,'\"');
                std::getline(*mpBuffer,rValue,'\"');
                mNumberOfLines++;

                QUEST_SERIALIZER_MODE_END
            }

            void write(const std::string& rValue){
                QUEST_SERIALIZER_MODE_BINARY

                const char* data = rValue.c_str();
                SizeType rDataSize = rValue.length()*sizeof(char);

                const char* data1 = reinterpret_cast<const char*>(&rDataSize);

                mpBuffer->write(data1,sizeof(SizeType));
                mpBuffer->write(data,rDataSize);

                QUEST_SERIALIZER_MODE_ASCII

                *mpBuffer << '\"' << rValue << '\"' << std::endl;

                QUEST_SERIALIZER_MODE_END
            }

            template<typename TDataType>
            void read(TDataType& rData){
                QUEST_SERIALIZER_MODE_BINARY

                mpBuffer->read((char*)(&rData),sizeof(TDataType));

                QUEST_SERIALIZER_MODE_ASCII

                *mpBuffer >> rData;
                mNumberOfLines++;

                QUEST_SERIALIZER_MODE_END
            }

            template<class TDataType>
            void write(const TDataType& rData){
                QUEST_SERIALIZER_MODE_BINARY

                const char* data = reinterpret_cast<const char*>(&rData);
                mpBuffer->write(data,sizeof(TDataType));

                QUEST_SERIALIZER_MODE_ASCII

                *mpBuffer << rData << std::endl;

                QUEST_SERIALIZER_MODE_END
            }

            template<typename TDataType>
            void read(std::vector<TDataType>& rData){
                QUEST_SERIALIZER_MODE_BINARY

                SizeType size;
                mpBuffer->read((char*)(&size),sizeof(SizeType));

                rData.resize(size);

                read(rData.begin(),rData.end(),sizeof(TDataType));

                QUEST_SERIALIZER_MODE_ASCII

                std::size_t size;
                *mpBuffer >> size;
                rData.resize(size);
                mNumberOfLines++;

                read(rData.begin(),rData.end());

                QUEST_SERIALIZER_MODE_END
            }

            template<typename TDataType>
            void write(const std::vector<TDataType>& rData){
                QUEST_SERIALIZER_MODE_BINARY

                SizeType rDatta_size = rData.size();

                const char* data = reinterpret_cast<const char*>(&rDatta_size);
                mpBuffer->write(data,sizeof(SizeType));

                write(rData.begin(),rData.end(),sizeof(TDataType));

                QUEST_SERIALIZER_MODE_ASCII

                *mpBuffer << rData.size() << std::endl;
                write(rData.begin(),rData.end());

                QUEST_SERIALIZER_MODE_END
            }

            template<typename TDataType>
            void read(DenseMatrix<TDataType>& rData){
                QUEST_SERIALIZER_MODE_BINARY

                SizeType size1;
                SizeType size2;

                mpBuffer->read((char*)(&size1),sizeof(SizeType));
                mpBuffer->read((char*)(&size2),sizeof(SizeType));

                rData.resize(size1,size2);

                read(rData.data().begin(),rData.data().end(),sizeof(TDataType));

                QUEST_SERIALIZER_MODE_ASCII

                SizeType size1;
                SizeType size2;

                *mpBuffer >> size1;
                mNumberOfLines++;
                *mpBuffer >> size2;
                mNumberOfLines++;

                rData.resize(size1,size2);
                read(rData.data().begin(),rData.data().end(),0);

                QUEST_SERIALIZER_MODE_END
            }

            template<typename TDataType>
            void write(const DenseMatrix<TDataType>& rData){
                QUEST_SERIALIZER_MODE_BINARY

                SizeType rData_size1 = rData.size1();
                SizeType rData_size2 = rData.size2();

                const char* data1 = reinterpret_cast<const char*>(&rData_size1);
                const char* data2 = reinterpret_cast<const char*>(&rData_size2);

                mpBuffer->write(data1,sizeof(SizeType));
                mpBuffer->write(data2,sizeof(SizeType));

                write(rData.data().begin(),rData.data().end(),sizeof(TDataType));

                QUEST_SERIALIZER_MODE_ASCII

                *mpBuffer << rData.size1() << std::endl;
                *mpBuffer << rData.size2() << std::endl;

                write(rData.data().begin(),rData.data().end(),0);

                QUEST_SERIALIZER_MODE_END
            }

            template<typename TIteratorType>
            void read(TIteratorType First, TIteratorType Last, SizeType Size){
                QUEST_SERIALIZER_MODE_BINARY

                for(;First!=Last;First++){
                    mpBuffer->read((char*)First,sizeof(size));
                }

                QUEST_SERIALIZER_MODE_ASCII

                for(;First!=Last;First++){
                    *mpBuffer >> *First;
                    mNumberOfLines++;
                }

                QUEST_SERIALIZER_MODE_END
            }

            template<typename TIteratorType>
            void write(TIteratorType First, TIteratorType Last, SizeType Size){
                QUEST_SERIALIZER_MODE_BINARY

                for(;First!=Last;First++){
                    const char* data = reinterpret_cast<const char*>(First);
                    mpBuffer->write(data,sizeof(size));
                }

                QUEST_SERIALIZER_MODE_ASCII

                for(;First!=Last;First++){
                    *mpBuffer << *First << std::endl;
                }

                QUEST_SERIALIZER_MODE_END
            }

            inline SizeType BlockCompatibleSize(SizeType rSize){
                typedef char BlockType;
                const SizeType block_size = sizeof(BlockType);
                return static_cast<SizeType>(((block_size-1)+rSize)/block_size);
            }

            void SeekBegin();

            void SeekEnd();

            Serializer& operator = (const Serializer& rOther);

            Serializer(const Serializer& rOther);

    }; // class Serializer


} // namespace Quest

#undef QUEST_SERIALIZER_MODE_BINARY
#undef QUEST_SERIALIZER_MODE_ASCII
#undef QUEST_SERIALIZER_MODE_END

#undef QUEST_SERIALIZATION_DIRECT_LOAD
#undef QUEST_SERIALIZATION_DIRECT_SAVE

#endif // SERIALIZER_HPP