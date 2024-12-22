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

    /**
     * @class Communicator
     * @brief 管理分布式 ModelPart 实例的通信
     * @details 类中仅保存通信所需的数据，实际的通信功能由派生类 MPICommunicator 实现
     */
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
            /**
             * @brief 构造函数
             */
            Communicator();

            /**
             * @brief 构造函数
             */
            Communicator(const DataCommunicator& rDataCommunicator);

            /**
             * @brief 拷贝构造函数
             */
            Communicator(const Communicator& rOther);

            /**
             * @brief 析构函数
             */
            virtual ~Communicator() = default;

            /**
             * @brief 创建一个Communicator指针
             */
            virtual Communicator::Pointer Create(const DataCommunicator& rDataCommunicator) const;

            /**
             * @brief 创建一个Communicator指针
             */
            Communicator::Pointer Create() const;


            Communicator& operator = (const Communicator& rOther) = delete;

            /**
             * @brief 判断是否为分布式模型
             */
            virtual bool IsDistributed() const;

            /**
             * @brief 获取当前进程的ID
             */
            virtual int MyPID() const;

            /**
             * @brief 获取总进程数
             */
            virtual int TotalProcesses() const;

            /**
             * @brief 返回在所有进程中共享的网格节点总数
             */
            SizeType GlobalNumberOfNodes() const;

            /**
             * @brief 返回在所有进程中共享的单元总数
             */
            SizeType GlobalNumberOfElements() const;

            /****
             * @brief 返回在所有进程中共享的条件总数
             */
            SizeType GlobalNumberOfConditions() const;

            /**
             * @brief 返回主-从约束的数量
             */
            SizeType GlobalNumberOfMasterSlaveConstraints() const;

            /**
             * @brief 返回颜色的数量
             */
            SizeType GetNumberOfColors() const;

            /**
             * @brief 设置颜色的数量
             */
            void SetNumberOfColors(SizeType NumberOfColors);

            /**
             * @brief 添加颜色
             */
            void AddColors(SizeType NumberOfAddedColors);

            /**
             * @brief 返回邻居的索引
             */
            NeighbourIndicesContainerType& NeighbourIndices();

            /**
             * @brief 返回邻居的索引
             */
            const NeighbourIndicesContainerType& NeighbourIndices() const;

            /**
             * @brief 设置本地Mesh
             */
            void SetLocalMesh(MeshType::Pointer pGivenMesh);

            /**
             * @brief 返回本地Mesh指针
             */
            MeshType::Pointer pLocalMesh();

            /**
             * @brief 返回幽灵Mesh指针
             */
            MeshType::Pointer pGhostMesh();

            /**
             * @brief 返回表面Mesh指针
             */
            MeshType::Pointer pInterfaceMesh();

            /**
             * @brief 返回本地Mesh指针
             */
            const MeshType::Pointer pLocalMesh() const;

            /**
             * @brief 返回幽灵Mesh指针
             */
            const MeshType::Pointer pGhostMesh() const;

            /**
             * @brief 返回表面Mesh指针
             */
            const MeshType::Pointer pInterfaceMesh() const;

            /**
             * @brief 通过索引从本地Mesh集中中获取本地Mesh指针
             */
            MeshType::Pointer pLocalMesh(IndexType ThisIndex);

            /**
             * @brief 通过索引从幽灵Mesh集合中获取幽灵Mesh指针
             */
            MeshType::Pointer pGhostMesh(IndexType ThisIndex);

            /**
             * @brief 通过索引从表面Mesh集合中获取表面Mesh指针
             */
            MeshType::Pointer pInterfaceMesh(IndexType ThisIndex);

            /**
             * @brief 通过索引从本地Mesh集中中获取本地Mesh指针
             */
            const MeshType::Pointer pLocalMesh(IndexType ThisIndex) const;

            /**
             * @brief 通过索引从幽灵Mesh集合中获取幽灵Mesh指针
             */
            const MeshType::Pointer pGhostMesh(IndexType ThisIndex) const;

            /**
             * @brief 通过索引从表面Mesh集合中获取表面Mesh指针
             */
            const MeshType::Pointer pInterfaceMesh(IndexType ThisIndex) const;

            /**
             * @brief 返回本地Mesh对象
             */
            MeshType& LocalMesh();

            /**
             * @brief 返回幽灵Mesh对象
             */
            MeshType& GhostMesh();

            /**
             * @brief 返回接口Mesh对象
             */
            MeshType& InterfaceMesh();

            /**
             * @brief 返回本地Mesh对象
             */
            const MeshType& LocalMesh() const;

            /**
             * @brief 返回幽灵Mesh对象
             */
            const MeshType& GhostMesh() const;

            /**
             * @brief 返回接口Mesh对象
             */
            const MeshType& InterfaceMesh() const;

            /**
             * @brief 通过索引从本地Mesh集合中获取本地Mesh对象
             */
            MeshType& LocalMesh(IndexType ThisIndex);

            /**
             * @brief 通过索引从幽灵Mesh集合中获取幽灵Mesh对象
             */
            MeshType& GhostMesh(IndexType ThisIndex);

            /**
             * @brief 通过索引从表面Mesh集合中获取表面Mesh对象
             */
            MeshType& InterfaceMesh(IndexType ThisIndex);

            /**
             * @brief 通过索引从本地Mesh集合中获取本地Mesh对象
             */
            const MeshType& LocalMesh(IndexType ThisIndex) const;

            /**
             * @brief 通过索引从幽灵Mesh集合中获取幽灵Mesh对象
             */
            const MeshType& GhostMesh(IndexType ThisIndex) const;

            /**
             * @brief 通过索引从表面Mesh集合中获取表面Mesh对象
             */
            const MeshType& InterfaceMesh(IndexType ThisIndex) const;

            /**
             * @brief 返回本地Mesh集合
             */
            MeshesContainerType& LocalMeshes();

            /**
             * @brief 返回幽灵Mesh集合
             */
            MeshesContainerType& GhostMeshes();

            /**
             * @brief 返回表面Mesh集合
             */
            MeshesContainerType& InterfaceMeshes();

            /**
             * @brief 返回本地Mesh集合
             */
            const MeshesContainerType& LocalMeshes() const;

            /**
             * @brief 返回幽灵Mesh集合
             */
            const MeshesContainerType& GhostMeshes() const;

            /**
             * @brief 返回表面Mesh集合
             */
            const MeshesContainerType& InterfaceMeshes() const;

            /**
             * @brief 返回数据通信器
             */
            virtual const DataCommunicator& GetDataCommunicator() const;

            /**
             * @brief 同步每个计算步骤中的节点数据
             */
            virtual bool SynchronizeNodalSolutionStepsData();

            /**
             * @brief 同步自由度数据
             */
            virtual bool SynchronizeDofs();

            /**
             * @brief 同步变量
             */
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

            /**
             * @brief 同步不同类型的非历史变量
             */
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

            /**
             * @brief 将节点解步骤数据中的变量同步到所有进程中的最大值
             */
            virtual bool SynchronizeCurrentDataToMax(const Variable<double>& rThisVariable);

            /**
             * @brief 将非历史变量同步到所有进程中的最大值
             */
            virtual bool SynchronizeNonHistoricalDataToMax(const Variable<double>& rThisVariable);

            /**
             * @brief 将节点解步骤数据中的变量同步到所有进程中的绝对值最大值
             */
            virtual bool SynchronizeCurrentDataToAbsMax(const Variable<double>& rThisVariable);

            /**
             * @brief 将非历史变量同步到所有进程中的绝对值最大值
             */
            virtual bool SynchronizeNonHistoricalDataToAbsMax(const Variable<double>& rThisVariable);

            /**
             * @brief 将节点解步骤数据中的变量同步到所有进程中的最小值
             */
            virtual bool SynchronizeCurrentDataToMin(const Variable<double>& rThisVariable);

            /**
             * @brief 将非历史变量同步到所有进程中的最小值
             */
            virtual bool SynchronizeNonHistoricalDataToMin(const Variable<double>& rThisVariable);

            /**
             * @brief 将节点解步骤数据中的变量同步到所有进程中的绝对值最小值
             */
            virtual bool SynchronizeCurrentDataToAbsMin(const Variable<double>& rThisVariable);

            /**
             * @brief 将非历史变量同步到所有进程中的绝对值最小值
             */
            virtual bool SynchronizeNonHistoricalDataToAbsMin(const Variable<double>& rThisVariable);

            /**
             * @brief 同步单元标志
             */
            virtual bool SynchronizeElementalFlags();

            /**
             * @brief 汇总不同类型的变量数据
             */
            virtual bool AssembleCurrentData(const Variable<int>& rThisVariable);


            virtual bool AssembleCurrentData(const Variable<double>& rThisVariable);


            virtual bool AssembleCurrentData(const Variable<Array1d<double,3>>& rThisVariable);


            virtual bool AssembleCurrentData(const Variable<Vector>& rThisVariable);


            virtual bool AssembleCurrentData(const Variable<Matrix>& rThisVariable);

            /**
             * @brief 汇总不同类型的非历史变量数据
             */
            virtual bool AssembleNonHistoricalData(const Variable<int>& rThisVariable);


            virtual bool AssembleNonHistoricalData(const Variable<double>& rThisVariable);


            virtual bool AssembleNonHistoricalData(const Variable<Array1d<double,3>>& rThisVariable);


            virtual bool AssembleNonHistoricalData(const Variable<DenseVector<Array1d<double,3>>>& rThisVariable);


            virtual bool AssembleNonHistoricalData(const Variable<Vector>& rThisVariable);


            virtual bool AssembleNonHistoricalData(const Variable<Matrix>& rThisVariable);

            /**
             * @brief 同步不同类型的单元非历史变量
             */
            virtual bool SynchronizeElementalNonHistoricalVariable(const Variable<int>& rThisVariable);


            virtual bool SynchronizeElementalNonHistoricalVariable(const Variable<double>& rThisVariable);


            virtual bool SynchronizeElementalNonHistoricalVariable(const Variable<Array1d<double,3>>& rThisVariable);


            virtual bool SynchronizeElementalNonHistoricalVariable(const Variable<DenseVector<Array1d<double,3>>>& rThisVariable);


            virtual bool SynchronizeElementalNonHistoricalVariable(const Variable<DenseVector<int>>& rThisVariable);


            virtual bool SynchronizeElementalNonHistoricalVariable(const Variable<Vector>& rThisVariable);


            virtual bool SynchronizeElementalNonHistoricalVariable(const Variable<Matrix>& rThisVariable);

            /**
             * @brief 在不同计算单元之间传输对象数据
             */
            virtual bool TransferObjects(std::vector<NodesContainerType>& SendObject, std::vector<NodesContainerType>& ReceiveObject);


            virtual bool TransferObjects(std::vector<ElementsContainerType>& SendObject, std::vector<ElementsContainerType>& ReceiveObject);


            virtual bool TransferObjects(std::vector<ConditionsContainerType>& SendObject, std::vector<ConditionsContainerType>& ReceiveObject);


            virtual bool TransferObjects(std::vector<NodesContainerType>& SendObject, std::vector<NodesContainerType>& ReceiveObject, Quest::Serializer& particleSerializer);


            virtual bool TransferObjects(std::vector<ElementsContainerType>& SendObject, std::vector<ElementsContainerType>& ReceiveObject, Quest::Serializer& particleSerializer);


            virtual bool TransferObjects(std::vector<ConditionsContainerType>& SendObject, std::vector<ConditionsContainerType>& ReceiveObject, Quest::Serializer& particleSerializer);

            /**
             * @brief 同步节点标志(or)
             */
            virtual bool SynchronizeOrNodalFlags(const Flags& TheFlags);

            /**
             * @brief 同步节点标志(and)
             */
            virtual bool SynchronizeAndNodalFlags(const Flags& TheFlags);

            /**
             * @brief 同步节点标志
             */
            virtual bool SynchronizeNodalFlags();

            /**
             * @brief 清除所有数据
             */
            void Clear();


            virtual std::string Info() const;


            virtual void PrintInfo(std::ostream& rOStream) const;


            virtual void PrintData(std::ostream& rOStream, const std::string& rPrefixString = "") const;

        protected:

        private:

        private:
            /**
             * @brief 颜色的数量，并行计算中颜色用于分组不同的计算任务
             */
            SizeType mNumberOfColors;

            /**
             * @brief 邻居的索引，用于确定每个节点的邻居节点
             * @details 邻居进程的索引通常用于标识需要交换数据的进程
             */
            NeighbourIndicesContainerType mNeighbourIndices;

            /**
             * @brief 指向本地Mesh的指针
             */
            MeshType::Pointer mpLocalMesh;

            /**
             * @brief 指向幽灵Mesh的指针
             * @details 幽灵Mesh用于存储与本地进程相关，但不属于当前进程计算域的一部分的Mesh
             */
            MeshType::Pointer mpGhostMesh;

            /**
             * @brief 指向接口Mesh的指针
             * @details 接口Mesh用于存储不同计算域之间的交界处的网格元素
             */
            MeshType::Pointer mpInterfaceMesh;

            /**
             * @brief 本地Mesh的集合
             */
            MeshesContainerType mLocalMeshes;

            /**
             * @brief 幽灵Mesh的集合
             */
            MeshesContainerType mGhostMeshes;

            /**
             * @brief 接口Mesh的集合
             */
            MeshesContainerType mInterfaceMeshes;

            /**
             * @brief 管理并行计算中不同进程间数据交换的类，负责进行进程间的通信
             */
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