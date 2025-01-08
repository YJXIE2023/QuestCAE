// 系统头文件
#include <set>

// 项目头文件
#include "includes/model_part_io.hpp"
#include "includes/quest_filesystem.hpp"
#include "IO/logger.hpp"
#include "utilities/compare_elements_and_conditions_utility.hpp"
#include "utilities/openmp_utils.hpp"
#include "utilities/quaternion.hpp"
#include "utilities/timer.hpp"


namespace Quest{

    /**
     * @brief 构造函数
     */
    ModelPartIO::ModelPartIO(std::filesystem::path const& Filename, const Flags Options)
        : mNumberOfLines(1)
        , mBaseFilename(Filename)
        , mOptions(Options)
    {
        Quest::shared_ptr<std::fstream> pFile = Quest::make_shared<std::fstream>();
        std::fstream::openmode OpenMode;

        // 设置打开模式
        if (mOptions.Is(IO::READ)) {
            OpenMode = std::fstream::in;
        } else if (mOptions.Is(IO::APPEND)) {
            OpenMode = std::fstream::in | std::fstream::app;
        } else if (mOptions.Is(IO::WRITE)) {
            OpenMode = std::fstream::out;
        } else {
            // If none of the READ, WRITE or APPEND are defined we will take READ as
            // default.
            OpenMode = std::fstream::in;
        }

        std::filesystem::path mdpa_file_name(Filename);
        mdpa_file_name += ".mdpa";
        std::filesystem::path time_file_name(Filename);
        time_file_name += ".time";

        pFile->open(mdpa_file_name.c_str(), OpenMode);

        QUEST_ERROR_IF_NOT(pFile->is_open()) << "Error opening mdpa file : " << mdpa_file_name << std::endl;

        mpStream = pFile;

        if (mOptions.IsNot(IO::SKIP_TIMER)) Timer::SetOuputFile(time_file_name.string());
    }


    /**
     * @brief 构造函数
     */
    ModelPartIO::ModelPartIO(Quest::shared_ptr<std::iostream> Stream, const Flags Options)
        : mNumberOfLines(1)
        , mOptions(Options)
    {
        QUEST_ERROR_IF(Stream == nullptr) << "Error: ModelPartIO Stream is invalid " << std::endl;

        QUEST_ERROR_IF(Stream == nullptr || Stream == Quest::shared_ptr<std::iostream>(NULL)) << "Error: ModelPartIO Stream is invalid " << std::endl;

        mpStream = Stream;
    }


    /**
     * @brief 析构函数
     */
    ModelPartIO::~ModelPartIO() {
        if (mOptions.IsNot(IO::SKIP_TIMER)) Timer::CloseOuputFile();
    }


    /**
     * @brief 读取一个节点
     */
    bool ModelPartIO::ReadNode(NodeType& rThisNode)
    {
        QUEST_ERROR << "Calling base class member. Please check the definition of derived class." << std::endl;
    }


    /**
     * @brief 读取一组节点
     */
    bool ModelPartIO::ReadNodes(NodesContainerType& rThisNodes)
    {
        QUEST_TRY
        ResetInput();
        std::string word;
        while(true)
        {
            ReadWord(word);
            if(mpStream->eof())
                break;
            ReadBlockName(word);
            if(word == "Nodes")
                ReadNodesBlock(rThisNodes);
            else
                SkipBlock(word);
        }

        return true;
        QUEST_CATCH("")
    }


    /**
     * @brief 读取节点数量
     */
    std::size_t ModelPartIO::ReadNodesNumber()
    {
        QUEST_TRY;
        ResetInput();
        std::string word;
        std::size_t num_nodes = 0;
        while(true)
        {
            ReadWord(word);
            if (mpStream->eof())
                break;
            ReadBlockName(word);
            if(word == "Nodes")
                num_nodes += CountNodesInBlock();
            else
                SkipBlock(word);
        }

        return num_nodes;
        QUEST_CATCH("");
    }


    void ModelPartIO::WriteNodes(NodesContainerType const& rThisNodes)
    {
        if (mOptions.Is(IO::SCIENTIFIC_PRECISION)) {
            (*mpStream) << std::setprecision(10) << std::scientific;
        }
        (*mpStream) << "Begin Nodes" << std::endl;
        for(NodesContainerType::const_iterator it_node = rThisNodes.begin() ; it_node != rThisNodes.end() ; ++it_node)
            (*mpStream) << "\t" << it_node->Id() << "\t" << it_node->X()  << "\t" << it_node->Y() << "\t" << it_node->Z() << "\n";
        (*mpStream) << "End Nodes" << std::endl << std::endl;
    }


    /**
     * @brief 读取一个属性对象
     */
    void ModelPartIO::ReadProperties(Properties& rThisProperties)
    {
        QUEST_ERROR << "Calling base class member. Please check the definition of derived class" << std::endl;
    }


    /**
     * @brief 读取一组属性对象
     */
    void ModelPartIO::ReadProperties(PropertiesContainerType& rThisProperties)
    {
        QUEST_TRY
        ResetInput();
        std::string word;
        while(true)
        {
            ReadWord(word);
            if(mpStream->eof())
                break;
            ReadBlockName(word);
            if(word == "Properties")
                ReadPropertiesBlock(rThisProperties);
            else
                SkipBlock(word);
        }
        QUEST_CATCH("")
    }


    /**
     * @brief 写入一组属性对象
     */
    void ModelPartIO::WriteProperties(PropertiesContainerType const& rThisProperties)
    {
        std::string aux_string;
        const std::string string_to_remove = "This properties contains 0 tables";

        if (rThisProperties.size() == 0) {
            (*mpStream) << "Begin Properties 0" << std::endl;
            (*mpStream) << "End Properties" << std::endl << std::endl;
        }

        for (auto i_properties = rThisProperties.begin() ; i_properties != rThisProperties.end() ; ++i_properties) {
            std::ostringstream aux_ostream;
            (*mpStream) << "Begin Properties " << i_properties->Id() << std::endl;
            i_properties->Data().PrintData(aux_ostream);

            aux_string = aux_ostream.str();

            if (i_properties->Has(CONSTITUTIVE_LAW)) {
                std::string::size_type it_constitutive_law_begin = aux_string.find("CONSTITUTIVE_LAW");

                if (it_constitutive_law_begin != std::string::npos) {
                std::string::size_type it_constitutive_law_end = aux_string.find('\n', it_constitutive_law_begin);
                    aux_string.erase(it_constitutive_law_begin, it_constitutive_law_end);
                }

                const ConstitutiveLaw::Pointer p_law = i_properties->GetValue(CONSTITUTIVE_LAW);
                auto components_cl = QuestComponents<ConstitutiveLaw>::GetComponents();
                std::string cl_name = "";
                for (const auto& comp_cl : components_cl) {
                    if (p_law->HasSameType(p_law.get(), comp_cl.second)) {
                        cl_name = comp_cl.first;
                        break;
                    }
                }
                if (cl_name != "") aux_string += "CONSTITUTIVE_LAW " + cl_name + "\n";
            }


            std::string::size_type it_to_remove = aux_string.find(string_to_remove);

            if (it_to_remove != std::string::npos) {
                aux_string.erase(it_to_remove, string_to_remove.length());
            }

            aux_string.erase(std::remove(aux_string.begin(), aux_string.end(), ':'), aux_string.end());

            (*mpStream) << aux_string << std::endl;
            (*mpStream) << "End Properties" << std::endl << std::endl;
        }
    }


    /**
     * @brief 读取一个几何对象
     */
    void ModelPartIO::ReadGeometry(
    NodesContainerType& rThisNodes,
    GeometryType::Pointer& pThisGeometries)
    {
        QUEST_ERROR << "Calling base class member. Please check the definition of derived class" << std::endl;
    }


    /**
     * @brief 读取一组几何对象
     */
    void ModelPartIO::ReadGeometries(
        NodesContainerType& rThisNodes,
        GeometryContainerType& rThisGeometries
    ){
        QUEST_TRY
        ResetInput();
        std::string word;
        while(true)
        {
            ReadWord(word);
            if(mpStream->eof())
                break;
            ReadBlockName(word);
            if(word == "Geometries")
                ReadGeometriesBlock(rThisNodes,rThisGeometries);
            else
                SkipBlock(word);
        }
        QUEST_CATCH("")
    }


    /**
     * @brief 读取几何体的相关关系
     */
    std::size_t ModelPartIO::ReadGeometriesConnectivities(ConnectivitiesContainerType& rGeometriesConnectivities){
        QUEST_TRY
        std::size_t number_of_geometries = 0;
        ResetInput();
        std::string word;
        while(true) {
            ReadWord(word);
            if(mpStream->eof())
                break;
            ReadBlockName(word);
            if(word == "Geometries")
                number_of_geometries += ReadGeometriesConnectivitiesBlock(rGeometriesConnectivities);
            else
                SkipBlock(word);
        }
        return number_of_geometries;

        QUEST_CATCH("")
    }


    /**
     * @brief 写入一组几何对象
     */
    void ModelPartIO::WriteGeometries(GeometryContainerType const& rThisGeometries){
        if (rThisGeometries.NumberOfGeometries() > 0) {
            std::string geometry_name;

            auto it_geometry = rThisGeometries.GeometriesBegin();
            auto geometries_components = QuestComponents<GeometryType>::GetComponents();

            CompareElementsAndConditionsUtility::GetRegisteredName(*it_geometry, geometry_name);

            (*mpStream) << "Begin Geometries\t" << geometry_name << std::endl;
            const auto it_geom_begin = rThisGeometries.Geometries().begin();
            (*mpStream) << "\t" << it_geom_begin->Id() << "\t";
            auto& r_geometry = *(it_geom_begin.base()->second);
            for (std::size_t i_node = 0; i_node < r_geometry.size(); i_node++)
                (*mpStream) << r_geometry[i_node].Id() << "\t";
            (*mpStream) << std::endl;

            auto it_geom_previous = it_geom_begin;
            auto it_geom_current = it_geom_begin;
            ++it_geom_current;

            for(std::size_t i = 1; i < rThisGeometries.NumberOfGeometries(); i++) {
                if(GeometryType::IsSame(*it_geom_previous, *it_geom_current)) {
                    (*mpStream) << "\t" << it_geom_current->Id() << "\t";
                    r_geometry = *(it_geom_current.base()->second);
                    for (std::size_t i_node = 0; i_node < r_geometry.size(); i_node++)
                        (*mpStream) << r_geometry[i_node].Id() << "\t";
                    (*mpStream) << std::endl;
                } else {
                    (*mpStream) << "End Geometries" << std::endl << std::endl;

                    CompareElementsAndConditionsUtility::GetRegisteredName(*it_geom_current, geometry_name);

                    (*mpStream) << "Begin Geometries\t" << geometry_name << std::endl;
                    (*mpStream) << "\t" << it_geom_current->Id() << "\t";
                    r_geometry = *(it_geom_current.base()->second);
                    for (std::size_t i_node = 0; i_node < r_geometry.size(); i_node++)
                        (*mpStream) << r_geometry[i_node].Id() << "\t";
                    (*mpStream) << std::endl;
                }

                ++it_geom_previous;
                ++it_geom_current;
            }

            (*mpStream) << "End Geometries" << std::endl << std::endl;
        }
    }


    /**
     * @brief 读取一个单元对象
     */
    void ModelPartIO::ReadElement(
        NodesContainerType& rThisNodes,
        PropertiesContainerType& rThisProperties,
        Element::Pointer& pThisElement
    ){
        QUEST_ERROR << "Calling base class member. Please check the definition of derived class" << std::endl;
    }


    /**
     * @brief 读取一组单元对象
     */
    void ModelPartIO::ReadElements(
        NodesContainerType& rThisNodes,
        PropertiesContainerType& rThisProperties,
        ElementsContainerType& rThisElements
    ){
        QUEST_TRY
        ResetInput();
        std::string word;
        while(true)
        {
            ReadWord(word);
            if(mpStream->eof())
                break;
            ReadBlockName(word);
            if(word == "Elements")
                ReadElementsBlock(rThisNodes,rThisProperties,rThisElements);
            else
                SkipBlock(word);
        }
        QUEST_CATCH("")
    }


    /**
     * @brief 读取单元关联关系
     */
    std::size_t  ModelPartIO::ReadElementsConnectivities(ConnectivitiesContainerType& rElementsConnectivities){
        QUEST_TRY
        std::size_t number_of_elements = 0;
        ResetInput();
        std::string word;
        while(true)
        {
            ReadWord(word);
            if(mpStream->eof())
                break;
            ReadBlockName(word);
            if(word == "Elements")
                number_of_elements += ReadElementsConnectivitiesBlock(rElementsConnectivities);
            else
                SkipBlock(word);
        }
        return number_of_elements;

        QUEST_CATCH("")
    }


    /**
     * @brief 写入一组单元
     */
    void ModelPartIO::WriteElements(ElementsContainerType const& rThisElements){
        if (rThisElements.size() > 0) {
            std::string element_name;

            auto it_element = rThisElements.begin();
            auto elements_components = QuestComponents<Element>::GetComponents();

            CompareElementsAndConditionsUtility::GetRegisteredName(*it_element, element_name);

            (*mpStream) << "Begin Elements\t" << element_name << std::endl;
            (*mpStream) << "\t" << rThisElements.begin()->Id() << "\t" << (rThisElements.begin()->pGetProperties())->Id() << "\t";
            for (std::size_t i_node = 0; i_node < rThisElements.begin()->GetGeometry().size(); i_node++)
                (*mpStream) << rThisElements.begin()->GetGeometry()[i_node].Id() << "\t";
            (*mpStream) << std::endl;

            for(std::size_t i = 1; i < rThisElements.size(); i++) {
                auto it_elem_previous = rThisElements.begin() + i - 1;
                auto it_elem_current = rThisElements.begin() + i;

                if(GeometricalObject::IsSame(*it_elem_previous, *it_elem_current)) {
                    (*mpStream) << "\t" << it_elem_current->Id() << "\t" << (it_elem_current->pGetProperties())->Id() << "\t";
                    for (std::size_t i_node = 0; i_node < it_elem_current->GetGeometry().size(); i_node++)
                        (*mpStream) << it_elem_current->GetGeometry()[i_node].Id() << "\t";
                    (*mpStream) << "\n";;
                } else {
                    (*mpStream) << "End Elements" << std::endl << std::endl;

                    CompareElementsAndConditionsUtility::GetRegisteredName(*it_elem_current, element_name);

                    (*mpStream) << "Begin Elements\t" << element_name << std::endl;
                    (*mpStream) << "\t" << it_elem_current->Id() << "\t" << (it_elem_current->pGetProperties())->Id() << "\t";
                    for (std::size_t i_node = 0; i_node < it_elem_current->GetGeometry().size(); i_node++)
                        (*mpStream) << it_elem_current->GetGeometry()[i_node].Id() << "\t";
                    (*mpStream) << "\n";;
                }
            }

            (*mpStream) << "End Elements" << std::endl << std::endl;
        }
    }


    /**
     * @brief 读取一组条件
     */
    void ModelPartIO::ReadConditions(
        NodesContainerType& rThisNodes,
        PropertiesContainerType& rThisProperties,
        ConditionsContainerType& rThisConditions
    ){
        QUEST_TRY
        ResetInput();
        std::string word;
        while(true)
        {
            ReadWord(word);
            if(mpStream->eof())
                break;
            ReadBlockName(word);
            if(word == "Conditions")
                ReadConditionsBlock(rThisNodes,rThisProperties,rThisConditions);
            else
                SkipBlock(word);
        }
        QUEST_CATCH("")
    }

    /**
     * @brief 读取条件关联关系
     */
    std::size_t ModelPartIO::ReadConditionsConnectivities(ConnectivitiesContainerType& rConditionsConnectivities){
        QUEST_TRY
        std::size_t number_of_elements = 0;
        ResetInput();
        std::string word;
        while(true)
        {
            ReadWord(word);
            if(mpStream->eof())
                break;
            ReadBlockName(word);
            if(word == "Conditions")
                number_of_elements += ReadConditionsConnectivitiesBlock(rConditionsConnectivities);
            else
                SkipBlock(word);
        }
        return number_of_elements;
        QUEST_CATCH("")
    }

    /**
     * @brief 写入一组条件
     */
    void ModelPartIO::WriteConditions(ConditionsContainerType const& rThisConditions){
        if (rThisConditions.size() > 0) {
            std::string condition_name;

            auto it_condition = rThisConditions.begin();
            auto conditions_components = QuestComponents<Condition>::GetComponents();

            // First we do the first condition
            CompareElementsAndConditionsUtility::GetRegisteredName(*it_condition, condition_name);

            (*mpStream) << "Begin Conditions\t" << condition_name << std::endl;
            (*mpStream) << "\t" << rThisConditions.begin()->Id() << "\t" << (rThisConditions.begin()->pGetProperties())->Id() << "\t";
            for (std::size_t i_node = 0; i_node < rThisConditions.begin()->GetGeometry().size(); i_node++)
                (*mpStream) << rThisConditions.begin()->GetGeometry()[i_node].Id() << "\t";
            (*mpStream) << std::endl;

            // Now we iterate over all the conditions
            for(std::size_t i = 1; i < rThisConditions.size(); i++) {
                auto it_cond_previous = rThisConditions.begin() + i - 1;
                auto it_cond_current = rThisConditions.begin() + i;

                if(GeometricalObject::IsSame(*it_cond_previous, *it_cond_current)) {
                    (*mpStream) << "\t" << it_cond_current->Id() << "\t" << (it_cond_current->pGetProperties())->Id() << "\t";
                    for (std::size_t i_node = 0; i_node < it_cond_current->GetGeometry().size(); i_node++)
                        (*mpStream) << it_cond_current->GetGeometry()[i_node].Id() << "\t";
                    (*mpStream) << "\n";;
                } else {
                    (*mpStream) << "End Conditions" << std::endl << std::endl;

                    CompareElementsAndConditionsUtility::GetRegisteredName(*it_cond_current, condition_name);

                    (*mpStream) << "Begin Conditions\t" << condition_name << std::endl;
                    (*mpStream) << "\t" << it_cond_current->Id() << "\t" << (it_cond_current->pGetProperties())->Id() << "\t";
                    for (std::size_t i_node = 0; i_node < it_cond_current->GetGeometry().size(); i_node++)
                        (*mpStream) << it_cond_current->GetGeometry()[i_node].Id() << "\t";
                    (*mpStream) << "\n";;
                }
            }

            (*mpStream) << "End Conditions" << std::endl << std::endl;
        }
    }


    /**
     * @brief 读取模型部件初始值
     */
    void ModelPartIO::ReadInitialValues(ModelPart& rThisModelPart){
        QUEST_TRY

        ElementsContainerType& rThisElements = rThisModelPart.Elements();
        ConditionsContainerType& rThisConditions = rThisModelPart.Conditions();


        ResetInput();
        std::string word;
        while(true)
        {
            ReadWord(word);
            if(mpStream->eof())
                break;
            ReadBlockName(word);
            if(word == "NodalData")
                ReadNodalDataBlock(rThisModelPart);
            else if(word == "ElementalData")
                ReadElementalDataBlock(rThisElements);
            else if(word == "ConditionalData")
                ReadConditionalDataBlock(rThisConditions);
            else
                SkipBlock(word);
        }
        QUEST_CATCH("")
    }


    /**
     * @brief 读取一个网格
     */
    void ModelPartIO::ReadMesh(MeshType & rThisMesh){
        QUEST_ERROR << "ModelPartIO does not implement this method." << std::endl;
    }


    /**
     * @brief 写入一个网格
     */
    void ModelPartIO::WriteMesh(const MeshType & rThisMesh){
        WriteProperties(rThisMesh.Properties());
        WriteNodes(rThisMesh.Nodes());
        WriteElements(rThisMesh.Elements());
        WriteConditions(rThisMesh.Conditions());
    }


    /**
     * @brief 读取模型部件
     */
    void ModelPartIO::ReadModelPart(ModelPart & rThisModelPart){
        QUEST_TRY

        Timer::Start("Reading Input");

        ResetInput();
        std::string word;
        while(true) {
            ReadWord(word);
            if(mpStream->eof())
                break;
            ReadBlockName(word);
            if(word == "ModelPartData") {
                if (mOptions.IsNot(IO::MESH_ONLY)) {
                    ReadModelPartDataBlock(rThisModelPart);
                } else {
                    SkipBlock("ModelPartData");
                }
            } else if(word == "Table") {
                if (mOptions.IsNot(IO::MESH_ONLY)) {
                    ReadTableBlock(rThisModelPart.Tables());
                } else {
                    SkipBlock("Table");
                }
            } else if(word == "Properties") {
                ReadPropertiesBlock(rThisModelPart.rProperties());
            } else if(word == "Nodes") {
                ReadNodesBlock(rThisModelPart);
            } else if(word == "Geometries") {
                ReadGeometriesBlock(rThisModelPart);
            } else if(word == "Elements") {
                ReadElementsBlock(rThisModelPart);
            } else if(word == "Conditions") {
                ReadConditionsBlock(rThisModelPart);
            } else if(word == "NodalData") {
                if (mOptions.IsNot(IO::MESH_ONLY)) {
                    ReadNodalDataBlock(rThisModelPart);
                } else {
                    SkipBlock("NodalData");
                }
            } else if(word == "ElementalData") {
                if (mOptions.IsNot(IO::MESH_ONLY)) {
                    ReadElementalDataBlock(rThisModelPart.Elements());
                } else {
                    SkipBlock("ElementalData");
                }
            } else if (word == "ConditionalData") {
                if (mOptions.IsNot(IO::MESH_ONLY)) {
                    ReadConditionalDataBlock(rThisModelPart.Conditions());
                } else {
                    SkipBlock("ConditionalData");
                }
            } else if(word == "CommunicatorData") {
                if (mOptions.IsNot(IO::MESH_ONLY)) {
                    ReadCommunicatorDataBlock(rThisModelPart.GetCommunicator(), rThisModelPart.Nodes());
                    rThisModelPart.GetCommunicator().LocalMesh().Elements() = rThisModelPart.Elements();
                    rThisModelPart.GetCommunicator().LocalMesh().Conditions() = rThisModelPart.Conditions();
                } else {
                    SkipBlock("CommunicatorData");
                }
            } else if (word == "Mesh") {
                ReadMeshBlock(rThisModelPart);
            } else if (word == "SubModelPart") {
                ReadSubModelPartBlock(rThisModelPart, rThisModelPart);
            }
        }
        QUEST_INFO("ModelPartIO") << "  [Total Lines Read : " << mNumberOfLines<<"]" << std::endl;
        Timer::Stop("Reading Input");
        QUEST_CATCH("")
    }


