/*----------------------------------------
model_part.hpp类的实现代码
----------------------------------------*/

// 系统头文件
#include <sstream>

// 项目头文件
#include "includes/define.hpp"
#include "includes/model_part.hpp"
#include "includes/exceptions.hpp"
#include "utilities/parallel_utilities.hpp"

namespace Quest{

    QUEST_CREATE_LOCAL_FLAG(ModelPart, ALL_ENTITIES, 0);
    QUEST_CREATE_LOCAL_FLAG(ModelPart, OVERRITE_ENTITIES, 1);


    ModelPart::ModelPart(VariablesList::Pointer pVariablesList, Model& rOwnerModel):
        ModelPart("Default", pVariablesList, rOwnerModel)
    {}


    ModelPart::ModelPart(
        const std::string& NewName,
        VariablesList::Pointer pVariablesList,
        Model& rOwnerModel
    ):  ModelPart(NewName, 1, pVariablesList, rOwnerModel)
    {}


    ModelPart::ModelPart(
        const std::string& NewName,
        IndexType NewBufferSize,
        VariablesList::Pointer pVariablesList,
        Model& rOwnerModel
    ):
        DataValueContainer(),
        Flags(),
        mBufferSize(NewBufferSize),
        mpProcessInfo(new ProcessInfo()),
        mGeometries(),
        mpVariablesList(pVariablesList),
        mpCommunicator(new Communicator),
        mpParentModelPart(nullptr),
        mSubModelParts(),
        mrModel(rOwnerModel)
    {
        QUEST_ERROR_IF(NewName.empty()) << "Please don't use empty name (\"\") when creating a ModelPart" << std::endl;

        QUEST_ERROR_IF_NOT(NewName.find('.') == std::string::npos) << "Please don't use names containing (\".\") when creating a ModelPart (used in \"" << NewName << "\")" << std::endl;

        mName = NewName;
        MeshType mesh;
        mMeshes.push_back(Quest::make_shared<MeshType>(mesh.Clone()));
        mpCommunicator->SetLocalMesh(pGetMesh());
    }


    ModelPart::~ModelPart(){
        Clear();
    }


    void ModelPart::Clear(){
        QUEST_TRY

        for(auto& r_sub_model_part : mSubModelParts){
            r_sub_model_part.Clear();
        }

        mSubModelParts.clear();

        for(auto& r_mesh : mMeshes){
            r_mesh.Clear();
        }

        mMeshes.clear();
        mMeshes.emplace_back(Quest::make_shared<MeshType>());


        mGeometries.Clear();
        mTables.clear();
        mpCommunicator->Clear();
        this->AssignFlags(Flags());

        QUEST_CATCH("")
    }


    void ModelPart::Reset(){
        QUEST_TRY

        Clear();

        mpVariablesList = Quest::make_intrusive<VariablesList>();
        mpProcessInfo = Quest::make_shared<ProcessInfo>();
        mBufferSize = 0;

        QUEST_CATCH("")
    }


    ModelPart::IndexType ModelPart::CreateSolutionStep(){
        QUEST_THROW_ERROR(std::logic_error, "This method needs updating and is not orking. Pooyan", "")
        return 0;
    }


    ModelPart::IndexType ModelPart::CloneSolutionStep(){
        QUEST_ERROR_IF(IsSubModelPart()) << "Calling the method of the sub model part "
            << Name() << " please call the one of the root model part: "
            << GetRootModelPart().Name() << std::endl;

        const int nnodes = static_cast<int>(Nodes().size());
        auto nodes_begin = NodesBegin();
        #pragma omp parallel for firstprivate(nodes_begin,nnodes)
        for(int i = 0; i < nnodes; i++){
            auto node_iterator = nodes_begin + i;
            node_iterator->CloneSolutionStepData();
        }

        mpProcessInfo->CloneSolutionStepInfo();

        mpProcessInfo->ClearHistory(mBufferSize);

        return 0;
    }


    ModelPart::IndexType ModelPart::CloneTimeStep(){
        QUEST_ERROR_IF(IsSubModelPart()) << "Calling the method of the sub model part "
            << Name() << " please call the one of the root model part: "
            << GetRootModelPart().Name() << std::endl;

        IndexType new_index = CloneSolutionStep();
        mpProcessInfo->SetAsTimeStepInfo();

        return new_index;
    }


    ModelPart::IndexType ModelPart::CreateTimeStep(double NewTime){
        QUEST_ERROR_IF(IsSubModelPart()) << "Calling the method of the sub model part "
            << Name() << " please call the one of the root model part: "
            << GetRootModelPart().Name() << std::endl;

        IndexType new_index = CreateSolutionStep();
        mpProcessInfo->SetAsTimeStepInfo(NewTime);

        return new_index;
    }


    ModelPart::IndexType ModelPart::CloneTimeStep(double NewTime){
        QUEST_ERROR_IF(IsSubModelPart()) << "Calling the method of the sub model part "
            << Name() << " please call the one of the root model part: "
            << GetRootModelPart().Name() << std::endl;

        IndexType new_index = CloneSolutionStep();
        mpProcessInfo->SetAsTimeStepInfo(NewTime);

        return new_index;
    }


    void ModelPart::OverwriteSolutionStepData(IndexType SourceSolutionStepIndex, IndexType DestinationSourceSolutionStepIndex){
        QUEST_ERROR_IF(IsSubModelPart()) << "Calling the method of the sub model part "
            << Name() << " please call the one of the root model part: "
            << GetRootModelPart().Name() << std::endl;

        for(NodeIterator node_iterator = NodesBegin(); node_iterator != NodesEnd(); ++node_iterator){
            node_iterator->OverwriteSolutionStepData(SourceSolutionStepIndex, DestinationSourceSolutionStepIndex);
        }
    }


    void ModelPart::ReduceTimeStep(ModelPart& rModelPart, double NewTime){
        QUEST_TRY

        QUEST_ERROR_IF(IsSubModelPart()) << "Calling the method of the sub model part "
            << Name() << " please call the one of the root model part: "
            << GetRootModelPart().Name() << std::endl;

        rModelPart.OverwriteSolutionStepData(1,0);
        rModelPart.GetProcessInfo().SetCurrentTime(NewTime);

        QUEST_CATCH("error in reducing the time step")
    }


    void ModelPart::AddNode(ModelPart::NodeType::Pointer pNewNode, ModelPart::IndexType ThisIndex){
        if(IsSubModelPart()){
            mpParentModelPart->AddNode(pNewNode, ThisIndex);
            GetMesh(ThisIndex).AddNode(pNewNode);
        } else {
            auto existing_node_it = this->GetMesh(ThisIndex).Nodes().find(pNewNode->Id());
            if(existing_node_it == GetMesh(ThisIndex).NodesEnd()){
                GetMesh(ThisIndex).AddNode(pNewNode);
            } else {
                if(&(*existing_node_it) != (pNewNode.get())){
                    QUEST_ERROR << "attempting to add pNewNode with Id :" << pNewNode->Id() << ", unfortunately a (different) node with the same Id already exists" << std::endl;
                }
            }
        }
    }


    void ModelPart::AddNodes(const std::vector<IndexType>& NodeIds, IndexType ThisIndex){
        QUEST_TRY
        if(IsSubModelPart()){
            ModelPart* root_model_part = &this->GetRootModelPart();
            ModelPart::NodesContainerType aux;
            aux.reserve(NodeIds.size());
            for(unsigned int i=0; i<NodeIds.size(); ++i){
                ModelPart::NodesContainerType::iterator it = root_model_part->Nodes().find(NodeIds[i]);
                if(it != root_model_part->NodesEnd()){
                    aux.push_back(*(it.base()));
                } else {
                    QUEST_ERROR << "while adding nodes to submodelpart, the node with Id " << NodeIds[i] << " does not exist in the root model part";
                }
            }

            ModelPart* current_part = this;
            while(current_part->IsSubModelPart()){
                for(auto it = aux.begin(); it!= aux.end(); ++it){
                    current_part->Nodes().push_back(*(it.base()));
                }

                current_part->Nodes().Unique();

                current_part = &(current_part->GetParentModelPart());
            }
        }

        QUEST_CATCH("")
    }


    ModelPart::NodeType::Pointer ModelPart::CreateNewNode(int Id, double x, double y, double z, VariablesList::Pointer pNewVariablesList, ModelPart::IndexType ThisIndex){
        QUEST_TRY
        if(IsSubModelPart()){
            NodeType::Pointer p_new_node = mpParentModelPart->CreateNewNode(Id, x, y, z, pNewVariablesList, ThisIndex);
            GetMesh(ThisIndex).AddNode(p_new_node);
            return p_new_node;
        }

        auto& root_nodes = this->Nodes();
        auto existing_node_it = root_nodes.find(Id);
        if(existing_node_it != root_nodes.end()){
            double distance = std::sqrt(std::pow(x - existing_node_it->X(), 2) + std::pow(y - existing_node_it->Y(), 2) + std::pow(z - existing_node_it->Z(), 2));

            QUEST_ERROR_IF(distance > std::numeric_limits<double>::epsilon()*1000)
                << "trying to create a node with Id " << Id << " however a node with the same Id already exists in the root model part. Existing node coordinates are " << existing_node_it->Coordinates() << " coordinates of the nodes we are attempting to create are :" << x << " " << y << " " << z;

            return *(existing_node_it.base());
        }

        NodeType::Pointer p_new_node = Quest::make_intrusive<NodeType>(Id, x, y, z);

        p_new_node->SetSolutionStepVariablesList(pNewVariablesList);

        p_new_node->SetBufferSize(mBufferSize);

        GetMesh(ThisIndex).AddNode(p_new_node);

        return p_new_node;
        QUEST_CATCH("")
    }


    ModelPart::NodeType::Pointer ModelPart::CreateNewNode(ModelPart::IndexType Id, double x, double y, double z, ModelPart::IndexType ThisIndex){
        return CreateNewNode(Id, x, y, z, mpVariablesList, ThisIndex);
    }


    ModelPart::NodeType::Pointer ModelPart::CreateNewNode(ModelPart::IndexType Id, double x, double y, double z, double* pThisData, ModelPart::IndexType ThisIndex){
        QUEST_TRY
        if(IsSubModelPart()){
            NodeType::Pointer p_new_node = mpParentModelPart->CreateNewNode(Id, x, y, z, pThisData, ThisIndex);
            GetMesh(ThisIndex).AddNode(p_new_node);
            return p_new_node;
        }

        NodesContainerType::iterator existing_node_it = this->GetMesh(ThisIndex).Nodes().find(Id);
        if(existing_node_it != GetMesh(ThisIndex).NodesEnd()){
            double distance = std::sqrt(std::pow(x - existing_node_it->X(), 2) + std::pow(y - existing_node_it->Y(), 2) + std::pow(z - existing_node_it->Z(), 2));

            QUEST_ERROR_IF(distance > std::numeric_limits<double>::epsilon()*1000)
                << "trying to create a node with Id " << Id << " however a node with the same Id already exists in the root model part. Existing node coordinates are " << existing_node_it->Coordinates() << " coordinates of the nodes we are attempting to create are :" << x << " " << y << " " << z;

            return *(existing_node_it.base());
        }

        NodeType::Pointer p_new_node = Quest::make_intrusive<NodeType>(Id, x, y, z, mpVariablesList, pThisData, mBufferSize);
        GetMesh(ThisIndex).AddNode(p_new_node);

        return p_new_node;
        QUEST_CATCH("")
    }


