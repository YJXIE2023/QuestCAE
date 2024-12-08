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

    /**
     * @class ProcessInfo
     * @brief 保存求解过程中各种参数的当前值
     * @details 用来存储定义的变量：如时间、求解步长、非线性步长或其他任何变量
     */
    class QUEST_API(QUEST_CORE) ProcessInfo: public DataValueContainer, public Flags{
        public:
            QUEST_CLASS_POINTER_DEFINITION(ProcessInfo);
            using BaseType = DataValueContainer;
            using SizeType = std::size_t;
            using IndexType = std::size_t;

            /**
             * @brief 默认构造函数
             */
            ProcessInfo():
                BaseType(),
                Flags(),
                mIsTimeStep(true),
                mSolutionStepIndex(),
                mpPreviousSolutionStepInfo(),
                mpPreviousTimeStepInfo()
            {}

            /**
             * @brief 复制构造函数
             */
            ProcessInfo(const ProcessInfo& rOther):
                BaseType(rOther),
                Flags(rOther),
                mIsTimeStep(rOther.mIsTimeStep),
                mSolutionStepIndex(rOther.mSolutionStepIndex),
                mpPreviousSolutionStepInfo(rOther.mpPreviousSolutionStepInfo),
                mpPreviousTimeStepInfo(rOther.mpPreviousTimeStepInfo)
            {}

            /**
             * @brief 析构函数
             */
            ~ProcessInfo() override {}

            /**
             * @brief 赋值运算符重载
             */
            ProcessInfo& operator=(const ProcessInfo& rOther){
                BaseType::operator=(rOther);
                Flags::operator=(rOther);
                mIsTimeStep = rOther.mIsTimeStep;
                mSolutionStepIndex = rOther.mSolutionStepIndex;
                mpPreviousSolutionStepInfo = rOther.mpPreviousSolutionStepInfo;
                mpPreviousTimeStepInfo = rOther.mpPreviousTimeStepInfo;
                return *this;
            }

            /**
             * @brief 创建时间步信息对象
             * @param SolutionStepIndex 时间步索引
             */
            void CreateTimeStepInfo(IndexType SolutionStepIndex = 0){
                CreateSolutionStepInfo(SolutionStepIndex);
                mIsTimeStep = true;
            }

            /**
             * @brief 克隆时间步信息对象
             */
            void CloneTimeStepInfo(IndexType SolutionStepIndex = 0){
                CreateSolutionStepInfo(SolutionStepIndex);
                mIsTimeStep = true;
            }

            /**
             * @brief 克隆时间步信息对象
             * @param SolutionStepIndex 时间步索引
             * @param SourceSolutionStepInfo 分析步信息源对象
             */
            void CloneTimeStepInfo(IndexType SolutionStepIndex, const ProcessInfo& SourceSolutionStepInfo){
                CloneSolutionStepInfo(SolutionStepIndex, SourceSolutionStepInfo);
                mIsTimeStep = true;
            }

            /**
             * @brief 创建时间步信息对象
             * @param NewTime 新的时间值
             * @param SolutionStepIndex 时间步索引
             */
            void CreateTimeStepInfo(double NewTime, IndexType SolutionStepIndex = 0){
                CreateTimeStepInfo(SolutionStepIndex);
                SetCurrentTime(NewTime);
            }

            /**
             * @brief 克隆时间步信息对象
             * @param NewTime 新的时间值
             * @param SourceSolutionStepIndex 分析步索引
             */
            void CloneTimeStepInfo(double NewTime, IndexType SourceSolutionStepIndex = 0){
                CloneTimeStepInfo(SourceSolutionStepIndex);
                SetCurrentTime(NewTime);
            }

            /**
             * @brief 创建时间步信息对象
             * @param NewTime 新的时间值
             * @param SolutionStepIndex 时间步索引
             * @param SourceSolutionStepInfo 求解步信息源对象
             */
            void CreateTimeStepInfo(double NewTime, IndexType SolutionStepIndex, const ProcessInfo& SourceSolutionStepInfo){
                CloneTimeStepInfo(SolutionStepIndex, SourceSolutionStepInfo);
                SetCurrentTime(NewTime);
            }

            /**
             * @brief 设置当前为时间步信息
             */
            void SetAsTimeStepInfo();

            /**
             * @brief 设置当前为时间步信息
             */
            void SetAsTimeStepInfo(double NewTime);

            /**
             * @brief 获取之前时间步信息对象指针
             * @param StepsBefore 移动多少时间步
             */
            ProcessInfo::Pointer pGetPreviousTimeStepInfo(IndexType StepsBefore = 1);

            /**
             * @brief 获取之前时间步信息对象指针
             * @param StepsBefore 移动多少时间步
             */
            const ProcessInfo::Pointer pGetPreviousTimeStepInfo(IndexType StepsBefore = 1) const;

            /**
             * @brief 获取之前时间步信息对象
             * @param StepsBefore 移动多少时间步
             */
            ProcessInfo& GetPreviousTimeStepInfo(IndexType StepsBefore = 1){
                return *pGetPreviousTimeStepInfo(StepsBefore);
            }

            /**
             * @brief 获取之前时间步信息对象
             * @param StepsBefore 移动多少时间步
             */
            const ProcessInfo& GetPreviousTimeStepInfo(IndexType StepsBefore = 1) const{
                return *pGetPreviousTimeStepInfo(StepsBefore);
            }

            /**
             * @brief 获取之前时间步信息对象的求解步索引
             */
            IndexType GetPreviousTimeStepIndex(IndexType StepsBefore = 1) const{
                return GetPreviousTimeStepInfo(StepsBefore).GetSolutionStepIndex();
            }

            /**
             * @brief 设置当前时间值
             * @param NewTime 新的时间值
             */
            void SetCurrentTime(double NewTime);

            /**
             * @brief 重新分配索引
             * @param BufferSize 要处理的时间步数的数量
             * @param BaseIndex 起始索引
             */
            void ReIndexBuffer(SizeType BufferSize, IndexType BaseIndex = 0);

            /**
             * @brief 创建求解步信息对象
             */
            void CreateSolutionStepInfo(IndexType SolutionStepIndex = 0);

            /**
             * @brief 克隆求解步信息对象
             */
            void CloneSolutionStepInfo();

            /**
             * @brief 克隆求解步信息对象
             */
            void CloneSolutionStepInfo(IndexType SourceSolutionStepIndex);

            /**
             * @brief 克隆求解步信息对象
             */
            void CloneSolutionStepInfo(IndexType SolutionStepIndex, const ProcessInfo& SourceSolutionStepInfo);

            /**
             * @brief 查找指定索引的求解步信息对象
             */
            ProcessInfo& FindSolutionStepInfo(IndexType ThisIndex);

            /**
             * @brief 移除指定索引的求解步信息对象
             */
            void RemoveSolutionStepInfo(IndexType SolutionStepIndex);

            /**
             * @brief 清除历史记录
             * @param StepsBefore 要清除的历史记录的步数
             */
            void ClearHistory(IndexType StepsBefore = 0);

            /**
             * @brief 获取之前求解步信息对象指针
             * @param StepsBefore 移动多少步
             */
            ProcessInfo::Pointer pGetPreviousSolutionStepInfo(IndexType StepsBefore = 1);

            /**
             * @brief 获取之前求解步信息对象指针
             * @param StepsBefore 移动多少步
             */
            const ProcessInfo::Pointer pGetPreviousSolutionStepInfo(IndexType StepsBefore = 1) const;

            /**
             * @brief 获取之前求解步信息对象
             * @param StepsBefore 移动多少步
             */
            ProcessInfo& GetPreviousSolutionStepInfo(IndexType StepsBefore = 1){
                return *pGetPreviousSolutionStepInfo(StepsBefore);
            }

            /**
             * @brief 获取之前求解步信息对象
             * @param StepsBefore 移动多少步
             */
            const ProcessInfo& GetPreviousSolutionStepInfo(IndexType StepsBefore = 1) const{
                return *pGetPreviousSolutionStepInfo(StepsBefore);
            }

            /**
             * @brief 获取之前求解步信息对象的求解步索引
             * @param StepsBefore 移动多少步
             */
            IndexType GetPreviousSolutionStepIndex(IndexType StepsBefore = 1) const{
                return GetPreviousSolutionStepInfo(StepsBefore).GetSolutionStepIndex();
            }

            /**
             * @brief 获取当前求解步索引
             */
            IndexType GetSolutionStepIndex() const{
                return mSolutionStepIndex;
            }

            /**
             * @brief 设置当前求解步索引
             * @param NewIndex 新的求解步索引
             */
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
            /**
             * @brief 标志当前是否为时间步信息
             */
            bool mIsTimeStep;

            /**
             * @brief 当前求解步数索引
             */
            IndexType mSolutionStepIndex;

            /**
             * @brief 指向前一分析步信息的指针
             */
            ProcessInfo::Pointer mpPreviousSolutionStepInfo;

            /**
             * @brief 指向前一时间步信息的指针
             */
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