    /**
     * @brief 写入模型部件
     */
    void ModelPartIO::WriteModelPart(const ModelPart & rThisModelPart){
        QUEST_ERROR_IF_NOT(mOptions.Is(IO::WRITE) || mOptions.Is(IO::APPEND)) << "ModelPartIO needs to be created in write or append mode to write a ModelPart!" << std::endl;

        Timer::Start("Writing Output");

        if (mOptions.IsNot(IO::MESH_ONLY))
            WriteTableBlock(rThisModelPart.Tables());
        WriteMesh(rThisModelPart.GetMesh());
        WriteGeometries(rThisModelPart.Geometries());
        if (mOptions.IsNot(IO::MESH_ONLY)) {
            WriteNodalDataBlock(rThisModelPart);
            WriteDataBlock(rThisModelPart.Elements(), "Element");
            WriteDataBlock(rThisModelPart.Conditions(),"Condition");
        }

        WriteSubModelPartBlock(rThisModelPart, "");

        QUEST_INFO("ModelPartIO") << "  [Total Lines Wrote : " << mNumberOfLines<<"]" << std::endl;

        Timer::Stop("Writing Output");
    }


    /**
     * @brief 读取节点图
     */
    std::size_t ModelPartIO::ReadNodalGraph(ConnectivitiesContainerType& rAuxConnectivities){
        ResetInput();
        std::string word;
        while(true) {
            ReadWord(word);
            if(mpStream->eof())
                break;
            ReadBlockName(word);
            if (word == "Nodes") {
                ScanNodeBlock();
            } else if (word == "Geometries") {
                FillNodalConnectivitiesFromGeometryBlock(rAuxConnectivities);
            } else if (word == "Elements") {
                FillNodalConnectivitiesFromElementBlock(rAuxConnectivities);
            } else if (word == "Conditions") {
                FillNodalConnectivitiesFromConditionBlock(rAuxConnectivities);
            } else {
                SkipBlock(word);
            }
        }

        // Checking the connectivities
        SizeType n=0;
        for (const auto& r_conn : rAuxConnectivities) {
            n++;
            QUEST_ERROR_IF(r_conn.size() == 0) << "Node #" << n << " caused an error during the construction of the nodal graph. Possible reasons are:\n"
                << "The node is a hanging node, not connected to any element or condition\n"
                << "The nodes are not consecutively numbered. This can be avoided by using the \"ReorderConsecutiveModelPartIO\"" << std::endl;
        }

        for (auto it = rAuxConnectivities.begin(); it != rAuxConnectivities.end(); it++) {
            std::sort(it->begin(),it->end());
            std::vector<SizeType>::iterator unique_end = std::unique(it->begin(),it->end());
            it->resize(unique_end - it->begin());
        }
        const SizeType num_nodes = rAuxConnectivities.size();

        return num_nodes;
    }


    /**
     * @brief 将模型部件分区
     */
    void ModelPartIO::DivideInputToPartitions(
        SizeType NumberOfPartitions,
        const PartitioningInfo& rPartitioningInfo
    ){
        QUEST_TRY

        const auto raw_file_name = mBaseFilename.stem();
        const auto folder_name = mBaseFilename.parent_path() / raw_file_name += "_partitioned";

        std::filesystem::remove_all(folder_name);
        FilesystemExtensions::MPISafeCreateDirectories(folder_name.string());

        OutputFilesContainerType output_files;
        output_files.reserve(NumberOfPartitions);

        for(SizeType i = 0 ; i < NumberOfPartitions ; i++)
        {
            const std::filesystem::path full_file_name = folder_name / raw_file_name += "_"+std::to_string(i)+".mdpa";
            std::ofstream* p_ofstream = new std::ofstream(full_file_name);
            QUEST_ERROR_IF_NOT(*p_ofstream) << "Error opening mdpa file : " << full_file_name << std::endl;

            output_files.push_back(p_ofstream);
        }

        DivideInputToPartitionsImpl(
            output_files,
            NumberOfPartitions,
            rPartitioningInfo);

        for(SizeType i = 0 ; i < NumberOfPartitions ; i++)
            delete output_files[i];

        QUEST_CATCH("")
    }


    /**
     * @Brief 将模型部件分区
     */
    void ModelPartIO::DivideInputToPartitions(
        Quest::shared_ptr<std::iostream> * pStreams,
        SizeType NumberOfPartitions,
        const PartitioningInfo& rPartitioningInfo
    ){

        QUEST_TRY

        OutputFilesContainerType output_files;
        output_files.reserve(NumberOfPartitions);

        for(SizeType i = 0 ; i < NumberOfPartitions ; i++)
        {
            output_files.push_back(static_cast<std::ostream *>(&*pStreams[i]));
        }

        DivideInputToPartitionsImpl(
            output_files,
            NumberOfPartitions,
            rPartitioningInfo);

        QUEST_CATCH("")
    }

    /**
     * @brief 交换流
     */
    void ModelPartIO::SwapStreamSource(Quest::shared_ptr<std::iostream> newStream){
        mpStream.swap(newStream);
    }


    /**
     * @brief 读取子模型部件单元和条件ID
     */
    void ModelPartIO::ReadSubModelPartElementsAndConditionsIds(
        std::string const& rModelPartName,
        std::unordered_set<SizeType> &rElementsIds,
        std::unordered_set<SizeType> &rConditionsIds
    ){
        QUEST_TRY
        ResetInput();
        std::string word;
        bool read_entities = false;
        while(true)
        {
            ReadWord(word);
            if(mpStream->eof())
                break;
            if (word == "SubModelPartElements" && read_entities){
                while(!mpStream->eof()) {
                    ReadWord(word); // Reading the element id or End
                    if(CheckEndBlock("SubModelPartElements", word))
                        break;
                    SizeType element_id;
                    ExtractValue(word,element_id);
                    rElementsIds.insert(ReorderedElementId(element_id));
                }
            }
            else if (word == "SubModelPartConditions"  && read_entities) {
                while(!mpStream->eof()) {
                    ReadWord(word); // Reading the condition id or End
                    if(CheckEndBlock("SubModelPartConditions", word))
                        break;
                    SizeType condition_id;
                    ExtractValue(word,condition_id);
                    rConditionsIds.insert(ReorderedConditionId(condition_id));
                }
                read_entities = false;
            }
            else if (word == rModelPartName) {
                read_entities = true;
            }
        }

        QUEST_CATCH("")
    }


    /**
     * @brief 读取节点图并从给定的实体列表中提取必要的连接信息
     */
    std::size_t ModelPartIO::ReadNodalGraphFromEntitiesList(
        ConnectivitiesContainerType& rAuxConnectivities,
        std::unordered_set<SizeType> &rElementsIds,
        std::unordered_set<SizeType> &rConditionsIds
    ){
        QUEST_TRY

        ResetInput();
        std::string word;
        while(true) {
            ReadWord(word);
            if(mpStream->eof())
                break;
            ReadBlockName(word);
            if (word == "Nodes") {
                ScanNodeBlock();
            } else if (word == "Geometries") {
                FillNodalConnectivitiesFromGeometryBlockInList(rAuxConnectivities, rElementsIds);
            } else if (word == "Elements") {
                FillNodalConnectivitiesFromElementBlockInList(rAuxConnectivities, rElementsIds);
            } else if (word == "Conditions") {
                FillNodalConnectivitiesFromConditionBlockInList(rAuxConnectivities, rConditionsIds);
            } else {
                SkipBlock(word);
            }
        }

        for (auto it = rAuxConnectivities.begin(); it != rAuxConnectivities.end(); it++) {
            std::sort(it->begin(),it->end());
            std::vector<SizeType>::iterator unique_end = std::unique(it->begin(),it->end());
            it->resize(unique_end - it->begin());
        }
        const SizeType num_nodes = rAuxConnectivities.size();

        return num_nodes;
        QUEST_CATCH("")
    }


    /**
     * @brief 读取块的名称
     */
    std::string& ModelPartIO::ReadBlockName(std::string& rBlockName){
        QUEST_TRY

        CheckStatement("Begin", rBlockName);
        ReadWord(rBlockName);

        return rBlockName;

        QUEST_CATCH("")
    }

    /**
     * @brief 跳过指定名称的块（Block）中的数据
     */
    void ModelPartIO::SkipBlock(std::string const& BlockName){
        QUEST_TRY

        std::string word;
        int number_of_nested_blocks = 0;

        while(!mpStream->eof())
        {
            ReadWord(word);
            if(word == "End")
            {
                ReadWord(word);
                if(number_of_nested_blocks == 0) {
                    CheckStatement(word , BlockName);
                    break;
                } else {
                    number_of_nested_blocks--;
                }
            }
            else if(word == "Begin")
            {
                number_of_nested_blocks++;
            }
        }

        QUEST_CATCH("")
    }

    /**
     * @brief 检查当前数据是否已达到指定块（Block）的结尾
     */
    bool ModelPartIO::CheckEndBlock(std::string const& BlockName, std::string& rWord){
        if(rWord == "End")
        {
            ReadWord(rWord);
            CheckStatement(BlockName, rWord);
            return true;
        }

        return false;
    }

    /**
     * @brief 读取一个模型部件（ModelPart）的数据块
     */
    void ModelPartIO::ReadModelPartDataBlock(ModelPart& rModelPart, const bool is_submodelpart){
        QUEST_TRY

        std::string variable_name;

        while(!mpStream->eof())
        {
            ReadWord(variable_name);
            if(!is_submodelpart){
                if(CheckEndBlock("ModelPartData", variable_name))
                    break;
            }
            else {
                if(CheckEndBlock("SubModelPartData", variable_name))
                    break;
            }
            if(QuestComponents<Variable<double> >::Has(variable_name))
            {
                std::string value;
                double temp;

                ReadWord(value); 
                ExtractValue(value,temp);
                rModelPart[QuestComponents<Variable<double> >::Get(variable_name)] = temp;
            }
            else if(QuestComponents<Variable<bool> >::Has(variable_name))
            {
                std::string value;
                bool temp;

                ReadWord(value); 
                ExtractValue(value,temp);
                rModelPart[QuestComponents<Variable<bool> >::Get(variable_name)] = temp;
            }
            else if(QuestComponents<Variable<int> >::Has(variable_name))
            {
                std::string value;
                int temp;

                ReadWord(value);
                ExtractValue(value,temp);
                rModelPart[QuestComponents<Variable<int> >::Get(variable_name)] = temp;
            }
            else if(QuestComponents<Variable<Array1d<double, 3> > >::Has(variable_name))
            {
                Vector temp_vector; 
                ReadVectorialValue(temp_vector);
                rModelPart[QuestComponents<Variable<Array1d<double,3> > >::Get(variable_name)] = temp_vector;
            }
            else if(QuestComponents<Variable<Quaternion<double> > >::Has(variable_name))
            {
                Vector temp_vector;
                ReadVectorialValue(temp_vector);
                rModelPart[QuestComponents<Variable<Quaternion<double> > >::Get(variable_name)] = temp_vector;
            }
            else if(QuestComponents<Variable<Matrix> >::Has(variable_name))
            {
                ReadVectorialValue(rModelPart[QuestComponents<Variable<Matrix> >::Get(variable_name)]);
            }
            else if(QuestComponents<Variable<std::string> >::Has(variable_name))
            {
                std::string value, temp;

                ReadWord(value); 
                ExtractValue(value,temp);
                rModelPart[QuestComponents<Variable<std::string> >::Get(variable_name)] = temp;
            }
            else
            {
                std::stringstream buffer;
                buffer << variable_name << " is not a valid variable!!!" << std::endl;
                buffer << " [Line " << mNumberOfLines << " ]";
                QUEST_ERROR << buffer.str() << std::endl;
            }
        }

        QUEST_CATCH("")
    }

    /**
     * @brief 写入一个模型部件（ModelPart）的数据块
     */
    void ModelPartIO::WriteModelPartDataBlock(ModelPart& rModelPart, const bool is_submodelpart){
        QUEST_TRY;

        (*mpStream) << "Begin ModelPartData" << std::endl;
        (*mpStream) << "End ModelPartData" << std::endl;

        QUEST_CATCH("");
    }

    /**
     * @brief 读取一个包含表格数据的块（TableBlock）
     */
    template<class TablesContainerType>
    void ModelPartIO::ReadTableBlock(TablesContainerType& rTables){
        QUEST_TRY

        ModelPart::TableType temp_table;

        std::string word;

        std::string variable_name;
        ReadWord(variable_name);

        if(!QuestComponents<VariableData>::Has(variable_name))
        {
            std::stringstream buffer;
            buffer << variable_name << " is not a valid argument variable!!! Table only accepts double arguments." << std::endl;
            buffer << " [Line " << mNumberOfLines << " ]";
            QUEST_ERROR << buffer.str() << std::endl;

        }

        VariableData const& r_x_variable = QuestComponents<VariableData>::Get(variable_name);

        ReadWord(variable_name);

        if(!QuestComponents<VariableData>::Has(variable_name))
        {
            std::stringstream buffer;
            buffer << variable_name << " is not a valid value variable!!! Table only accepts double values." << std::endl;
            buffer << " [Line " << mNumberOfLines << " ]";
            QUEST_ERROR << buffer.str() << std::endl;

        }
        VariableData const& r_y_variable = QuestComponents<VariableData>::Get(variable_name);

        while(!mpStream->eof())
        {
            double x;
            double y;
            ReadWord(word);
            if(CheckEndBlock("Table", word))
                break;

            ExtractValue(word, x);
            ReadWord(word);
            ExtractValue(word, y);

            temp_table.insert(x,y);
        }

        rTables.SetTable(r_x_variable, r_y_variable, temp_table);

        QUEST_CATCH("")
    }

    /**
     * @brief 读取一组表格
     */
    void ModelPartIO::ReadTableBlock(ModelPart::TablesContainerType& rTables){
        QUEST_TRY

        ModelPart::TableType temp_table;

        SizeType table_id;
        std::string word;

        ReadWord(word);
        ExtractValue(word, table_id);

        std::string variable_name;

        ReadWord(variable_name);
        temp_table.SetNameOfX(variable_name);

        ReadWord(variable_name);
        temp_table.SetNameOfY(variable_name);

        while(!mpStream->eof())
        {
            double x;
            double y;
            ReadWord(word);
            if(CheckEndBlock("Table", word))
                break;

            ExtractValue(word, x);
            ReadWord(word);
            ExtractValue(word, y);

            temp_table.insert(x,y);
        }

        rTables.insert(table_id, temp_table);

        QUEST_CATCH("")
    }

    /**
     * @brief 将表格数据写入文件或数据流中的块（TableBlock）
     */
    template<class TablesContainerType>
    void ModelPartIO::WriteTableBlock(TablesContainerType& rTables){
        std::string variable1, variable2; 

        variable1 = "DISTANCE";
        variable2 = "TIME";

        auto numTables = rTables.end() - rTables.begin();

        for(unsigned int i = 0; i < numTables; i++)
        {
            auto itTable = rTables.begin() + i;

            const auto& Data = itTable->Data();
            std::size_t size = Data.size();

            (*mpStream) << "Begin Table" << i << "\t" << variable1 << "\t" << variable2 << " // NOTE: The variables does not correspond with the real ones. Right now the QUEST Table's does not store the variables"<< std::endl;

            for(std::size_t j = 1 ; j < size ; j++)
            {
                const auto X = Data[j].first;
                const auto Y = (Data[j].second)[0];

                (*mpStream) << X << "\t" << Y << std::endl;
            }

            (*mpStream) << "End Table" << std::endl;
        }
    }

    /**
     * @brief 将表格数据写入表格容器中
     */
    void ModelPartIO::WriteTableBlock(ModelPart::TablesContainerType& rTables){
        std::string variable1, variable2; 

        variable1 = "DISTANCE";
        variable2 = "TIME";

        auto numTables = rTables.end() - rTables.begin();

        for(unsigned int i = 0; i < numTables; i++)
        {
            auto itTable = rTables.begin() + i;

            const auto& Data = itTable->Data();
            std::size_t size = Data.size();

            (*mpStream) << "Begin Table" << i << "\t" << variable1 << "\t" << variable2 << " // NOTE: The variables does not correspond with the real ones. Right now the QUEST Table's does not store the variables"<< std::endl;

            for(std::size_t j = 1 ; j < size ; j++)
            {
                const auto X = Data[j].first;
                const auto Y = (Data[j].second)[0];

                (*mpStream) << X << "\t" << Y << std::endl;
            }

            (*mpStream) << "End Table" << std::endl;
        }
    }

    /**
     * @brief 读取包含节点数据的块（NodesBlock）
     */
    void ModelPartIO::ReadNodesBlock(NodesContainerType& rThisNodes){
        QUEST_TRY

        SizeType temp_id;
        double x,y,z;
        std::string word;

        SizeType number_of_nodes_read = 0;

        QUEST_INFO("ModelPartIO") << "  [Reading Nodes    : ";

        while(!mpStream->eof())
        {
            ReadWord(word);
            if(CheckEndBlock("Nodes", word))
                break;

            ExtractValue(word, temp_id);
            ReadWord(word);
            ExtractValue(word, x);
            ReadWord(word);
            ExtractValue(word, y);
            ReadWord(word);
            ExtractValue(word, z);
            NodeType::Pointer temp_node = Quest::make_intrusive< NodeType >( ReorderedNodeId(temp_id), x, y, z);
            temp_node->X0() = temp_node->X();
            temp_node->Y0() = temp_node->Y();
            temp_node->Z0() = temp_node->Z();

            rThisNodes.push_back(temp_node);
            number_of_nodes_read++;
        }
        QUEST_INFO("") << number_of_nodes_read << " nodes read]" << std::endl;

        unsigned int numer_of_nodes_read = rThisNodes.size();
        rThisNodes.Unique();
        QUEST_WARNING_IF("ModelPartIO", rThisNodes.size() != numer_of_nodes_read) << "attention! we read " << numer_of_nodes_read << " but there are only " << rThisNodes.size() << " non repeated nodes" << std::endl;

        QUEST_CATCH("")
    }

    /**
     * @brief 读取包含节点数据的块，专门针对 ModelPart 对象
     */
    void ModelPartIO::ReadNodesBlock(ModelPart& rModelPart){
        QUEST_TRY

        SizeType id;
        double x;
        double y;
        double z;

        std::string word;

        SizeType number_of_nodes_read = 0;
        const unsigned int old_size = rModelPart.Nodes().size();

        typedef std::map< unsigned int, Array1d<double,3> > map_type;
        map_type read_coordinates;

        QUEST_INFO("ModelPartIO") << "  [Reading Nodes    : ";

        while(!mpStream->eof())
        {
            ReadWord(word);
            if(CheckEndBlock("Nodes", word))
                break;

            ExtractValue(word, id);
            ReadWord(word);
            ExtractValue(word, x);
            ReadWord(word);
            ExtractValue(word, y);
            ReadWord(word);
            ExtractValue(word, z);

            Array1d<double,3> coords;
            coords[0]=x;
            coords[1]=y;
            coords[2]=z;
            read_coordinates[ReorderedNodeId(id)] = coords;
            number_of_nodes_read++;
        }

        const int nnodes = read_coordinates.size();
        const int nthreads = ParallelUtilities::GetNumThreads();
        std::vector<int> partition;
        OpenMPUtils::DivideInPartitions(nnodes, nthreads, partition);

        map_type::const_iterator it = read_coordinates.begin();
        for(int ithread=0; ithread<nthreads; ithread++)
        {
            #pragma omp parallel
            {
                if(OpenMPUtils::ThisThread() == ithread)
                {
                    for(int i=partition[ithread]; i<partition[ithread+1]; i++)
                    {
                        const unsigned int node_id = it->first;
                        const Array1d<double,3>& coords = it->second;
                        rModelPart.CreateNewNode(node_id,coords[0],coords[1],coords[2]);
                        it++;
                    }
                }
            }
        }

        QUEST_INFO("") << number_of_nodes_read << " nodes read]" << std::endl;
        QUEST_WARNING_IF("ModelPartIO", rModelPart.Nodes().size() - old_size != number_of_nodes_read) << "attention! we read " << number_of_nodes_read << " but there are only " << rModelPart.Nodes().size() - old_size<< " non repeated nodes" << std::endl;

        QUEST_CATCH("")
    }

    /**
     * @brief 计算当前块（NodesBlock）中包含的节点数量
     */
    std::size_t ModelPartIO::CountNodesInBlock(){
        QUEST_TRY;

        std::vector<SizeType> found_ids;

        SizeType temp_id;

        std::string word;

        SizeType number_of_nodes_read = 0;

        while(!mpStream->eof())
        {
            ReadWord(word);
            if(CheckEndBlock("Nodes", word))
                break;

            ExtractValue(word, temp_id);
            found_ids.push_back(temp_id);

            ReadWord(word);
            ReadWord(word); 
            ReadWord(word); 

            number_of_nodes_read++;
        }

        std::sort(found_ids.begin(),found_ids.end());
        std::vector<std::size_t>::iterator unique_end = std::unique(found_ids.begin(),found_ids.end());
        std::size_t number_of_unique_nodes = std::distance(found_ids.begin(),unique_end);

        QUEST_WARNING_IF("ModelPartIO", number_of_unique_nodes != number_of_nodes_read) << "attention! we read " << number_of_nodes_read << " but there are only " << number_of_unique_nodes << " non repeated nodes" << std::endl;

        return number_of_nodes_read;

        QUEST_CATCH("");
    }

    /**
     * @brief 读取包含属性数据的块
     */
    void ModelPartIO::ReadPropertiesBlock(PropertiesContainerType& rThisProperties){
        QUEST_TRY

        Properties::Pointer props = Quest::make_shared<Properties>();
        Properties& temp_properties = *props;
        //Properties temp_properties;

        std::string word;
        std::string variable_name;

        SizeType temp_properties_id;

        ReadWord(word);
        ExtractValue(word, temp_properties_id);
        temp_properties.SetId(temp_properties_id);

        while(!mpStream->eof())
        {
            ReadWord(variable_name);
            if(CheckEndBlock("Properties", variable_name))
                break;

            if(variable_name == "Begin") 
            {
                ReadBlockName(variable_name);
                if(variable_name == "Table") 
                    ReadTableBlock(temp_properties);
            }
            else if(QuestComponents<Variable<std::string> >::Has(variable_name))
            {
                std::string value;
                std::string  temp;

                ReadWord(value); 
                ExtractValue(value,temp);
                temp_properties[QuestComponents<Variable<std::string> >::Get(variable_name)] = temp;
            }
            else if(QuestComponents<Variable<double> >::Has(variable_name))
            {
                std::string value;
                double temp;

                ReadWord(value); 
                ExtractValue(value,temp);
                temp_properties[QuestComponents<Variable<double> >::Get(variable_name)] = temp;
            }
            else if(QuestComponents<Variable<int> >::Has(variable_name))
            {
                std::string value;
                int temp;

                ReadWord(value); 
                ExtractValue(value,temp);
                temp_properties[QuestComponents<Variable<int> >::Get(variable_name)] = temp;
            }
            else if(QuestComponents<Variable<bool> >::Has(variable_name))
            {
                std::string value;
                bool temp;

                ReadWord(value); 
                ExtractValue(value,temp);
                temp_properties[QuestComponents<Variable<bool> >::Get(variable_name)] = temp;
            }
            else if(QuestComponents<Variable<Array1d<double, 3> > >::Has(variable_name))
            {
                Vector temp_vector; 
                ReadVectorialValue(temp_vector);
                temp_properties[QuestComponents<Variable<Array1d<double,3> > >::Get(variable_name)] = temp_vector;
            }
            else if(QuestComponents<Variable<Quaternion<double> > >::Has(variable_name))
            {
                Vector temp_vector; 
                ReadVectorialValue(temp_vector);
                temp_properties[QuestComponents<Variable<Quaternion<double> > >::Get(variable_name)] = temp_vector;
            }
            else if(QuestComponents<Variable<Vector> >::Has(variable_name))
            {
                ReadVectorialValue(temp_properties[QuestComponents<Variable<Vector> >::Get(variable_name)]);
            }
            else if(QuestComponents<Variable<Matrix> >::Has(variable_name))
            {
                ReadVectorialValue(temp_properties[QuestComponents<Variable<Matrix> >::Get(variable_name)]);
            }
            else if(QuestComponents<Variable<ConstitutiveLaw::Pointer> >::Has(variable_name))
            {
                ReadConstitutiveLawValue(temp_properties[QuestComponents<Variable<ConstitutiveLaw::Pointer> >::Get(variable_name)]);
            }
            else
            {
                std::stringstream buffer;
                buffer << variable_name << " is not a valid variable!!!" << std::endl;
                buffer << " [Line " << mNumberOfLines << " ]";
                QUEST_ERROR << buffer.str() << std::endl;
            }

        }

        rThisProperties.push_back(props);

        QUEST_CATCH("")
    }