    ModelPart::NodeType::Pointer ModelPart::CreateNewNode(ModelPart::IndexType NodeId, const ModelPart::NodeType& rSourceNode, ModelPart::IndexType ThisIndex){
        return CreateNewNode(NodeId, rSourceNode.X(), rSourceNode.Y(), rSourceNode.Z(), mpVariablesList, ThisIndex);
    }


    void ModelPart::AssignNode(ModelPart::NodeType::Pointer pThisNode, ModelPart::IndexType ThisIndex){
        if(IsSubModelPart()){
            mpParentModelPart->AssignNode(pThisNode, ThisIndex);

            GetMesh(ThisIndex).AddNode(pThisNode);

            return;
        }

        pThisNode->SetSolutionStepVariablesList(mpVariablesList);

        pThisNode->SetBufferSize(mBufferSize);

        GetMesh(ThisIndex).AddNode(pThisNode);
    }


    void ModelPart::RemoveNode(ModelPart::IndexType NodeId, ModelPart::IndexType ThisIndex){
        GetMesh(ThisIndex).RemoveNode(NodeId);

        for(SubModelPartIterator i_sub_model_part = SubModelPartsBegin(); i_sub_model_part != SubModelPartsEnd(); ++i_sub_model_part){
            i_sub_model_part->RemoveNode(NodeId, ThisIndex);
        }
    }


    void ModelPart::RemoveNode(ModelPart::NodeType& ThisNode, ModelPart::IndexType ThisIndex){
        GetMesh(ThisIndex).RemoveNode(ThisNode);

        for(SubModelPartIterator i_sub_model_part = SubModelPartsBegin(); i_sub_model_part != SubModelPartsEnd(); ++i_sub_model_part){
            i_sub_model_part->RemoveNode(ThisNode, ThisIndex);
        }
    }


    void ModelPart::RemoveNode(ModelPart::NodeType::Pointer pThisNode, ModelPart::IndexType ThisIndex){
        GetMesh(ThisIndex).RemoveNode(pThisNode);

        for(SubModelPartIterator i_sub_model_part = SubModelPartsBegin(); i_sub_model_part != SubModelPartsEnd(); ++i_sub_model_part){
            i_sub_model_part->RemoveNode(pThisNode, ThisIndex);
        }
    }


    void ModelPart::RemoveNodeFromAllLevels(ModelPart::IndexType NodeId, ModelPart::IndexType ThisIndex){
        if(IsSubModelPart()){
            mpParentModelPart->RemoveNodeFromAllLevels(NodeId, ThisIndex);
            return;
        }
        RemoveNode(NodeId, ThisIndex);
    }


    void ModelPart::RemoveNodeFromAllLevels(ModelPart::NodeType& ThisNode, ModelPart::IndexType ThisIndex){
        if(IsSubModelPart()){
            mpParentModelPart->RemoveNode(ThisNode, ThisIndex);
            return;
        }
        RemoveNode(ThisNode, ThisIndex);
    }


    void ModelPart::RemoveNodeFromAllLevels(ModelPart::NodeType::Pointer pThisNode, ModelPart::IndexType ThisIndex){
        if(IsSubModelPart()){
            mpParentModelPart->RemoveNode(pThisNode, ThisIndex);
            return;
        }
        RemoveNode(pThisNode, ThisIndex);
    }


    void ModelPart::RemoveNodes(Flags IdentifierFlag){
        auto remove_nodes_from_mesh = [&](ModelPart::MeshType& r_mesh){
            const unsigned int nnodes = r_mesh.Nodes().size();
            unsigned int erase_count = 0;
            #pragma omp parallel for reduction(+:erase_count)
            for(int i = 0; i < static_cast<int>(nnodes); ++i){
                ModelPart::NodesContainerType::iterator i_node = r_mesh.NodesBegin() + i;

                if(i_node->IsNot(IdentifierFlag)){
                    ++erase_count;
                }
            }

            ModelPart::NodesContainerType temp_nodes_container;
            temp_nodes_container.reserve(r_mesh.Nodes().size() - erase_count);

            temp_nodes_container.swap(r_mesh.Nodes());

            for(ModelPart::NodesContainerType::iterator i_node = temp_nodes_container.begin(); i_node != temp_nodes_container.end(); ++i_node){
                if(i_node->IsNot(IdentifierFlag)){
                    (r_mesh.Nodes()).push_back(std::move(*(i_node.base())));
                }
            }
        };

        for(auto& r_mesh : this->GetMeshes()){
            remove_nodes_from_mesh(r_mesh);
        }

        if(IsDistributed()){
            this->GetCommunicator().SynchronizeOrNodalFlags(IdentifierFlag);

            remove_nodes_from_mesh(this->GetCommunicator().LocalMesh());
            for(auto& r_mesh: this->GetCommunicator().LocalMeshes()){
                remove_nodes_from_mesh(r_mesh);
            }

            remove_nodes_from_mesh(this->GetCommunicator().GhostMesh());
            for(auto& r_mesh: this->GetCommunicator().GhostMeshes()){
                remove_nodes_from_mesh(r_mesh);
            }
        
            remove_nodes_from_mesh(this->GetCommunicator().InterfaceMesh());
            for(auto& r_mesh: this->GetCommunicator().InterfaceMeshes()){
                remove_nodes_from_mesh(r_mesh);
            }
        }

        for(auto& r_sub_model_part : SubModelParts()){
            r_sub_model_part.RemoveNodes(IdentifierFlag);
        }
    }


    void ModelPart::RemoveNodesFromAllLevels(Flags IdentifierFlag){
        ModelPart& root_model_part = GetRootModelPart();
        root_model_part.RemoveNodes(IdentifierFlag);
    }


    ModelPart& ModelPart::GetRootModelPart(){
        if(IsSubModelPart()){
            return mpParentModelPart->GetRootModelPart();
        } else {
            return *this;
        }
    }


    const ModelPart& ModelPart::GetRootModelPart() const{
        if(IsSubModelPart()){
            return mpParentModelPart->GetRootModelPart();
        } else {
            return *this;
        }
    }


    void ModelPart::SetNodalSolutionStepVariablesList(){
        QUEST_ERROR_IF(IsSubModelPart()) << "Calling the method of the sub model part "
            << Name() << " please call the one of the root model part: "
            << GetRootModelPart().Name() << std::endl;

        auto& r_nodes_array = this->Nodes();
        block_for_each(r_nodes_array,[&](NodeType& rNode) {
            rNode.SetSolutionStepVariablesList(mpVariablesList);
        });
    }

    
    void ModelPart::AddTable(ModelPart::IndexType TableId, ModelPart::TableType::Pointer pNewTable){
        if(IsSubModelPart()){
            mpParentModelPart->AddTable(TableId, pNewTable);
        }

        mTables.insert(TableId, pNewTable);
    }


    void ModelPart::RemoveTable(ModelPart::IndexType TableId){
        mTables.erase(TableId);

        for(SubModelPartIterator i_sub_model_part = SubModelPartsBegin(); i_sub_model_part != SubModelPartsEnd(); ++i_sub_model_part){
            i_sub_model_part->RemoveTable(TableId);
        }
    }


    void ModelPart::RemoveTableFromAllLevels(ModelPart::IndexType TableId){
        if(IsSubModelPart()){
            mpParentModelPart->RemoveTableFromAllLevels(TableId);
            return;
        }
        RemoveTable(TableId);
    }


    ModelPart::SizeType ModelPart::NumberOfProperties(IndexType ThisIndex) const{
        return GetMesh(ThisIndex).NumberOfProperties();
    }


    void ModelPart::AddProperties(ModelPart::PropertiesType::Pointer pNewProperties, ModelPart::IndexType ThisIndex){
        if(IsSubModelPart()){
            mpParentModelPart->AddProperties(pNewProperties, ThisIndex);
        }

        auto existing_prop_it = GetMesh(ThisIndex).Properties().find(pNewProperties->Id());
        if(existing_prop_it != GetMesh(ThisIndex).Properties().end()){
            QUEST_ERROR_IF( &(*existing_prop_it) != pNewProperties.get() )
                << "trying to add a property with existing Id within the model part : " << Name() << ", property Id is :" << pNewProperties->Id();
        } else {
            GetMesh(ThisIndex).AddProperties(pNewProperties);
        }
    }


    bool ModelPart::HasProperties(
        IndexType PropertiesId,
        IndexType ThisIndex
    ) const {
        auto pprop_it = GetMesh(ThisIndex).Properties().find(PropertiesId);
        if(pprop_it != GetMesh(ThisIndex).Properties().end()){
            return true;
        }

        return false;
    }


    bool ModelPart::RecursivelyHasProperties(
        IndexType PropertiesId,
        IndexType ThisIndex
    ) const {
        auto pprop_it = GetMesh(ThisIndex).Properties().find(PropertiesId);
        if(pprop_it != GetMesh(ThisIndex).Properties().end()){
            return true;
        } else {
            if(IsSubModelPart()){
                return mpParentModelPart->RecursivelyHasProperties(PropertiesId, ThisIndex);
            } else {
                return false;
            }
        }
    }


    ModelPart::PropertiesType::Pointer ModelPart::CreateNewProperties(
        IndexType PropertiesId,
        IndexType ThisIndex
    ) {
        auto pprop_it = GetMesh(ThisIndex).Properties().find(PropertiesId);
        if(pprop_it != GetMesh(ThisIndex).Properties().end()){
            QUEST_ERROR << "Property #" << PropertiesId << " already existing. Please use pGetProperties() instead" << std::endl;
        } else {
            if(IsSubModelPart()){
                PropertiesType::Pointer pprop = mpParentModelPart->CreateNewProperties(PropertiesId, ThisIndex);
                GetMesh(ThisIndex).AddProperties(pprop);
                return pprop;
            } else {
                PropertiesType::Pointer pnew_property = Quest::make_shared<PropertiesType>(PropertiesId);
                GetMesh(ThisIndex).AddProperties(pnew_property);
                return pnew_property;
            }
        }
    }


