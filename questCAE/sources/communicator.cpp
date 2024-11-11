/*----------------------------------
communicator.hpp头文件的实现代码
----------------------------------*/

// 项目头文件
#include "includes/communicator.hpp"
#include "includes/data_communicator.hpp"
#include "includes/parallel_enviroment.hpp"

namespace Quest{

    Communicator::Communicator():
        mNumberOfColors(1),
        mpLocalMesh(MeshType::Pointer(new MeshType)),
        mpGhostMesh(MeshType::Pointer(new MeshType)),
        mpInterfaceMesh(MeshType::Pointer(new MeshType)),
        mrDataCommunicator(ParallelEnvironment::GetDataCommunicator("Serial"))
    {
        MeshType mesh;
        mLocalMeshes.push_back(Quest::make_shared<MeshType>(mesh.Clone()));
        mGhostMeshes.push_back(Quest::make_shared<MeshType>(mesh.Clone()));
        mInterfaceMeshes.push_back(Quest::make_shared<MeshType>(mesh.Clone()));
    }


    Communicator::Communicator(const DataCommunicator& rDataCommunicator):
        mNumberOfColors(1),
        mpLocalMesh(MeshType::Pointer(new MeshType)),
        mpGhostMesh(MeshType::Pointer(new MeshType)),
        mpInterfaceMesh(MeshType::Pointer(new MeshType)),
        mrDataCommunicator(rDataCommunicator)
    {
        MeshType mesh;
        mLocalMeshes.push_back(Quest::make_shared<MeshType>(mesh.Clone()));
        mGhostMeshes.push_back(Quest::make_shared<MeshType>(mesh.Clone()));
        mInterfaceMeshes.push_back(Quest::make_shared<MeshType>(mesh.Clone()));
    }


    Communicator::Communicator(const Communicator& rOther):
        mNumberOfColors(rOther.mNumberOfColors),
        mNeighbourIndices(rOther.mNeighbourIndices),
        mpLocalMesh(MeshType::Pointer(rOther.mpLocalMesh)),
        mpGhostMesh(MeshType::Pointer(rOther.mpGhostMesh)),
        mpInterfaceMesh(MeshType::Pointer(rOther.mpInterfaceMesh)),
        mLocalMeshes(rOther.mLocalMeshes),
        mGhostMeshes(rOther.mGhostMeshes),
        mInterfaceMeshes(rOther.mInterfaceMeshes),
        mrDataCommunicator(rOther.mrDataCommunicator)
    {
    }


    Communicator::Pointer Communicator::Create(const DataCommunicator& rDataCommunicator) const{
        QUEST_TRY

        return Quest::make_shared<Communicator>(rDataCommunicator);

        QUEST_CATCH("")
    }


    Communicator::Pointer Communicator::Create() const{
        return Create(mrDataCommunicator);
    }


    bool Communicator::IsDistributed() const{
        return false;
    }


    int Communicator::MyPID() const{
        return mrDataCommunicator.Rank();
    }


    int Communicator::TotalProcesses() const{
        return mrDataCommunicator.Size();
    }


    Communicator::SizeType Communicator::GlobalNumberOfNodes() const{
        return mrDataCommunicator.SumAll(static_cast<unsigned int>(mpLocalMesh->NumberOfNodes()));
    }


    Communicator::SizeType Communicator::GlobalNumberOfElements() const{
        return mrDataCommunicator.SumAll(static_cast<unsigned int>(mpLocalMesh->NumberOfElements()));
    }


    Communicator::SizeType Communicator::GlobalNumberOfConditions() const{
        return mrDataCommunicator.SumAll(static_cast<unsigned int>(mpLocalMesh->NumberOfConditions()));
    }


    Communicator::SizeType Communicator::GlobalNumberOfMasterSlaveConstraints() const{
        return mrDataCommunicator.SumAll(static_cast<unsigned int>(mpLocalMesh->NumberOfMasterSlaveConstraints()));
    }


    Communicator::SizeType Communicator::GetNumberOfColors() const{
        return mNumberOfColors;
    }


    void Communicator::SetNumberOfColors(SizeType NewNumberOfColors){
        if(NewNumberOfColors == mNumberOfColors){
            return;
        }

        mNumberOfColors = NewNumberOfColors;
        MeshType mesh;

        mLocalMeshes.clear();
        mGhostMeshes.clear();
        mInterfaceMeshes.clear();

        for(IndexType i = 0; i < mNumberOfColors; ++i){
            mLocalMeshes.push_back(Quest::make_shared<MeshType>(mesh.Clone()));
            mGhostMeshes.push_back(Quest::make_shared<MeshType>(mesh.Clone()));
            mInterfaceMeshes.push_back(Quest::make_shared<MeshType>(mesh.Clone()));
        }
    }


