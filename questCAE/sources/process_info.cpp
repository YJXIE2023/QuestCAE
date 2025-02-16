/*--------------------------------------------
process_info.hpp 头文件实现代码
--------------------------------------------*/

// 项目头文件
#include "includes/process_info.hpp"
#include "includes/variables.hpp"

namespace Quest{

    using IndexType = ProcessInfo::IndexType;

    void ProcessInfo::SetAsTimeStepInfo(){
        mIsTimeStep = true;
        SetCurrentTime((*this)(TIME));
    }

    void ProcessInfo::SetAsTimeStepInfo(double NewTime){
        mIsTimeStep = true;
        SetCurrentTime(NewTime);
    }
    
    ProcessInfo::Pointer ProcessInfo::pGetPreviousTimeStepInfo(IndexType StepsBefore){
        if(StepsBefore > 1){
            return mpPreviousTimeStepInfo->pGetPreviousTimeStepInfo(--StepsBefore);
        }

        QUEST_ERROR_IF(StepsBefore == 0) << "Steps before = 0" << std::endl;
        QUEST_ERROR_IF_NOT(mpPreviousTimeStepInfo) << "No previoous time step exists." << std::endl;

        return mpPreviousTimeStepInfo;
    }

    const ProcessInfo::Pointer ProcessInfo::pGetPreviousTimeStepInfo(IndexType StepsBefore) const{
        if(StepsBefore > 1){
            return mpPreviousTimeStepInfo->pGetPreviousTimeStepInfo(--StepsBefore);
        }

        QUEST_ERROR_IF(StepsBefore == 0) << "Steps before = 0" << std::endl;
        QUEST_ERROR_IF_NOT(mpPreviousTimeStepInfo) << "No previoous time step exists." << std::endl;

        return mpPreviousTimeStepInfo;
    }

    void ProcessInfo::SetCurrentTime(double NewTime){
        (*this)(TIME) = NewTime;
        if(!mpPreviousTimeStepInfo){
            (*this)(DELTA_TIME) = NewTime;
        } else {
            (*this)(DELTA_TIME) = NewTime - mpPreviousTimeStepInfo->GetValue(TIME);
        }
    }

    void ProcessInfo::ReIndexBuffer(SizeType BufferSize, IndexType BaseIndex){
        mSolutionStepIndex = BaseIndex;
        if(BufferSize > 1){
            if(mpPreviousSolutionStepInfo){
                mpPreviousSolutionStepInfo->ReIndexBuffer(BufferSize - 1, BaseIndex + 1);
            }
        }
    }

    void ProcessInfo::CreateSolutionStepInfo(IndexType SolutionStepIndex){
        mpPreviousSolutionStepInfo = Pointer(new ProcessInfo(*this));
        mSolutionStepIndex = SolutionStepIndex;
        if(mIsTimeStep){
            mpPreviousTimeStepInfo = mpPreviousSolutionStepInfo;
        }
        mIsTimeStep = false;
        BaseType::Clear();
    }

    void ProcessInfo::CloneSolutionStepInfo(){
        mpPreviousSolutionStepInfo = Pointer(new ProcessInfo(*this));
        mSolutionStepIndex = 0;
        if(mIsTimeStep){
            mpPreviousTimeStepInfo = mpPreviousSolutionStepInfo;
        }
        mIsTimeStep = false;
    }

    void ProcessInfo::CloneSolutionStepInfo(IndexType SourceSolutionStepIndex){
        ProcessInfo& source_info = FindSolutionStepInfo(SourceSolutionStepIndex);
        if(source_info.GetSolutionStepIndex() == SourceSolutionStepIndex){
            mpPreviousSolutionStepInfo = Pointer(new ProcessInfo(*this));
            mSolutionStepIndex = 0;
            BaseType::operator=(source_info);
            if(mIsTimeStep){
                mpPreviousTimeStepInfo = mpPreviousSolutionStepInfo;
            }
            mIsTimeStep = false;
        } else {
            CreateSolutionStepInfo(0);
        }
    }