    ModelPart::PropertiesType::Pointer ModelPart::pGetProperties(
        IndexType PropertiesId,
        IndexType ThisIndex
    ){
        auto pprop_it = GetMesh(ThisIndex).Properties().find(PropertiesId);
        if(pprop_it != GetMesh(ThisIndex).Properties().end()){
            return *(pprop_it.base());
        } else {
            if(IsSubModelPart()){
                PropertiesType::Pointer pprop = mpParentModelPart->pGetProperties(PropertiesId, ThisIndex);
                GetMesh(ThisIndex).AddProperties(pprop);
                return pprop;
            } else {
                QUEST_WARNING("ModelPart") << "Property " << PropertiesId << " does not exist!. Creating and adding new property. Please use CreateNewProperties() instead" << std::endl;
                PropertiesType::Pointer pnew_property = Quest::make_shared<PropertiesType>(PropertiesId);
                GetMesh(ThisIndex).AddProperties(pnew_property);
                return pnew_property;
            }
        }
    }


    const ModelPart::PropertiesType::Pointer ModelPart::pGetProperties(
        IndexType PropertiesId,
        IndexType ThisIndex
    ) const {
        auto pprop_it = GetMesh(ThisIndex).Properties().find(PropertiesId);
        if(pprop_it != GetMesh(ThisIndex).Properties().end()){
            return *(pprop_it.base());
        } else {
            if(IsSubModelPart()){
                PropertiesType::Pointer pprop = mpParentModelPart->pGetProperties(PropertiesId, ThisIndex);
                return pprop;
            } else {
                QUEST_ERROR << "Property " << PropertiesId << " does not exist!. This is constant model part and cannot be created a new one" << std::endl;
            }
        }
    }


    ModelPart::PropertiesType& ModelPart::GetProperties(
        IndexType PropertiesId,
        IndexType ThisIndex
    ){
        auto pprop_it = GetMesh(ThisIndex).Properties().find(PropertiesId);
        if(pprop_it != GetMesh(ThisIndex).Properties().end()){
            return *pprop_it;
        } else {
            if(IsSubModelPart()){
                PropertiesType::Pointer pprop = mpParentModelPart->pGetProperties(PropertiesId, ThisIndex);
                GetMesh(ThisIndex).AddProperties(pprop);
                return *pprop;
            } else {
                QUEST_WARNING("ModelPart") << "Property " << PropertiesId << " does not exist!. Creating and adding new property. Please use CreateNewProperties() instead" << std::endl;
                PropertiesType::Pointer pnew_property = Quest::make_shared<PropertiesType>(PropertiesId);
                GetMesh(ThisIndex).AddProperties(pnew_property);
                return *pnew_property;
            }
        }
    }


    const ModelPart::PropertiesType& ModelPart::GetProperties(
        IndexType PropertiesId,
        IndexType ThisIndex
    ) const {
        auto pprop_it = GetMesh(ThisIndex).Properties().find(PropertiesId);
        if(pprop_it != GetMesh(ThisIndex).Properties().end()){
            return *pprop_it;
        } else {
            if(IsSubModelPart()){
                PropertiesType::Pointer pprop = mpParentModelPart->pGetProperties(PropertiesId, ThisIndex);
                return *pprop;
            } else {
                QUEST_ERROR << "Property " << PropertiesId << " does not exist!. This is constant model part and cannot be created a new one" << std::endl;
            }
        }
    }


    bool ModelPart::HasProperties(
        const std::string& rAddress,
        IndexType ThisIndex
    ) const {
        const std::vector<IndexType> component_name = TrimComponentName(rAddress);
        if(HasProperties(component_name[0], ThisIndex)){
            bool has_properties = true;
            Properties::Pointer p_prop = pGetProperties(component_name[0], ThisIndex);
            for(IndexType i = 1; i < component_name.size(); ++i){
                if(p_prop->HasSubProperties(component_name[i])){
                    p_prop = p_prop->pGetSubProperties(component_name[i]);
                } else {
                    return false;
                }
            }
            return has_properties;
        } else {
            return false;
        }
    }


    Properties::Pointer ModelPart::pGetProperties(
        const std::string& rAddress,
        IndexType ThisIndex
    ){
        const std::vector<IndexType> component_name = TrimComponentName(rAddress);
        if(HasProperties(component_name[0], ThisIndex)){
            Properties::Pointer p_prop = pGetProperties(component_name[0], ThisIndex);
            for(IndexType i = 1; i < component_name.size(); ++i){
                if(p_prop->HasSubProperties(component_name[i])){
                    p_prop = p_prop->pGetSubProperties(component_name[i]);
                } else {
                    QUEST_ERROR << "Index is wrong, does not correspond with any sub Properties Id: " << rAddress << std::endl;
                }
            }
            return p_prop;
        } else {
            QUEST_ERROR << "First index is wrong, does not correspond with any sub Properties Id: " << component_name[0] << std::endl;
        }
    }


    const Properties::Pointer ModelPart::pGetProperties(
        const std::string& rAddress,
        IndexType ThisIndex
    ) const {
        const std::vector<IndexType> component_name = TrimComponentName(rAddress);
        if(HasProperties(component_name[0], ThisIndex)){
            Properties::Pointer p_prop = pGetProperties(component_name[0], ThisIndex);
            for(IndexType i = 1; i < component_name.size(); ++i){
                if(p_prop->HasSubProperties(component_name[i])){
                    p_prop = p_prop->pGetSubProperties(component_name[i]);
                } else {
                    QUEST_ERROR << "Index is wrong, does not correspond with any sub Properties Id: " << rAddress << std::endl;
                }
            }
            return p_prop;
        } else {
            QUEST_ERROR << "First index is wrong, does not correspond with any sub Properties Id: " << component_name[0] << std::endl;
        }
    }


    Properties& ModelPart::GetProperties(
        const std::string& rAddress,
        IndexType MeshIndex
    ){
        return *pGetProperties(rAddress, MeshIndex);
    }


    const Properties& ModelPart::GetProperties(
        const std::string& rAddress,
        IndexType MeshIndex
    ) const {
        return *pGetProperties(rAddress, MeshIndex);
    }


    void ModelPart::RemoveProperties(ModelPart::IndexType PropertiesId, IndexType ThisIndex){
        GetMesh(ThisIndex).RemoveProperties(PropertiesId);

        for(SubModelPartIterator i_sub_model_part = SubModelPartsBegin(); i_sub_model_part != SubModelPartsEnd(); ++i_sub_model_part){
            i_sub_model_part->RemoveProperties(PropertiesId, ThisIndex);
        }
    }


    void ModelPart::RemoveProperties(ModelPart::PropertiesType& ThisProperties, ModelPart::IndexType ThisIndex){
        GetMesh(ThisIndex).RemoveProperties(ThisProperties);

        for(SubModelPartIterator i_sub_model_part = SubModelPartsBegin(); i_sub_model_part != SubModelPartsEnd(); ++i_sub_model_part){
            i_sub_model_part->RemoveProperties(ThisProperties, ThisIndex);
        }
    }


    void ModelPart::RemoveProperties(ModelPart::PropertiesType::Pointer pThisProperties, ModelPart::IndexType ThisIndex){
        GetMesh(ThisIndex).RemoveProperties(pThisProperties);

        for(SubModelPartIterator i_sub_model_part = SubModelPartsBegin(); i_sub_model_part != SubModelPartsEnd(); ++i_sub_model_part){
            i_sub_model_part->RemoveProperties(pThisProperties, ThisIndex);
        }
    }


    void ModelPart::RemovePropertiesFromAllLevels(ModelPart::IndexType PropertiesId, IndexType ThisIndex){
        if(IsSubModelPart()){
            mpParentModelPart->RemovePropertiesFromAllLevels(PropertiesId, ThisIndex);
            return;
        }

        RemoveProperties(PropertiesId, ThisIndex);
    }


    void ModelPart::RemovePropertiesFromAllLevels(ModelPart::PropertiesType& ThisProperties, ModelPart::IndexType ThisIndex){
        if(IsSubModelPart()){
            mpParentModelPart->RemoveProperties(ThisProperties, ThisIndex);
        }

        RemoveProperties(ThisProperties, ThisIndex);
    }


    void ModelPart::RemovePropertiesFromAllLevels(ModelPart::PropertiesType::Pointer pThisProperties, ModelPart::IndexType ThisIndex){
        if(IsSubModelPart()){
            mpParentModelPart->RemoveProperties(pThisProperties, ThisIndex);
        }

        RemoveProperties(pThisProperties, ThisIndex);
    }


    void ModelPart::AddElement(ModelPart::ElementType::Pointer pNewElement, ModelPart::IndexType ThisIndex){
        if(IsSubModelPart()){
            mpParentModelPart->AddElement(pNewElement, ThisIndex);
            GetMesh(ThisIndex).AddElement(pNewElement);
        } else {
            auto existing_elem_it = this->GetMesh(ThisIndex).Elements().find(pNewElement->Id());
            if(existing_elem_it == GetMesh(ThisIndex).Elements().end()){
                GetMesh(ThisIndex).AddElement(pNewElement);
            } else {
                QUEST_ERROR_IF( &(*existing_elem_it) != pNewElement.get() )
                    << "attempting to add pNewElement with Id :" << pNewElement->Id() << ", unfortunately a (different) element with the same Id already exists" << std::endl;
            }
        }
    }


    void ModelPart::AddElements(const std::vector<IndexType>& ElementIds, IndexType ThisIndex){
        QUEST_TRY
        if(IsSubModelPart()){
            ModelPart* root_model_part = &this->GetRootModelPart();
            ModelPart::ElementsContainerType aux;
            aux.reserve(ElementIds.size());
            for(unsigned int i = 0; i < ElementIds.size(); ++i){
                ModelPart::ElementsContainerType::iterator it = root_model_part->Elements().find(ElementIds[i]);
                if(it != root_model_part->ElementsEnd()){
                    aux.push_back(*(it.base()));
                } else {
                    QUEST_ERROR << "the element with Id " << ElementIds[i] << " does not exist in the root model part";
                }
            }

            ModelPart* current_part = this;
            while(current_part->IsSubModelPart()){
                for(auto it = aux.begin(); it != aux.end(); ++it){
                    current_part->Elements().push_back(*(it.base()));
                }

                current_part->Elements().Unique();

                current_part = &(current_part->GetParentModelPart());
            }
        }
        QUEST_CATCH("")
    }