    void Communicator::AddColors(SizeType NewNumberOfColors){
        if(NewNumberOfColors < 1){
            return;
        }

        mNumberOfColors += NewNumberOfColors;
        MeshType mesh;

        for(IndexType i = 0; i < NewNumberOfColors; ++i){
            mLocalMeshes.push_back(Quest::make_shared<MeshType>(mesh.Clone()));
            mGhostMeshes.push_back(Quest::make_shared<MeshType>(mesh.Clone()));
            mInterfaceMeshes.push_back(Quest::make_shared<MeshType>(mesh.Clone()));
        }
    }


    Communicator::NeighbourIndicesContainerType& Communicator::NeighbourIndices(){
        return mNeighbourIndices;
    }


    const Communicator::NeighbourIndicesContainerType& Communicator::NeighbourIndices() const{
        return mNeighbourIndices;
    }


    void Communicator::SetLocalMesh(MeshType::Pointer pGivenMesh){
        mpLocalMesh = pGivenMesh;
    }


    Communicator::MeshType::Pointer Communicator::pLocalMesh(){
        return mpLocalMesh;
    }


    Communicator::MeshType::Pointer Communicator::pGhostMesh(){
        return mpGhostMesh;
    }


    Communicator::MeshType::Pointer Communicator::pInterfaceMesh(){
        return mpInterfaceMesh;
    }


    const Communicator::MeshType::Pointer Communicator::pLocalMesh() const{
        return mpLocalMesh;
    }


    const Communicator::MeshType::Pointer Communicator::pGhostMesh() const{
        return mpGhostMesh;
    }


    const Communicator::MeshType::Pointer Communicator::pInterfaceMesh() const{
        return mpInterfaceMesh;
    }


    Communicator::MeshType::Pointer Communicator::pLocalMesh(IndexType ThisIndex){
        return mLocalMeshes(ThisIndex);
    }


    Communicator::MeshType::Pointer Communicator::pGhostMesh(IndexType ThisIndex){
        return mGhostMeshes(ThisIndex);
    }


    Communicator::MeshType::Pointer Communicator::pInterfaceMesh(IndexType ThisIndex){
        return mInterfaceMeshes(ThisIndex);
    }


    const Communicator::MeshType::Pointer Communicator::pLocalMesh(IndexType ThisIndex) const{
        return mLocalMeshes(ThisIndex);
    }


    const Communicator::MeshType::Pointer Communicator::pGhostMesh(IndexType ThisIndex) const{
        return mGhostMeshes(ThisIndex);
    }


    const Communicator::MeshType::Pointer Communicator::pInterfaceMesh(IndexType ThisIndex) const{
        return mInterfaceMeshes(ThisIndex);
    }


    Communicator::MeshType& Communicator::LocalMesh(){
        return *mpLocalMesh;
    }


    Communicator::MeshType& Communicator::GhostMesh(){
        return *mpGhostMesh;
    }


    Communicator::MeshType& Communicator::InterfaceMesh(){
        return *mpInterfaceMesh;
    }


    const Communicator::MeshType& Communicator::LocalMesh() const{
        return *mpLocalMesh;
    }


    const Communicator::MeshType& Communicator::GhostMesh() const{
        return *mpGhostMesh;
    }


    const Communicator::MeshType& Communicator::InterfaceMesh() const{
        return *mpInterfaceMesh;
    }


    Communicator::MeshType& Communicator::LocalMesh(IndexType ThisIndex){
        return mLocalMeshes[ThisIndex];
    }


    Communicator::MeshType& Communicator::GhostMesh(IndexType ThisIndex){
        return mGhostMeshes[ThisIndex];
    }


    Communicator::MeshType& Communicator::InterfaceMesh(IndexType ThisIndex){
        return mInterfaceMeshes[ThisIndex];
    }


    const Communicator::MeshType& Communicator::LocalMesh(IndexType ThisIndex) const{
        return mLocalMeshes[ThisIndex];
    }


    const Communicator::MeshType& Communicator::GhostMesh(IndexType ThisIndex) const{
        return mGhostMeshes[ThisIndex];
    }


    const Communicator::MeshType& Communicator::InterfaceMesh(IndexType ThisIndex) const{
        return mInterfaceMeshes[ThisIndex];
    }


    Communicator::MeshesContainerType& Communicator::LocalMeshes(){
        return mLocalMeshes;
    }


    Communicator::MeshesContainerType& Communicator::GhostMeshes(){
        return mGhostMeshes;
    }


    Communicator::MeshesContainerType& Communicator::InterfaceMeshes(){
        return mInterfaceMeshes;
    }