    /**
     * @brief 读取包含属性数据的块
     */
    void ModelPartIO::ReadGeometriesBlock(ModelPart& rModelPart){
        QUEST_TRY

        SizeType id;
        SizeType node_id;
        SizeType number_of_read_geometries = 0;

        std::string word;
        std::string geometry_name;

        ReadWord(geometry_name);
        QUEST_INFO("ModelPartIO") << "  [Reading Geometries : ";

        if(!QuestComponents<GeometryType>::Has(geometry_name)) {
            std::stringstream buffer;
            buffer << "Geometry " << geometry_name << " is not registered in Quest.";
            buffer << " Please check the spelling of the geometry name and see if the application which containing it, is registered correctly.";
            buffer << " [Line " << mNumberOfLines << " ]";
            QUEST_ERROR << buffer.str() << std::endl;
            return;
        }

        GeometryType const& r_clone_geometry = QuestComponents<GeometryType>::Get(geometry_name);
        SizeType number_of_nodes = r_clone_geometry.size();
        Element::NodesArrayType temp_geometry_nodes;
        ModelPart::GeometryContainerType aux_geometries;

        while(!mpStream->eof()) {
            ReadWord(word); // Reading the geometry id or End
            if(CheckEndBlock("Geometries", word))
                break;

            ExtractValue(word,id);
            temp_geometry_nodes.clear();
            for(SizeType i = 0 ; i < number_of_nodes ; i++) {
                ReadWord(word); // Reading the node id;
                ExtractValue(word, node_id);
                temp_geometry_nodes.push_back( *(FindKey(rModelPart.Nodes(), ReorderedNodeId(node_id), "Node").base()));
            }

            aux_geometries.AddGeometry(r_clone_geometry.Create(ReorderedGeometryId(id), temp_geometry_nodes));
            number_of_read_geometries++;

        }
        QUEST_INFO("") << number_of_read_geometries << " geometries read] [Type: " <<geometry_name << "]" << std::endl;

        rModelPart.AddGeometries(aux_geometries.GeometriesBegin(), aux_geometries.GeometriesEnd());

        QUEST_CATCH("")
    }

    /**
     * @brief 读取几何体数据的块，存储到 NodesContainerType 和 GeometryContainerType 中
     */
    void ModelPartIO::ReadGeometriesBlock(NodesContainerType& rThisNodes, GeometryContainerType& rThisGeometries){
        QUEST_TRY

        SizeType id;
        SizeType node_id;
        SizeType number_of_read_geometries = 0;


        std::string word;
        std::string geometry_name;

        ReadWord(geometry_name);
        QUEST_INFO("ModelPartIO") << "  [Reading Geometries : ";

        if(!QuestComponents<GeometryType>::Has(geometry_name)) {
            std::stringstream buffer;
            buffer << "Geometry " << geometry_name << " is not registered in Quest.";
            buffer << " Please check the spelling of the geometry name and see if the application which containing it, is registered correctly.";
            buffer << " [Line " << mNumberOfLines << " ]";
            QUEST_ERROR << buffer.str() << std::endl;
            return;
        }

        GeometryType const& r_clone_geometry = QuestComponents<GeometryType>::Get(geometry_name);
        SizeType number_of_nodes = r_clone_geometry.size();
        Element::NodesArrayType temp_geometry_nodes;

        while(!mpStream->eof()) {
            ReadWord(word); // Reading the geometry id or End
            if(CheckEndBlock("Geometries", word))
                break;

            ExtractValue(word,id);
            temp_geometry_nodes.clear();
            for(SizeType i = 0 ; i < number_of_nodes ; i++) {
                ReadWord(word); // Reading the node id;
                ExtractValue(word, node_id);
                temp_geometry_nodes.push_back( *(FindKey(rThisNodes, ReorderedNodeId(node_id), "Node").base()));
            }

            rThisGeometries.AddGeometry(r_clone_geometry.Create(ReorderedGeometryId(id), temp_geometry_nodes));
            number_of_read_geometries++;

        }
        QUEST_INFO("") << number_of_read_geometries << " geometries read] [Type: " <<geometry_name << "]" << std::endl;

        QUEST_CATCH("")
    }

    /**
     * @brief 读取包含元素数据的块
     */
    void ModelPartIO::ReadElementsBlock(ModelPart& rModelPart){
        QUEST_TRY

        ModelPart::ElementsContainerType aux_elems;
        ReadElementsBlock(rModelPart.Nodes(), rModelPart.rProperties(), aux_elems);
        rModelPart.AddElements(aux_elems.begin(), aux_elems.end());

        QUEST_CATCH("")
    }

    /**
     * @brief 读取包含元素数据的块，并将节点、属性和元素数据分别存储到各自的容器中
     */
    void ModelPartIO::ReadElementsBlock(NodesContainerType& rThisNodes, PropertiesContainerType& rThisProperties, ElementsContainerType& rThisElements){
        QUEST_TRY

        SizeType id;
        SizeType properties_id;
        SizeType node_id;
        SizeType number_of_read_elements = 0;


        std::string word;
        std::string element_name;

        ReadWord(element_name);
        QUEST_INFO("ModelPartIO") << "  [Reading Elements : ";

        if(!QuestComponents<Element>::Has(element_name))
        {
            std::stringstream buffer;
            buffer << "Element " << element_name << " is not registered in Quest.";
            buffer << " Please check the spelling of the element name and see if the application which containing it, is registered correctly.";
            buffer << " [Line " << mNumberOfLines << " ]";
            QUEST_ERROR << buffer.str() << std::endl;
            return;
        }

        Element const& r_clone_element = QuestComponents<Element>::Get(element_name);
        SizeType number_of_nodes = r_clone_element.GetGeometry().size();
        Element::NodesArrayType temp_element_nodes;


        while(!mpStream->eof())
        {
            ReadWord(word); 
            if(CheckEndBlock("Elements", word))
                break;

            ExtractValue(word,id);
            ReadWord(word); 
            ExtractValue(word, properties_id);
            Properties::Pointer p_temp_properties = *(FindKey(rThisProperties, properties_id, "Properties").base());
            temp_element_nodes.clear();
            for(SizeType i = 0 ; i < number_of_nodes ; i++)
            {
                ReadWord(word); 
                ExtractValue(word, node_id);
                temp_element_nodes.push_back( *(FindKey(rThisNodes, ReorderedNodeId(node_id), "Node").base()));
            }

            rThisElements.push_back(r_clone_element.Create(ReorderedElementId(id), temp_element_nodes, p_temp_properties));
            number_of_read_elements++;

        }
        QUEST_INFO("") << number_of_read_elements << " elements read] [Type: " <<element_name << "]" << std::endl;
        rThisElements.Unique();

        QUEST_CATCH("")
    }

    /**
     * @brief 读取包含条件数据的块
     */
    void ModelPartIO::ReadConditionsBlock(ModelPart& rModelPart){
        QUEST_TRY

        ModelPart::ConditionsContainerType aux_conds;
        ReadConditionsBlock(rModelPart.Nodes(), rModelPart.rProperties(), aux_conds);
        rModelPart.AddConditions(aux_conds.begin(), aux_conds.end());

        QUEST_CATCH("")
    }

    /**
     * @brief 读取条件数据的块，并将节点、属性和条件数据分别存储到容器中
     */
    void ModelPartIO::ReadConditionsBlock(NodesContainerType& rThisNodes, PropertiesContainerType& rThisProperties, ConditionsContainerType& rThisConditions){
        QUEST_TRY

        SizeType id;
        SizeType properties_id;
        SizeType node_id;
        SizeType number_of_read_conditions = 0;


        std::string word;
        std::string condition_name;

        ReadWord(condition_name);
        QUEST_INFO("ModelPartIO") << "  [Reading Conditions : ";

        if(!QuestComponents<Condition>::Has(condition_name))
        {
            std::stringstream buffer;
            buffer << "Condition " << condition_name << " is not registered in Quest.";
            buffer << " Please check the spelling of the condition name and see if the application containing it is registered correctly.";
            buffer << " [Line " << mNumberOfLines << " ]";
            QUEST_ERROR << buffer.str() << std::endl;
            return;
        }

        Condition const& r_clone_condition = QuestComponents<Condition>::Get(condition_name);
        SizeType number_of_nodes = r_clone_condition.GetGeometry().size();
        Condition::NodesArrayType temp_condition_nodes;

        while(!mpStream->eof())
        {
            ReadWord(word); // Reading the condition id or End
            if(CheckEndBlock("Conditions", word))
                break;

            ExtractValue(word,id);
            ReadWord(word); // Reading the properties id;
            ExtractValue(word, properties_id);
            Properties::Pointer p_temp_properties = *(FindKey(rThisProperties, properties_id, "Properties").base());
            temp_condition_nodes.clear();
            for(SizeType i = 0 ; i < number_of_nodes ; i++)
            {
                ReadWord(word); // Reading the node id;
                ExtractValue(word, node_id);
                temp_condition_nodes.push_back( *(FindKey(rThisNodes, ReorderedNodeId(node_id), "Node").base()));
            }

            rThisConditions.push_back(r_clone_condition.Create(ReorderedConditionId(id), temp_condition_nodes, p_temp_properties));
            number_of_read_conditions++;
        }
        QUEST_INFO("") << number_of_read_conditions << " conditions read] [Type: " << condition_name << "]" << std::endl;
        rThisConditions.Unique();

        QUEST_CATCH("")
    }

    /**
     * @brief 读取节点数据的块
     */
    void ModelPartIO::ReadNodalDataBlock(ModelPart& rThisModelPart){
        QUEST_TRY

        NodesContainerType& rThisNodes = rThisModelPart.Nodes();

        std::string variable_name;

        ReadWord(variable_name);

        VariablesList rThisVariables = rThisModelPart.GetNodalSolutionStepVariablesList();

        if(QuestComponents<Flags >::Has(variable_name)) {
            ReadNodalFlags(rThisNodes, QuestComponents<Flags >::Get(variable_name));
        } else if(QuestComponents<Variable<int> >::Has(variable_name)) {
            const bool has_been_added = rThisVariables.Has(QuestComponents<Variable<int> >::Get(variable_name)) ;
            if( !has_been_added && mOptions.Is(IGNORE_VARIABLES_ERROR) ) {
                QUEST_WARNING("ModelPartIO") <<"WARNING: Skipping NodalData block. Variable "<<variable_name<<" has not been added to ModelPart '"<<rThisModelPart.Name()<<"'"<<std::endl<<std::endl;
                SkipBlock("NodalData");
            }
            else {
                QUEST_ERROR_IF_NOT(has_been_added) << "The nodal solution step container does not have this variable: " << variable_name << "." << std::endl;
                ReadNodalScalarVariableData(rThisNodes, QuestComponents<Variable<int> >::Get(variable_name));
            }
        } else if(QuestComponents<Variable<double> >::Has(variable_name)) {
            const bool has_been_added = rThisVariables.Has(QuestComponents<Variable<double> >::Get(variable_name)) ;
            if( !has_been_added && mOptions.Is(IGNORE_VARIABLES_ERROR) ) {
                QUEST_WARNING("ModelPartIO")<<"WARNING: Skipping NodalData block. Variable "<<variable_name<<" has not been added to ModelPart '"<<rThisModelPart.Name()<<"'"<<std::endl<<std::endl;
                SkipBlock("NodalData");
            } else {
                QUEST_ERROR_IF_NOT(has_been_added) << "The nodal solution step container does not have this variable: " << variable_name << "." << std::endl;
                ReadNodalDofVariableData(rThisNodes, QuestComponents<Variable<double> >::Get(variable_name));
            }
        } else if(QuestComponents<Variable<Array1d<double, 3> > >::Has(variable_name)) {
            const bool has_been_added = rThisVariables.Has(QuestComponents<Variable<Array1d<double, 3> > >::Get(variable_name)) ;
            if( !has_been_added && mOptions.Is(IGNORE_VARIABLES_ERROR) ) {
                QUEST_WARNING("ModelPartIO")<<"WARNING: Skipping NodalData block. Variable "<<variable_name<<" has not been added to ModelPart '"<<rThisModelPart.Name()<<"'"<<std::endl<<std::endl;
            } else {
                QUEST_ERROR_IF_NOT(has_been_added) << "The nodal solution step container does not have this variable: " << variable_name << "." << std::endl;
                ReadNodalVectorialVariableData(rThisNodes, QuestComponents<Variable<Array1d<double, 3> > >::Get(variable_name), Vector(3));
            }
        } else if(QuestComponents<Variable<Quaternion<double> > >::Has(variable_name)) {
            const bool has_been_added = rThisVariables.Has(QuestComponents<Variable<Quaternion<double> > >::Get(variable_name)) ;
            if( !has_been_added && mOptions.Is(IGNORE_VARIABLES_ERROR) ) {
                QUEST_WARNING("ModelPartIO")<<"WARNING: Skipping NodalData block. Variable "<<variable_name<<" has not been added to ModelPart '"<<rThisModelPart.Name()<<"'"<<std::endl<<std::endl;
            } else {
                QUEST_ERROR_IF_NOT(has_been_added) << "The nodal solution step container does not have this variable: " << variable_name << "." << std::endl;
                ReadNodalVectorialVariableData(rThisNodes, QuestComponents<Variable<Quaternion<double> > >::Get(variable_name), Vector(4));
            }
        } else if(QuestComponents<Variable<Matrix> >::Has(variable_name)) {
            ReadNodalVectorialVariableData(rThisNodes, QuestComponents<Variable<Matrix> >::Get(variable_name), Matrix(3,3));
        } else if(QuestComponents<Variable<Vector> >::Has(variable_name)) {
            ReadNodalVectorialVariableData(rThisNodes, QuestComponents<Variable<Vector> >::Get(variable_name), Vector(3));
        } else if(QuestComponents<VariableData>::Has(variable_name)) {
            std::stringstream buffer;
            buffer << variable_name << " is not supported to be read by this IO or the type of variable is not registered correctly" << std::endl;
            buffer << " [Line " << mNumberOfLines << " ]";
            QUEST_ERROR << buffer.str() << std::endl;
        } else {
            std::stringstream buffer;
            buffer << variable_name << " is not a valid variable!!!" << std::endl;
            buffer << " [Line " << mNumberOfLines << " ]";
            QUEST_ERROR << buffer.str() << std::endl;
        }

        QUEST_CATCH("")
    }

    /**
     * @brief 将节点数据写入文件或数据流中的块
     */
    void ModelPartIO::WriteNodalDataBlock(ModelPart& rThisModelPart){
        QUEST_TRY

        auto& r_this_nodes = rThisModelPart.Nodes();
        const std::size_t number_of_nodes = r_this_nodes.size();
        const auto it_node_begin = r_this_nodes.begin();

        const auto& r_flags = QuestComponents<Flags>::GetComponents();
        for (auto& r_flag : r_flags) {
            const auto& r_flag_name = r_flag.first;
            const auto& r_variable_flag = *(r_flag.second);
            int to_consider = (r_flag_name == "ALL_DEFINED" || r_flag_name == "ALL_TRUE") ? -1 : 0;
            if (to_consider == 0) {
                for(std::size_t j = 0; j < number_of_nodes; j++) {
                    auto it_node = it_node_begin + j;
                    if (it_node->Is(r_variable_flag)) {
                        to_consider = 1;
                        break;
                    }
                }
            }
            if (to_consider == 1) {
                (*mpStream) << "Begin NodalData\t" << r_flag_name << std::endl;
                for(std::size_t j = 0; j < number_of_nodes; j++) {
                    auto it_node = it_node_begin + j;
                    if (it_node->Is(r_variable_flag)) {
                        (*mpStream) << it_node->Id() <<"\n";
                    }
                }
                (*mpStream) << "End NodalData" << std::endl << std::endl;
            }
        }

        VariablesList& r_this_variables = rThisModelPart.GetNodalSolutionStepVariablesList();
        std::string variable_name;

        for(std::size_t i = 0; i < r_this_variables.size(); i++) {
            auto it_var = r_this_variables.begin() + i;

            variable_name = it_var->Name();

            if(QuestComponents<Variable<int>>::Has(variable_name)) {
                (*mpStream) << "Begin NodalData\t" << variable_name << std::endl;
                const auto& r_variable = QuestComponents<Quest::Variable<int> >::Get(variable_name);
                for(std::size_t j = 0; j < number_of_nodes; j++) {
                    auto it_node = it_node_begin + j;
                    const bool is_fixed = it_node->IsFixed(r_variable);
                    (*mpStream) << it_node->Id() <<"\t" << is_fixed << "\t" << it_node->FastGetSolutionStepValue(r_variable, 0) << std::endl;
                }
                (*mpStream) << "End NodalData" << std::endl << std::endl;
            } else if(QuestComponents<Variable<double>>::Has(variable_name)) {
                (*mpStream) << "Begin NodalData\t" << variable_name << std::endl;
                const auto& r_variable = QuestComponents<Quest::Variable<double> >::Get(variable_name);
                for(std::size_t j = 0; j < number_of_nodes; j++) {
                    auto it_node = it_node_begin + j;
                    const bool is_fixed = it_node->IsFixed(r_variable);
                    (*mpStream) << it_node->Id() <<"\t" << is_fixed << "\t" << it_node->FastGetSolutionStepValue(r_variable, 0) << std::endl;
                }
                (*mpStream) << "End NodalData" << std::endl << std::endl;
            }  else if(QuestComponents<Variable<Array1d<double, 3> > >::Has(variable_name))  {
                if(QuestComponents<Variable<double>>::Has(variable_name + "_X")) { 
                    (*mpStream) << "Begin NodalData\t" << variable_name << "_X" << std::endl;
                    const auto& r_variable_x = QuestComponents<Variable<double> >::Get(variable_name+"_X");
                    for(std::size_t j = 0; j < number_of_nodes; j++) {
                        auto it_node = it_node_begin + j;
                        const bool is_fixed = it_node->IsFixed(r_variable_x);
                        (*mpStream) << it_node->Id() <<"\t" << is_fixed << "\t" << it_node->FastGetSolutionStepValue(r_variable_x, 0) << std::endl;
                    }
                    (*mpStream) << "End NodalData" << std::endl << std::endl;

                    (*mpStream) << "Begin NodalData\t" << variable_name << "_Y" << std::endl;
                    const auto&  r_variable_y = QuestComponents<Variable<double> >::Get(variable_name+"_Y");
                    for(std::size_t j = 0; j < number_of_nodes; j++) {
                        auto it_node = it_node_begin + j;
                        const bool is_fixed = it_node->IsFixed(r_variable_y);
                        (*mpStream) << it_node->Id() <<"\t" << is_fixed << "\t" << it_node->FastGetSolutionStepValue(r_variable_y, 0) << std::endl;
                    }
                    (*mpStream) << "End NodalData" << std::endl << std::endl;

                    (*mpStream) << "Begin NodalData\t" << variable_name << "_Z" << std::endl;
                    const auto&  r_variable_z = QuestComponents<Variable<double> >::Get(variable_name+"_Z");
                    for(std::size_t j = 0; j < number_of_nodes; j++) {
                        auto it_node = it_node_begin + j;
                        const bool is_fixed = it_node->IsFixed(r_variable_z);
                        (*mpStream) << it_node->Id() <<"\t" << is_fixed << "\t" << it_node->FastGetSolutionStepValue(r_variable_z, 0) << std::endl;
                    }
                    (*mpStream) << "End NodalData" << std::endl << std::endl;
                } else {
                    QUEST_WARNING("ModelPartIO") << variable_name << " is not a valid variable for output!!!" << std::endl;
                }
            } else {
                QUEST_WARNING("ModelPartIO") << variable_name << " is not a valid variable for output!!!" << std::endl;
            }

        }

        QUEST_CATCH("")
    }

    /**
     * @brief 读取节点的自由度（DOF）变量数据
     */
    template<class TVariableType>
    void ModelPartIO::ReadNodalDofVariableData(NodesContainerType& rThisNodes, const TVariableType& rVariable){
        QUEST_TRY

        SizeType id;
        bool is_fixed;
        double nodal_value;

        std::string value;

        while(!mpStream->eof())
        {
            ReadWord(value); 
            if(CheckEndBlock("NodalData", value))
                break;

            ExtractValue(value, id);
            typename NodesContainerType::iterator it_node = FindKey(rThisNodes, ReorderedNodeId(id), "Node");

            ReadWord(value);
            ExtractValue(value, is_fixed);
            if(is_fixed)
                it_node->Fix(rVariable);

            ReadWord(value);
            ExtractValue(value, nodal_value);

            it_node->GetSolutionStepValue(rVariable, 0) =  nodal_value;
        }

        QUEST_CATCH("")
    }

    /**
     * @brief 读取节点标志数据
     */
    void ModelPartIO::ReadNodalFlags(NodesContainerType& rThisNodes, Flags const& rFlags){

        QUEST_TRY

        SizeType id;

        std::string value;

        while(!mpStream->eof())
        {
            ReadWord(value); // reading id
            if(CheckEndBlock("NodalData", value))
                break;

            ExtractValue(value, id);

            FindKey(rThisNodes, ReorderedNodeId(id), "Node")->Set(rFlags);
        }

        QUEST_CATCH("")
    }

    /**
     * @brief 读取节点的标量变量数据
     */
    template<class TVariableType>
    void ModelPartIO::ReadNodalScalarVariableData(NodesContainerType& rThisNodes, const TVariableType& rVariable){
        QUEST_TRY

        SizeType id;
        bool is_fixed;
        typename TVariableType::Type nodal_value;

        std::string value;

        while(!mpStream->eof())
        {
            ReadWord(value); 
            if(CheckEndBlock("NodalData", value))
                break;

            ExtractValue(value, id);

            ReadWord(value);
            ExtractValue(value, is_fixed);
            if(is_fixed)
            {
                std::stringstream buffer;
                buffer << "Only double variables or components can be fixed.";
                buffer <<  " [Line " << mNumberOfLines << " ]";
                QUEST_ERROR << buffer.str() << std::endl;
            }

            ReadWord(value);
            ExtractValue(value, nodal_value);

            FindKey(rThisNodes, ReorderedNodeId(id), "Node")->GetSolutionStepValue(rVariable, 0) =  nodal_value;
        }

        QUEST_CATCH("")
    }