    ModelPart::ElementType::Pointer ModelPart::CreateNewElement(
        std::string ElementName,
        ModelPart::IndexType Id,
        std::vector<ModelPart::IndexType> ElementNodeIds,
        ModelPart::PropertiesType::Pointer pProperties,
        ModelPart::IndexType ThisIndex
    ){
        QUEST_TRY
        if(IsSubModelPart()){
            ElementType::Pointer p_new_element = mpParentModelPart->CreateNewElement(ElementName, Id, ElementNodeIds, pProperties, ThisIndex);
            GetMesh(ThisIndex).AddElement(p_new_element);
            return p_new_element;
        }

        Geometry<Node>::PointsArrayType pElementNodes;

        for(unsigned int i = 0; i < ElementNodeIds.size(); ++i){
            pElementNodes.push_back(pGetNode(ElementNodeIds[i]));
        }

        return CreateNewElement(ElementName, Id, pElementNodes, pProperties, ThisIndex);
        QUEST_CATCH("")
    }


    ModelPart::ElementType::Pointer ModelPart::CreateNewElement(
        std::string ElementName,
        ModelPart::IndexType Id,
        Geometry<Node>::PointsArrayType pElementNodes,
        ModelPart::PropertiesType::Pointer pProperties,
        ModelPart::IndexType ThisIndex
    ){
        QUEST_TRY
        if(IsSubModelPart()){
            ElementType::Pointer p_new_element = mpParentModelPart->CreateNewElement(ElementName, Id, pElementNodes, pProperties, ThisIndex);
            GetMesh(ThisIndex).AddElement(p_new_element);
            return p_new_element;
        }

        auto existing_element_iterator = GetMesh(ThisIndex).Elements().find(Id);
        QUEST_ERROR_IF(existing_element_iterator != GetMesh(ThisIndex).ElementsEnd())
            << "trying to construct an element with ID " << Id << " however an element with the same Id already exists";

        const ElementType& r_clone_element = QuestComponents<ElementType>::Get(ElementName);
        Element::Pointer p_lement = r_clone_element.Create(Id, pElementNodes, pProperties);

        GetMesh(ThisIndex).AddElement(p_lement);

        return p_lement;
        QUEST_CATCH("")
    }


    ModelPart::ElementType::Pointer ModelPart::CreateNewElement(
        std::string ElementName,
        ModelPart::IndexType Id,
        typename GeometryType::Pointer pGeometry,
        ModelPart::PropertiesType::Pointer pProperties,
        ModelPart::IndexType ThisIndex
    ){
        QUEST_TRY
        if(IsSubModelPart()){
            ElementType::Pointer p_new_element = mpParentModelPart->CreateNewElement(ElementName, Id, pGeometry, pProperties, ThisIndex);
            GetMesh(ThisIndex).AddElement(p_new_element);
            return p_new_element;
        }

        auto existing_element_iterator = GetMesh(ThisIndex).Elements().find(Id);
        QUEST_ERROR_IF(existing_element_iterator != GetMesh(ThisIndex).ElementsEnd())
            << "trying to construct an element with ID " << Id << " however an element with the same Id already exists";

        const ElementType& r_clone_element = QuestComponents<ElementType>::Get(ElementName);
        Element::Pointer p_lement = r_clone_element.Create(Id, pGeometry, pProperties);

        GetMesh(ThisIndex).AddElement(p_lement);

        return p_lement;
        QUEST_CATCH("")
    }


    void ModelPart::RemoveElement(ModelPart::IndexType ElementId, ModelPart::IndexType ThisIndex){
        GetMesh(ThisIndex).RemoveElement(ElementId);

        for(SubModelPartIterator i_sub_model_part = SubModelPartsBegin(); i_sub_model_part != SubModelPartsEnd(); ++i_sub_model_part){
            i_sub_model_part->RemoveElement(ElementId, ThisIndex);
        }
    }


    void ModelPart::RemoveElement(ModelPart::ElementType& ThisElement, ModelPart::IndexType ThisIndex){
        GetMesh(ThisIndex).RemoveElement(ThisElement);

        for(SubModelPartIterator i_sub_model_part = SubModelPartsBegin(); i_sub_model_part != SubModelPartsEnd(); ++i_sub_model_part){
            i_sub_model_part->RemoveElement(ThisElement, ThisIndex);
        }
    }


    void ModelPart::RemoveElement(ModelPart::ElementType::Pointer pThisElement, ModelPart::IndexType ThisIndex){
        GetMesh(ThisIndex).RemoveElement(pThisElement);

        for(SubModelPartIterator i_sub_model_part = SubModelPartsBegin(); i_sub_model_part != SubModelPartsEnd(); ++i_sub_model_part){
            i_sub_model_part->RemoveElement(pThisElement, ThisIndex);
        }
    }


    void ModelPart::RemoveElementFromAllLevels(ModelPart::IndexType ElementId, ModelPart::IndexType ThisIndex){
        if(IsSubModelPart()){
            mpParentModelPart->RemoveElement(ElementId, ThisIndex);
            return;
        }

        RemoveElement(ElementId, ThisIndex);
    }


    void ModelPart::RemoveElementFromAllLevels(ModelPart::ElementType& ThisElement, ModelPart::IndexType ThisIndex){
        if(IsSubModelPart()){
            mpParentModelPart->RemoveElement(ThisElement, ThisIndex);
            return;
        }

        RemoveElement(ThisElement, ThisIndex);
    }


    void ModelPart::RemoveElementFromAllLevels(ModelPart::ElementType::Pointer pThisElement, ModelPart::IndexType ThisIndex){
        if(IsSubModelPart()){
            mpParentModelPart->RemoveElement(pThisElement, ThisIndex);
            return;
        }

        RemoveElement(pThisElement, ThisIndex);
    }


    void ModelPart::RemoveElements(Flags IdentifierFlag){
        auto& meshes = this->GetMeshes();
        for(ModelPart::MeshesContainerType::iterator i_mesh = meshes.begin(); i_mesh != meshes.end(); ++i_mesh){
            const unsigned int nelements = i_mesh->Elements().size();
            unsigned int erase_count = 0;
            #pragma omp parallel for reduction(+:erase_count)
            for(int i=0; i<static_cast<int>(nelements); ++i)
            {
                auto i_elem = i_mesh->ElementsBegin() + i;

                if(i_elem->IsNot(IdentifierFlag)){
                    ++erase_count;
                }
            }

            ModelPart::ElementsContainerType temp_elements_container;
            temp_elements_container.reserve(i_mesh->Elements().size() - erase_count);

            temp_elements_container.swap(i_mesh->Elements());

            for(ModelPart::ElementsContainerType::iterator i_elem = temp_elements_container.begin(); i_elem != temp_elements_container.end(); ++i_elem){
                if(i_elem->IsNot(IdentifierFlag)){
                    (i_mesh->Elements()).push_back(std::move(*(i_elem.base())));
                }
            }
        }

        for(SubModelPartIterator i_sub_model_part = SubModelPartsBegin(); i_sub_model_part != SubModelPartsEnd(); ++i_sub_model_part){
            i_sub_model_part->RemoveElements(IdentifierFlag);
        }
    }


    void ModelPart::RemoveElementsFromAllLevels(Flags IdentifierFlag){
        ModelPart& root_model_part = GetRootModelPart();
        root_model_part.RemoveElements(IdentifierFlag);
    }


    void ModelPart::AddMasterSlaveConstraint(
        MasterSlaveConstraintType::Pointer pNewMasterSlaveConstraint,
        IndexType ThisIndex
    ){
        if(IsSubModelPart()){
            mpParentModelPart->AddMasterSlaveConstraint(pNewMasterSlaveConstraint, ThisIndex);
            GetMesh(ThisIndex).AddMasterSlaveConstraint(pNewMasterSlaveConstraint);
        } else {
            auto existing_constraint_it = GetMesh(ThisIndex).MasterSlaveConstraints().find(pNewMasterSlaveConstraint->Id());
            if(existing_constraint_it == GetMesh(ThisIndex).MasterSlaveConstraintsEnd()){
                GetMesh(ThisIndex).AddMasterSlaveConstraint(pNewMasterSlaveConstraint);
            } else {
                QUEST_ERROR_IF( &(*existing_constraint_it) != pNewMasterSlaveConstraint.get() )
                    << "attempting to add pNewMasterSlaveConstraint with Id :" << pNewMasterSlaveConstraint->Id() << ", unfortunately a (different) constraint with the same Id already exists" << std::endl;
            }
        }
    }


    void ModelPart::AddMasterSlaveConstraints(const std::vector<IndexType>& MasterSlaveConstraintIds, IndexType ThisIndex){
        QUEST_TRY
        if(IsSubModelPart()){
            ModelPart* root_model_part = &this->GetRootModelPart();
            ModelPart::MasterSlaveConstraintContainerType aux;
            aux.reserve(MasterSlaveConstraintIds.size());
            for(unsigned int i = 0; i < MasterSlaveConstraintIds.size(); ++i){
                ModelPart::MasterSlaveConstraintContainerType::iterator it = root_model_part->MasterSlaveConstraints().find(MasterSlaveConstraintIds[i]);
                if(it != root_model_part->MasterSlaveConstraintsEnd()){
                    aux.push_back(*(it.base()));
                } else {
                    QUEST_ERROR << "the constraint with Id " << MasterSlaveConstraintIds[i] << " does not exist in the root model part";
                }
            }

            ModelPart* current_part = this;
            while(current_part->IsSubModelPart()){
                for(auto it = aux.begin(); it != aux.end(); it++){
                    current_part->MasterSlaveConstraints().push_back(*(it.base()));
                }

                current_part->MasterSlaveConstraints().Unique();

                current_part = &(current_part->GetParentModelPart());
            }
        }
        QUEST_CATCH("")
    }


    ModelPart::MasterSlaveConstraintType::Pointer ModelPart::CreateNewMasterSlaveConstraint(
        const std::string& ConstraintName,
        IndexType Id,
        ModelPart::DofsVectorType& rMasterDofsVector,
        ModelPart::DofsVectorType& rSlaveDofsVector,
        const ModelPart::MatrixType& RelationMatrix,
        const ModelPart::VectorType& ConstantVector,
        IndexType ThisIndex
    ){
        QUEST_TRY
        MeshType& r_mesh = GetMesh(ThisIndex);
        ModelPart::MasterSlaveConstraintType::Pointer p_new_constraint;

        if(IsSubModelPart()){
            p_new_constraint = mpParentModelPart->CreateNewMasterSlaveConstraint(
                ConstraintName, 
                Id, 
                rMasterDofsVector, 
                rSlaveDofsVector, 
                RelationMatrix, 
                ConstantVector, 
                ThisIndex
            );

            if(&r_mesh != &mpParentModelPart->GetMesh(ThisIndex)){
                QUEST_ERROR_IF_NOT(r_mesh.AddMasterSlaveConstraint(p_new_constraint))
                    << "trying to insert a master-slave constraint with ID "
                    << Id << " but a constraint with the same ID already exists\n";
            }
        } else {
            const ModelPart::MasterSlaveConstraintType& r_registered_constraint = QuestComponents<ModelPart::MasterSlaveConstraintType>::Get(ConstraintName);
            p_new_constraint = r_registered_constraint.Create(
                Id, 
                rMasterDofsVector, 
                rSlaveDofsVector, 
                RelationMatrix, 
                ConstantVector
            );

            QUEST_ERROR_IF_NOT(r_mesh.AddMasterSlaveConstraint(p_new_constraint))
                << "trying to insert a master-slave constraint with ID "
                << Id << " but a constraint with the same ID already exists\n";
        }

        return p_new_constraint;
        QUEST_CATCH("")
    }


