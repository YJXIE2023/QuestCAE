/*---------------------------------
管理分布式 ModelPart 实例的数据通信
---------------------------------*/

#ifndef QUEST_COMMUNICATOR_HPP
#define QUEST_COMMUNICATOR_HPP

// 系统头文件
#include <string>
#include <iostream>
#include <sstream>
#include <cstddef>

// 项目头文件
#include "includes/define.hpp"
#include "includes/condition.hpp"
#include "includes/element.hpp"
#include "includes/mesh.hpp"

namespace Quest{

    class DataCommunicator;

    class QUEST_API(QUEST_CORE) Communicator{
        public:
            QUEST_CLASS_POINTER_DEFINITION(Communicator);

            using IndexType                            = unsigned int;
            using SizeType                             = unsigned int;
            using NodeType                             = Node;
            using PropertiesType                       = Properties;
            using ElementType                          = Element;
            using ConditionType                        = Condition;
            using NeighbourIndicesContainerType        = DenseVector<int>;
            using MeshType                             = Mesh<NodeType, PropertiesType, ElementType, ConditionType>;
            using MeshesContainerType                  = PointerVector<MeshType>;
            using NodesContainerType                   = MeshType::NodesContainerType;
            using NodeIterator                         = MeshType::NodeIterator;
            using NodeConstantIterator                 = MeshType::NodeConstantIterator;
            using PropertiesContainerType              = MeshType::PropertiesContainerType;
            using PropertiesIterator                   = MeshType::PropertiesIterator;
            using PropertiesConstantIterator           = MeshType::PropertiesConstantIterator;
            using ElementsContainerType                = MeshType::ElementsContainerType;
            using ElementsIterator                     = MeshType::ElementIterator;
            using ElementsConstantIterator             = MeshType::ElementConstantIterator;
            using ConditionsContainerType              = MeshType::ConditionsContainerType;
            using ConditionsIterator                   = MeshType::ConditionIterator;
            using ConditionsConstantIterator           = MeshType::ConditionConstantIterator;

        public:
            Communicator();


            Communicator(const DataCommunicator& rDataCommunicator);


            Communicator(const Communicator& rOther);


            virtual ~Communicator() = default;


            virtual Communicator::Pointer Create(const DataCommunicator& rDataCommunicator) const;


            Communicator::Pointer Create() const;


            Communicator& operator = (const Communicator& rOther) = delete;


            virtual bool IsDistributed() const;


            virtual int MyPID() const;


            virtual int TotalProcesses() const;


            SizeType GlobalNumberOfNodes() const;


            SizeType GlobalNumberOfElements() const;


            SizeType GlobalNumberOfConditions() const;


            SizeType GlobalNumberOfMasterSlaveConstraints() const;


            SizeType GetNumberOfColors() const;


            void SetNumberOfColors(SizeType NumberOfColors);


            void AddColors(SizeType NumberOfAddedColors);


            NeighbourIndicesContainerType& NeighbourIndices();


            const NeighbourIndicesContainerType& NeighbourIndices() const;


            void SetLocalMesh(MeshType::Pointer pGivenMesh);


            MeshType::Pointer pLocalMesh();


            MeshType::Pointer pGhostMesh();


            MeshType::Pointer pInterfaceMesh();


            const MeshType::Pointer pLocalMesh() const;


            const MeshType::Pointer pGhostMesh() const;


            const MeshType::Pointer pInterfaceMesh() const;


            MeshType::Pointer pLocalMesh(IndexType ThisIndex);


            MeshType::Pointer pGhostMesh(IndexType ThisIndex);


            MeshType::Pointer pInterfaceMesh(IndexType ThisIndex);


            const MeshType::Pointer pLocalMesh(IndexType ThisIndex) const;


            const MeshType::Pointer pGhostMesh(IndexType ThisIndex) const;


            const MeshType::Pointer pInterfaceMesh(IndexType ThisIndex) const;


            MeshType& LocalMesh();


            MeshType& GhostMesh();