    const Communicator::MeshesContainerType& Communicator::LocalMeshes() const{
        return mLocalMeshes;
    }


    const Communicator::MeshesContainerType& Communicator::GhostMeshes() const{
        return mGhostMeshes;
    }


    const Communicator::MeshesContainerType& Communicator::InterfaceMeshes() const{
        return mInterfaceMeshes;
    }


    const DataCommunicator& Communicator::GetDataCommunicator() const{
        return mrDataCommunicator;
    }


    bool Communicator::SynchronizeNodalSolutionStepsData(){
        return true;
    }


    bool Communicator::SynchronizeDofs(){
        return true;
    }


    bool Communicator::SynchronizeVariable(const Variable<int>& rThisVariable){
        return true;
    }


    bool Communicator::SynchronizeVariable(const Variable<double>& rThisVariable){
        return true;
    }


    bool Communicator::SynchronizeVariable(const Variable<bool>& rThisVariable){
        return true;
    }


    bool Communicator::SynchronizeVariable(const Variable<Array1d<double, 3>>& rThisVariable){
        return true;
    }


    bool Communicator::SynchronizeVariable(const Variable<Array1d<double, 4>>& rThisVariable){
        return true;
    }


    bool Communicator::SynchronizeVariable(const Variable<Array1d<double, 6>>& rThisVariable){
        return true;
    }


    bool Communicator::SynchronizeVariable(const Variable<Array1d<double, 9>>& rThisVariable){
        return true;
    }


    bool Communicator::SynchronizeVariable(const Variable<Vector>& rThisVariable){
        return true;
    }


    bool Communicator::SynchronizeVariable(const Variable<Matrix>& rThisVariable){
        return true;
    }


    bool Communicator::SynchronizeVariable(const Variable<Quaternion<double>>& rThisVariable){
        return true;
    }


    bool Communicator::SynchronizeNonHistoricalVariable(const Variable<int>& rThisVariable){
        return true;
    }


    bool Communicator::SynchronizeNonHistoricalVariable(const Variable<double>& rThisVariable){
        return true;
    }


    bool Communicator::SynchronizeNonHistoricalVariable(const Variable<bool>& rThisVariable){
        return true;
    }


    bool Communicator::SynchronizeNonHistoricalVariable(const Variable<Array1d<double, 3>>& rThisVariable){
        return true;
    }


    bool Communicator::SynchronizeNonHistoricalVariable(const Variable<Array1d<double, 4>>& rThisVariable){
        return true;
    }


    bool Communicator::SynchronizeNonHistoricalVariable(const Variable<Array1d<double, 6>>& rThisVariable){
        return true;
    }


    bool Communicator::SynchronizeNonHistoricalVariable(const Variable<Array1d<double, 9>>& rThisVariable){
        return true;
    }


    bool Communicator::SynchronizeNonHistoricalVariable(const Variable<Vector>& rThisVariable){
        return true;
    }


    bool Communicator::SynchronizeNonHistoricalVariable(const Variable<Matrix>& rThisVariable){
        return true;
    }


    bool Communicator::SynchronizeNonHistoricalVariable(const Variable<Quaternion<double>>& rThisVariable){
        return true;
    }


    bool Communicator::SynchronizeCurrentDataToMax(const Variable<double>& rThisVariable){
        return true;
    }


    bool Communicator::SynchronizeNonHistoricalDataToMax(const Variable<double>& rThisVariable){
        return true;
    }


    bool Communicator::SynchronizeCurrentDataToAbsMax(const Variable<double>& rThisVariable){
        return true;
    }


    bool Communicator::SynchronizeNonHistoricalDataToAbsMax(const Variable<double>& rThisVariable){
        return true;
    }


    bool Communicator::SynchronizeCurrentDataToMin(const Variable<double>& rThisVariable){
        return true;
    }


    bool Communicator::SynchronizeNonHistoricalDataToMin(const Variable<double>& rThisVariable){
        return true;
    }


    bool Communicator::SynchronizeCurrentDataToAbsMin(const Variable<double>& rThisVariable){
        return true;
    }


    bool Communicator::SynchronizeNonHistoricalDataToAbsMin(const Variable<double>& rThisVariable){
        return true;
    }


    bool Communicator::SynchronizeElementalFlags(){
        return true;
    }


    bool Communicator::AssembleCurrentData(const Variable<int>& rThisVariable){
        return true;
    }


    bool Communicator::AssembleCurrentData(const Variable<double>& rThisVariable){
        return true;
    }


    bool Communicator::AssembleCurrentData(const Variable<Array1d<double, 3>>& rThisVariable){
        return true;
    }