    ModelPart::MasterSlaveConstraintType::Pointer ModelPart::CreateNewMasterSlaveConstraint(
        const std::string& ConstraintName,
        ModelPart::IndexType Id,
        ModelPart::NodeType& rMasterNode,
        const ModelPart::DoubleVariableType& rMasterVariable,
        ModelPart::NodeType& rSlaveNode,
        const ModelPart::DoubleVariableType& rSlaveVariable,
        const double Weight,
        const double Constant,
        IndexType ThisIndex
    ){
        QUEST_ERROR_IF_NOT(rMasterNode.HasDofFor(rMasterVariable))
            << "Master node " << rMasterNode.Id() << " has no variable " << rMasterVariable.Name() << "\n";

        QUEST_ERROR_IF_NOT(rSlaveNode.HasDofFor(rSlaveVariable))
            << "Slave node " << rSlaveNode.Id() << " has no variable " << rSlaveVariable.Name() << "\n";

        QUEST_TRY

        ModelPart::MasterSlaveConstraintType::Pointer p_new_constraint;
        MeshType& r_mesh = this->GetMesh(ThisIndex);

        if(IsSubModelPart()){
            p_new_constraint = mpParentModelPart->CreateNewMasterSlaveConstraint(
                ConstraintName, 
                Id, 
                rMasterNode, 
                rMasterVariable, 
                rSlaveNode, 
                rSlaveVariable, 
                Weight, 
                Constant, 
                ThisIndex
            );

            if(&r_mesh != &mpParentModelPart->GetMesh(ThisIndex)){
                QUEST_ERROR_IF_NOT(r_mesh.AddMasterSlaveConstraint(p_new_constraint))
                    << "trying to insert a master-slave constraint with ID "
                    << Id << " but a constraint with the same ID already exists\n";
            }
        } else {
            const ModelPart::MasterSlaveConstraintType& r_clone_constraint = QuestComponents<ModelPart::MasterSlaveConstraintType>::Get(ConstraintName);
            p_new_constraint = r_clone_constraint.Create(
                Id, 
                rMasterNode, 
                rMasterVariable, 
                rSlaveNode, 
                rSlaveVariable, 
                Weight, 
                Constant
            );

            QUEST_ERROR_IF_NOT(r_mesh.AddMasterSlaveConstraint(p_new_constraint))
                << "trying to insert a master-slave constraint with ID "
                << Id << " but a constraint with the same ID already exists\n";
        }

        return p_new_constraint;
        QUEST_CATCH("")
    }


    void ModelPart::RemoveMasterSlaveConstraint(ModelPart::IndexType MasterSlaveConstraintId, IndexType ThisIndex){
        GetMesh(ThisIndex).RemoveMasterSlaveConstraint(MasterSlaveConstraintId);

        for(SubModelPartIterator i_sub_model_part = SubModelPartsBegin(); i_sub_model_part != SubModelPartsEnd(); ++i_sub_model_part){
            i_sub_model_part->RemoveMasterSlaveConstraint(MasterSlaveConstraintId, ThisIndex);
        }
    }


    void ModelPart::RemoveMasterSlaveConstraint(ModelPart::MasterSlaveConstraintType& ThisMasterSlaveConstraint, IndexType ThisIndex){
        GetMesh(ThisIndex).RemoveMasterSlaveConstraint(ThisMasterSlaveConstraint);

        for(SubModelPartIterator i_sub_model_part = SubModelPartsBegin(); i_sub_model_part != SubModelPartsEnd(); ++i_sub_model_part){
            i_sub_model_part->RemoveMasterSlaveConstraint(ThisMasterSlaveConstraint, ThisIndex);
        }
    }


    void ModelPart::RemoveMasterSlaveConstraintFromAllLevels(ModelPart::IndexType MasterSlaveConstraintId, IndexType ThisIndex){
        if(IsSubModelPart()){
            mpParentModelPart->RemoveMasterSlaveConstraintFromAllLevels(MasterSlaveConstraintId, ThisIndex);
        }
        RemoveMasterSlaveConstraint(MasterSlaveConstraintId, ThisIndex);
    }


    void ModelPart::RemoveMasterSlaveConstraintFromAllLevels(ModelPart::MasterSlaveConstraintType& ThisMasterSlaveConstraint, IndexType ThisIndex){
        if(IsSubModelPart()){
            mpParentModelPart->RemoveMasterSlaveConstraintFromAllLevels(ThisMasterSlaveConstraint, ThisIndex);
        }
        RemoveMasterSlaveConstraint(ThisMasterSlaveConstraint, ThisIndex);
    }


    void ModelPart::RemoveMasterSlaveConstraints(Flags IdentifierFlag){
        auto& meshes = this->GetMeshes();
        for(auto it_mesh = meshes.begin(); it_mesh != meshes.end(); ++it_mesh){
            const SizeType nconstraints = it_mesh->MasterSlaveConstraints().size();
            SizeType erase_count = 0;
            #pragma omp parallel for reduction(+:erase_count)
            for(int i=0; i<static_cast<int>(nconstraints); ++i)
            {
                auto it_constraint = it_mesh->MasterSlaveConstraintsBegin() + i;

                if(it_constraint->IsNot(IdentifierFlag)){
                    ++erase_count;
                }
            }

            ModelPart::MasterSlaveConstraintContainerType temp_constraints_container;
            temp_constraints_container.reserve(it_mesh->MasterSlaveConstraints().size() - erase_count);

            temp_constraints_container.swap(it_mesh->MasterSlaveConstraints());

            for(auto it_constraint = temp_constraints_container.begin(); it_constraint != temp_constraints_container.end(); ++it_constraint){
                if(it_constraint->IsNot(IdentifierFlag)){
                    (it_mesh->MasterSlaveConstraints()).push_back(std::move(*(it_constraint.base())));
                }
            }

            for(SubModelPartIterator i_sub_model_part = SubModelPartsBegin(); i_sub_model_part != SubModelPartsEnd(); ++i_sub_model_part){
                i_sub_model_part->RemoveMasterSlaveConstraints(IdentifierFlag);
            }
        }
    }


    void ModelPart::RemoveMasterSlaveConstraintsFromAllLevels(Flags IdentifierFlag){
        ModelPart& root_model_part = GetRootModelPart();
        root_model_part.RemoveMasterSlaveConstraints(IdentifierFlag);
    }


    ModelPart::MasterSlaveConstraintType::Pointer ModelPart::pGetMasterSlaveConstraint(ModelPart::IndexType MasterSlaveConstraintId, IndexType ThisIndex){
        return GetMesh(ThisIndex).pGetMasterSlaveConstraint(MasterSlaveConstraintId);
    }


    ModelPart::MasterSlaveConstraintType& ModelPart::GetMasterSlaveConstraint(ModelPart::IndexType MasterSlaveConstraintId, IndexType ThisIndex){
        return GetMesh(ThisIndex).GetMasterSlaveConstraint(MasterSlaveConstraintId);
    }


    const ModelPart::MasterSlaveConstraintType& ModelPart::GetMasterSlaveConstraint(ModelPart::IndexType MasterSlaveConstraintId, IndexType ThisIndex) const{
        return GetMesh(ThisIndex).GetMasterSlaveConstraint(MasterSlaveConstraintId);
    }


    void ModelPart::AddCondition(ModelPart::ConditionType::Pointer pNewCondition, IndexType ThisIndex){
        if(IsSubModelPart()){
            mpParentModelPart->AddCondition(pNewCondition, ThisIndex);
            GetMesh(ThisIndex).AddCondition(pNewCondition);
        } else {
            auto existing_condition_it = GetMesh(ThisIndex).Conditions().find(pNewCondition->Id());
            if(existing_condition_it == GetMesh(ThisIndex).ConditionsEnd()){
                GetMesh(ThisIndex).AddCondition(pNewCondition);
            } else {
                QUEST_ERROR_IF( &(*existing_condition_it) != pNewCondition.get() )
                    << "attempting to add pNewCondition with Id :" << pNewCondition->Id() << ", unfortunately a (different) condition with the same Id already exists" << std::endl;
            }
        }
    }


    void ModelPart::AddConditions(const std::vector<IndexType>& ConditionIds, IndexType ThisIndex){
        QUEST_TRY
        if(IsSubModelPart()){
            ModelPart* root_model_part = &this->GetRootModelPart();
            ModelPart::ConditionsContainerType aux;
            aux.reserve(ConditionIds.size());
            for(unsigned int i = 0; i < ConditionIds.size(); ++i){
                ModelPart::ConditionsContainerType::iterator it = root_model_part->Conditions().find(ConditionIds[i]);
                if(it != root_model_part->ConditionsEnd()){
                    aux.push_back(*(it.base()));
                } else {
                    QUEST_ERROR << "the condition with Id " << ConditionIds[i] << " does not exist in the root model part";
                }
            }

            ModelPart* current_part = this;
            while(current_part->IsSubModelPart()){
                for(auto it = aux.begin(); it != aux.end(); it++){
                    current_part->Conditions().push_back(*(it.base()));
                }

                current_part->Conditions().Unique();

                current_part = &(current_part->GetParentModelPart());
            }
        }
        QUEST_CATCH("")
    }


    ModelPart::ConditionType::Pointer ModelPart::CreateNewCondition(
        std::string ConditionName,
        ModelPart::IndexType Id,
        std::vector<IndexType> ConditionNodeIds,
        ModelPart::PropertiesType::Pointer pProperties,
        ModelPart::IndexType ThisIndex
    ){
        QUEST_TRY
        Geometry<Node>::PointsArrayType pConditionNodes;

        for(IndexType i = 0; i < ConditionNodeIds.size(); ++i){
            pConditionNodes.push_back(pGetNode(ConditionNodeIds[i]));
        }

        return CreateNewCondition(ConditionName, Id, pConditionNodes, pProperties, ThisIndex);
        QUEST_CATCH("")
    }


