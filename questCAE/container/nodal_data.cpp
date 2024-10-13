/*--------------------------------------------
nodal_data.hpp类的实现代码
--------------------------------------------*/

// 项目头文件
#include "container/nodal_data.hpp"

namespace Quest{

    NodalData::NodalData(IndexType TheId):mId(TheId),mSolutionStepsNodalData(){}

    NodalData::NodalData(IndexType TheId, VariablesList::Pointer pVariablesList, SizeType NewQueueSize):
        mId(TheId),
        mSolutionStepsNodalData(pVariablesList, NewQueueSize){}

    NodalData::NodalData(IndexType TheId, VariablesList::Pointer pVariablesList, const BlockType* ThisData, SizeType NewQueueSize):
        mId(TheId),
        mSolutionStepsNodalData(pVariablesList, ThisData, NewQueueSize){}

    NodalData& NodalData::operator=(const NodalData& Other){
        mId = Other.mId;
        mSolutionStepsNodalData = Other.mSolutionStepsNodalData;

        return *this;
    }

    std::string NodalData::Info() const{
        std::stringstream buffer;
        buffer << "NodalData";

        return buffer.str();
    }

    void NodalData::PrintInfo(std::ostream& rOstream) const{
        rOstream << Info();
    }

    void NodalData::PrintData(std::ostream& rOstream) const{
        rOstream << "Id                  : " << mId << std::endl;
        rOstream << "Solution Steps Data : " << mSolutionStepsNodalData << std::endl;
    }

    void NodalData::save(Serializer& rSerializer) const{
        rSerializer.save("Id", mId);
        rSerializer.save("SolutionStepsNodalData", mSolutionStepsNodalData);
    }

    void NodalData::load(Serializer& rSerializer){
        rSerializer.load("Id", mId);
        rSerializer.load("SolutionStepsNodalData", mSolutionStepsNodalData);
    }

} // namespace Quest