    bool Communicator::AssembleCurrentData(const Variable<Vector>& rThisVariable){
        return true;
    }


    bool Communicator::AssembleCurrentData(const Variable<Matrix>& rThisVariable){
        return true;
    }


    bool Communicator::AssembleNonHistoricalData(const Variable<int>& rThisVariable){
        return true;
    }


    bool Communicator::AssembleNonHistoricalData(const Variable<double>& rThisVariable){
        return true;
    }


    bool Communicator::AssembleNonHistoricalData(const Variable<Array1d<double, 3>>& rThisVariable){
        return true;
    }


    bool Communicator::AssembleNonHistoricalData(const Variable<DenseVector<Array1d<double, 3>>>& rThisVariable){
        return true;
    }


    bool Communicator::AssembleNonHistoricalData(const Variable<Vector>& rThisVariable){
        return true;
    }


    bool Communicator::AssembleNonHistoricalData(const Variable<Matrix>& rThisVariable){
        return true;
    }


    bool Communicator::SynchronizeElementalNonHistoricalVariable(const Variable<int>& rThisVariable){
        return true;
    }


    bool Communicator::SynchronizeElementalNonHistoricalVariable(const Variable<double>& rThisVariable){
        return true;
    }


    bool Communicator::SynchronizeElementalNonHistoricalVariable(const Variable<Array1d<double, 3>>& rThisVariable){
        return true;
    }


    bool Communicator::SynchronizeElementalNonHistoricalVariable(const Variable<DenseVector<Array1d<double, 3>>>& rThisVariable){
        return true;
    }


    bool Communicator::SynchronizeElementalNonHistoricalVariable(const Variable<DenseVector<int>>& rThisVariable){
        return true;
    }


    bool Communicator::SynchronizeElementalNonHistoricalVariable(const Variable<Vector>& rThisVariable){
        return true;
    }


    bool Communicator::SynchronizeElementalNonHistoricalVariable(const Variable<Matrix>& rThisVariable){
        return true;
    }


    bool Communicator::TransferObjects(std::vector<NodesContainerType>& SendObjects, std::vector<NodesContainerType>& ReceiveObjects){
        return true;
    }


    bool Communicator::TransferObjects(std::vector<ElementsContainerType>& SendObjects, std::vector<ElementsContainerType>& ReceiveObjects){
        return true;
    }


    bool Communicator::TransferObjects(std::vector<ConditionsContainerType>& SendObjects, std::vector<ConditionsContainerType>& ReceiveObjects){
        return true;
    }


    bool Communicator::TransferObjects(std::vector<NodesContainerType>& SendObjects, std::vector<NodesContainerType>& ReceiveObjects, Quest::Serializer& particalSerializer){
        return true;
    }


    bool Communicator::TransferObjects(std::vector<ElementsContainerType>& SendObjects, std::vector<ElementsContainerType>& ReceiveObjects, Quest::Serializer& particalSerializer){
        return true;
    }


    bool Communicator::TransferObjects(std::vector<ConditionsContainerType>& SendObjects, std::vector<ConditionsContainerType>& ReceiveObjects, Quest::Serializer& particalSerializer){
        return true;
    }


    bool Communicator::SynchronizeOrNodalFlags(const Flags& TheFlags){
        return true;
    }


    bool Communicator::SynchronizeAndNodalFlags(const Flags& TheFlags){
        return true;
    }


    bool Communicator::SynchronizeNodalFlags(){
        return true;
    }


    void Communicator::Clear(){
        mNumberOfColors = 0;
        mNeighbourIndices.clear();
        mpLocalMesh->MeshType::Clear();
        mpGhostMesh->MeshType::Clear();
        mpInterfaceMesh->MeshType::Clear();
        mLocalMeshes.clear();
        mGhostMeshes.clear();
        mInterfaceMeshes.clear();
    }


    std::string Communicator::Info() const{
        std::stringstream buffer;
        buffer << "Communicator";
        return buffer.str();
    }


    void Communicator::PrintInfo(std::ostream& rOstream) const{
        rOstream << this->Info();
    }


    void Communicator::PrintData(std::ostream& rOstream, const std::string& rPrefixString) const{
        rOstream << rPrefixString << "    Local Mesh : " << std::endl;
        mpLocalMesh->Mesh::PrintData(rOstream, rPrefixString + "        ");
        rOstream << rPrefixString << "    Ghost Mesh : " << std::endl;
        mpGhostMesh->Mesh::PrintData(rOstream, rPrefixString + "        ");
        rOstream << rPrefixString << "    Interface Mesh : " << std::endl;
        mpInterfaceMesh->Mesh::PrintData(rOstream, rPrefixString + "        ");
    }


} // namespace Quest