    /**
     * @brief 读取节点的矢量变量数据
     */
    template<class TVariableType, class TDataType>
        void ModelPartIO::ReadNodalVectorialVariableData(NodesContainerType& rThisNodes, const TVariableType& rVariable, TDataType Dummy){
        QUEST_TRY

        SizeType id;
        bool is_fixed;
        TDataType nodal_value;

        std::string value;

        while(!mpStream->eof())
        {
            ReadWord(value); // reading id
            if(CheckEndBlock("NodalData", value))
                break;

            ExtractValue(value, id);

            // reading is_fixed
            ReadWord(value);
            ExtractValue(value, is_fixed);
            if(is_fixed)
            {
                std::stringstream buffer;
                buffer << "Only double variables or components can be fixed.";
                buffer <<  " [Line " << mNumberOfLines << " ]";
                QUEST_ERROR << buffer.str() << std::endl;
            }

            // reading nodal_value
            ReadVectorialValue(nodal_value);

            FindKey(rThisNodes, ReorderedNodeId(id), "Node")->GetSolutionStepValue(rVariable, 0) =  nodal_value;
        }

        QUEST_CATCH("")
    }

    /**
     * @brief 读取包含单元数据的块
     */
    void ModelPartIO::ReadElementalDataBlock(ElementsContainerType& rThisElements){
        QUEST_TRY

        std::string variable_name;

        ReadWord(variable_name);

        if(QuestComponents<Variable<bool> >::Has(variable_name)) {
            ReadElementalScalarVariableData(rThisElements, static_cast<Variable<bool> const& >(QuestComponents<Variable<bool> >::Get(variable_name)));
        } else if(QuestComponents<Variable<int> >::Has(variable_name)) {
            ReadElementalScalarVariableData(rThisElements, static_cast<Variable<int> const& >(QuestComponents<Variable<int> >::Get(variable_name)));
        } else if(QuestComponents<Variable<double> >::Has(variable_name)) {
            ReadElementalScalarVariableData(rThisElements, static_cast<Variable<double> const& >(QuestComponents<Variable<double> >::Get(variable_name)));
        } else if(QuestComponents<Variable<Array1d<double, 3> > >::Has(variable_name)) {
            ReadElementalVectorialVariableData(rThisElements, static_cast<Variable<Array1d<double, 3> > const& >(QuestComponents<Variable<Array1d<double, 3> > >::Get(variable_name)), Vector(3));
        } else if(QuestComponents<Variable<Quaternion<double> > >::Has(variable_name)) {
            ReadElementalVectorialVariableData(rThisElements, static_cast<Variable<Quaternion<double> > const& >(QuestComponents<Variable<Quaternion<double> > >::Get(variable_name)), Vector(4));
        } else if(QuestComponents<Variable<Matrix> >::Has(variable_name)) {
            ReadElementalVectorialVariableData(rThisElements, static_cast<Variable<Matrix > const& >(QuestComponents<Variable<Matrix> >::Get(variable_name)), Matrix(3,3));
        } else if(QuestComponents<Variable<Vector> >::Has(variable_name)) {
            ReadElementalVectorialVariableData(rThisElements, static_cast<Variable<Vector > const& >(QuestComponents<Variable<Vector> >::Get(variable_name)), Vector(3));
        } else {
            std::stringstream buffer;
            buffer << variable_name << " is not a valid variable!!!" << std::endl;
            buffer << " [Line " << mNumberOfLines << " ]";
            QUEST_ERROR << buffer.str() << std::endl;
        }

        QUEST_CATCH("")
    }

    /**
     * @brief 将对象容器的数据写入数据块
     */
    template<class TObjectsContainerType>
    void ModelPartIO::WriteDataBlock(const TObjectsContainerType& rThisObjectContainer, const std::string& rObjectName){
        std::unordered_set<std::string> variables;

        for(auto& object :rThisObjectContainer){
            for(auto& var:object.GetData()){
                auto const& is_included = variables.find(var.first->Name());
                if(is_included == variables.end()){
                    variables.insert(var.first->Name());
                    if(QuestComponents<Variable<bool>>::Has(var.first->Name())){
                        WriteDataBlock<Variable<bool>, TObjectsContainerType>(rThisObjectContainer, var.first, rObjectName);
                    } else if(QuestComponents<Variable<int>>::Has(var.first->Name())){
                        WriteDataBlock<Variable<int>, TObjectsContainerType>(rThisObjectContainer, var.first, rObjectName);
                    } else if(QuestComponents<Variable<double>>::Has(var.first->Name())){
                        WriteDataBlock<Variable<double>, TObjectsContainerType>(rThisObjectContainer, var.first, rObjectName);
                    } else if(QuestComponents<Variable<Array1d<double,3>>>::Has(var.first->Name())){
                        WriteDataBlock<Variable<Array1d<double,3>>, TObjectsContainerType>(rThisObjectContainer, var.first, rObjectName);
                    } else if(QuestComponents<Variable<Quaternion<double>>>::Has(var.first->Name())){
                        WriteDataBlock<Variable<Quaternion<double>>, TObjectsContainerType>(rThisObjectContainer, var.first, rObjectName);
                    } else if(QuestComponents<Variable<Vector>>::Has(var.first->Name())){
                        WriteDataBlock<Variable<Vector>, TObjectsContainerType>(rThisObjectContainer, var.first, rObjectName);
                    } else if(QuestComponents<Variable<Matrix>>::Has(var.first->Name())){
                        WriteDataBlock<Variable<Matrix>, TObjectsContainerType>(rThisObjectContainer, var.first, rObjectName);
                    } else {
                        QUEST_WARNING("ModelPartIO") << var.first->Name() << " is not a valid variable for output!!!" << std::endl;
                    }
                }
            }
        }
    }
    
    /**
     * @brief 将指定变量的数据写入对象容器
     */
    template<class TVariableType, class TObjectsContainerType>
    void ModelPartIO::WriteDataBlock(const TObjectsContainerType& rThisObjectContainer,const VariableData* rVariable, const std::string& rObjectName){
        const TVariableType& variable = QuestComponents<TVariableType>::Get(rVariable->Name());
        (*mpStream) << "Begin "<<rObjectName<<"alData "<<variable.Name()<<std::endl;
        for(auto& object : rThisObjectContainer){
            if(object.Has(variable)){
                (*mpStream)<<object.Id()<<"\t"<<object.GetValue(variable)<<std::endl;
            }
        }
        (*mpStream)<<"End "<<rObjectName<<"alData\n"<<std::endl;
    }

    /**
     * @brief 读取单元的标量变量数据
     */
    template<class TVariableType>
    void ModelPartIO::ReadElementalScalarVariableData(ElementsContainerType& rThisElements, const TVariableType& rVariable){
        QUEST_TRY

        SizeType id;
        double elemental_value;

        std::string value;

        while(!mpStream->eof())
        {
            ReadWord(value); 
            if(CheckEndBlock("ElementalData", value))
                break;

            ExtractValue(value, id);

            ReadWord(value);
            ExtractValue(value, elemental_value);

            ModelPart::ElementIterator i_result = rThisElements.find(ReorderedElementId(id));
            if(i_result != rThisElements.end())
                i_result->GetValue(rVariable) =  elemental_value;
            else
                QUEST_WARNING("ModelPartIO")  << "WARNING! Assigning " << rVariable.Name() << " to not existing element #" << id << " [Line " << mNumberOfLines << " ]" << std::endl;
        }

        QUEST_CATCH("")
    }

    /**
     * @brief 读取单元的矢量变量数据
     */
    template<class TVariableType, class TDataType>
    void ModelPartIO::ReadElementalVectorialVariableData(ElementsContainerType& rThisElements, const TVariableType& rVariable, TDataType Dummy){
        QUEST_TRY

        SizeType id;
        TDataType elemental_value;

        std::string value;

        while(!mpStream->eof())
        {
            ReadWord(value);
            if(CheckEndBlock("ElementalData", value))
                break;

            ExtractValue(value, id);

            ReadVectorialValue(elemental_value);

            ModelPart::ElementIterator i_result = rThisElements.find(ReorderedElementId(id));
            if(i_result != rThisElements.end())
                i_result->GetValue(rVariable) =  elemental_value;
            else
                QUEST_WARNING("ModelPartIO")  << "WARNING! Assigning " << rVariable.Name() << " to not existing element #" << id << " [Line " << mNumberOfLines << " ]" << std::endl;
        }

        QUEST_CATCH("")
    }
    
    /**
     * @brief 读取包含条件数据的块
     */
    void ModelPartIO::ReadConditionalDataBlock(ConditionsContainerType& rThisConditions){
        QUEST_TRY

        std::string variable_name;

        ReadWord(variable_name);

        if(QuestComponents<Variable<double> >::Has(variable_name)) {
            ReadConditionalScalarVariableData(rThisConditions, static_cast<Variable<double> const& >(QuestComponents<Variable<double> >::Get(variable_name)));
        } else if(QuestComponents<Variable<bool> >::Has(variable_name)) {
            ReadConditionalScalarVariableData(rThisConditions, static_cast<Variable<bool> const& >(QuestComponents<Variable<bool> >::Get(variable_name)));
        } else if(QuestComponents<Variable<int> >::Has(variable_name)) {
            ReadConditionalScalarVariableData(rThisConditions, static_cast<Variable<int> const& >(QuestComponents<Variable<int> >::Get(variable_name)));
        } else if(QuestComponents<Variable<Array1d<double, 3> > >::Has(variable_name)) {
            ReadConditionalVectorialVariableData(rThisConditions, static_cast<Variable<Array1d<double, 3> > const& >(QuestComponents<Variable<Array1d<double, 3> > >::Get(variable_name)), Vector(3));
        } else if(QuestComponents<Variable<Quaternion<double> > >::Has(variable_name)) {
            ReadConditionalVectorialVariableData(rThisConditions, static_cast<Variable<Quaternion<double> > const& >(QuestComponents<Variable<Quaternion<double> > >::Get(variable_name)), Vector(4));
        } else if(QuestComponents<Variable<Matrix> >::Has(variable_name)) {
            ReadConditionalVectorialVariableData(rThisConditions, static_cast<Variable<Matrix > const& >(QuestComponents<Variable<Matrix> >::Get(variable_name)), Matrix(3,3));
        } else if(QuestComponents<Variable<Vector> >::Has(variable_name)) {
            ReadConditionalVectorialVariableData(rThisConditions, static_cast<Variable<Vector > const& >(QuestComponents<Variable<Vector> >::Get(variable_name)), Vector(3));
        } else {
            std::stringstream buffer;
            buffer << variable_name << " is not a valid variable!!!" << std::endl;
            buffer << " [Line " << mNumberOfLines << " ]";
            QUEST_ERROR << buffer.str() << std::endl;
        }

        QUEST_CATCH("")
    }

    /**
     * @brief 读取条件的标量变量数据
     */
    template<class TVariableType>
    void ModelPartIO::ReadConditionalScalarVariableData(ConditionsContainerType& rThisConditions, const TVariableType& rVariable){
        QUEST_TRY

        SizeType id;
        double conditional_value;

        std::string value;

        while(!mpStream->eof())
        {
            ReadWord(value);
            if(CheckEndBlock("ConditionalData", value))
                break;

            ExtractValue(value, id);

            ReadWord(value);
            ExtractValue(value, conditional_value);

            ModelPart::ConditionIterator i_result = rThisConditions.find(ReorderedConditionId(id));
            if(i_result != rThisConditions.end())
                i_result->GetValue(rVariable) =  conditional_value;
            else
                QUEST_WARNING("ModelPartIO")  << "WARNING! Assigning " << rVariable.Name() << " to not existing condition #" << id << " [Line " << mNumberOfLines << " ]" << std::endl;
        }

        QUEST_CATCH("")
    }

    /**
     * @brief 读取条件的矢量变量数据
     */
    template<class TVariableType, class TDataType>
    void ModelPartIO::ReadConditionalVectorialVariableData(ConditionsContainerType& rThisConditions, const TVariableType& rVariable, TDataType Dummy){
        QUEST_TRY

        SizeType id;
        TDataType conditional_value;

        std::string value;

        while(!mpStream->eof())
        {
            ReadWord(value); // reading id
            if(CheckEndBlock("ConditionalData", value))
                break;

            ExtractValue(value, id);

            // reading nodal_value
            ReadVectorialValue(conditional_value);

            ModelPart::ConditionIterator i_result = rThisConditions.find(ReorderedConditionId(id));
            if(i_result != rThisConditions.end())
                i_result->GetValue(rVariable) =  conditional_value;
            else
                QUEST_WARNING("ModelPartIO")  << "WARNING! Assigning " << rVariable.Name() << " to not existing condition #" << id << " [Line " << mNumberOfLines << " ]" << std::endl;
        }

        QUEST_CATCH("")
    }

    /**
     * @brief 读取几何的连接性数据块
     */
    SizeType ReadGeometriesConnectivitiesBlock(ConnectivitiesContainerType& rThisConnectivities){
        QUEST_TRY

        SizeType id;
        SizeType node_id;
        SizeType number_of_connectivities = 0;

        std::string word;
        std::string geometry_name;

        ReadWord(geometry_name);
        if(!QuestComponents<GeometryType>::Has(geometry_name)) {
            std::stringstream buffer;
            buffer << "Geometry " << geometry_name << " is not registered in Quest.";
            buffer << " Please check the spelling of the geometry name and see if the application containing it is registered correctly.";
            buffer << " [Line " << mNumberOfLines << " ]";
            QUEST_ERROR << buffer.str() << std::endl;
            return number_of_connectivities;
        }

        GeometryType const& r_clone_geometry = QuestComponents<GeometryType>::Get(geometry_name);
        SizeType number_of_nodes = r_clone_geometry.size();
        ConnectivitiesContainerType::value_type temp_geometry_nodes;

        while(!mpStream->eof()) {
            ReadWord(word); 
            if(CheckEndBlock("Geometries", word))
                break;

            ExtractValue(word,id);
            ReadWord(word); 
            temp_geometry_nodes.clear();
            for(SizeType i = 0 ; i < number_of_nodes ; i++) {
                ReadWord(word); 
                ExtractValue(word, node_id);
                temp_geometry_nodes.push_back(ReorderedNodeId(node_id));
            }
            const int index = ReorderedGeometryId(id) - 1;
            const int size = rThisConnectivities.size();
            if(index == size) { 
                rThisConnectivities.push_back(temp_geometry_nodes);
            } else if(index < size) {
                rThisConnectivities[index]= temp_geometry_nodes;
            } else {
                rThisConnectivities.resize(index+1);
                rThisConnectivities[index]= temp_geometry_nodes;

            }
            number_of_connectivities++;
        }
        return number_of_connectivities;

        QUEST_CATCH("")
    }

    /**
     * @brief 读取单元的连接性数据块
     */
    SizeType ModelPartIO::ReadElementsConnectivitiesBlock(ConnectivitiesContainerType& rThisConnectivities){
        QUEST_TRY

        SizeType id;
        SizeType node_id;
        SizeType number_of_connectivities = 0;

        std::string word;
        std::string element_name;

        ReadWord(element_name);
        if(!QuestComponents<Element>::Has(element_name))
        {
            std::stringstream buffer;
            buffer << "Element " << element_name << " is not registered in Quest.";
            buffer << " Please check the spelling of the element name and see if the application containing it is registered correctly.";
            buffer << " [Line " << mNumberOfLines << " ]";
            QUEST_ERROR << buffer.str() << std::endl;
            return number_of_connectivities;
        }

        Element const& r_clone_element = QuestComponents<Element>::Get(element_name);
        SizeType number_of_nodes = r_clone_element.GetGeometry().size();
        ConnectivitiesContainerType::value_type temp_element_nodes;

        while(!mpStream->eof())
        {
            ReadWord(word); 
            if(CheckEndBlock("Elements", word))
                break;

            ExtractValue(word,id);
            ReadWord(word); 
            temp_element_nodes.clear();
            for(SizeType i = 0 ; i < number_of_nodes ; i++)
            {
                ReadWord(word);
                ExtractValue(word, node_id);
                temp_element_nodes.push_back(ReorderedNodeId(node_id));
            }
            const int index = ReorderedElementId(id) - 1;
            const int size = rThisConnectivities.size();
            if(index == size)  
                rThisConnectivities.push_back(temp_element_nodes);
            else if(index < size)
                rThisConnectivities[index]= temp_element_nodes;
            else
            {
                rThisConnectivities.resize(index+1);
                rThisConnectivities[index]= temp_element_nodes;

            }
            number_of_connectivities++;
        }
        return number_of_connectivities;

        QUEST_CATCH("")
    }

    /**
     * @brief 读取条件的连接性数据块
     */
    SizeType ModelPartIO::ReadConditionsConnectivitiesBlock(ConnectivitiesContainerType& rThisConnectivities){
        QUEST_TRY

        SizeType id;
        SizeType node_id;
        SizeType number_of_connectivities = 0;

        std::string word;
        std::string condition_name;

        ReadWord(condition_name);
        if(!QuestComponents<Condition>::Has(condition_name))
        {
            std::stringstream buffer;
            buffer << "Condition " << condition_name << " is not registered in Quest.";
            buffer << " Please check the spelling of the condition name and see if the application containing it is registered correctly.";
            buffer << " [Line " << mNumberOfLines << " ]";
            QUEST_ERROR << buffer.str() << std::endl;
            return number_of_connectivities;
        }

        Condition const& r_clone_condition = QuestComponents<Condition>::Get(condition_name);
        SizeType number_of_nodes = r_clone_condition.GetGeometry().size();
        ConnectivitiesContainerType::value_type temp_condition_nodes;

        while(!mpStream->eof())
        {
            ReadWord(word); 
            if(CheckEndBlock("Conditions", word))
                break;

            ExtractValue(word,id);
            ReadWord(word); 
            temp_condition_nodes.clear();
            for(SizeType i = 0 ; i < number_of_nodes ; i++)
            {
                ReadWord(word);
                ExtractValue(word, node_id);
                temp_condition_nodes.push_back(ReorderedNodeId(node_id));
            }

            const int index = ReorderedConditionId(id) - 1;
            const int size = rThisConnectivities.size();
            if(index == size) 
                rThisConnectivities.push_back(temp_condition_nodes);
            else if(index < size)
                rThisConnectivities[index]= temp_condition_nodes;
            else
            {
                rThisConnectivities.resize(index+1);
                rThisConnectivities[index]= temp_condition_nodes;

            }
            number_of_connectivities++;
        }

        return number_of_connectivities;

        QUEST_CATCH("")
    }

    /**
     * @brief 通过几何体块填充节点的连接性数据
     */
    void ModelPartIO::FillNodalConnectivitiesFromGeometryBlock(ConnectivitiesContainerType& rNodalConnectivities){
        QUEST_TRY;

        SizeType id;
        SizeType node_id;
        SizeType position;
        SizeType used_size = rNodalConnectivities.size();
        SizeType reserved_size = (rNodalConnectivities.capacity() > 0) ? rNodalConnectivities.capacity() : 1;

        std::string word;
        std::string geometry_name;

        ReadWord(geometry_name);
        if(!QuestComponents<GeometryType>::Has(geometry_name)) {
            std::stringstream buffer;
            buffer << "Geometry " << geometry_name << " is not registered in Quest.";
            buffer << " Please check the spelling of the geometry name and see if the application containing it is registered correctly.";
            buffer << " [Line " << mNumberOfLines << " ]";
            QUEST_ERROR << buffer.str() << std::endl;
        }

        GeometryType const& r_clone_geometry = QuestComponents<GeometryType>::Get(geometry_name);
        SizeType n_nodes_in_geom = r_clone_geometry.size();
        ConnectivitiesContainerType::value_type temp_geometry_nodes;

        while(!mpStream->eof()) {
            ReadWord(word);
            if(CheckEndBlock("Geometries", word))
                break;

            ExtractValue(word,id);
            ReadWord(word);
            temp_geometry_nodes.clear();
            for(SizeType i = 0 ; i < n_nodes_in_geom ; i++) {
                ReadWord(word); 
                ExtractValue(word, node_id);
                temp_geometry_nodes.push_back(ReorderedNodeId(node_id));
            }

            for (SizeType i = 0; i < n_nodes_in_geom; i++) {
                position = temp_geometry_nodes[i]-1; 
                if (position >= used_size) {
                    used_size = position+1;
                    if (position >= reserved_size) {
                        reserved_size = (used_size > reserved_size) ? 2*used_size : 2*reserved_size;
                        rNodalConnectivities.reserve(reserved_size);
                    }
                    rNodalConnectivities.resize(used_size);
                }

                for (SizeType j = 0; j < i; j++)
                    rNodalConnectivities[position].push_back(temp_geometry_nodes[j]);
                for (SizeType j = i+1; j < n_nodes_in_geom; j++)
                    rNodalConnectivities[position].push_back(temp_geometry_nodes[j]);
            }
        }

        QUEST_CATCH("");
    }

    /**
     * @brief 通过单元块填充节点的连接性数据
     */
    void ModelPartIO::FillNodalConnectivitiesFromElementBlock(ConnectivitiesContainerType& rNodalConnectivities){
        QUEST_TRY;

        SizeType id;
        SizeType node_id;
        SizeType position;
        SizeType used_size = rNodalConnectivities.size();
        SizeType reserved_size = (rNodalConnectivities.capacity() > 0) ? rNodalConnectivities.capacity() : 1;

        std::string word;
        std::string element_name;

        ReadWord(element_name);
        if(!QuestComponents<Element>::Has(element_name))
        {
            std::stringstream buffer;
            buffer << "Element " << element_name << " is not registered in Quest.";
            buffer << " Please check the spelling of the element name and see if the application containing it is registered correctly.";
            buffer << " [Line " << mNumberOfLines << " ]";
            QUEST_ERROR << buffer.str() << std::endl;
        }

        Element const& r_clone_element = QuestComponents<Element>::Get(element_name);
        SizeType n_nodes_in_elem = r_clone_element.GetGeometry().size();
        ConnectivitiesContainerType::value_type temp_element_nodes;

        while(!mpStream->eof())
        {
            ReadWord(word); 
            if(CheckEndBlock("Elements", word))
                break;

            ExtractValue(word,id);
            ReadWord(word); 
            temp_element_nodes.clear();
            for(SizeType i = 0 ; i < n_nodes_in_elem ; i++)
            {
                ReadWord(word); 
                ExtractValue(word, node_id);
                temp_element_nodes.push_back(ReorderedNodeId(node_id));
            }

            for (SizeType i = 0; i < n_nodes_in_elem; i++)
            {
                position = temp_element_nodes[i]-1; 
                if (position >= used_size)
                {
                    used_size = position+1;
                    if (position >= reserved_size)
                    {
                        reserved_size = (used_size > reserved_size) ? 2*used_size : 2*reserved_size;
                        rNodalConnectivities.reserve(reserved_size);
                    }
                    rNodalConnectivities.resize(used_size);
                }

                for (SizeType j = 0; j < i; j++)
                    rNodalConnectivities[position].push_back(temp_element_nodes[j]);
                for (SizeType j = i+1; j < n_nodes_in_elem; j++)
                    rNodalConnectivities[position].push_back(temp_element_nodes[j]);
            }
        }

        QUEST_CATCH("");
    }

