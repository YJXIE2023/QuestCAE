/*---------------------------------
fill_communicator.hpp类的实现代码
-----------------------------------*/

// 项目头文件
#include "includes/fill_communicator.hpp"
#include "includes/data_communicator.hpp"
#include "includes/parallel_enviroment.hpp"

namespace Quest{

    FillCommunicator::FillCommunicator(ModelPart& rModelPart, const DataCommunicator& rDataCommunicator):
        mrDataCom(rDataCommunicator),
        mrBaseModelPart(rModelPart){}

    
    void FillCommunicator::Execute(){
        QUEST_TRY
        QUEST_CATCH("")
    }


    void FillCommunicator::PrintDebugInfo(){
        PrintModelPartDebugInfo(mrBaseModelPart);
    }

    void FillCommunicator::PrintModelPartDebugInfo(const ModelPart& rModelPart){
        QUEST_TRY

        std::cout.flush();
        const auto& r_communicator = rModelPart.GetCommunicator();
        const auto& r_data_communicator = r_communicator.GetDataCommunicator();
        r_data_communicator.Barrier();

        const int rank = r_data_communicator.Rank();
        const int num_processors = r_data_communicator.Size();
        QUEST_ERROR_IF_NOT(rank == 0) << "Serial FillCommunicator current rank is not 0." << std::endl;
        QUEST_ERROR_IF_NOT(num_processors == 1) << "Serial FillCommunicator number of processors larger than q." << std::endl;


        QUEST_ERROR_IF_NOT(r_communicator.NeighbourIndices().size() == 0) << "There are not expected neighbour indices" << std::endl;
        QUEST_ERROR_IF_NOT(r_communicator.GhostMesh().NumberOfNodes() == 0) << "There are not expected nodes in the ghost mesh" << std::endl;
        QUEST_ERROR_IF_NOT(r_communicator.InterfaceMesh().NumberOfNodes() == 0) << "There are not expected nodes in the interface mesh" << std::endl;

        QUEST_CATCH("")
    }


    std::string FillCommunicator::Info() const{
        std::stringstream buffer;
        buffer << "FillCommunicator";
        return buffer.str();
    }

    void FillCommunicator::PrintInfo(std::ostream& rOstream) const{
        rOstream << "FillCommunicator" << std::endl;
    }


    void FillCommunicator::PrintData(std::ostream& rOstream)const {}

}