    void ProcessInfo::CloneSolutionStepInfo(IndexType SolutionStepIndex, const ProcessInfo& SourceSolutionStepInfo){
        mpPreviousSolutionStepInfo = Pointer(new ProcessInfo(*this));
        mSolutionStepIndex = SolutionStepIndex;
        BaseType::operator=(SourceSolutionStepInfo);
        if(mIsTimeStep){
            mpPreviousTimeStepInfo = mpPreviousSolutionStepInfo;
        }
        mIsTimeStep = false;
    }

    ProcessInfo& ProcessInfo::FindSolutionStepInfo(IndexType ThisIndex){
        if(ThisIndex == mSolutionStepIndex){
            return *this;
        }

        if(!mpPreviousSolutionStepInfo){
            return *this;
        }

        return mpPreviousTimeStepInfo->FindSolutionStepInfo(ThisIndex);
    }

    void ProcessInfo::RemoveSolutionStepInfo(IndexType SolutionStepIndex){
        if(!mpPreviousSolutionStepInfo){
            return;
        }

        if(mpPreviousSolutionStepInfo->GetSolutionStepIndex() == SolutionStepIndex){
            mpPreviousSolutionStepInfo = mpPreviousSolutionStepInfo->pGetPreviousSolutionStepInfo();
        } else {
            mpPreviousSolutionStepInfo->RemoveSolutionStepInfo(SolutionStepIndex);
        }
    }

    void ProcessInfo::ClearHistory(IndexType StepsBefore){
        if(StepsBefore == 0){
            mpPreviousTimeStepInfo = Pointer();
            mpPreviousSolutionStepInfo = Pointer();
        } else {
            if(mpPreviousTimeStepInfo){
                mpPreviousTimeStepInfo->ClearHistory(--StepsBefore);
            } else {
                mpPreviousSolutionStepInfo->ClearHistory(--StepsBefore);
            }
        }
    }

    ProcessInfo::Pointer ProcessInfo::pGetPreviousSolutionStepInfo(IndexType StepsBefore){
        if(StepsBefore > 1){
            return mpPreviousSolutionStepInfo->pGetPreviousSolutionStepInfo(--StepsBefore);
        }

        QUEST_ERROR_IF(StepsBefore == 0) << "Steps before = 0" << std::endl;
        QUEST_ERROR_IF_NOT(mpPreviousTimeStepInfo) << "No previoous solution step exists." << std::endl;

        return mpPreviousSolutionStepInfo;
    }

    const ProcessInfo::Pointer ProcessInfo::pGetPreviousSolutionStepInfo(IndexType StepsBefore) const{
        if(StepsBefore > 1){
            return mpPreviousSolutionStepInfo->pGetPreviousSolutionStepInfo(--StepsBefore);
        }

        QUEST_ERROR_IF(StepsBefore == 0) << "Steps before = 0" << std::endl;
        QUEST_ERROR_IF_NOT(mpPreviousTimeStepInfo) << "No previoous solution step exists." << std::endl;

        return mpPreviousSolutionStepInfo;
    }

    void ProcessInfo::save(Serializer& rSerializer) const{
        QUEST_TRY

        QUEST_SERIALIZE_SAVE_BASE_CLASS(rSerializer, BaseType);
        QUEST_SERIALIZE_SAVE_BASE_CLASS(rSerializer, Flags);
        rSerializer.save("Is Time Step", mIsTimeStep);
        rSerializer.save("Solution Step Index", mSolutionStepIndex);
        rSerializer.save("Previous Solution Step Info", mpPreviousSolutionStepInfo);
        rSerializer.save("Previous Time Step Info", mpPreviousTimeStepInfo);

        QUEST_CATCH("")
    }

    void ProcessInfo::load(Serializer& rSerializer){
        QUEST_TRY

        QUEST_SERIALIZE_LOAD_BASE_CLASS(rSerializer, BaseType);
        QUEST_SERIALIZE_LOAD_BASE_CLASS(rSerializer, Flags);
        rSerializer.load("Is Time Step", mIsTimeStep);
        rSerializer.load("Solution Step Index", mSolutionStepIndex);
        rSerializer.load("Previous Solution Step Info", mpPreviousSolutionStepInfo);
        rSerializer.load("Previous Time Step Info", mpPreviousTimeStepInfo);

        QUEST_CATCH("")
    }


} // namespace Quest