    /**
     * @brief 通过条件块填充节点的连接性数据
     */
    void ModelPartIO::FillNodalConnectivitiesFromConditionBlock(ConnectivitiesContainerType& rNodalConnectivities){
        QUEST_TRY;

        SizeType id;
        SizeType node_id;
        SizeType position;
        SizeType used_size = rNodalConnectivities.size();
        SizeType reserved_size = (rNodalConnectivities.capacity() > 0) ? rNodalConnectivities.capacity() : 1;

        std::string word;
        std::string condition_name;

        ReadWord(condition_name);
        if(!QuestComponents<Condition>::Has(condition_name))
        {
            std::stringstream buffer;
            buffer << "Condition " << condition_name << " is not registered in Quest.";
            buffer << " Please check the spelling of the condition name and see if the application containing it is registered correctly.";
            buffer << " [Line " << mNumberOfLines << " ]";
            QUEST_ERROR << buffer.str() << std::endl;
        }

        Condition const& r_clone_condition = QuestComponents<Condition>::Get(condition_name);
        SizeType n_nodes_in_cond = r_clone_condition.GetGeometry().size();
        ConnectivitiesContainerType::value_type temp_condition_nodes;

        while(!mpStream->eof())
        {
            ReadWord(word); 
            if(CheckEndBlock("Conditions", word))
                break;

            ExtractValue(word,id);
            ReadWord(word); 
            temp_condition_nodes.clear();
            for(SizeType i = 0 ; i < n_nodes_in_cond ; i++)
            {
                ReadWord(word); 
                ExtractValue(word, node_id);
                temp_condition_nodes.push_back(ReorderedNodeId(node_id));
            }

            for (SizeType i = 0; i < n_nodes_in_cond; i++)
            {
                position = temp_condition_nodes[i]-1; 
                if (position >= used_size)
                {
                    used_size = position+1;
                    if (position >= reserved_size)
                    {
                        reserved_size = (used_size > reserved_size) ? 2*used_size : 2*reserved_size;
                        rNodalConnectivities.reserve(reserved_size);
                    }
                    rNodalConnectivities.resize(used_size);
                }

                for (SizeType j = 0; j < i; j++)
                    rNodalConnectivities[position].push_back(temp_condition_nodes[j]);
                for (SizeType j = i+1; j < n_nodes_in_cond; j++)
                    rNodalConnectivities[position].push_back(temp_condition_nodes[j]);
            }
        }

        QUEST_CATCH("");
    }

    /**
     * @brief 通过几何体块中的几何体 ID 列表填充节点连接性数据
     */
    void ModelPartIO::FillNodalConnectivitiesFromGeometryBlockInList(
        ConnectivitiesContainerType& rNodalConnectivities,
        std::unordered_set<SizeType>& rGeometriesIds
    ){
        QUEST_TRY;

        SizeType id;
        SizeType node_id;
        SizeType position;
        SizeType used_size = rNodalConnectivities.size();
        SizeType reserved_size = (rNodalConnectivities.capacity() > 0) ? rNodalConnectivities.capacity() : 1;

        std::string word;
        std::string geometry_name;

        ReadWord(geometry_name);
        if(!QuestComponents<GeometryType>::Has(geometry_name)) {
            std::stringstream buffer;
            buffer << "Geometry " << geometry_name << " is not registered in Quest.";
            buffer << " Please check the spelling of the geometry name and see if the application containing it is registered correctly.";
            buffer << " [Line " << mNumberOfLines << " ]";
            QUEST_ERROR << buffer.str() << std::endl;
        }

        GeometryType const& r_clone_geometry = QuestComponents<GeometryType>::Get(geometry_name);
        SizeType n_nodes_in_elem = r_clone_geometry.size();
        ConnectivitiesContainerType::value_type temp_geometry_nodes;

        while(!mpStream->eof()) {
            ReadWord(word); 
            if(CheckEndBlock("Geometries", word))
                break;

            ExtractValue(word,id);
            ReadWord(word); 
            temp_geometry_nodes.clear();
            for(SizeType i = 0 ; i < n_nodes_in_elem ; i++) {
                ReadWord(word);
                ExtractValue(word, node_id);
                temp_geometry_nodes.push_back(ReorderedNodeId(node_id));
            }

            if (rGeometriesIds.find(ReorderedGeometryId(id)) != rGeometriesIds.end()) {
                for (SizeType i = 0; i < n_nodes_in_elem; i++) {
                    position = temp_geometry_nodes[i]-1;
                    if (position >= used_size) {
                        used_size = position+1;
                        if (position >= reserved_size)
                        {
                            reserved_size = (used_size > reserved_size) ? 2*used_size : 2*reserved_size;
                            rNodalConnectivities.reserve(reserved_size);
                        }
                        rNodalConnectivities.resize(used_size);
                    }

                    for (SizeType j = 0; j < i; j++)
                        rNodalConnectivities[position].push_back(temp_geometry_nodes[j]);
                    for (SizeType j = i+1; j < n_nodes_in_elem; j++)
                        rNodalConnectivities[position].push_back(temp_geometry_nodes[j]);
                }
            }
        }

        QUEST_CATCH("");
    }

    /**
     * @brief 通过单元块中的单元 ID 列表填充节点连接性数据
     */
    void ModelPartIO::FillNodalConnectivitiesFromElementBlockInList(
        ConnectivitiesContainerType& rNodalConnectivities,
        std::unordered_set<SizeType>& rElementsIds
    ){
        QUEST_TRY;

        SizeType id;
        SizeType node_id;
        SizeType position;
        SizeType used_size = rNodalConnectivities.size();
        SizeType reserved_size = (rNodalConnectivities.capacity() > 0) ? rNodalConnectivities.capacity() : 1;

        std::string word;
        std::string element_name;

        ReadWord(element_name);
        if(!QuestComponents<Element>::Has(element_name))
        {
            std::stringstream buffer;
            buffer << "Element " << element_name << " is not registered in Quest.";
            buffer << " Please check the spelling of the element name and see if the application containing it is registered correctly.";
            buffer << " [Line " << mNumberOfLines << " ]";
            QUEST_ERROR << buffer.str() << std::endl;
        }

        Element const& r_clone_element = QuestComponents<Element>::Get(element_name);
        SizeType n_nodes_in_elem = r_clone_element.GetGeometry().size();
        ConnectivitiesContainerType::value_type temp_element_nodes;

        while(!mpStream->eof())
        {
            ReadWord(word); 
            if(CheckEndBlock("Elements", word))
                break;

            ExtractValue(word,id);
            ReadWord(word); 
            temp_element_nodes.clear();
            for(SizeType i = 0 ; i < n_nodes_in_elem ; i++)
            {
                ReadWord(word); 
                ExtractValue(word, node_id);
                temp_element_nodes.push_back(ReorderedNodeId(node_id));
            }

            if (rElementsIds.find(ReorderedElementId(id)) != rElementsIds.end()) {
                for (SizeType i = 0; i < n_nodes_in_elem; i++)
                {
                    position = temp_element_nodes[i]-1; 
                    if (position >= used_size)
                    {
                        used_size = position+1;
                        if (position >= reserved_size)
                        {
                            reserved_size = (used_size > reserved_size) ? 2*used_size : 2*reserved_size;
                            rNodalConnectivities.reserve(reserved_size);
                        }
                        rNodalConnectivities.resize(used_size);
                    }

                    for (SizeType j = 0; j < i; j++)
                        rNodalConnectivities[position].push_back(temp_element_nodes[j]);
                    for (SizeType j = i+1; j < n_nodes_in_elem; j++)
                        rNodalConnectivities[position].push_back(temp_element_nodes[j]);
                }
            }
        }

        QUEST_CATCH("");
    }

    /**
     * @brief 通过条件块中的条件 ID 列表填充节点连接性数据
     */
    void ModelPartIO::FillNodalConnectivitiesFromConditionBlockInList(
        ConnectivitiesContainerType& rNodalConnectivities,
        std::unordered_set<SizeType>& rConditionsIds
    ){
        QUEST_TRY;

        SizeType id;
        SizeType node_id;
        SizeType position;
        SizeType used_size = rNodalConnectivities.size();
        SizeType reserved_size = (rNodalConnectivities.capacity() > 0) ? rNodalConnectivities.capacity() : 1;

        std::string word;
        std::string condition_name;

        ReadWord(condition_name);
        if(!QuestComponents<Condition>::Has(condition_name))
        {
            std::stringstream buffer;
            buffer << "Condition " << condition_name << " is not registered in Quest.";
            buffer << " Please check the spelling of the condition name and see if the application containing it is registered correctly.";
            buffer << " [Line " << mNumberOfLines << " ]";
            QUEST_ERROR << buffer.str() << std::endl;
        }

        Condition const& r_clone_condition = QuestComponents<Condition>::Get(condition_name);
        SizeType n_nodes_in_cond = r_clone_condition.GetGeometry().size();
        ConnectivitiesContainerType::value_type temp_condition_nodes;

        while(!mpStream->eof())
        {
            ReadWord(word); // Reading the condition id or End
            if(CheckEndBlock("Conditions", word))
                break;

            ExtractValue(word,id);
            ReadWord(word); // Reading the properties id;
            temp_condition_nodes.clear();
            for(SizeType i = 0 ; i < n_nodes_in_cond ; i++)
            {
                ReadWord(word); // Reading the node id;
                ExtractValue(word, node_id);
                temp_condition_nodes.push_back(ReorderedNodeId(node_id));
            }

            if (rConditionsIds.find(ReorderedConditionId(id)) != rConditionsIds.end()) {

                for (SizeType i = 0; i < n_nodes_in_cond; i++)
                {
                    position = temp_condition_nodes[i]-1; // Ids start from 1, position in rNodalConnectivities starts from 0
                    if (position >= used_size)
                    {
                        used_size = position+1;
                        if (position >= reserved_size)
                        {
                            reserved_size = (used_size > reserved_size) ? 2*used_size : 2*reserved_size;
                            rNodalConnectivities.reserve(reserved_size);
                        }
                        rNodalConnectivities.resize(used_size);
                    }

                    for (SizeType j = 0; j < i; j++)
                        rNodalConnectivities[position].push_back(temp_condition_nodes[j]);
                    for (SizeType j = i+1; j < n_nodes_in_cond; j++)
                        rNodalConnectivities[position].push_back(temp_condition_nodes[j]);
                }
            }
        }

        QUEST_CATCH("");
    }

    /**
     * @brief 读取通信器的相关数据块
     */
    void ModelPartIO::ReadCommunicatorDataBlock(Communicator& rThisCommunicator, NodesContainerType& rThisNodes){
        QUEST_TRY

        std::string word;
        while(true)
        {
            ReadWord(word);
            if(mpStream->eof())
                break;
            if(CheckEndBlock("CommunicatorData", word))
                break;
            if(word == "NEIGHBOURS_INDICES")
            {
                ReadVectorialValue(rThisCommunicator.NeighbourIndices());
            }
            else if(word == "NUMBER_OF_COLORS")
            {
                ReadWord(word);
                SizeType number_of_colors;
                ExtractValue(word, number_of_colors);
                rThisCommunicator.SetNumberOfColors(number_of_colors);
            }
            else
            {
                ReadBlockName(word);
                if(word == "LocalNodes")
                {
                    ReadCommunicatorLocalNodesBlock(rThisCommunicator, rThisNodes);
                }
                else if(word == "GhostNodes")
                {
                    ReadCommunicatorGhostNodesBlock(rThisCommunicator, rThisNodes);
                }
                else
                {
                    SkipBlock(word);
                }
            }
        }

        return ;

        QUEST_CATCH("")
    }

    /**
     * @brief 读取本地节点的通信数据块
     */
    void ModelPartIO::ReadCommunicatorLocalNodesBlock(Communicator& rThisCommunicator, NodesContainerType& rThisNodes){
        QUEST_TRY

        SizeType interface_id;
        SizeType node_id;

        std::string word;
        std::string condition_name;

        ReadWord(word);
        ExtractValue(word,interface_id);

        if(interface_id > rThisCommunicator.GetNumberOfColors())
        {
            std::stringstream buffer;
            buffer << "Interface " << interface_id << " is not valid.";
            buffer << " The number of colors is " << rThisCommunicator.GetNumberOfColors() << " and the interface id must be les than or equal to number of colors" ;
            buffer << " [Line " << mNumberOfLines << " ]";
            QUEST_ERROR << buffer.str() << std::endl;
        }

        Communicator::MeshType* p_local_mesh;
        Communicator::MeshType* p_interface_mesh;

        if(interface_id == 0)
        {
            p_local_mesh = &(rThisCommunicator.LocalMesh());
            p_interface_mesh = &(rThisCommunicator.InterfaceMesh());
        }
        else
        {
            p_local_mesh = &(rThisCommunicator.LocalMesh(interface_id-1));
            p_interface_mesh = &(rThisCommunicator.InterfaceMesh(interface_id-1));
        }

        NodesContainerType aux_local;
        NodesContainerType aux_interface;

        while(!mpStream->eof())
        {
            ReadWord(word); 
            if(CheckEndBlock("LocalNodes", word))
                break;

            ExtractValue(word,node_id);
            NodesContainerType::iterator it_node = FindKey(rThisNodes, ReorderedNodeId(node_id), "Node");
            auto p_node = *(it_node.base());
            aux_local.push_back(p_node);
            aux_interface.push_back(p_node);
        }

        for(auto it = aux_local.begin(); it!= aux_local.end(); it++)
            p_local_mesh->Nodes().push_back(*(it.base()));

        for(auto it = aux_interface.begin(); it!= aux_interface.end(); it++)
            p_interface_mesh->Nodes().push_back(*(it.base()));

        p_local_mesh->Nodes().Unique();
        p_interface_mesh->Nodes().Unique();

        QUEST_CATCH("")
    }

    /**
     * @brief 读取幽灵节点（ghost nodes）的通信数据块
     */
    void ModelPartIO::ReadCommunicatorGhostNodesBlock(Communicator& rThisCommunicator, NodesContainerType& rThisNodes){
        QUEST_TRY

        SizeType interface_id;
        SizeType node_id;


        std::string word;
        std::string condition_name;

        ReadWord(word); 
        ExtractValue(word,interface_id);


        if(interface_id > rThisCommunicator.GetNumberOfColors())
        {
            std::stringstream buffer;
            buffer << "Interface " << interface_id << " is not valid.";
            buffer << " The number of colors is " << rThisCommunicator.GetNumberOfColors() << " and the interface id must be les than or equal to number of colors" ;
            buffer << " [Line " << mNumberOfLines << " ]";
            QUEST_ERROR << buffer.str() << std::endl;
        }

        Communicator::MeshType* p_ghost_mesh;
        Communicator::MeshType* p_interface_mesh;

        if(interface_id == 0)
        {
            p_ghost_mesh = &(rThisCommunicator.GhostMesh());
            p_interface_mesh = &(rThisCommunicator.InterfaceMesh());
        }
        else
        {
            p_ghost_mesh = &(rThisCommunicator.GhostMesh(interface_id-1));
            p_interface_mesh = &(rThisCommunicator.InterfaceMesh(interface_id-1));
        }

        while(!mpStream->eof())
        {
            ReadWord(word); 
            if(CheckEndBlock("GhostNodes", word))
                break;

            ExtractValue(word,node_id);
            NodesContainerType::iterator it_node = FindKey(rThisNodes, ReorderedNodeId(node_id), "Node");
            p_ghost_mesh->Nodes().push_back(*(it_node.base()));
            p_interface_mesh->Nodes().push_back(*(it_node.base()));
        }

        p_ghost_mesh->Nodes().Unique();
        p_interface_mesh->Nodes().Unique();

        QUEST_CATCH("")
    }

    /**
     * @brief 读取网格块
     */
    void ModelPartIO::ReadMeshBlock(ModelPart& rModelPart){
        QUEST_TRY

        std::string word;
        SizeType mesh_id;

        ReadWord(word);
        ExtractValue(word, mesh_id);

        SizeType number_of_meshes = rModelPart.NumberOfMeshes();

        QUEST_ERROR_IF(mesh_id > 1000000) << "Too large mesh id : " << mesh_id << std::endl;

        QUEST_ERROR_IF(mesh_id == 0) << "The mesh zero is the reference mesh and already created. You cannot create a mesh 0 with mesh block." << std::endl;

        MeshType empty_mesh;
        for(SizeType i = number_of_meshes ; i < mesh_id + 1 ; i++)
            rModelPart.GetMeshes().push_back(Quest::make_shared<MeshType>(empty_mesh.Clone()));

        MeshType& mesh = rModelPart.GetMesh(mesh_id);

        while(true)
        {
            ReadWord(word);

            if(mpStream->eof())
            {
                break;
            }

            if(CheckEndBlock("Mesh", word))
            {
                    break;
            }

            ReadBlockName(word);
            if(word == "MeshData")
            {
                ReadMeshDataBlock(mesh);
            }
            else if(word == "MeshNodes")
            {
                ReadMeshNodesBlock(rModelPart, mesh);
            }

            else if(word == "MeshElements")
            {
                ReadMeshElementsBlock(rModelPart, mesh);
            }

            else if(word == "MeshConditions")
            {
                ReadMeshConditionsBlock(rModelPart, mesh);
            }

            else
            {
                SkipBlock(word);
            }
        }

        QUEST_CATCH("")

    }

    /**
     * @brief 写入网格块
     */
    void ModelPartIO::WriteMeshBlock(ModelPart& rModelPart){}

    /**
     * @brief 读取网格数据块
     */
    void ModelPartIO::ReadMeshDataBlock(MeshType& rMesh){
        QUEST_TRY

        std::string variable_name;

        while(!mpStream->eof())
        {
            ReadWord(variable_name);
            if(CheckEndBlock("MeshData", variable_name))
                break;
            if(QuestComponents<Variable<double> >::Has(variable_name))
            {
                std::string value;
                double temp;

                ReadWord(value); // reading value
                ExtractValue(value,temp);
                rMesh[QuestComponents<Variable<double> >::Get(variable_name)] = temp;
            }
            else if(QuestComponents<Variable<bool> >::Has(variable_name))
            {
                std::string value;
                bool temp;

                ReadWord(value); // reading value
                ExtractValue(value,temp);
                rMesh[QuestComponents<Variable<bool> >::Get(variable_name)] = temp;
            }
            else if(QuestComponents<Variable<int> >::Has(variable_name))
            {
                std::string value;
                int temp;

                ReadWord(value); // reading value
                ExtractValue(value,temp);
                rMesh[QuestComponents<Variable<int> >::Get(variable_name)] = temp;
            }
            else if(QuestComponents<Variable<Array1d<double, 3> > >::Has(variable_name))
            {
                Vector temp_vector; // defining a Vector because for Array1d the operator >> is not defined yet!
                ReadVectorialValue(temp_vector);
                rMesh[QuestComponents<Variable<Array1d<double,3> > >::Get(variable_name)] = temp_vector;
            }
            else if(QuestComponents<Variable<Quaternion<double> > >::Has(variable_name))
            {
                Vector temp_vector; // defining a Vector because for Quaternion the operator >> is not defined yet!
                ReadVectorialValue(temp_vector);
                rMesh[QuestComponents<Variable<Quaternion<double> > >::Get(variable_name)] = temp_vector;
            }
            else if(QuestComponents<Variable<Matrix> >::Has(variable_name))
            {
                ReadVectorialValue(rMesh[QuestComponents<Variable<Matrix> >::Get(variable_name)]);
            }
            else if(QuestComponents<Variable<std::string> >::Has(variable_name))
            {
                std::string value;
        std::string  temp;

                ReadWord(value); // reading value
                ExtractValue(value,temp);
                rMesh[QuestComponents<Variable<std::string> >::Get(variable_name)] = temp;
            }
            else
            {
                std::stringstream buffer;
                buffer << variable_name << " is not a valid variable!!!" << std::endl;
                buffer << " [Line " << mNumberOfLines << " ]";
                QUEST_ERROR << buffer.str() << std::endl;
            }


        }

        QUEST_CATCH("")
    }

    /**
     * @brief 读取网格节点块
     */
    void ModelPartIO::ReadMeshNodesBlock(ModelPart& rModelPart, MeshType& rMesh){
        QUEST_TRY

        SizeType node_id;

        std::string word;


        while(!mpStream->eof())
        {
            ReadWord(word); 
            if(CheckEndBlock("MeshNodes", word))
                break;

            ExtractValue(word,node_id);
            NodesContainerType::iterator it_node = FindKey(rModelPart.Nodes(), ReorderedNodeId(node_id), "Node");
            rMesh.Nodes().push_back(*(it_node.base()));
        }

        rMesh.Nodes().Sort();
        QUEST_CATCH("")
    }

    /**
     * @brief 读取网格单元块
     */
    void ModelPartIO::ReadMeshElementsBlock(ModelPart& rModelPart, MeshType& rMesh){
        QUEST_TRY

        SizeType element_id;

        std::string word;


        while(!mpStream->eof())
        {
            ReadWord(word); 
            if(CheckEndBlock("MeshElements", word))
                break;

            ExtractValue(word,element_id);
            ElementsContainerType::iterator i_element = FindKey(rModelPart.Elements(), ReorderedElementId(element_id), "Element");
            rMesh.Elements().push_back(*(i_element.base()));
        }

        rMesh.Elements().Sort();
        QUEST_CATCH("")
    }

    /**
     * @brief 读取网格条件块
     */
    void ModelPartIO::ReadMeshConditionsBlock(ModelPart& rModelPart, MeshType& rMesh){
        QUEST_TRY

        SizeType condition_id;

        std::string word;


        while(!mpStream->eof())
        {
            ReadWord(word); 
            if(CheckEndBlock("MeshConditions", word))
                break;

            ExtractValue(word,condition_id);
            ConditionsContainerType::iterator i_condition = FindKey(rModelPart.Conditions(), ReorderedConditionId(condition_id), "Condition");
            rMesh.Conditions().push_back(*(i_condition.base()));
        }

        rMesh.Conditions().Sort();
        QUEST_CATCH("")
    }