    ModelPart::ConditionType::Pointer ModelPart::CreateNewCondition(
        std::string ConditionName,
        ModelPart::IndexType Id,
        Geometry<Node>::PointsArrayType& pConditionNodes,
        ModelPart::PropertiesType::Pointer pProperties,
        ModelPart::IndexType ThisIndex
    ){
        QUEST_TRY
        if(IsSubModelPart()){
            ConditionType::Pointer p_new_condition = mpParentModelPart->CreateNewCondition(
                ConditionName, 
                Id, 
                pConditionNodes, 
                pProperties, 
                ThisIndex
            );
            GetMesh(ThisIndex).AddCondition(p_new_condition);
            return p_new_condition;
        }

        auto existing_condition_it = GetMesh(ThisIndex).Conditions().find(Id);
        QUEST_ERROR_IF(existing_condition_it != GetMesh(ThisIndex).ConditionsEnd())
            << "attempting to create a condition with Id " << Id << " but a condition with the same Id already exists\n";

        const ConditionType& r_clone_condition = QuestComponents<ConditionType>::Get(ConditionName);
        ConditionType::Pointer p_new_condition = r_clone_condition.Create(Id, pConditionNodes, pProperties);
        GetMesh(ThisIndex).AddCondition(p_new_condition);

        return p_new_condition;
        QUEST_CATCH("")
    }


    ModelPart::ConditionType::Pointer ModelPart::CreateNewCondition(
        std::string ConditionName,
        ModelPart::IndexType Id,
        typename GeometryType::Pointer pGeometry,
        ModelPart::PropertiesType::Pointer pProperties,
        ModelPart::IndexType ThisIndex
    ){
        QUEST_TRY
        if(IsSubModelPart()){
            ConditionType::Pointer p_new_condition = mpParentModelPart->CreateNewCondition(
                ConditionName, 
                Id, 
                pGeometry, 
                pProperties, 
                ThisIndex
            );
            GetMesh(ThisIndex).AddCondition(p_new_condition);
            return p_new_condition;
        }

        auto existing_condition_it = GetMesh(ThisIndex).Conditions().find(Id);
        QUEST_ERROR_IF(existing_condition_it != GetMesh(ThisIndex).ConditionsEnd())
            << "attempting to create a condition with Id " << Id << " but a condition with the same Id already exists\n";

        const ConditionType& r_clone_condition = QuestComponents<ConditionType>::Get(ConditionName);
        ConditionType::Pointer p_new_condition = r_clone_condition.Create(Id, pGeometry, pProperties);
        GetMesh(ThisIndex).AddCondition(p_new_condition);

        return p_new_condition;
        QUEST_CATCH("")
    }


    void ModelPart::RemoveCondition(ModelPart::IndexType ConditionId, ModelPart::IndexType ThisIndex){
        GetMesh(ThisIndex).RemoveCondition(ConditionId);

        for(SubModelPartIterator i_sub_model_part = SubModelPartsBegin(); i_sub_model_part != SubModelPartsEnd(); ++i_sub_model_part){
            i_sub_model_part->RemoveCondition(ConditionId, ThisIndex);
        }
    }


    void ModelPart::RemoveCondition(ModelPart::ConditionType& ThisCondition, ModelPart::IndexType ThisIndex){
        GetMesh(ThisIndex).RemoveCondition(ThisCondition);

        for(SubModelPartIterator i_sub_model_part = SubModelPartsBegin(); i_sub_model_part != SubModelPartsEnd(); ++i_sub_model_part){
            i_sub_model_part->RemoveCondition(ThisCondition, ThisIndex);
        }
    }


    void ModelPart::RemoveCondition(ModelPart::ConditionType::Pointer pThisCondition, ModelPart::IndexType ThisIndex){
        GetMesh(ThisIndex).RemoveCondition(pThisCondition);

        for(SubModelPartIterator i_sub_model_part = SubModelPartsBegin(); i_sub_model_part != SubModelPartsEnd(); ++i_sub_model_part){
            i_sub_model_part->RemoveCondition(pThisCondition, ThisIndex);
        }
    }


    void ModelPart::RemoveConditionFromAllLevels(ModelPart::IndexType ConditionId, ModelPart::IndexType ThisIndex){
        if(IsSubModelPart()){
            mpParentModelPart->RemoveCondition(ConditionId, ThisIndex);
            return;
        }

        RemoveCondition(ConditionId, ThisIndex);
    }


    void ModelPart::RemoveConditionFromAllLevels(ModelPart::ConditionType& ThisCondition, ModelPart::IndexType ThisIndex){
        if(IsSubModelPart()){
            mpParentModelPart->RemoveCondition(ThisCondition, ThisIndex);
            return;
        }

        RemoveCondition(ThisCondition, ThisIndex);
    }


    void ModelPart::RemoveConditionFromAllLevels(ModelPart::ConditionType::Pointer pThisCondition, ModelPart::IndexType ThisIndex){
        if(IsSubModelPart()){
            mpParentModelPart->RemoveCondition(pThisCondition, ThisIndex);
            return;
        }

        RemoveCondition(pThisCondition, ThisIndex);
    }


    void ModelPart::RemoveConditions(Flags IdentifierFlag){
        auto& meshes = this->GetMeshes();
        for(ModelPart::MeshesContainerType::iterator it_mesh = meshes.begin(); it_mesh != meshes.end(); ++it_mesh){
            const unsigned int nconditions = it_mesh->Conditions().size();
            unsigned int erase_count = 0;
            #pragma omp parallel for reduction(+:erase_count)
            for(int i=0; i<static_cast<int>(nconditions); ++i)
            {
                auto i_cond = it_mesh->ConditionsBegin() + i;

                if(i_cond->IsNot(IdentifierFlag)){
                    ++erase_count;
                }
            }

            ModelPart::ConditionsContainerType temp_conditions_container;
            temp_conditions_container.reserve(it_mesh->Conditions().size() - erase_count);

            temp_conditions_container.swap(it_mesh->Conditions());

            for(ModelPart::ConditionsContainerType::iterator it_condition = temp_conditions_container.begin(); it_condition != temp_conditions_container.end(); ++it_condition){
                if(it_condition->IsNot(IdentifierFlag)){
                    (it_mesh->Conditions()).push_back(std::move(*(it_condition.base())));
                }
            }

            for(SubModelPartIterator i_sub_model_part = SubModelPartsBegin(); i_sub_model_part != SubModelPartsEnd(); ++i_sub_model_part){
                i_sub_model_part->RemoveConditions(IdentifierFlag);
            }
        }
    }


    void ModelPart::RemoveConditionsFromAllLevels(Flags IdentifierFlag){
        ModelPart& root_model_part = GetRootModelPart();
        root_model_part.RemoveConditions(IdentifierFlag);
    }


    ModelPart::GeometryType::Pointer ModelPart::CreateNewGeometry(
        const std::string& rGeometryName,
        const std::vector<ModelPart::IndexType>& rGeometryNodeIds
    ){
        if(IsSubModelPart()){
            GeometryType::Pointer p_new_geometry = mpParentModelPart->CreateNewGeometry(rGeometryName, rGeometryNodeIds);
            this->AddGeometry(p_new_geometry);
            return p_new_geometry;
        }

        GeometryType::PointsArrayType p_geometry_nodes;
        for(IndexType i = 0; i < rGeometryNodeIds.size(); ++i){
            p_geometry_nodes.push_back(pGetNode(rGeometryNodeIds[i]));
        }

        return CreateNewGeometry(rGeometryName, p_geometry_nodes);
    }


    ModelPart::GeometryType::Pointer ModelPart::CreateNewGeometry(
        const std::string& rGeometryName,
        GeometryType::PointsArrayType pGeometryNodes
    ){
        QUEST_TRY
        if(IsSubModelPart()){
            GeometryType::Pointer p_new_geometry = mpParentModelPart->CreateNewGeometry(rGeometryName, pGeometryNodes);
            this->AddGeometry(p_new_geometry);
            return p_new_geometry;
        }

        const GeometryType& r_clone_geometry = QuestComponents<GeometryType>::Get(rGeometryName);
        GeometryType::Pointer p_new_geometry = r_clone_geometry.Create(pGeometryNodes);
        this->AddGeometry(p_new_geometry);

        return p_new_geometry;
        QUEST_CATCH("")
    }


    ModelPart::GeometryType::Pointer ModelPart::CreateNewGeometry(
        const std::string& rGeometryName,
        GeometryType::Pointer pGeometry
    ){
        QUEST_TRY
        if(IsSubModelPart()){
            GeometryType::Pointer p_new_geometry = mpParentModelPart->CreateNewGeometry(rGeometryName, pGeometry);
            this->AddGeometry(p_new_geometry);
            return p_new_geometry;
        }

        const GeometryType& r_clone_geometry = QuestComponents<GeometryType>::Get(rGeometryName);
        GeometryType::Pointer p_new_geometry = r_clone_geometry.Create(pGeometry);
        this->AddGeometry(p_new_geometry);

        return p_new_geometry;
        QUEST_CATCH("")
    }


    ModelPart::GeometryType::Pointer ModelPart::CreateNewGeometry(
        const std::string& rGeometryName,
        const IndexType GeometryId,
        const std::vector<ModelPart::IndexType>& rGeometryNodeIds
    ){
        if(IsSubModelPart()){
            GeometryType::Pointer p_new_geometry = mpParentModelPart->CreateNewGeometry(rGeometryName, GeometryId, rGeometryNodeIds);
            this->AddGeometry(p_new_geometry);
            return p_new_geometry;
        }

        GeometryType::PointsArrayType p_geometry_nodes;
        for(IndexType i = 0; i < rGeometryNodeIds.size(); ++i){
            p_geometry_nodes.push_back(pGetNode(rGeometryNodeIds[i]));
        }

        return CreateNewGeometry(rGeometryName, GeometryId, p_geometry_nodes);
    }


    ModelPart::GeometryType::Pointer ModelPart::CreateNewGeometry(
        const std::string& rGeometryName,
        const IndexType GeometryId,
        GeometryType::PointsArrayType pGeometryNodes
    ){
        QUEST_TRY
        if(IsSubModelPart()){
            GeometryType::Pointer p_new_geometry = mpParentModelPart->CreateNewGeometry(rGeometryName, GeometryId, pGeometryNodes);
            this->AddGeometry(p_new_geometry);
            return p_new_geometry;
        }

        QUEST_ERROR_IF(this->HasGeometry(GeometryId))
            << "Trying to construct an geometry with ID: " << GeometryId << ". A geometry with the same Id exists already." << std::endl;

        const GeometryType& r_clone_geometry = QuestComponents<GeometryType>::Get(rGeometryName);
        GeometryType::Pointer p_new_geometry = r_clone_geometry.Create(GeometryId, pGeometryNodes);
        this->AddGeometry(p_new_geometry);

        return p_new_geometry;
        QUEST_CATCH("")
    }