            MeshType& InterfaceMesh();


            const MeshType& LocalMesh() const;


            const MeshType& GhostMesh() const;


            const MeshType& InterfaceMesh() const;


            MeshType& LocalMesh(IndexType ThisIndex);


            MeshType& GhostMesh(IndexType ThisIndex);


            MeshType& InterfaceMesh(IndexType ThisIndex);


            const MeshType& LocalMesh(IndexType ThisIndex) const;


            const MeshType& GhostMesh(IndexType ThisIndex) const;


            const MeshType& InterfaceMesh(IndexType ThisIndex) const;


            MeshesContainerType& LocalMeshes();


            MeshesContainerType& GhostMeshes();


            MeshesContainerType& InterfaceMeshes();


            const MeshesContainerType& LocalMeshes() const;


            const MeshesContainerType& GhostMeshes() const;


            const MeshesContainerType& InterfaceMeshes() const;


            virtual const DataCommunicator& GetDataCommunicator() const;


            virtual bool SynchronizeNodalSolutionStepsData();


            virtual bool SynchronizeDofs();


            virtual bool SynchronizeVariable(const Variable<int>& rThisVariable);


            virtual bool SynchronizeVariable(const Variable<double>& rThisVariable);


            virtual bool SynchronizeVariable(const Variable<bool>& rThisVariable);


            virtual bool SynchronizeVariable(const Variable<Array1d<double,3>>& rThisVariable);


            virtual bool SynchronizeVariable(const Variable<Array1d<double,4>>& rThisVariable);


            virtual bool SynchronizeVariable(const Variable<Array1d<double,6>>& rThisVariable);


            virtual bool SynchronizeVariable(const Variable<Array1d<double,9>>& rThisVariable);


            virtual bool SynchronizeVariable(const Variable<Vector>& rThisVariable);


            virtual bool SynchronizeVariable(const Variable<Matrix>& rThisVariable);


            virtual bool SynchronizeVariable(const Variable<Quaternion<double>>& rThisVariable);


            virtual bool SynchronizeNonHistoricalVariable(const Variable<int>& rThisVariable);


            virtual bool SynchronizeNonHistoricalVariable(const Variable<double>& rThisVariable);


            virtual bool SynchronizeNonHistoricalVariable(const Variable<bool>& rThisVariable);


            virtual bool SynchronizeNonHistoricalVariable(const Variable<Array1d<double,3>>& rThisVariable);


            virtual bool SynchronizeNonHistoricalVariable(const Variable<Array1d<double,4>>& rThisVariable);


            virtual bool SynchronizeNonHistoricalVariable(const Variable<Array1d<double,6>>& rThisVariable);


            virtual bool SynchronizeNonHistoricalVariable(const Variable<Array1d<double,9>>& rThisVariable);


            virtual bool SynchronizeNonHistoricalVariable(const Variable<Vector>& rThisVariable);


            virtual bool SynchronizeNonHistoricalVariable(const Variable<Matrix>& rThisVariable);


            virtual bool SynchronizeNonHistoricalVariable(const Variable<Quaternion<double>>& rThisVariable);


            virtual bool SynchronizeCurrentDataToMax(const Variable<double>& rThisVariable);


            virtual bool SynchronizeNonHistoricalDataToMax(const Variable<double>& rThisVariable);


            virtual bool SynchronizeCurrentDataToAbsMax(const Variable<double>& rThisVariable);


            virtual bool SynchronizeNonHistoricalDataToAbsMax(const Variable<double>& rThisVariable);


            virtual bool SynchronizeCurrentDataToMin(const Variable<double>& rThisVariable);


            virtual bool SynchronizeNonHistoricalDataToMin(const Variable<double>& rThisVariable);


            virtual bool SynchronizeCurrentDataToAbsMin(const Variable<double>& rThisVariable);


            virtual bool SynchronizeNonHistoricalDataToAbsMin(const Variable<double>& rThisVariable);


            virtual bool SynchronizeElementalFlags();


