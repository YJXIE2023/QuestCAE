/*--------------------------------------------
用于存储和管理求解过程中的各种参数和变量
--------------------------------------------*/

#ifndef QUEST_PROCESS_INFO_HPP
#define QUEST_PROCESS_INFO_HPP

// 系统头文件
#include <string>
#include <iostream>
#include <sstream>
#include <cstddef>

// 项目头文件
#include "includes/define.hpp"
#include "container/data_value_container.hpp"
#include "container/flags.hpp"

namespace Quest{

    class QUEST_API(QUEST_CORE) ProcessInfo: public DataValueContainer, public Flags{
        public:
            QUEST_CLASS_POINTER_DEFINITION(ProcessInfo);
            using BaseType = DataValueContainer;
            using SizeType = std::size_t;
            using IndexType = std::size_t;


            ProcessInfo():
                BaseType(),
                Flags(),
                mIsTimeStep(true),
                mSolutionStepIndex(),
                mpPreviousSolutionStepInfo(),
                mpPreviousTimeStepInfo()
            {}


            ProcessInfo(const ProcessInfo& rOther):
                BaseType(rOther),
                Flags(rOther),
                mIsTimeStep(rOther.mIsTimeStep),
                mSolutionStepIndex(rOther.mSolutionStepIndex),
                mpPreviousSolutionStepInfo(rOther.mpPreviousSolutionStepInfo),
                mpPreviousTimeStepInfo(rOther.mpPreviousTimeStepInfo)
            {}

            
            ~ProcessInfo() override {}


            ProcessInfo& operator=(const ProcessInfo& rOther){
                BaseType::operator=(rOther);
                Flags::operator=(rOther);
                mIsTimeStep = rOther.mIsTimeStep;
                mSolutionStepIndex = rOther.mSolutionStepIndex;
                mpPreviousSolutionStepInfo = rOther.mpPreviousSolutionStepInfo;
                mpPreviousTimeStepInfo = rOther.mpPreviousTimeStepInfo;
                return *this;
            }


            void CreateTimeStepInfo(IndexType SolutionStepIndex = 0){
                CreateSolutionStepInfo(SolutionStepIndex);
                mIsTimeStep = true;
            }


            void CloneTimeStepInfo(IndexType SolutionStepIndex = 0){
                CreateSolutionStepInfo(SolutionStepIndex);
                mIsTimeStep = true;
            }


            void CloneTimeStepInfo(IndexType SolutionStepIndex, const ProcessInfo& SourceSolutionStepInfo){
                CloneSolutionStepInfo(SolutionStepIndex, SourceSolutionStepInfo);
                mIsTimeStep = true;
            }


            void CreateTimeStepInfo(double NewTime, IndexType SolutionStepIndex = 0){
                CreateTimeStepInfo(SolutionStepIndex);
                SetCurrentTime(NewTime);
            }


            void CloneTimeStepInfo(double NewTime, IndexType SourceSolutionStepIndex = 0){
                CloneTimeStepInfo(SourceSolutionStepIndex);
                SetCurrentTime(NewTime);
            }


            void CreateTimeStepInfo(double NewTime, IndexType SolutionStepIndex, const ProcessInfo& SourceSolutionStepInfo){
                CloneTimeStepInfo(SolutionStepIndex, SourceSolutionStepInfo);
                SetCurrentTime(NewTime);
            }


            void SetAsTimeStepInfo();


            void SetAsTimeStepInfo(double NewTime);


            ProcessInfo::Pointer pGetPreviousTimeStepInfo(IndexType StepsBefore = 1);


            const ProcessInfo::Pointer pGetPreviousTimeStepInfo(IndexType StepsBefore = 1) const;


            ProcessInfo& GetPreviousTimeStepInfo(IndexType StepsBefore = 1){
                return *pGetPreviousTimeStepInfo(StepsBefore);
            }


            const ProcessInfo& GetPreviousTimeStepInfo(IndexType StepsBefore = 1) const{
                return *pGetPreviousTimeStepInfo(StepsBefore);
            }


            IndexType GetPreviousTimeStepIndex(IndexType StepsBefore = 1) const{
                return GetPreviousTimeStepInfo(StepsBefore).GetSolutionStepIndex();
            }


            void SetCurrentTime(double NewTime);


            void ReIndexBuffer(SizeType BufferSize, IndexType BaseIndex = 0);


            void CreateSolutionStepInfo(IndexType SolutionStepIndex = 0);


            void CloneSolutionStepInfo();


            void CloneSolutionStepInfo(IndexType SourceSolutionStepIndex);


            void CloneSolutionStepInfo(IndexType SolutionStepIndex, const ProcessInfo& SourceSolutionStepInfo);


            ProcessInfo& FindSolutionStepInfo(IndexType ThisIndex);


            void RemoveSolutionStepInfo(IndexType SolutionStepIndex);


            void ClearHistory(IndexType StepsBefore = 0);


            ProcessInfo::Pointer pGetPreviousSolutionStepInfo(IndexType StepsBefore = 1);


            const ProcessInfo::Pointer pGetPreviousSolutionStepInfo(IndexType StepsBefore = 1) const;

            
            ProcessInfo& GetPreviousSolutionStepInfo(IndexType StepsBefore = 1){
                return *pGetPreviousSolutionStepInfo(StepsBefore);
            }


            const ProcessInfo& GetPreviousSolutionStepInfo(IndexType StepsBefore = 1) const{
                return *pGetPreviousSolutionStepInfo(StepsBefore);
            }


            IndexType GetPreviousSolutionStepIndex(IndexType StepsBefore = 1) const{
                return GetPreviousSolutionStepInfo(StepsBefore).GetSolutionStepIndex();
            }


            IndexType GetSolutionStepIndex() const{
                return mSolutionStepIndex;
            }


            void SetSolutionStepIndex(IndexType NewIndex){
                mSolutionStepIndex = NewIndex;
            }


            std::string Info() const override{
                return "Process Info";
            }


            void PrintInfo(std::ostream& rOstream) const override{
                rOstream << Info();
            }


            void PrintData(std::ostream& rOstream) const override{
                rOstream << "    Current solution step index: " << mSolutionStepIndex << std::endl;
                BaseType::PrintData(rOstream);
            }


        protected:

        private:
            friend class Serializer;

            void save(Serializer& rSerializer) const override;

            void load(Serializer& rSerializer) override;

        private:
            bool mIsTimeStep;
            IndexType mSolutionStepIndex;
            ProcessInfo::Pointer mpPreviousSolutionStepInfo;
            ProcessInfo::Pointer mpPreviousTimeStepInfo;

    };

    inline std::istream& operator>>(std::istream& rIstream, ProcessInfo& rThis);

    inline std::ostream& operator<<(std::ostream& rOstream, const ProcessInfo& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }

} // namespace Quest


#endif //QUEST_PROCESS_INFO_HPP