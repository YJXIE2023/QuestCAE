/*-------------------------------------
多进程或多线程环境下实现数据通信和同步
-------------------------------------*/

#ifndef FILL_COMMUNICATOR_HPP
#define FILL_COMMUNICATOR_HPP

// 项目头文件
#include "includes/define.hpp"
#include "includes/model_part.hpp"

namespace Quest{

    class QUEST_API FillCommunicator{
        public:
            enum class FillCommunicatorEchoLevel{
                NO_PRINTING = 0,
                INFO = 1,
                DEBUG_INFO = 2
            };

            QUEST_CLASS_POINTER_DEFINITION(FillCommunicator);

            FillCommunicator(ModelPart& rModelPart, const DataCommunicator& rDataCommunicator);

            FillCommunicator(const FillCommunicator& rOther) = delete;

            virtual ~FillCommunicator() = default;

            FillCommunicator& operator=(const FillCommunicator& rOther) = delete;

            virtual void Execute();

            void PrintDebugInfo();

            virtual void PrintModelPartDebugInfo(const ModelPart& rModelPart);

            void SetEchoLevel(const FillCommunicatorEchoLevel EchoLevel){
                mEchoLevel = EchoLevel;
            }

            FillCommunicatorEchoLevel GetEchoLevel() const{
                return mEchoLevel;
            }

            virtual std::string Info() const;

            virtual void PrintInfo(std::ostream& rOStream) const;

            virtual void PrintData(std::ostream& rOStream) const;

        protected:
            const DataCommunicator& mrDataCom;

            FillCommunicatorEchoLevel mEchoLevel = FillCommunicatorEchoLevel::NO_PRINTING;

            ModelPart& GetBaseModelPart(){
                return mrBaseModelPart;
            }

        private:
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