            virtual bool AssembleCurrentData(const Variable<int>& rThisVariable);


            virtual bool AssembleCurrentData(const Variable<double>& rThisVariable);


            virtual bool AssembleCurrentData(const Variable<Array1d<double,3>>& rThisVariable);


            virtual bool AssembleCurrentData(const Variable<Vector>& rThisVariable);


            virtual bool AssembleCurrentData(const Variable<Matrix>& rThisVariable);


            virtual bool AssembleNonHistoricalData(const Variable<int>& rThisVariable);


            virtual bool AssembleNonHistoricalData(const Variable<double>& rThisVariable);


            virtual bool AssembleNonHistoricalData(const Variable<Array1d<double,3>>& rThisVariable);


            virtual bool AssembleNonHistoricalData(const Variable<DenseVector<Array1d<double,3>>>& rThisVariable);


            virtual bool AssembleNonHistoricalData(const Variable<Vector>& rThisVariable);


            virtual bool AssembleNonHistoricalData(const Variable<Matrix>& rThisVariable);


            virtual bool SynchronizeElementalNonHistoricalVariable(const Variable<int>& rThisVariable);


            virtual bool SynchronizeElementalNonHistoricalVariable(const Variable<double>& rThisVariable);


            virtual bool SynchronizeElementalNonHistoricalVariable(const Variable<Array1d<double,3>>& rThisVariable);


            virtual bool SynchronizeElementalNonHistoricalVariable(const Variable<DenseVector<Array1d<double,3>>>& rThisVariable);


            virtual bool SynchronizeElementalNonHistoricalVariable(const Variable<DenseVector<int>>& rThisVariable);


            virtual bool SynchronizeElementalNonHistoricalVariable(const Variable<Vector>& rThisVariable);


            virtual bool SynchronizeElementalNonHistoricalVariable(const Variable<Matrix>& rThisVariable);


            virtual bool TransferObjects(std::vector<NodesContainerType>& SendObject, std::vector<NodesContainerType>& ReceiveObject);


            virtual bool TransferObjects(std::vector<ElementsContainerType>& SendObject, std::vector<ElementsContainerType>& ReceiveObject);


            virtual bool TransferObjects(std::vector<ConditionsContainerType>& SendObject, std::vector<ConditionsContainerType>& ReceiveObject);


            virtual bool TransferObjects(std::vector<NodesContainerType>& SendObject, std::vector<NodesContainerType>& ReceiveObject, Quest::Serializer& particleSerializer);


            virtual bool TransferObjects(std::vector<ElementsContainerType>& SendObject, std::vector<ElementsContainerType>& ReceiveObject, Quest::Serializer& particleSerializer);


            virtual bool TransferObjects(std::vector<ConditionsContainerType>& SendObject, std::vector<ConditionsContainerType>& ReceiveObject, Quest::Serializer& particleSerializer);


            virtual bool SynchronizeOrNodalFlags(const Flags& TheFlags);


            virtual bool SynchronizeAndNodalFlags(const Flags& TheFlags);


            virtual bool SynchronizeNodalFlags();


            void Clear();


            virtual std::string Info() const;


            virtual void PrintInfo(std::ostream& rOStream) const;


            virtual void PrintData(std::ostream& rOStream, const std::string& rPrefixString = "") const;

        protected:

        private:

        private:
            SizeType mNumberOfColors;
            NeighbourIndicesContainerType mNeighbourIndices;
            MeshType::Pointer mpLocalMesh;
            MeshType::Pointer mpGhostMesh;
            MeshType::Pointer mpInterfaceMesh;
            MeshesContainerType mLocalMeshes;
            MeshesContainerType mGhostMeshes;
            MeshesContainerType mInterfaceMeshes;
            const DataCommunicator& mrDataCommunicator;

    };

    inline std::istream& operator >> (std::istream& rIstream, Communicator& rThis);

    inline std::ostream& operator << (std::ostream& rOstream, const Communicator& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }

} // namespace Quest

#endif //QUEST_COMMUNICATOR_HPP