    /**
     * @brief 读取网格属性块
     */
    void ModelPartIO::ReadMeshPropertiesBlock(ModelPart& rModelPart, MeshType& rMesh){
        QUEST_TRY

        Properties::Pointer props = Quest::make_shared<Properties>();
        Properties& temp_properties = *props;

        std::string word;
        std::string variable_name;

        SizeType temp_properties_id;

        ReadWord(word);
        ExtractValue(word, temp_properties_id);
        temp_properties.SetId(temp_properties_id);


        while(!mpStream->eof())
        {
            ReadWord(variable_name);
            if(CheckEndBlock("MeshProperties", variable_name))
                break;

        if(QuestComponents<Variable<std::string> >::Has(variable_name))
            {
                std::string value;
                std::string  temp;

                ReadWord(value);
                ExtractValue(value,temp);
                temp_properties[QuestComponents<Variable<std::string> >::Get(variable_name)] = temp;
            }
        else if(QuestComponents<Variable<double> >::Has(variable_name))
            {
                std::string value;
                double temp;

                ReadWord(value); 
                ExtractValue(value,temp);
                temp_properties[QuestComponents<Variable<double> >::Get(variable_name)] = temp;
            }
            else if(QuestComponents<Variable<int> >::Has(variable_name))
            {
                std::string value;
                int temp;

                ReadWord(value); 
                ExtractValue(value,temp);
                temp_properties[QuestComponents<Variable<int> >::Get(variable_name)] = temp;
            }
            else if(QuestComponents<Variable<bool> >::Has(variable_name))
            {
                std::string value;
                bool temp;

                ReadWord(value);
                ExtractValue(value,temp);
                temp_properties[QuestComponents<Variable<bool> >::Get(variable_name)] = temp;
            }
            else if(QuestComponents<Variable<Array1d<double, 3> > >::Has(variable_name))
            {
                Vector temp_vector; 
                ReadVectorialValue(temp_vector);
                temp_properties[QuestComponents<Variable<Array1d<double,3> > >::Get(variable_name)] = temp_vector;
            }
            else if(QuestComponents<Variable<Quaternion<double> > >::Has(variable_name))
            {
                Vector temp_vector; 
                ReadVectorialValue(temp_vector);
                temp_properties[QuestComponents<Variable<Quaternion<double> > >::Get(variable_name)] = temp_vector;
            }
            else if(QuestComponents<Variable<Vector> >::Has(variable_name))
            {
                ReadVectorialValue(temp_properties[QuestComponents<Variable<Vector> >::Get(variable_name)]);
            }
            else if(QuestComponents<Variable<Matrix> >::Has(variable_name))
            {
                ReadVectorialValue(temp_properties[QuestComponents<Variable<Matrix> >::Get(variable_name)]);
            }
            else
            {
                std::stringstream buffer;
                buffer << variable_name << " is not a valid variable!!!" << std::endl;
                buffer << " [Line " << mNumberOfLines << " ]";
                QUEST_ERROR << buffer.str() << std::endl;
            }

        }

        rMesh.Properties().push_back(props);

        QUEST_CATCH("")
    }

    /**
     * @brief 读取子模型部件块
     */
    void ModelPartIO::ReadSubModelPartBlock(ModelPart& rMainModelPart, ModelPart& rParentModelPart){
        QUEST_TRY

        std::string word;

        ReadWord(word);

        ModelPart& r_sub_model_part = rParentModelPart.CreateSubModelPart(word);

        while (true)
        {
            ReadWord(word);
            if (CheckEndBlock("SubModelPart", word))
                break;

            ReadBlockName(word);
            if (word == "SubModelPartData") {
                if (mOptions.IsNot(IO::MESH_ONLY)) {
                    ReadSubModelPartDataBlock(r_sub_model_part);
                } else {
                    SkipBlock("SubModelPartData");
                }
            } else if (word == "SubModelPartTables") {
                if (mOptions.IsNot(IO::MESH_ONLY)) {
                    ReadSubModelPartTablesBlock(rMainModelPart, r_sub_model_part);
                } else {
                    SkipBlock("SubModelPartTables");
                }
            } else if (word == "SubModelPartProperties") {
                ReadSubModelPartPropertiesBlock(rMainModelPart, r_sub_model_part);
            } else if (word == "SubModelPartNodes") {
                ReadSubModelPartNodesBlock(rMainModelPart, r_sub_model_part);
            } else if (word == "SubModelPartElements") {
                ReadSubModelPartElementsBlock(rMainModelPart, r_sub_model_part);
            } else if (word == "SubModelPartConditions") {
                ReadSubModelPartConditionsBlock(rMainModelPart, r_sub_model_part);
            } else if (word == "SubModelPartGeometries") {
                ReadSubModelPartGeometriesBlock(rMainModelPart, r_sub_model_part);
            } else if (word == "SubModelPart") {
                ReadSubModelPartBlock(rMainModelPart, r_sub_model_part);
            }
        }

        QUEST_CATCH("")
    }

    /**
     * @brief 写入子模型部件块
     */
    void ModelPartIO::WriteSubModelPartBlock(ModelPart& rMainModelPart, const std::string& InitialTabulation){
        QUEST_TRY;

        const std::vector<std::string> sub_model_part_names = rMainModelPart.GetSubModelPartNames();

        for (unsigned int i_sub = 0; i_sub < sub_model_part_names.size(); i_sub++) {

            const std::string sub_model_part_name = sub_model_part_names[i_sub];
            ModelPart& r_sub_model_part = rMainModelPart.GetSubModelPart(sub_model_part_name);

            (*mpStream) << InitialTabulation << "Begin SubModelPart\t" << sub_model_part_name << std::endl;

            (*mpStream) << InitialTabulation << "\tBegin SubModelPartData" << std::endl;

            (*mpStream) << InitialTabulation  << "\tEnd SubModelPartData" << std::endl;

            (*mpStream) << InitialTabulation  << "\tBegin SubModelPartTables" << std::endl;

            (*mpStream) << InitialTabulation << "\tEnd SubModelPartTables" << std::endl;

            (*mpStream) << InitialTabulation << "\tBegin SubModelPartNodes" << std::endl;
            NodesContainerType& rThisNodes = r_sub_model_part.Nodes();
            auto numNodes = rThisNodes.end() - rThisNodes.begin();
            for(unsigned int i = 0; i < numNodes; i++) {
                auto itNode = rThisNodes.begin() + i;
                (*mpStream) << InitialTabulation << "\t\t" << itNode->Id() << "\n";;
            }
            (*mpStream) << InitialTabulation << "\tEnd SubModelPartNodes" << std::endl;

            (*mpStream) << InitialTabulation << "\tBegin SubModelPartElements" << std::endl;
            ElementsContainerType& rThisElements = r_sub_model_part.Elements();
            auto num_elements = rThisElements.end() - rThisElements.begin();
            for(unsigned int i = 0; i < num_elements; i++) {
                auto itElem = rThisElements.begin() + i;
                (*mpStream) << InitialTabulation << "\t\t" << itElem->Id() << "\n";;
            }
            (*mpStream) << InitialTabulation << "\tEnd SubModelPartElements" << std::endl;

            (*mpStream) << InitialTabulation << "\tBegin SubModelPartConditions" << std::endl;
            ConditionsContainerType& rThisConditions= r_sub_model_part.Conditions();
            auto numConditions = rThisConditions.end() - rThisConditions.begin();
            for(unsigned int i = 0; i < numConditions; i++) {
                auto itCond = rThisConditions.begin() + i;
                (*mpStream) << InitialTabulation << "\t\t" << itCond->Id() << "\n";;
            }
            (*mpStream) << InitialTabulation << "\tEnd SubModelPartConditions" << std::endl;

            WriteSubModelPartBlock(r_sub_model_part, InitialTabulation+"\t");

            (*mpStream) << InitialTabulation << "End SubModelPart\t" << std::endl << std::endl;
        }

        QUEST_CATCH("");
    }

    /**
     * @brief 读取子模型部件的数据块
     */
    void ModelPartIO::ReadSubModelPartDataBlock(ModelPart& rModelPart){
        QUEST_TRY

        ReadModelPartDataBlock(rModelPart, true);

        QUEST_CATCH("")
    }

    /**
     * @brief 读取子模型部件的表格数据块
     */
    void ModelPartIO::ReadSubModelPartTablesBlock(ModelPart& rMainModelPart, ModelPart& rSubModelPart){
        QUEST_TRY

        SizeType table_id;
        std::string word;

        while (!mpStream->eof())
        {
            ReadWord(word); 
            if (CheckEndBlock("SubModelPartTables", word))
                break;

            ExtractValue(word, table_id);
            ModelPart::TablesContainerType::iterator i_table = FindKey(rMainModelPart.Tables(), table_id, "Table");
            rSubModelPart.AddTable((i_table.base())->first, (i_table.base())->second);
        }
        QUEST_CATCH("")
    }

    /**
     * @brief 读取子模型部件的属性数据块
     */
    void ModelPartIO::ReadSubModelPartPropertiesBlock(ModelPart& rMainModelPart, ModelPart& rSubModelPart){
        QUEST_TRY

        SizeType properties_id;
        std::string word;

        while (!mpStream->eof())
        {
            ReadWord(word); 
            if (CheckEndBlock("SubModelPartProperties", word))
                break;

            ExtractValue(word, properties_id);
            PropertiesContainerType::iterator i_properties = FindKey(rMainModelPart.rProperties(), properties_id, "Properties");
            rSubModelPart.AddProperties(*(i_properties.base()));
        }
        QUEST_CATCH("")
    }

    /**
     * @brief 读取子模型部件的节点块
     */
    void ModelPartIO::ReadSubModelPartNodesBlock(ModelPart& rMainModelPart, ModelPart& rSubModelPart){
        QUEST_TRY

        SizeType node_id;
        std::string word;

                std::vector<SizeType> ordered_ids;

        while (!mpStream->eof())
        {
            ReadWord(word); 
            if (CheckEndBlock("SubModelPartNodes", word))
                break;

            ExtractValue(word, node_id);
                        ordered_ids.push_back(ReorderedNodeId(node_id));
        }

        std::sort(ordered_ids.begin(), ordered_ids.end());
                rSubModelPart.AddNodes(ordered_ids);

        QUEST_CATCH("")
    }

    /**
     * @brief 读取子模型部件的单元块
     */
    void ModelPartIO::ReadSubModelPartElementsBlock(ModelPart& rMainModelPart, ModelPart& rSubModelPart){
        QUEST_TRY

        SizeType element_id;
        std::string word;
                std::vector<SizeType> ordered_ids;

        while (!mpStream->eof())
        {
            ReadWord(word); 
            if (CheckEndBlock("SubModelPartElements", word))
                break;

            ExtractValue(word, element_id);
                        ordered_ids.push_back(ReorderedElementId(element_id));
        }
        std::sort(ordered_ids.begin(), ordered_ids.end());
        rSubModelPart.AddElements(ordered_ids);

        QUEST_CATCH("")
    }

    /**
     * @brief 读取子模型部件的条件块
     */
    void ModelPartIO::ReadSubModelPartConditionsBlock(ModelPart& rMainModelPart, ModelPart& rSubModelPart){
        QUEST_TRY

        SizeType condition_id;
        std::string word;
                std::vector<SizeType> ordered_ids;

        while (!mpStream->eof())
        {
            ReadWord(word); // Reading the node id or End
            if (CheckEndBlock("SubModelPartConditions", word))
                break;

            ExtractValue(word, condition_id);
                        ordered_ids.push_back(ReorderedConditionId(condition_id));
        }
        std::sort(ordered_ids.begin(), ordered_ids.end());
        rSubModelPart.AddConditions(ordered_ids);

        QUEST_CATCH("")
    }

    /**
     * @brief 读取子模型部件的几何体块
     */
    void ModelPartIO::ReadSubModelPartGeometriesBlock(
        ModelPart &rMainModelPart,
        ModelPart &rSubModelPart
    ){
        QUEST_TRY

        SizeType geometry_id;
        std::string word;
        std::vector<SizeType> ordered_ids;

        while (!mpStream->eof())
        {
            ReadWord(word); // Reading the geometry id or End
            if (CheckEndBlock("SubModelPartGeometries", word))
                break;

            ExtractValue(word, geometry_id);
            ordered_ids.push_back(geometry_id);
        }
        std::sort(ordered_ids.begin(), ordered_ids.end());
        rSubModelPart.AddGeometries(ordered_ids);

        QUEST_CATCH("")
    }

    /**
     * @brief 根据分区信息将输入数据分割成多个部分
     */
    void ModelPartIO::DivideInputToPartitionsImpl(
        OutputFilesContainerType& rOutputFiles,
        SizeType NumberOfPartitions,
        const PartitioningInfo& rPartitioningInfo
    ){
        QUEST_TRY

        ResetInput();
        std::string word;

        while(true)
        {
            ReadWord(word);
            if(mpStream->eof())
                break;
            ReadBlockName(word);
            if(word == "ModelPartData")
                DivideModelPartDataBlock(rOutputFiles);
            else if(word == "Table")
                DivideTableBlock(rOutputFiles);
            else if(word == "Properties")
                DividePropertiesBlock(rOutputFiles);
            else if(word == "Nodes")
                DivideNodesBlock(rOutputFiles, rPartitioningInfo.NodesAllPartitions);
            else if(word == "Elements")
                DivideElementsBlock(rOutputFiles, rPartitioningInfo.ElementsAllPartitions);
            else if(word == "Conditions")
                DivideConditionsBlock(rOutputFiles, rPartitioningInfo.ConditionsAllPartitions);
            else if(word == "NodalData")
                DivideNodalDataBlock(rOutputFiles, rPartitioningInfo.NodesAllPartitions);
            else if(word == "ElementalData")
                DivideElementalDataBlock(rOutputFiles, rPartitioningInfo.ElementsAllPartitions);
            else if(word == "ConditionalData")
                DivideConditionalDataBlock(rOutputFiles, rPartitioningInfo.ConditionsAllPartitions);
            else if (word == "Mesh")
                DivideMeshBlock(rOutputFiles, rPartitioningInfo.NodesAllPartitions, rPartitioningInfo.ElementsAllPartitions, rPartitioningInfo.ConditionsAllPartitions);
            else if (word == "SubModelPart")
                DivideSubModelPartBlock(rOutputFiles, rPartitioningInfo.NodesAllPartitions, rPartitioningInfo.ElementsAllPartitions, rPartitioningInfo.ConditionsAllPartitions);

        }

        WritePartitionIndices(rOutputFiles, rPartitioningInfo.NodesPartitions, rPartitioningInfo.NodesAllPartitions);

        WriteCommunicatorData(rOutputFiles, NumberOfPartitions, rPartitioningInfo.Graph, rPartitioningInfo.NodesPartitions, rPartitioningInfo.ElementsPartitions, rPartitioningInfo.ConditionsPartitions, rPartitioningInfo.NodesAllPartitions, rPartitioningInfo.ElementsAllPartitions, rPartitioningInfo.ConditionsAllPartitions);

        QUEST_INFO("ModelPartIO") << "  [Total Lines Read : " << mNumberOfLines<<"]" << std::endl;

        QUEST_CATCH("")
    }

    /**
     * @brief 分割模型部件的数据块
     */
    void ModelPartIO::DivideModelPartDataBlock(OutputFilesContainerType& OutputFiles){
        QUEST_TRY
        std::string block;

        WriteInAllFiles(OutputFiles, "Begin ModelPartData\n");

        ReadBlock(block, "ModelPartData");
        WriteInAllFiles(OutputFiles, block);

        WriteInAllFiles(OutputFiles, "End ModelPartData\n");

        QUEST_CATCH("")
    }

    /**
     * @brief 分割表格块
     */
    void ModelPartIO::DivideTableBlock(OutputFilesContainerType& OutputFiles){
        QUEST_TRY

        std::string block;

        WriteInAllFiles(OutputFiles, "Begin Table ");

        ReadBlock(block, "Table");
        WriteInAllFiles(OutputFiles, block);

        WriteInAllFiles(OutputFiles, "End Table\n");

        QUEST_CATCH("")
    }

    /**
     * @brief 分割属性块
     */
    void ModelPartIO::DividePropertiesBlock(OutputFilesContainerType& OutputFiles){
        QUEST_TRY

        std::string block;

        WriteInAllFiles(OutputFiles, "Begin Properties ");

        ReadBlock(block, "Properties");
        WriteInAllFiles(OutputFiles, block);

        WriteInAllFiles(OutputFiles, "End Properties\n");

        QUEST_CATCH("")
    }

    /**
     * @brief 分割节点块
     */
    void ModelPartIO::DivideNodesBlock(
        OutputFilesContainerType& OutputFiles,
        const PartitionIndicesContainerType& NodesAllPartitions
    ){
    QUEST_TRY

    std::string word;

    WriteInAllFiles(OutputFiles, "Begin Nodes \n");

    SizeType id;

    while(!mpStream->eof())
    {
        ReadWord(word);
        if(CheckEndBlock("Nodes", word))
            break;

        ExtractValue(word, id);

        if(ReorderedNodeId(id) > NodesAllPartitions.size())
        {
            std::stringstream buffer;
            buffer << "Invalid node id : " << id;
            buffer << " [Line " << mNumberOfLines << " ]";
            QUEST_ERROR << buffer.str() << std::endl;
        }

        std::stringstream node_data;
        node_data << ReorderedNodeId(id) << '\t'; 
        ReadWord(word);
        node_data << word << '\t'; 
        ReadWord(word);
        node_data << word << '\t'; 
        ReadWord(word);
        node_data << word << '\n'; 

        for(SizeType i = 0 ; i < NodesAllPartitions[ReorderedNodeId(id)-1].size() ; i++)
        {
            SizeType partition_id = NodesAllPartitions[ReorderedNodeId(id)-1][i];
            if(partition_id > OutputFiles.size())
            {
                std::stringstream buffer;
                buffer << "Invalid partition id : " << partition_id;
                buffer << " for node " << id << " [Line " << mNumberOfLines << " ]";
                QUEST_ERROR << buffer.str() << std::endl;
            }

            *(OutputFiles[partition_id]) << node_data.str();
        }

    }

    WriteInAllFiles(OutputFiles, "End Nodes\n");

    QUEST_CATCH("")
}

    /**
     * @brief 分割几何体块
     */
    void ModelPartIO::DivideGeometriesBlock(
        OutputFilesContainerType& OutputFiles,
        const PartitionIndicesContainerType& GeometriesAllPartitions
    ){
        QUEST_TRY

        std::string word;
        std::string geometry_name;

        ReadWord(geometry_name);
        if(!QuestComponents<GeometryType>::Has(geometry_name)) {
            std::stringstream buffer;
            buffer << "Geometry " << geometry_name << " is not registered in Quest.";
            buffer << " Please check the spelling of the geometry name and see if the application containing it is registered correctly.";
            buffer << " [Line " << mNumberOfLines << " ]";
            QUEST_ERROR << buffer.str() << std::endl;
            return;
        }

        GeometryType const& r_clone_geometry = QuestComponents<GeometryType>::Get(geometry_name);
        SizeType number_of_nodes = r_clone_geometry.size();

        WriteInAllFiles(OutputFiles, "Begin Geometries " +  geometry_name);

        SizeType id;

        while(!mpStream->eof()) {
            ReadWord(word); 
            if(CheckEndBlock("Geometries", word))
                break;

            ExtractValue(word,id);
            if(ReorderedGeometryId(id) > GeometriesAllPartitions.size()) {
                std::stringstream buffer;
                buffer << "Invalid geometry id : " << id;
                buffer << " [Line " << mNumberOfLines << " ]";
                QUEST_ERROR << buffer.str() << std::endl;
            }

            std::stringstream geometry_data;
            geometry_data << '\n' << ReorderedGeometryId(id) << '\t'; 
            ReadWord(word); 
            geometry_data << word << '\t'; 

            for(SizeType i = 0 ; i < number_of_nodes ; i++) {
                ReadWord(word); 
                SizeType node_id;
                ExtractValue(word, node_id);
                geometry_data << ReorderedNodeId(node_id) << '\t'; 
            }


            for(SizeType i = 0 ; i < GeometriesAllPartitions[ReorderedGeometryId(id)-1].size() ; i++) {
                SizeType partition_id = GeometriesAllPartitions[ReorderedGeometryId(id)-1][i];
                if(partition_id > OutputFiles.size()) {
                    std::stringstream buffer;
                    buffer << "Invalid partition id : " << partition_id;
                    buffer << " for node " << id << " [Line " << mNumberOfLines << " ]";
                    QUEST_ERROR << buffer.str() << std::endl;
                }

                *(OutputFiles[partition_id]) << geometry_data.str();
            }

        }

        WriteInAllFiles(OutputFiles, "\nEnd Geometries\n");

        QUEST_CATCH("")
    }

    /**
     * @brief 分割单元块
     */
    void ModelPartIO::DivideElementsBlock(
        OutputFilesContainerType& OutputFiles,
        const PartitionIndicesContainerType& ElementsAllPartitions
    ){
        QUEST_TRY


        std::string word;
        std::string element_name;

        ReadWord(element_name);
        if(!QuestComponents<Element>::Has(element_name))
        {
            std::stringstream buffer;
            buffer << "Element " << element_name << " is not registered in Quest.";
            buffer << " Please check the spelling of the element name and see if the application containing it is registered correctly.";
            buffer << " [Line " << mNumberOfLines << " ]";
            QUEST_ERROR << buffer.str() << std::endl;
            return;
        }

        Element const& r_clone_element = QuestComponents<Element>::Get(element_name);
        SizeType number_of_nodes = r_clone_element.GetGeometry().size();

        WriteInAllFiles(OutputFiles, "Begin Elements " +  element_name);

        SizeType id;

        while(!mpStream->eof())
        {
            ReadWord(word); 
            if(CheckEndBlock("Elements", word))
                break;

            ExtractValue(word,id);
            if(ReorderedElementId(id) > ElementsAllPartitions.size())
            {
                std::stringstream buffer;
                buffer << "Invalid element id : " << id;
                buffer << " [Line " << mNumberOfLines << " ]";
                QUEST_ERROR << buffer.str() << std::endl;
            }

            std::stringstream element_data;
            element_data << '\n' << ReorderedElementId(id) << '\t';
            ReadWord(word);
            element_data << word << '\t';

            for(SizeType i = 0 ; i < number_of_nodes ; i++)
            {
                ReadWord(word); 
                SizeType node_id;
                ExtractValue(word, node_id);
                element_data << ReorderedNodeId(node_id) << '\t'; 
            }


            for(SizeType i = 0 ; i < ElementsAllPartitions[ReorderedElementId(id)-1].size() ; i++)
            {
                SizeType partition_id = ElementsAllPartitions[ReorderedElementId(id)-1][i];
                if(partition_id > OutputFiles.size())
                {
                    std::stringstream buffer;
                    buffer << "Invalid partition id : " << partition_id;
                    buffer << " for node " << id << " [Line " << mNumberOfLines << " ]";
                    QUEST_ERROR << buffer.str() << std::endl;
                }

                *(OutputFiles[partition_id]) << element_data.str();
            }

        }

        WriteInAllFiles(OutputFiles, "\nEnd Elements\n");

        QUEST_CATCH("")
    }