    ModelPart::GeometryType::Pointer ModelPart::CreateNewGeometry(
        const std::string& rGeometryName,
        const IndexType GeometryId,
        GeometryType::Pointer pGeometry
    ){
        QUEST_TRY
        if(IsSubModelPart()){
            GeometryType::Pointer p_new_geometry = mpParentModelPart->CreateNewGeometry(rGeometryName, GeometryId, pGeometry);
            this->AddGeometry(p_new_geometry);
            return p_new_geometry;
        }

        QUEST_ERROR_IF(this->HasGeometry(GeometryId))
            << "Trying to construct an geometry with ID: " << GeometryId << ". A geometry with the same Id exists already." << std::endl;

        const GeometryType& r_clone_geometry = QuestComponents<GeometryType>::Get(rGeometryName);
        GeometryType::Pointer p_new_geometry = r_clone_geometry.Create(GeometryId, pGeometry);
        this->AddGeometry(p_new_geometry);

        return p_new_geometry;
        QUEST_CATCH("")
    }


    ModelPart::GeometryType::Pointer ModelPart::CreateNewGeometry(
        const std::string& rGeometryTypeName,
        const std::string& rGeoemtryIdentifierName,
        const std::vector<ModelPart::IndexType>& rGeometryNodeIds
    ){
        if(IsSubModelPart()){
            GeometryType::Pointer p_new_geometry = mpParentModelPart->CreateNewGeometry(rGeometryTypeName, rGeoemtryIdentifierName, rGeometryNodeIds);
            this->AddGeometry(p_new_geometry);
            return p_new_geometry;
        }

        GeometryType::PointsArrayType p_geometry_nodes;
        for(IndexType i = 0; i < rGeometryNodeIds.size(); ++i){
            p_geometry_nodes.push_back(pGetNode(rGeometryNodeIds[i]));
        }

        return CreateNewGeometry(rGeometryTypeName, rGeoemtryIdentifierName, p_geometry_nodes);
    }


    ModelPart::GeometryType::Pointer ModelPart::CreateNewGeometry(
        const std::string& rGeometryTypeName,
        const std::string& rGeoemtryIdentifierName,
        GeometryType::PointsArrayType pGeometryNodes
    ){
        QUEST_TRY
        if(IsSubModelPart()){
            GeometryType::Pointer p_new_geometry = mpParentModelPart->CreateNewGeometry(rGeometryTypeName, rGeoemtryIdentifierName, pGeometryNodes);
            this->AddGeometry(p_new_geometry);
            return p_new_geometry;
        }

        QUEST_ERROR_IF(this->HasGeometry(rGeoemtryIdentifierName))
            << "Trying to construct an geometry with identifier: " << rGeoemtryIdentifierName << ". A geometry with the same identifier exists already." << std::endl;

        const GeometryType& r_clone_geometry = QuestComponents<GeometryType>::Get(rGeometryTypeName);
        GeometryType::Pointer p_new_geometry = r_clone_geometry.Create(rGeoemtryIdentifierName, pGeometryNodes);
        this->AddGeometry(p_new_geometry);

        return p_new_geometry;
        QUEST_CATCH("")
    }


    ModelPart::GeometryType::Pointer ModelPart::CreateNewGeometry(
        const std::string& rGeometryTypeName,
        const std::string& rGeoemtryIdentifierName,
        GeometryType::Pointer pGeometry
    ){
        QUEST_TRY
        if(IsSubModelPart()){
            GeometryType::Pointer p_new_geometry = mpParentModelPart->CreateNewGeometry(rGeometryTypeName, rGeoemtryIdentifierName, pGeometry);
            this->AddGeometry(p_new_geometry);
            return p_new_geometry;
        }

        QUEST_ERROR_IF(this->HasGeometry(rGeoemtryIdentifierName))
            << "Trying to construct an geometry with identifier: " << rGeoemtryIdentifierName << ". A geometry with the same identifier exists already." << std::endl;

        const GeometryType& r_clone_geometry = QuestComponents<GeometryType>::Get(rGeometryTypeName);
        GeometryType::Pointer p_new_geometry = r_clone_geometry.Create(rGeoemtryIdentifierName, pGeometry);
        this->AddGeometry(p_new_geometry);

        return p_new_geometry;
        QUEST_CATCH("")
    }


    void ModelPart::AddGeometry(typename ModelPart::GeometryType::Pointer pNewGeometry){
        if(IsSubModelPart()){
            if(!mpParentModelPart->HasGeometry(pNewGeometry->Id())){
                mpParentModelPart->AddGeometry(pNewGeometry);
            }
        }

        mGeometries.AddGeometry(pNewGeometry);
    }


    void ModelPart::AddGeometries(const std::vector<IndexType>& GeometriesIds){
        QUEST_TRY
        if(IsSubModelPart()){
            ModelPart* p_root_model_part = &this->GetRootModelPart();
            std::vector<GeometryType::Pointer> aux;
            aux.reserve(GeometriesIds.size());
            for(auto& r_id : GeometriesIds){
                auto it_found = p_root_model_part->Geometries().find(r_id);
                if(it_found != p_root_model_part->GeometriesEnd()){
                    aux.push_back(it_found.operator->());
                } else {
                    QUEST_ERROR << "The geometry with Id " << r_id << " does not exist in the root model part" << std::endl;
                }
            }

            ModelPart* p_current_part = this;
            while(p_current_part->IsSubModelPart()){
                for(auto& p_geom : aux){
                    p_current_part->AddGeometry(p_geom);
                }

                p_current_part = &(p_current_part->GetParentModelPart());
            }
        }
        QUEST_CATCH("")
    }


    void ModelPart::RemoveGeometry(const IndexType GeometryId){
        mGeometries.RemoveGeometry(GeometryId);

        for(SubModelPartIterator i_sub_model_part = SubModelPartsBegin(); i_sub_model_part != SubModelPartsEnd(); ++i_sub_model_part){
            i_sub_model_part->RemoveGeometry(GeometryId);
        }
    }


    void ModelPart::RemoveGeometry(std::string GeometryName){
        mGeometries.RemoveGeometry(GeometryName);

        for(SubModelPartIterator i_sub_model_part = SubModelPartsBegin(); i_sub_model_part != SubModelPartsEnd(); ++i_sub_model_part){
            i_sub_model_part->RemoveGeometry(GeometryName);
        }
    }


    void ModelPart::RemoveGeometryFromAllLevels(const IndexType GeometryId){
        if(IsSubModelPart()){
            mpParentModelPart->RemoveGeometry(GeometryId);
            return;
        }

        RemoveGeometry(GeometryId);
    }


    void ModelPart::RemoveGeometryFromAllLevels(std::string GeometryName){
        if(IsSubModelPart()){
            mpParentModelPart->RemoveGeometry(GeometryName);
            return;
        }

        RemoveGeometry(GeometryName);
    }


    ModelPart& ModelPart::CreateSubModelPart(const std::string& NewSubModelPartName){
        const auto delim_pos = NewSubModelPartName.find(":");
        const std::string& sub_model_part_name = NewSubModelPartName.substr(0, delim_pos);

        if(delim_pos == std::string::npos){
            QUEST_ERROR_IF(mSubModelParts.find(NewSubModelPartName) != mSubModelParts.end())
                << "There is an already existing sub model part with name \"" << NewSubModelPartName
                << "\" in model part: \"" << FullName() << "\"" << std::endl;

            ModelPart* praw = new ModelPart(NewSubModelPartName, this->mpVariablesList, this->GetModel());
            Quest::shared_ptr<ModelPart> p_model_part(praw);
            p_model_part->SetParentModelPart(this);
            p_model_part->mBufferSize = this->mBufferSize;
            p_model_part->mpProcessInfo = this->mpProcessInfo;
            mSubModelParts.insert(p_model_part);
            return *p_model_part;
        } else {
            ModelPart *p;
            SubModelPartIterator i = mSubModelParts.find(sub_model_part_name);
            if(i == mSubModelParts.end()){
                p = &CreateSubModelPart(sub_model_part_name);
            } else {
                p = &(*i);
            }
            return p->CreateSubModelPart(NewSubModelPartName.substr(delim_pos+1));
        }
    }


    ModelPart& ModelPart::GetSubModelPart(const std::string& SubModelPartName){
        const auto delim_pos = SubModelPartName.find('.');
        const std::string& sub_model_part_name = SubModelPartName.substr(0, delim_pos);

        SubModelPartIterator i = mSubModelParts.find(sub_model_part_name);
        if(i == mSubModelParts.end()){
            ErrorNonExistingSubModelPart(sub_model_part_name);
        }

        if(delim_pos == std::string::npos){
            return *i;
        } else {
            return i->GetSubModelPart(SubModelPartName.substr(delim_pos+1));
        }
    }


    const ModelPart& ModelPart::GetSubModelPart(const std::string& SubModelPartName) const{
        const auto delim_pos = SubModelPartName.find('.');
        const std::string& sub_model_part_name = SubModelPartName.substr(0, delim_pos);

        const auto i = mSubModelParts.find(sub_model_part_name);
        if(i == mSubModelParts.end()){
            ErrorNonExistingSubModelPart(sub_model_part_name);
        }

        if(delim_pos == std::string::npos){
            return *i;
        } else {
            return i->GetSubModelPart(SubModelPartName.substr(delim_pos+1));
        }
    }


    ModelPart* ModelPart::pGetSubModelPart(const std::string& SubModelPartName){
        const auto delim_pos = SubModelPartName.find('.');
        const std::string& sub_model_part_name = SubModelPartName.substr(0, delim_pos);

        SubModelPartIterator i = mSubModelParts.find(sub_model_part_name);
        if(i == mSubModelParts.end()){
            ErrorNonExistingSubModelPart(sub_model_part_name);
        }

        if(delim_pos == std::string::npos){
            return (i.base()->second).get();
        } else {
            return i->pGetSubModelPart(SubModelPartName.substr(delim_pos+1));
        }
    }


    void ModelPart::RemoveSubModelPart(const std::string& SubModelPartName){
        const auto delim_pos = SubModelPartName.find('.');
        const std::string& sub_model_part_name = SubModelPartName.substr(0, delim_pos);

        SubModelPartIterator i = mSubModelParts.find(sub_model_part_name);
        if(delim_pos == std::string::npos){
            if(i == mSubModelParts.end()){
                std::stringstream warning_msg;
                warning_msg << "Trying to remove sub model part with name \"" << SubModelPartName
                    << "\" in model part \"" << FullName() << "\" which does not exist.\n"
                    << "The the following sub model parts are available:";
                for(const auto& r_avail_smp_name : GetSubModelPartNames()){
                    warning_msg << "\n\t" "\"" << r_avail_smp_name << "\"";
                }
                QUEST_WARNING("ModelPart") << warning_msg.str() << std::endl;
            } else {
                mSubModelParts.erase(SubModelPartName);
            }
        } else {
            if(i == mSubModelParts.end()){
                ErrorNonExistingSubModelPart(sub_model_part_name);
            }

            return i->RemoveSubModelPart(SubModelPartName.substr(delim_pos+1));
        }
    }


