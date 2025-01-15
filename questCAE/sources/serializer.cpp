// 项目头文件
#include "includes/serializer.hpp"
#include "container/model.hpp"
#include "container/variable.hpp"
#include "includes/quest_components.hpp"

namespace Quest{

    QUEST_CREATE_LOCAL_FLAG( Serializer, MPI,  0 );
    QUEST_CREATE_LOCAL_FLAG( Serializer, SHALLOW_GLOBAL_POINTERS_SERIALIZATION, 1 );

    Serializer::RegisteredObjectsContainerType Serializer::msRegisteredObjects;

    Serializer::RegisteredObjectsNameContainerType Serializer::msRegisteredObjectsName;

    VariableData* Serializer::GetVariableData(std::string const & VariableName)
    {
        return QuestComponents<VariableData>::pGet(VariableName);
    }

    void Serializer::SetLoadState() {
        mLoadedPointers.clear();
        SeekBegin();
    }

    void Serializer::SeekBegin() {
        mpBuffer->seekg(0, mpBuffer->beg);
    }

    void Serializer::SeekEnd() {
        mpBuffer->seekg(0, mpBuffer->end);
    }

    void Serializer::load(std::string const & rTag, ModelPart*& pValue)
    {
        PointerType pointer_type = SP_INVALID_POINTER;
        void* p_pointer;
        read(pointer_type);

        if(pointer_type != SP_INVALID_POINTER)
        {
            read(p_pointer);
            LoadedPointersContainerType::iterator i_pointer = mLoadedPointers.find(p_pointer);
            if(i_pointer == mLoadedPointers.end())
            {
                if(pointer_type == SP_BASE_CLASS_POINTER)
                {
                    QUEST_ERROR_IF(!pValue) << "an already constructed modelpart must be passed to load a ModelPart" <<std::endl;
                }
                else if(pointer_type == SP_DERIVED_CLASS_POINTER)
                {
                    QUEST_ERROR << "should not find SP_DERIVED_CLASS_POINTER for ModelPart load" << std::endl;
                }

                mLoadedPointers[p_pointer]=&pValue;
                load(rTag, *pValue);
            }
            else
            {
                QUEST_ERROR << "modelpart has already been serialized - should not be done twice!" << std::endl;
            }
        }
    }

    void Serializer::load(std::string const & rTag, Quest::unique_ptr<ModelPart>& pValue)
    {
        PointerType pointer_type = SP_INVALID_POINTER;
        void* p_pointer;
        read(pointer_type);

        if(pointer_type != SP_INVALID_POINTER)
        {
            read(p_pointer);
            LoadedPointersContainerType::iterator i_pointer = mLoadedPointers.find(p_pointer);
            if(i_pointer == mLoadedPointers.end())
            {
                if(pointer_type == SP_BASE_CLASS_POINTER)
                {
                    QUEST_ERROR_IF(!pValue) << "an already constructed modelpart must be passed to load a ModelPart" <<std::endl;
                }
                else if(pointer_type == SP_DERIVED_CLASS_POINTER)
                {
                    QUEST_ERROR << "should not find SP_DERIVED_CLASS_POINTER for ModelPart load" << std::endl;
                }

                mLoadedPointers[p_pointer]=&pValue;
                load(rTag, *pValue);
            }
            else
            {
                QUEST_ERROR << "modelpart has already been serialized - should not be done twice!" << std::endl;
            }
        }
    }

    void Serializer::load(std::string const & rTag, Quest::shared_ptr<ModelPart>& pValue)
    {
        PointerType pointer_type = SP_INVALID_POINTER;
        void* p_pointer;
        read(pointer_type);

        if(pointer_type != SP_INVALID_POINTER)
        {
            read(p_pointer);
            LoadedPointersContainerType::iterator i_pointer = mLoadedPointers.find(p_pointer);
            if(i_pointer == mLoadedPointers.end())
            {
                if(pointer_type == SP_BASE_CLASS_POINTER)
                {
                    QUEST_ERROR_IF(!pValue) << "an already constructed modelpart must be passed to load a ModelPart" <<std::endl;
                }
                else if(pointer_type == SP_DERIVED_CLASS_POINTER)
                {
                    QUEST_ERROR << "should not find SP_DERIVED_CLASS_POINTER for ModelPart load" << std::endl;
                }

                mLoadedPointers[p_pointer]=&pValue;
                load(rTag, *pValue);
            }
            else
            {
                QUEST_ERROR << "modelpart has already been serialized - should not be done twice!" << std::endl;
            }
        }
    }

}