    /**
     * @brief 分割条件块
     */
    void ModelPartIO::DivideConditionsBlock(
        OutputFilesContainerType& OutputFiles,
        const PartitionIndicesContainerType & ConditionsAllPartitions
    ){
        QUEST_TRY

        std::string word;
        std::string condition_name;

        ReadWord(condition_name);
        if(!QuestComponents<Condition>::Has(condition_name))
        {
            std::stringstream buffer;
            buffer << "Condition " << condition_name << " is not registered in Quest.";
            buffer << " Please check the spelling of the condition name and see if the application containing it is registered correctly.";
            buffer << " [Line " << mNumberOfLines << " ]";
            QUEST_ERROR << buffer.str() << std::endl;
            return;
        }

        Condition const& r_clone_condition = QuestComponents<Condition>::Get(condition_name);
        SizeType number_of_nodes = r_clone_condition.GetGeometry().size();

        WriteInAllFiles(OutputFiles, "Begin Conditions " +  condition_name);

        SizeType id;

        while(!mpStream->eof())
        {
            ReadWord(word);
            if(CheckEndBlock("Conditions", word))
                break;

            ExtractValue(word,id);
            if(ReorderedConditionId(id) > ConditionsAllPartitions.size())
            {
                std::stringstream buffer;
                buffer << "Invalid condition id : " << id;
                buffer << " [Line " << mNumberOfLines << " ]";
                QUEST_ERROR << buffer.str() << std::endl;
            }

            std::stringstream condition_data;
            condition_data << '\n' << ReorderedConditionId(id) << '\t'; 
            ReadWord(word); 
            condition_data << word << '\t'; 

            for(SizeType i = 0 ; i < number_of_nodes ; i++)
            {
                ReadWord(word);
                SizeType node_id;
                ExtractValue(word, node_id);
                condition_data << ReorderedNodeId(node_id) << '\t';
            }


            for(SizeType i = 0 ; i < ConditionsAllPartitions[ReorderedConditionId(id)-1].size() ; i++)
            {
                SizeType partition_id = ConditionsAllPartitions[ReorderedConditionId(id)-1][i];
                if(partition_id > OutputFiles.size())
                {
                    std::stringstream buffer;
                    buffer << "Invalid partition id : " << partition_id;
                    buffer << " for node " << id << " [Line " << mNumberOfLines << " ]";
                    QUEST_ERROR << buffer.str() << std::endl;
                }

                *(OutputFiles[partition_id]) << condition_data.str();
            }

        }

        WriteInAllFiles(OutputFiles, "\nEnd Conditions\n");

        QUEST_CATCH("")
    }

    /**
     * @brief 分割节点的数据块
     */
    void ModelPartIO::DivideNodalDataBlock(
        OutputFilesContainerType& OutputFiles,
        const PartitionIndicesContainerType& NodesAllPartitions
    ){
        QUEST_TRY

        std::string word;

        WriteInAllFiles(OutputFiles, "Begin NodalData ");

        std::string variable_name;

        ReadWord(variable_name);

        WriteInAllFiles(OutputFiles, variable_name);
        WriteInAllFiles(OutputFiles, "\n");

        if(QuestComponents<Flags>::Has(variable_name)) {
            DivideFlagVariableData(OutputFiles, NodesAllPartitions);
        } else if(QuestComponents<Variable<double> >::Has(variable_name)) {
            DivideDofVariableData(OutputFiles, NodesAllPartitions);
        } else if(QuestComponents<Variable<int> >::Has(variable_name)) {
            DivideDofVariableData(OutputFiles, NodesAllPartitions);
        } else if(QuestComponents<Variable<bool> >::Has(variable_name)) {
            DivideDofVariableData(OutputFiles, NodesAllPartitions);
        } else if(QuestComponents<Variable<Array1d<double, 3> > >::Has(variable_name)) {
            DivideVectorialVariableData<Vector>(OutputFiles, NodesAllPartitions, "NodalData");
        } else if(QuestComponents<Variable<Quaternion<double> > >::Has(variable_name)) {
            DivideVectorialVariableData<Vector>(OutputFiles, NodesAllPartitions, "NodalData");
        } else if(QuestComponents<Variable<Vector> >::Has(variable_name)) {
            DivideVectorialVariableData<Vector>(OutputFiles, NodesAllPartitions, "NodalData" );
        } else if(QuestComponents<Variable<Matrix> >::Has(variable_name)) {
            DivideVectorialVariableData<Matrix>(OutputFiles, NodesAllPartitions, "NodalData" );
        } else if(QuestComponents<VariableData>::Has(variable_name)) {
            std::stringstream buffer;
            buffer << variable_name << " is not supported to be read by this IO or the type of variable is not registered correctly" << std::endl;
            buffer << " [Line " << mNumberOfLines << " ]";
            QUEST_ERROR << buffer.str() << std::endl;;
        } else {
            std::stringstream buffer;
            buffer << variable_name << " is not a valid variable!!!" << std::endl;
            buffer << " [Line " << mNumberOfLines << " ]";
            QUEST_ERROR << buffer.str() << std::endl;
        }

        WriteInAllFiles(OutputFiles, "End NodalData\n");

        QUEST_CATCH("")
    }

    /**
     * @brief 分割标志变量的数据块
     */
    void ModelPartIO::DivideFlagVariableData(
        OutputFilesContainerType& OutputFiles,
        const PartitionIndicesContainerType& NodesAllPartitions
    ){
        QUEST_TRY

        SizeType id;

        std::string word;

        while(!mpStream->eof()) {
            ReadWord(word); 
            if(CheckEndBlock("NodalData", word))
                break;

            ExtractValue(word, id);

            if(ReorderedNodeId(id) > NodesAllPartitions.size()) {
                std::stringstream buffer;
                buffer << "Invalid node id : " << id;
                buffer << " [Line " << mNumberOfLines << " ]";
                QUEST_ERROR << buffer.str() << std::endl;;
            }

            std::stringstream node_data;
            node_data << ReorderedNodeId(id) << '\n'; 

            for(SizeType i = 0 ; i < NodesAllPartitions[ReorderedNodeId(id)-1].size() ; i++) {
                SizeType partition_id = NodesAllPartitions[ReorderedNodeId(id)-1][i];
                if(partition_id > OutputFiles.size()) {
                    std::stringstream buffer;
                    buffer << "Invalid partition id : " << partition_id;
                    buffer << " for node " << id << " [Line " << mNumberOfLines << " ]";
                    QUEST_ERROR << buffer.str() << std::endl;;
                }

                *(OutputFiles[partition_id]) << node_data.str();
            }
        }

        QUEST_CATCH("")
    }

    /**
     * @brief 分割自由度变量的数据块
     */
    void ModelPartIO::DivideDofVariableData(
        OutputFilesContainerType& OutputFiles,
        const PartitionIndicesContainerType& NodesAllPartitions
    ){
        QUEST_TRY

        SizeType id;

        std::string word;

        while(!mpStream->eof())
        {
            ReadWord(word); 
            if(CheckEndBlock("NodalData", word))
                break;

            ExtractValue(word, id);

            if(ReorderedNodeId(id) > NodesAllPartitions.size())
            {
                std::stringstream buffer;
                buffer << "Invalid node id : " << id;
                buffer << " [Line " << mNumberOfLines << " ]";
                QUEST_ERROR << buffer.str() << std::endl;
            }

            std::stringstream node_data;
            node_data << ReorderedNodeId(id) << '\t'; 
            ReadWord(word);
            node_data << word << '\t'; 
            ReadWord(word);
            node_data << word << '\n'; 

            for(SizeType i = 0 ; i < NodesAllPartitions[ReorderedNodeId(id)-1].size() ; i++)
            {
                SizeType partition_id = NodesAllPartitions[ReorderedNodeId(id)-1][i];
                if(partition_id > OutputFiles.size())
                {
                    std::stringstream buffer;
                    buffer << "Invalid partition id : " << partition_id;
                    buffer << " for node " << id << " [Line " << mNumberOfLines << " ]";
                    QUEST_ERROR << buffer.str() << std::endl;
                }

                *(OutputFiles[partition_id]) << node_data.str();
            }
        }

        QUEST_CATCH("")
    }

    /**
     * @brief 分割矢量变量数据块
     */
    template<class TValueType>
    void ModelPartIO::DivideVectorialVariableData(
        OutputFilesContainerType& OutputFiles,
        const PartitionIndicesContainerType& EntitiesPartitions,
        std::string BlockName
    ){
        QUEST_TRY

        SizeType id;

        std::string word;

        bool is_fixed;

        std::string value;

        while(!mpStream->eof())
        {
            ReadWord(word); 
            if(CheckEndBlock(BlockName, word))
                break;

            ExtractValue(word, id);

            SizeType index = 0;
            if (BlockName == "NodalData"){
                index = ReorderedNodeId(id);
            } else if (BlockName == "ElementalData"){
                index = ReorderedElementId(id);
            } else if (BlockName == "ConditionalData"){
                index = ReorderedConditionId(id);
            } else{
                QUEST_ERROR << "Invalid block name :" << BlockName << std::endl;
            }

            if(index > EntitiesPartitions.size())
            {
                std::stringstream buffer;
                buffer << "Invalid id : " << id;
                buffer << " [Line " << mNumberOfLines << " ]";
                QUEST_ERROR << buffer.str() << std::endl;
            }

            std::stringstream entity_data;
            entity_data << index << '\t'; 

            if(BlockName == "NodalData")
            {
                ReadWord(value);
                ExtractValue(value, is_fixed);
                if(is_fixed)
                {
                    std::stringstream buffer;
                    buffer << "Only double variables or components can be fixed.";
                    buffer <<  " [Line " << mNumberOfLines << " ]";
                    QUEST_ERROR << buffer.str() << std::endl;
                }
                entity_data << is_fixed << "\t";
            }

            TValueType temp;
            ReadVectorialValue(temp);

            for(SizeType i = 0 ; i < EntitiesPartitions[index-1].size() ; i++)
            {
                SizeType partition_id = EntitiesPartitions[index-1][i];
                if(partition_id > OutputFiles.size())
                {
                    std::stringstream buffer;
                    buffer << "Invalid partition id : " << partition_id;
                    buffer << " for entity " << id << " [Line " << mNumberOfLines << " ]";
                    QUEST_ERROR << buffer.str() << std::endl;
                }

                *(OutputFiles[partition_id]) << entity_data.str() << temp << std::endl;
            }
        }


        QUEST_CATCH("")
    }


    /**
     * @brief 分割单元的数据块
     */
    void ModelPartIO::DivideElementalDataBlock(
        OutputFilesContainerType& OutputFiles,
        const PartitionIndicesContainerType& ElementsAllPartitions
    ){
        QUEST_TRY

        std::string word;

        WriteInAllFiles(OutputFiles, "Begin ElementalData ");

        std::string variable_name;

        ReadWord(variable_name);

        WriteInAllFiles(OutputFiles, variable_name);
        WriteInAllFiles(OutputFiles, "\n");

        if(QuestComponents<Variable<double> >::Has(variable_name)) {
            DivideScalarVariableData(OutputFiles, ElementsAllPartitions, "ElementalData");
        } else if(QuestComponents<Variable<bool> >::Has(variable_name)) {
            DivideScalarVariableData(OutputFiles, ElementsAllPartitions, "ElementalData");
        } else if(QuestComponents<Variable<int> >::Has(variable_name)) {
            DivideScalarVariableData(OutputFiles, ElementsAllPartitions, "ElementalData");
        } else if(QuestComponents<Variable<Array1d<double, 3> > >::Has(variable_name)) {
            DivideVectorialVariableData<Vector>(OutputFiles, ElementsAllPartitions, "ElementalData");
        } else if(QuestComponents<Variable<Quaternion<double> > >::Has(variable_name)) {
            DivideVectorialVariableData<Vector>(OutputFiles, ElementsAllPartitions, "ElementalData");
        } else if(QuestComponents<Variable<Vector> >::Has(variable_name)) {
            DivideVectorialVariableData<Vector>(OutputFiles, ElementsAllPartitions, "ElementalData");
        } else if(QuestComponents<Variable<Matrix> >::Has(variable_name)) {
            DivideVectorialVariableData<Matrix>(OutputFiles, ElementsAllPartitions, "ElementalData");
        } else if(QuestComponents<VariableData>::Has(variable_name)) {
            std::stringstream buffer;
            buffer << variable_name << " is not supported to be read by this IO or the type of variable is not registered correctly" << std::endl;
            buffer << " [Line " << mNumberOfLines << " ]";
            QUEST_ERROR << buffer.str() << std::endl;
        } else {
            std::stringstream buffer;
            buffer << variable_name << " is not a valid variable!!!" << std::endl;
            buffer << " [Line " << mNumberOfLines << " ]";
            QUEST_ERROR << buffer.str() << std::endl;
        }

        WriteInAllFiles(OutputFiles, "End ElementalData\n");

        QUEST_CATCH("")
    }

    /**
     * @brief 分割单元的标量变量数据块
     */
    void ModelPartIO::DivideScalarVariableData(
        OutputFilesContainerType& OutputFiles,
        const PartitionIndicesContainerType& EntitiesPartitions,
        std::string BlockName
    ){
        QUEST_TRY

        SizeType id;

        std::string word;


        while(!mpStream->eof())
        {
            ReadWord(word); // reading id
            if(CheckEndBlock(BlockName, word))
                break;

            ExtractValue(word, id);
            SizeType index = 0;
            if(BlockName == "ElementalData")
                index = ReorderedElementId(id);
            else if(BlockName == "ConditionalData")
                index = ReorderedConditionId(id);
            else
                QUEST_ERROR << "Invalid block name :" << BlockName << std::endl;

            if(index > EntitiesPartitions.size())
            {
                std::stringstream buffer;
                buffer << "Invalid id : " << id;
                buffer << " [Line " << mNumberOfLines << " ]";
                QUEST_ERROR << buffer.str() << std::endl;
            }

            std::stringstream entity_data;
            entity_data << index << '\t'; // id
            ReadWord(word);
            entity_data << word <<'\n'; // value

            for(SizeType i = 0 ; i < EntitiesPartitions[index-1].size() ; i++)
            {
                SizeType partition_id = EntitiesPartitions[index-1][i];
                if(partition_id > OutputFiles.size())
                {
                    std::stringstream buffer;
                    buffer << "Invalid partition id : " << partition_id;
                    buffer << " for entity " << id << " [Line " << mNumberOfLines << " ]";
                    QUEST_ERROR << buffer.str() << std::endl;
                }

                *(OutputFiles[partition_id]) << entity_data.str();
            }
        }


        QUEST_CATCH("")
    }

    /**
     * @brief 分割条件的数据块
     */
    void ModelPartIO::DivideConditionalDataBlock(
        OutputFilesContainerType& OutputFiles,
        const PartitionIndicesContainerType& ConditionsAllPartitions
    ){
        QUEST_TRY

        std::string word, variable_name;

        WriteInAllFiles(OutputFiles, "Begin ConditionalData ");

        ReadWord(variable_name);

        WriteInAllFiles(OutputFiles, variable_name);
        WriteInAllFiles(OutputFiles, "\n");

        if(QuestComponents<Variable<double> >::Has(variable_name)) {
            DivideScalarVariableData(OutputFiles, ConditionsAllPartitions, "ConditionalData");
        } else if(QuestComponents<Variable<bool> >::Has(variable_name)) {
            DivideScalarVariableData(OutputFiles, ConditionsAllPartitions, "ConditionalData");
        } else if(QuestComponents<Variable<int> >::Has(variable_name)) {
            DivideScalarVariableData(OutputFiles, ConditionsAllPartitions, "ConditionalData");
        } else if(QuestComponents<Variable<Array1d<double, 3> > >::Has(variable_name)) {
            DivideVectorialVariableData<Vector>(OutputFiles, ConditionsAllPartitions, "ConditionalData");
        } else if(QuestComponents<Variable<Quaternion<double> > >::Has(variable_name)) {
            DivideVectorialVariableData<Vector>(OutputFiles, ConditionsAllPartitions, "ConditionalData");
        } else if(QuestComponents<Variable<Vector> >::Has(variable_name)) {
            DivideVectorialVariableData<Vector>(OutputFiles, ConditionsAllPartitions, "ConditionalData");
        } else if(QuestComponents<Variable<Matrix> >::Has(variable_name)) {
            DivideVectorialVariableData<Matrix>(OutputFiles, ConditionsAllPartitions, "ConditionalData");
        } else if(QuestComponents<VariableData>::Has(variable_name)) {
            std::stringstream buffer;
            buffer << variable_name << " is not supported to be read by this IO or the type of variable is not registered correctly" << std::endl;
            buffer << " [Line " << mNumberOfLines << " ]";
            QUEST_ERROR << buffer.str() << std::endl;
        } else {
            std::stringstream buffer;
            buffer << variable_name << " is not a valid variable!!!" << std::endl;
            buffer << " [Line " << mNumberOfLines << " ]";
            QUEST_ERROR << buffer.str() << std::endl;
        }

        WriteInAllFiles(OutputFiles, "End ConditionalData\n");

        QUEST_CATCH("")
    }

    /**
     * @brief 分割网格块
     */
    void ModelPartIO::DivideMeshBlock(
        OutputFilesContainerType& OutputFiles,
        const PartitionIndicesContainerType& NodesAllPartitions,
        const PartitionIndicesContainerType& ElementsAllPartitions,
        const PartitionIndicesContainerType& ConditionsAllPartitions
    ){
        QUEST_TRY

        std::string word;
        ReadWord(word);

        word += "\n";


        WriteInAllFiles(OutputFiles, "Begin Mesh " + word);

        while(!mpStream->eof())
        {
            ReadWord(word);

            if(CheckEndBlock("Mesh", word))
                break;

            ReadBlockName(word);
            if(word == "MeshData")
                DivideMeshDataBlock(OutputFiles);
            else if(word == "MeshNodes")
                DivideMeshNodesBlock(OutputFiles, NodesAllPartitions);
            else if(word == "MeshElements")
                DivideMeshElementsBlock(OutputFiles, ElementsAllPartitions);
            else if(word == "MeshConditions")
                DivideMeshConditionsBlock(OutputFiles, ConditionsAllPartitions);
            else
                SkipBlock(word);
        }

        WriteInAllFiles(OutputFiles, "End Mesh\n");

        QUEST_CATCH("")
    }

    /**
     * @brief 分割子模型部件块
     */
    void ModelPartIO::DivideSubModelPartBlock(
        OutputFilesContainerType& OutputFiles,
        const PartitionIndicesContainerType& NodesAllPartitions,
        const PartitionIndicesContainerType& ElementsAllPartitions,
        const PartitionIndicesContainerType& ConditionsAllPartitions
    ){
        QUEST_TRY

        std::string word;
        ReadWord(word);

        word += "\n";


        WriteInAllFiles(OutputFiles, "Begin SubModelPart " + word);

        while (!mpStream->eof())
        {
            ReadWord(word);

            if (CheckEndBlock("SubModelPart", word))
                break;

            ReadBlockName(word);
            if (word == "SubModelPartData")
                DivideSubModelPartDataBlock(OutputFiles);
            else if (word == "SubModelPartTables")
                DivideSubModelPartTableBlock(OutputFiles);
            else if (word == "SubModelPartNodes")
                DivideSubModelPartNodesBlock(OutputFiles, NodesAllPartitions);
            else if (word == "SubModelPartElements")
                DivideSubModelPartElementsBlock(OutputFiles, ElementsAllPartitions);
            else if (word == "SubModelPartConditions")
                DivideSubModelPartConditionsBlock(OutputFiles, ConditionsAllPartitions);
            else if (word == "SubModelPart")
                DivideSubModelPartBlock(OutputFiles, NodesAllPartitions, ElementsAllPartitions, ConditionsAllPartitions);
            else
                SkipBlock(word);
        }

        WriteInAllFiles(OutputFiles, "End SubModelPart\n");

        QUEST_CATCH("")
    }

    /**
     * @brief 分割网格数据块
     */
    void ModelPartIO::DivideMeshDataBlock(OutputFilesContainerType& OutputFiles){
        QUEST_TRY
        std::string block;

        WriteInAllFiles(OutputFiles, "Begin MeshData");

        ReadBlock(block, "MeshData");
        WriteInAllFiles(OutputFiles, block);

        WriteInAllFiles(OutputFiles, "End MeshData\n");
        QUEST_CATCH("")
    }

    /**
     * @brief 分割网格节点块
     */
    void ModelPartIO::DivideMeshNodesBlock(
        OutputFilesContainerType& OutputFiles,
        const PartitionIndicesContainerType& NodesAllPartitions
    ){
        QUEST_TRY

        std::string word;

        WriteInAllFiles(OutputFiles, "Begin MeshNodes \n");

        SizeType id;

        while(!mpStream->eof())
        {
            ReadWord(word);

            if(CheckEndBlock("MeshNodes", word))
                break;

            ExtractValue(word, id);

            if(ReorderedNodeId(id) > NodesAllPartitions.size())
            {
                std::stringstream buffer;
                buffer << "Invalid node id : " << id;
                buffer << " [Line " << mNumberOfLines << " ]";
                QUEST_ERROR << buffer.str() << std::endl;
            }

            for(SizeType i = 0 ; i < NodesAllPartitions[ReorderedNodeId(id)-1].size() ; i++)
            {
                SizeType partition_id = NodesAllPartitions[ReorderedNodeId(id)-1][i];
                if(partition_id > OutputFiles.size())
                {
                    std::stringstream buffer;
                    buffer << "Invalid partition id : " << partition_id;
                    buffer << " for node " << id << " [Line " << mNumberOfLines << " ]";
                    QUEST_ERROR << buffer.str() << std::endl;
                }

                *(OutputFiles[partition_id]) << ReorderedNodeId(id) << std::endl;
            }

        }

        WriteInAllFiles(OutputFiles, "End MeshNodes\n");

        QUEST_CATCH("")
    }

    /**
     * @brief 分割网格单元块
     */
    void ModelPartIO::DivideMeshElementsBlock(
        OutputFilesContainerType& OutputFiles,
        const PartitionIndicesContainerType& ElementsAllPartitions
    ){
        QUEST_TRY

        std::string word;

        WriteInAllFiles(OutputFiles, "Begin MeshElements \n");

        SizeType id;

        while(!mpStream->eof())
        {
            ReadWord(word);

            if(CheckEndBlock("MeshElements", word))
                break;

            ExtractValue(word, id);

            if(ReorderedElementId(id) > ElementsAllPartitions.size())
            {
                std::stringstream buffer;
                buffer << "Invalid element id : " << id;
                buffer << " [Line " << mNumberOfLines << " ]";
                QUEST_ERROR << buffer.str() << std::endl;
            }

            for(SizeType i = 0 ; i < ElementsAllPartitions[ReorderedElementId(id)-1].size() ; i++)
            {
                SizeType partition_id = ElementsAllPartitions[ReorderedElementId(id)-1][i];
                if(partition_id > OutputFiles.size())
                {
                    std::stringstream buffer;
                    buffer << "Invalid partition id : " << partition_id;
                    buffer << " for element " << id << " [Line " << mNumberOfLines << " ]";
                    QUEST_ERROR << buffer.str() << std::endl;
                }

                *(OutputFiles[partition_id]) << ReorderedElementId(id) << std::endl;
            }

        }

        WriteInAllFiles(OutputFiles, "End MeshElements\n");

        QUEST_CATCH("")
    }