    void ModelPart::RemoveSubModelPart(ModelPart& ThisSubModelPart){
        std::string name = ThisSubModelPart.Name();

        SubModelPartIterator i_sub_model_part = mSubModelParts.find(name);

        QUEST_ERROR_IF(i_sub_model_part == mSubModelParts.end())
            << "The sub model part  \"" << name << "\" does not exist in the \"" << Name() << "\" model part to be removed" << std::endl;

        mSubModelParts.erase(name);
    }


    ModelPart& ModelPart::GetParentModelPart(){
        if(IsSubModelPart()){
            return *mpParentModelPart;
        } else {
            return *this;
        }
    }


    const ModelPart& ModelPart::GetParentModelPart() const{
        if(IsSubModelPart()){
            return *mpParentModelPart;
        } else {
            return *this;
        }
    }


    bool ModelPart::HasSubModelPart(const std::string& ThisSubModelPartName) const{
        const auto delim_pos = ThisSubModelPartName.find('.');
        const std::string& sub_model_part_name = ThisSubModelPartName.substr(0, delim_pos);

        auto i = mSubModelParts.find(sub_model_part_name);
        if(i == mSubModelParts.end()){
            return false;
        } else {
            if(delim_pos != std::string::npos){
                return i->HasSubModelPart(ThisSubModelPartName.substr(delim_pos+1));
            } else {
                return true;
            }
        }
    }


    std::vector<std::string> ModelPart::GetSubModelPartNames() const{
        std::vector<std::string> SubModelPartsNames;
        SubModelPartsNames.reserve(NumberOfSubModelParts());

        for(auto& r_sub_model_part : mSubModelParts){
            SubModelPartsNames.push_back(r_sub_model_part.Name());
        }

        return SubModelPartsNames;
    }


    void ModelPart::SetBufferSize(ModelPart::IndexType NewBufferSize){
        QUEST_ERROR_IF(IsSubModelPart()) << "Calling the method of the sub model part "
            << Name() << " please call the one of the root model part: "
            << GetRootModelPart().Name() << std::endl;

        for(auto& r_sub_model_part : mSubModelParts){
            r_sub_model_part.SetBufferSizeSubModelParts(NewBufferSize);
        }

        mBufferSize = NewBufferSize;

        auto nodes_begin = NodesBegin();
        const int nnodes = static_cast<int>(Nodes().size());
        #pragma omp parallel for firstprivate(nodes_begin,nnodes)
        for(int i = 0; i < nnodes; ++i){
            auto node_iterator = nodes_begin + i;
            node_iterator->SetBufferSize(NewBufferSize);
        }
    }


    void ModelPart::SetBufferSizeSubModelParts(ModelPart::IndexType NewBufferSize){
        for(auto& r_sub_model_part : mSubModelParts){
            r_sub_model_part.SetBufferSizeSubModelParts(NewBufferSize);
        }

        mBufferSize = NewBufferSize;
    }


    int ModelPart::Check() const{
        QUEST_TRY
        
        const ProcessInfo& r_current_process_info = this->GetProcessInfo();

        block_for_each(this->Elements(), [&r_current_process_info](const Element& rElement){
            rElement.Check(r_current_process_info);
        });

        block_for_each(this->Conditions(), [&r_current_process_info](const Condition& rCondition){
            rCondition.Check(r_current_process_info);
        });

        block_for_each(this->MasterSlaveConstraints(), [&r_current_process_info](const MasterSlaveConstraint& rMasterSlaveConstraint){
            rMasterSlaveConstraint.Check(r_current_process_info);
        });

        return 0;

        QUEST_CATCH("")
    }


    std::string ModelPart::Info() const{
        return "-" + mName + "- model part";
    }


    void ModelPart::PrintInfo(std::ostream& rOStream) const{
        rOStream << Info();
    }


    void ModelPart::PrintData(std::ostream& rOStream) const{
        DataValueContainer::PrintData(rOStream);

        if(!IsSubModelPart()){
            rOStream << "    Buffer size: " << mBufferSize << std::endl;
        }

        rOStream << "    Number of tables : " << NumberOfTables() << std::endl;
        rOStream << "    Number of sub model parts : " << NumberOfSubModelParts() << std::endl;
        if(!IsSubModelPart()){
            if(IsDistributed()){
                rOStream << "    Distributed; Communicator has " << mpCommunicator->TotalProcesses() << " total processes" << std::endl;
            }
            mpProcessInfo->PrintData(rOStream);
        }
        rOStream << std::endl;
        rOStream << "    Number of Geometries : " << mGeometries.NumberOfGeometries() << std::endl;
        for(IndexType i = 0; i < mMeshes.size(); ++i){
            rOStream << "    Mesh " << i << " :" << std::endl;
            GetMesh(i).PrintData(rOStream, "    ");
        }
        rOStream << std::endl;

        std::vector<std::string> sub_model_part_names;
        sub_model_part_names.reserve(NumberOfSubModelParts());
        for(const auto& r_sub_model_part : mSubModelParts){
            sub_model_part_names.push_back(r_sub_model_part.Name());
        }
        std::sort(sub_model_part_names.begin(), sub_model_part_names.end());

        for(const auto& r_sub_model_part_name : sub_model_part_names){
            const auto& r_sub_model_part = *(mSubModelParts.find(r_sub_model_part_name));
            r_sub_model_part.PrintInfo(rOStream, "    ");
            rOStream << std::endl;
            r_sub_model_part.PrintData(rOStream, "    ");
        }
    }


    void ModelPart::PrintInfo(std::ostream& rOStream, const std::string& PrefixString) const{
        rOStream << PrefixString << Info() << std::endl;
    }


    void ModelPart::PrintData(std::ostream& rOStream, const std::string& PrefixString) const{
        if(!IsSubModelPart()){
            rOStream << PrefixString << "Buffer size: " << mBufferSize << std::endl;
        }

        rOStream << PrefixString << "Number of tables : " << NumberOfTables() << std::endl;
        rOStream << PrefixString << "Number of sub model parts : " << NumberOfSubModelParts() << std::endl;
        if(!IsSubModelPart()){
            mpProcessInfo->PrintData(rOStream);
        }
        rOStream << std::endl;
        rOStream << PrefixString << "Number of Geometries : " << mGeometries.NumberOfGeometries() << std::endl;

        for(IndexType i = 0; i < mMeshes.size(); ++i){
            rOStream << PrefixString << "    Mesh " << i << " :" << std::endl;
            GetMesh(i).PrintData(rOStream, PrefixString + "    ");
        }

        std::vector<std::string> sub_model_part_names;
        sub_model_part_names.reserve(NumberOfSubModelParts());
        for(const auto& r_sub_model_part : mSubModelParts){
            sub_model_part_names.push_back(r_sub_model_part.Name());
        }
        std::sort(sub_model_part_names.begin(), sub_model_part_names.end());

        for(const auto& r_sub_model_part_name : sub_model_part_names){
            const auto& r_sub_model_part = *(mSubModelParts.find(r_sub_model_part_name));
            r_sub_model_part.PrintInfo(rOStream, PrefixString + "    ");
            rOStream << std::endl;
            r_sub_model_part.PrintData(rOStream, PrefixString + "    ");
        }
    }


    void ModelPart::save(Serializer& rSerializer) const {
        QUEST_SERIALIZE_SAVE_BASE_CLASS(rSerializer, DataValueContainer);
        QUEST_SERIALIZE_SAVE_BASE_CLASS(rSerializer, Flags);
        rSerializer.save("Name", mName);
        rSerializer.save("Buffer Size", mBufferSize);
        rSerializer.save("ProcessInfo", mpProcessInfo);
        rSerializer.save("Tables", mTables);
        rSerializer.save("Varaibles List", mpVariablesList);
        rSerializer.save("Meshes", mMeshes);
        rSerializer.save("Geometries", mGeometries);
        rSerializer.save("NumberOfSubModelParts", NumberOfSubModelParts());

        for(SubModelPartConstantIterator i_sub_model_part = SubModelPartsBegin(); i_sub_model_part != SubModelPartsEnd(); ++i_sub_model_part){
            rSerializer.save("SubModelPartName", i_sub_model_part->Name());
        }

        for(SubModelPartConstantIterator i_sub_model_part = SubModelPartsBegin(); i_sub_model_part != SubModelPartsEnd(); ++i_sub_model_part){
            rSerializer.save("SubModelPart", *i_sub_model_part);
        }
    }


    void ModelPart::load(Serializer& rSerializer) {
        QUEST_SERIALIZE_LOAD_BASE_CLASS(rSerializer, DataValueContainer);
        QUEST_SERIALIZE_LOAD_BASE_CLASS(rSerializer, Flags);
        std::string ModelPartName;
        rSerializer.load("Name", ModelPartName);

        QUEST_ERROR_IF(mName != ModelPartName)
            << "trying to load a model part called :   " << ModelPartName << "    into an object named :   " << mName << " the two names should coincide but do not" << std::endl;

        rSerializer.load("Buffer Size", mBufferSize);
        rSerializer.load("ProcessInfo", mpProcessInfo);
        rSerializer.load("Tables", mTables);    
        rSerializer.load("Varaibles List", mpVariablesList);
        rSerializer.load("Meshes", mMeshes);
        rSerializer.load("Geometries", mGeometries);

        SizeType number_of_subModelparts;
        rSerializer.load("NumberOfSubModelParts", number_of_subModelparts);

        std::vector<std::string> sub_model_part_names;
        for(SizeType i = 0; i < number_of_subModelparts; ++i){
            std::string sub_model_part_name;
            rSerializer.load("SubModelPartName", sub_model_part_name);
            sub_model_part_names.push_back(sub_model_part_name);
        }

        for(const auto& name : sub_model_part_names){
            auto& subpart = CreateSubModelPart(name);
            rSerializer.load("SubModelPart", subpart);
        }

        for(SubModelPartIterator i_sub_model_part = SubModelPartsBegin(); i_sub_model_part != SubModelPartsEnd(); ++i_sub_model_part){
            i_sub_model_part->SetParentModelPart(this);
        }
    }


    void ModelPart::ErrorNonExistingSubModelPart(const std::string& SubModelPartName) const{
        std::stringstream error_msg;
        error_msg << "There is no sub model part with name \"" << SubModelPartName
            << "\" in model part \"" << FullName() << "\"\n"
            << "The following sub model parts are available:";
        for(const auto& r_avail_smp_name : GetSubModelPartNames()){
            error_msg << "\n\t" "\"" << r_avail_smp_name << "\"";
        }
        QUEST_ERROR << error_msg.str() << std::endl;
    }

} // namespace Quest