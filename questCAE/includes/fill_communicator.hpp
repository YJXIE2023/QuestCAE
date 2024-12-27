#ifndef QUEST_FILL_COMMUNICATOR_HPP
#define QUEST_FILL_COMMUNICATOR_HPP

// 项目头文件
#include "includes/define.hpp"
#include "includes/model_part.hpp"

namespace Quest{

    /**
     * @class FillCommunicator
     * @brief 定义填充通讯器的API基类
     * @details 为派生的ParallelFillCommunicator类提供接口
     */
    class QUEST_API(QUEST_CORE) FillCommunicator{
        public:
            enum class FillCommunicatorEchoLevel{
                NO_PRINTING = 0,  // 完全不输出任何信息
                INFO = 1,         // 只输出一些附加的基本信息
                DEBUG_INFO = 2    // 输出调试信息
            };

            QUEST_CLASS_POINTER_DEFINITION(FillCommunicator);

            /**
             * @brief 构造函数
             */
            FillCommunicator(ModelPart& rModelPart, const DataCommunicator& rDataCommunicator);

            /**
             * @brief 拷贝构造函数
             */
            FillCommunicator(const FillCommunicator& rOther) = delete;

            /**
             * @brief 析构函数
             */
            virtual ~FillCommunicator() = default;

            /**
             * @brief 赋值运算符
             */
            FillCommunicator& operator=(const FillCommunicator& rOther) = delete;

            /**
             * @brief 执行通信器填充操作
             * @details 此方法用于执行通信器的填充。在当前的串行情况下，此方法不执行任何操作。
             */
            virtual void Execute();

            /**
             * @brief 打印详细网格信息的函数
             * 警告：仅用于调试，因为会输出大量信息
             */
            void PrintDebugInfo();

            /**
             * @brief 打印指定模型部分网格信息的函数
             * @details 此函数用于检查并打印一些网格信息
             * @note 在当前的串行情况下，函数内容几乎为空，仅执行基本检查
             * @param rModelPart 引用要检查的模型部分
             */
            virtual void PrintModelPartDebugInfo(const ModelPart& rModelPart);

            /**
             * @brief 设置输出详细程度等级
             * @param EchoLevel 输出详细程度等级
             */
            void SetEchoLevel(const FillCommunicatorEchoLevel EchoLevel){
                mEchoLevel = EchoLevel;
            }

            /**
             * @brief 获取输出详细程度等级
             */
            FillCommunicatorEchoLevel GetEchoLevel() const{
                return mEchoLevel;
            }

            virtual std::string Info() const;

            virtual void PrintInfo(std::ostream& rOStream) const;

            virtual void PrintData(std::ostream& rOStream) const;

        protected:
            /**
             * @brief 数据通信器
             */
            const DataCommunicator& mrDataCom;

            FillCommunicatorEchoLevel mEchoLevel = FillCommunicatorEchoLevel::NO_PRINTING;

            /**
             * @brief 获取基本模型部件
             */
            ModelPart& GetBaseModelPart(){
                return mrBaseModelPart;
            }

        private:
            /**
             * @brief 基本模型部件
             */
            ModelPart& mrBaseModelPart;

    };

    inline std::istream& operator >>(std::istream& rIstream, FillCommunicator& rThis){
        return rIstream;
    }

    inline std::ostream& operator <<(std::ostream& rOstream, const FillCommunicator& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }

} // namespace Quest


#endif // FILL_COMMUNICATOR_HPP