    /**
     * @brief 分割网格条件块
     */
    void ModelPartIO::DivideMeshConditionsBlock(
        OutputFilesContainerType& OutputFiles,
        const PartitionIndicesContainerType& ConditionsAllPartitions
    ){
        QUEST_TRY

        std::string word;

        WriteInAllFiles(OutputFiles, "Begin MeshConditions \n");

        SizeType id;

        while(!mpStream->eof())
        {
            ReadWord(word);

            if(CheckEndBlock("MeshConditions", word))
                break;

            ExtractValue(word, id);

            if(ReorderedConditionId(id) > ConditionsAllPartitions.size())
            {
                std::stringstream buffer;
                buffer << "Invalid condition id : " << id;
                buffer << " [Line " << mNumberOfLines << " ]";
                QUEST_ERROR << buffer.str() << std::endl;
            }

            for(SizeType i = 0 ; i < ConditionsAllPartitions[ReorderedConditionId(id)-1].size() ; i++)
            {
                SizeType partition_id = ConditionsAllPartitions[ReorderedConditionId(id)-1][i];
                if(partition_id > OutputFiles.size())
                {
                    std::stringstream buffer;
                    buffer << "Invalid partition id : " << partition_id;
                    buffer << " for condition " << id << " [Line " << mNumberOfLines << " ]";
                    QUEST_ERROR << buffer.str() << std::endl;
                }

                *(OutputFiles[partition_id]) << ReorderedConditionId(id) << std::endl;
            }

        }

        WriteInAllFiles(OutputFiles, "End MeshConditions\n");

        QUEST_CATCH("")
    }


    /**
     * @brief 分割子模型部件的数据块
     */
    void ModelPartIO::DivideSubModelPartDataBlock(OutputFilesContainerType& OutputFiles){
        QUEST_TRY
        std::string block;

        WriteInAllFiles(OutputFiles, "Begin SubModelPartData");

        ReadBlock(block, "SubModelPartData");
        WriteInAllFiles(OutputFiles, block);

        WriteInAllFiles(OutputFiles, "End SubModelPartData\n");
        QUEST_CATCH("")
    }

    /**
     * @brief 分割子模型部件的表格块
     */
    void ModelPartIO::DivideSubModelPartTableBlock(OutputFilesContainerType& OutputFiles){
        QUEST_TRY
        std::string block;

        WriteInAllFiles(OutputFiles, "Begin SubModelPartTables");

        ReadBlock(block, "SubModelPartTables");
        WriteInAllFiles(OutputFiles, block);

        WriteInAllFiles(OutputFiles, "End SubModelPartTables\n");
        QUEST_CATCH("")
    }

    /**
     * @brief 分割子模型部件的节点块
     */
    void ModelPartIO::DivideSubModelPartNodesBlock(
        OutputFilesContainerType& OutputFiles,
        const PartitionIndicesContainerType& NodesAllPartitions
    ){
        QUEST_TRY

        std::string word;

        WriteInAllFiles(OutputFiles, "Begin SubModelPartNodes \n");

        SizeType id;

        while (!mpStream->eof())
        {
            ReadWord(word);

            if (CheckEndBlock("SubModelPartNodes", word))
                break;

            ExtractValue(word, id);

            if (ReorderedNodeId(id) > NodesAllPartitions.size())
            {
                std::stringstream buffer;
                buffer << "Invalid node id : " << id;
                buffer << " [Line " << mNumberOfLines << " ]";
                QUEST_ERROR << buffer.str() << std::endl;
            }

            for (SizeType i = 0; i < NodesAllPartitions[ReorderedNodeId(id) - 1].size(); i++)
            {
                SizeType partition_id = NodesAllPartitions[ReorderedNodeId(id) - 1][i];
                if (partition_id > OutputFiles.size())
                {
                    std::stringstream buffer;
                    buffer << "Invalid partition id : " << partition_id;
                    buffer << " for node " << id << " [Line " << mNumberOfLines << " ]";
                    QUEST_ERROR << buffer.str() << std::endl;
                }

                *(OutputFiles[partition_id]) << ReorderedNodeId(id) << std::endl;
            }

        }

        WriteInAllFiles(OutputFiles, "End SubModelPartNodes\n");

        QUEST_CATCH("")
    }

    /**
     * @brief 分割子模型部件的单元块
     */
    void ModelPartIO::DivideSubModelPartElementsBlock(
        OutputFilesContainerType& OutputFiles,
        const PartitionIndicesContainerType& ElementsAllPartitions
    ){
        QUEST_TRY

        std::string word;

        WriteInAllFiles(OutputFiles, "Begin SubModelPartElements \n");

        SizeType id;

        while (!mpStream->eof())
        {
            ReadWord(word);

            if (CheckEndBlock("SubModelPartElements", word))
                break;

            ExtractValue(word, id);

            if (ReorderedElementId(id) > ElementsAllPartitions.size())
            {
                std::stringstream buffer;
                buffer << "Invalid element id : " << id;
                buffer << " [Line " << mNumberOfLines << " ]";
                QUEST_ERROR << buffer.str() << std::endl;
            }

            for (SizeType i = 0; i < ElementsAllPartitions[ReorderedElementId(id) - 1].size(); i++)
            {
                SizeType partition_id = ElementsAllPartitions[ReorderedElementId(id) - 1][i];
                if (partition_id > OutputFiles.size())
                {
                    std::stringstream buffer;
                    buffer << "Invalid partition id : " << partition_id;
                    buffer << " for element " << id << " [Line " << mNumberOfLines << " ]";
                    QUEST_ERROR << buffer.str() << std::endl;
                }

                *(OutputFiles[partition_id]) << ReorderedElementId(id) << std::endl;
            }

        }

        WriteInAllFiles(OutputFiles, "End SubModelPartElements\n");

        QUEST_CATCH("")
    }

    /**
     * @brief 分割子模型部件的条件块
     */
    void ModelPartIO::DivideSubModelPartConditionsBlock(
        OutputFilesContainerType& OutputFiles,
        const PartitionIndicesContainerType& ConditionsAllPartitions
    ){
        QUEST_TRY

        std::string word;

        WriteInAllFiles(OutputFiles, "Begin SubModelPartConditions \n");

        SizeType id;

        while (!mpStream->eof())
        {
            ReadWord(word);

            if (CheckEndBlock("SubModelPartConditions", word))
                break;

            ExtractValue(word, id);

            if (ReorderedConditionId(id) > ConditionsAllPartitions.size())
            {
                std::stringstream buffer;
                buffer << "Invalid condition id : " << id;
                buffer << " [Line " << mNumberOfLines << " ]";
                QUEST_ERROR << buffer.str() << std::endl;
            }

            for (SizeType i = 0; i < ConditionsAllPartitions[ReorderedConditionId(id) - 1].size(); i++)
            {
                SizeType partition_id = ConditionsAllPartitions[ReorderedConditionId(id) - 1][i];
                if (partition_id > OutputFiles.size())
                {
                    std::stringstream buffer;
                    buffer << "Invalid partition id : " << partition_id;
                    buffer << " for condition " << id << " [Line " << mNumberOfLines << " ]";
                    QUEST_ERROR << buffer.str() << std::endl;
                }

                *(OutputFiles[partition_id]) << ReorderedConditionId(id) << std::endl;
            }

        }

        WriteInAllFiles(OutputFiles, "End SubModelPartConditions\n");

        QUEST_CATCH("")
    }

    /**
     * @brief 写入分区信息
     */
    void ModelPartIO::WritePartitionIndices(OutputFilesContainerType& OutputFiles, PartitionIndicesType const&  NodesPartitions, PartitionIndicesContainerType const& NodesAllPartitions){
        WriteInAllFiles(OutputFiles, "Begin NodalData PARTITION_INDEX\n");

        for(SizeType i_node = 0 ; i_node != NodesAllPartitions.size() ; i_node++)
        {
            for(SizeType i = 0 ; i < NodesAllPartitions[i_node].size() ; i++)
            {
                SizeType partition_id = NodesAllPartitions[i_node][i];
                if(partition_id > OutputFiles.size())
                {
                    std::stringstream buffer;
                    buffer << "Invalid partition id : " << partition_id;
                    buffer << " for node " << i_node+1 << " [Line " << mNumberOfLines << " ]";
                    QUEST_ERROR << buffer.str() << std::endl;
                }

                const SizeType node_partition = NodesPartitions[i_node];
                *(OutputFiles[partition_id]) << i_node + 1 << "  0  " << node_partition << std::endl;
            }
        }

        WriteInAllFiles(OutputFiles, "End NodalData \n");
    }

    /**
     * @brief 写入通信器的相关数据
     */
    void ModelPartIO::WriteCommunicatorData(
        OutputFilesContainerType& OutputFiles, 
        SizeType NumberOfPartitions, 
        const GraphType& DomainsColoredGraph,
        const PartitionIndicesType& NodesPartitions,
        const PartitionIndicesType& ElementsPartitions,
        const PartitionIndicesType& ConditionsPartitions,
        const PartitionIndicesContainerType& NodesAllPartitions,
        const PartitionIndicesContainerType& ElementsAllPartitions,
        const PartitionIndicesContainerType& ConditionsAllPartitions
    ){
        WriteInAllFiles(OutputFiles, "Begin CommunicatorData \n");

        WriteInAllFiles(OutputFiles, "NEIGHBOURS_INDICES    ");
        for(SizeType i_partition = 0 ; i_partition < NumberOfPartitions ; i_partition++)
        {
            DenseVector<int> indices = row(DomainsColoredGraph, i_partition);
            *(OutputFiles[i_partition]) << indices << std::endl;
        }

        SizeType number_of_colors = 0;

        for(SizeType i_partition = 0 ; i_partition < DomainsColoredGraph.size1() ; i_partition++)
            for(SizeType i_interface = 0 ; i_interface < DomainsColoredGraph.size2() ; i_interface++)
                if(DomainsColoredGraph(i_partition, i_interface) >= 0)
                    if(number_of_colors < i_interface)
                        number_of_colors = i_interface;

        number_of_colors++; 

        for(SizeType i_partition = 0 ; i_partition < NumberOfPartitions ; i_partition++)
        {
            DenseVector<int> indices = row(DomainsColoredGraph, i_partition);
            *(OutputFiles[i_partition]) << "NUMBER_OF_COLORS    " << number_of_colors << std::endl;
        }

        WriteInAllFiles(OutputFiles, "    Begin LocalNodes 0\n");

        for(SizeType i = 0 ; i < NodesPartitions.size() ; i++)
            *(OutputFiles[NodesPartitions[i]]) << "    " << i+1 << std::endl;

        WriteInAllFiles(OutputFiles, "    End LocalNodes \n");

        std::vector<PartitionIndicesContainerType> local_nodes_indices(NumberOfPartitions, PartitionIndicesContainerType(number_of_colors));
        std::vector<PartitionIndicesContainerType> ghost_nodes_indices(NumberOfPartitions, PartitionIndicesContainerType(number_of_colors));

        DenseMatrix<int> interface_indices = scalar_matrix<int>(NumberOfPartitions, NumberOfPartitions, -1);

        for(SizeType i_partition = 0 ; i_partition < NumberOfPartitions ; i_partition++)
        {
            DenseVector<int> neighbours_indices = row(DomainsColoredGraph, i_partition);

            for(SizeType i = 0 ; i <  neighbours_indices.size() ; i++)
                if(SizeType(neighbours_indices[i]) < NumberOfPartitions)
                    interface_indices(i_partition,neighbours_indices[i]) = i;
        }

        for(SizeType i = 0 ; i < NodesPartitions.size() ; i++)
        {
            const SizeType node_partition = NodesPartitions[i];
            const SizeType node_id = i + 1;

            PartitionIndicesType const& node_all_partitions = NodesAllPartitions[i];

            for(SizeType j = 0 ; j < node_all_partitions.size() ; j++)
            {
                SizeType i_node_partition = node_all_partitions[j];
                if(node_partition != i_node_partition)
                {
                    SizeType local_interface_index = interface_indices(node_partition, i_node_partition);
                    SizeType ghost_interface_index = interface_indices(i_node_partition, node_partition);
                    local_nodes_indices[node_partition][local_interface_index].push_back(node_id);
                    ghost_nodes_indices[i_node_partition][ghost_interface_index].push_back(node_id);
                }
            }
        }

        for(SizeType i_partition = 0 ; i_partition < NumberOfPartitions ; i_partition++)
        {
            PartitionIndicesContainerType& partition_local_nodes_indices = local_nodes_indices[i_partition];

            for(SizeType i_interface = 0 ; i_interface < partition_local_nodes_indices.size() ; i_interface++)
            {
                if(partition_local_nodes_indices[i_interface].size() > 0)
                {
                    *(OutputFiles[i_partition]) << "    Begin LocalNodes " << i_interface + 1 << std::endl;
                    for(SizeType i_interface_node = 0 ; i_interface_node < partition_local_nodes_indices[i_interface].size() ; i_interface_node++)
                        *(OutputFiles[i_partition]) << "    " << partition_local_nodes_indices[i_interface][i_interface_node] << std::endl;
                    *(OutputFiles[i_partition]) << "    End LocalNodes " << std::endl;
                }
            }

            PartitionIndicesContainerType& partition_ghost_nodes_indices = ghost_nodes_indices[i_partition];

            std::set<unsigned int> all_ghost_nodes_indices;

            for(SizeType i_interface = 0 ; i_interface < partition_ghost_nodes_indices.size() ; i_interface++)
            {
                if(partition_ghost_nodes_indices[i_interface].size() > 0)
                {
                    *(OutputFiles[i_partition]) << "    Begin GhostNodes " << i_interface + 1 << std::endl;
                    for(SizeType i_interface_node = 0 ; i_interface_node < partition_ghost_nodes_indices[i_interface].size() ; i_interface_node++)
                    {
                        *(OutputFiles[i_partition]) << "    " << partition_ghost_nodes_indices[i_interface][i_interface_node] << std::endl;
                        all_ghost_nodes_indices.insert(partition_ghost_nodes_indices[i_interface][i_interface_node]);
                    }
                    *(OutputFiles[i_partition]) << "    End GhostNodes "  << std::endl;
                }
            }

            *(OutputFiles[i_partition]) << "    Begin GhostNodes " << 0 << std::endl;
            for(std::set<unsigned int>::iterator id = all_ghost_nodes_indices.begin() ; id != all_ghost_nodes_indices.end() ; id++)
            {
                *(OutputFiles[i_partition]) << "    " << *id << std::endl;
            }
            *(OutputFiles[i_partition]) << "    End GhostNodes "  << std::endl;
        }

        WriteInAllFiles(OutputFiles, "End CommunicatorData \n");
    }

    /**
     * @brief 写入通信器的本地节点信息
     */
    void ModelPartIO::WriteCommunicatorLocalNodes(OutputFilesContainerType& OutputFiles, SizeType NumberOfPartitions, PartitionIndicesType const& NodesPartitions, PartitionIndicesContainerType const& NodesAllPartitions){
        WriteInAllFiles(OutputFiles, "    Begin LocalNodes 0\n");

        for(SizeType i = 0 ; i < NodesPartitions.size() ; i++)
            *(OutputFiles[NodesPartitions[i]]) << "    " << i+1 << std::endl;

        WriteInAllFiles(OutputFiles, "    End LocalNodes \n");

        PartitionIndicesContainerType local_nodes_indices(NumberOfPartitions);
    }

    /**
     * @brief 将某个字符串写入到所有输出文件中
     */
    void ModelPartIO::WriteInAllFiles(OutputFilesContainerType& OutputFiles, std::string const& ThisWord){
        for(SizeType i = 0 ; i < OutputFiles.size() ; i++)
            *(OutputFiles[i]) << ThisWord;
    }

    /**
     * @brief 在容器中查找指定的键
     */
    template<class TContainerType, class TKeyType>
    typename TContainerType::iterator ModelPartIO::FindKey(TContainerType& ThisContainer , TKeyType ThisKey, std::string ComponentName){
        typename TContainerType::iterator i_result;
        if((i_result = ThisContainer.find(ThisKey)) == ThisContainer.end())
        {
            std::stringstream buffer;
            buffer << ComponentName << " #" << ThisKey << " is not found.";
            buffer << " [Line " << mNumberOfLines << " ]";
            QUEST_ERROR << buffer.str() << std::endl;
        }

        return i_result;
    }

    /**
     * @brief 读取矢量值
     */
    template<class TValueType>
    TValueType& ModelPartIO::ReadVectorialValue(TValueType& rValue){
        std::stringstream value;

        char c = SkipWhiteSpaces();
        while((c != '(') && !mpStream->eof())
        {
            value << c;
            c = GetCharacter();
        }
        int open_parantesis = 1;
        while((open_parantesis != 0) && !mpStream->eof())
        {
            value << c;
            c = GetCharacter();
            if(c == '(')
                open_parantesis++;
            if(c == ')')
                open_parantesis--;
        }
        value << c; 

        value >>  rValue;

        return rValue;
    }

    /**
     * @brief 从输入的字符串（rWord）中提取值并存储到 rValue 中
     */
    template<class TValueType>
    TValueType& ModelPartIO::ExtractValue(std::string rWord, TValueType & rValue){
        std::stringstream value(rWord);

        value >> rValue;

        return rValue;
    }

    /**
     * @brief 从输入的字符串 rWord 中提取布尔值，并存储到 rValue 中
     */
    bool& ModelPartIO::ExtractValue(std::string rWord, bool & rValue){
        if (rWord == "1" || rWord == "true" || rWord == "True") {
            rValue = true;
        } else if (rWord == "0" || rWord == "false" || rWord == "False") {
            rValue = false;
        } else {
            QUEST_ERROR << "Boolean argument could not be determined: " << rWord << std::endl;
        }

        return rValue;
    }

    /**
     * @brief 读取材料本构定律的值
     */
    void ModelPartIO::ReadConstitutiveLawValue(ConstitutiveLaw::Pointer& rValue){
        std::string value;
        ReadWord(value);
        rValue = QuestComponents<ConstitutiveLaw>::Get(value).Clone();
    }

    /**
     * @brief 读取一个单词并存储到 Word 中
     */
    ModelPartIO& ModelPartIO::ReadWord(std::string& Word){
        Word.clear();

        char c = SkipWhiteSpaces();
        while(!mpStream->eof() && !std::isspace(c))
        {
            Word += c;
            c = GetCharacter();
        }

        return *this;
    }

    /**
     * @brief 读取一个数据块，并存储到 Block 中
     */
    ModelPartIO& ModelPartIO::ReadBlock(std::string& Block, std::string const& BlockName){
        Block.clear();
        std::vector<std::string> nested_block_names;
        nested_block_names.push_back(BlockName);

        char c = GetCharacter();
        std::string word;

        while(!mpStream->eof())
        {
            if(c == 'E')
            {
                word.clear();
                while(!mpStream->eof() && !std::isspace(c))
                {
                    word += c;
                    c = GetCharacter();
                }
                if (CheckEndBlock(nested_block_names.back(), word))
                {
                    nested_block_names.pop_back();
                    if(nested_block_names.empty())
                    {
                        break;
                    }
                    else
                    {
                        Block += "End ";
                    }
                }

                Block += word;
            }
            else if (c == 'B')
            {
                word.clear();
                while (!mpStream->eof() && !std::isspace(c))
                {
                    word += c;
                    c = GetCharacter();
                }
                if (word == "Begin")
                {
                    Block += word;
                    Block += c;
                    ReadWord(word);
                    nested_block_names.push_back(word);
                }

                Block += word;
            }

            Block += c;

            c = GetCharacter();
        }

        return *this;
    }

    /**
     * @brief 跳过输入流中的空白字符（如空格、制表符、换行符等）
     */
    char ModelPartIO::SkipWhiteSpaces(){
        char c = GetCharacter();
        while(std::isspace(c))
            c = GetCharacter();
        return c;
    }

    /**
     * @brief 获取输入流中的下一个字符
     */
    char ModelPartIO::GetCharacter(){
        char c;
        if(mpStream->get(c))
        {
            if(c == '\n')
                mNumberOfLines++;
            else if(c == '/') 
            {
                char next_c = mpStream->peek();
                if(next_c == '/') 
                {
                    while((mpStream->get(c)) && (c != '\n')); 
                    if(!mpStream->eof())
                        mNumberOfLines++;
                }
                else if(next_c == '*') 
                {
                    while((mpStream->get(c)) && !((c == '*') && (mpStream->peek() == '/'))) 
                        if(c == '\n')
                            mNumberOfLines++;
                    mpStream->get(c);
                    c = GetCharacter(); 
                }
            }
        }
        else
            c = 0;

        return c;
    }

    /**
     * @brief 检查语句是否匹配给定的单词
     */
    void ModelPartIO::CheckStatement(std::string const& rStatement, std::string const& rGivenWord) const{
        QUEST_ERROR_IF(rGivenWord != rStatement )<< "A \"" << rStatement << "\" statement was expected but the given statement was \"" << rGivenWord << "\"" << " [Line " << mNumberOfLines << " ]" << std::endl;
    }

    /**
     * @brief 重置输入流
     */
    void ModelPartIO::ResetInput(){
        mpStream->clear();
        mpStream->seekg(0, std::ios_base::beg);
        mNumberOfLines = 1;
    }

    /**
     * @brief 创建分区
     */
    inline void ModelPartIO::CreatePartition(unsigned int NumberOfThreads,const int NumberOfRows, DenseVector<unsigned int>& partitions){
        partitions.resize(NumberOfThreads+1);
        int partition_size = number_of_rows / NumberOfThreads;
        partitions[0] = 0;
        partitions[NumberOfThreads] = number_of_rows;
        for(unsigned int i = 1; i<NumberOfThreads; i++)
            partitions[i] = partitions[i-1] + partition_size ;
    }

    /**
     * @brief 扫描节点块并对每个节点调用 ReorderedNodeId
     */
    void ModelPartIO::ScanNodeBlock(){
        std::string word;
        SizeType node_id;
        while(!mpStream->eof())
        {
            ReadWord(word);
            if(CheckEndBlock("Nodes", word))
                break;

            ExtractValue(word, node_id);
            ReorderedNodeId(node_id);

            ReadWord(word);
            ReadWord(word); 
            ReadWord(word); 
        }
    }


    /**
     * @brief 重排节点ID
     */
    virtual ModelPartIO::SizeType ReorderedNodeId(ModelPartIO::SizeType NodeId){
        return NodeId;
    }

    /**
     * @brief 重排几何体ID
     */
    virtual ModelPartIO::SizeType ReorderedGeometryId(ModelPartIO::SizeType GeometryId){
        return GeometryId;
    }

    /**
     * @brief 重排单元ID
     */
    virtual ModelPartIO::SizeType ReorderedElementId(ModelPartIO::SizeType ElementId){
        return ElementId;
    }

    /**
     * @brief 重排条件ID
     */
    virtual ModelPartIO::SizeType ReorderedConditionId(ModelPartIO::SizeType ConditionId){
        return ConditionId;
    }

}