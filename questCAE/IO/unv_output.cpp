// 系统头文件
#include <iostream>
#include <fstream>

// 项目头文件
#include "includes/model_part.hpp"
#include "includes/exceptions.hpp"
#include "IO/unv_output.hpp"

namespace Quest{

    /**
     * @brief 构造函数
     */
    UnvOutput::UnvOutput(Quest::ModelPart &modelPart, const std::string &outFileWithoutExtension)
    : mrOutputModelPart(modelPart),
      mOutputFileName(outFileWithoutExtension + ".unv") {}


    /**
     * @brief 初始化输出文件
     */
    void InitializeOutputFile() {
        std::ofstream outputFile;
        outputFile.open(mOutputFileName, std::ios::out | std::ios::trunc);
        outputFile.close();
    }


    /**
     * @brief 写入模型部件关联的网格数据
     */
    void WriteMesh() {
        WriteNodes();
        WriteElements();
    }


    /**
     * @brief 写入模型部件关联的节点数据
     */
    void WriteNodes(){
        std::ofstream outputFile;
        outputFile.open(mOutputFileName, std::ios::out | std::ios::app);

        outputFile << std::scientific;
        outputFile << std::setprecision(15);

        const int exportCoordinateSystemNumber = 0;
        const int displacementCoordinateSystemNumber = 0;
        const int color = 0;

        outputFile << std::setw(6) << "-1" << "\n";
        outputFile << std::setw(6) << as_integer(DatasetID::NODES_DATASET) << "\n";

        for (auto &node_i : mrOutputModelPart.Nodes()) {
            int node_label = node_i.Id();
            double x_coordinate = node_i.X();
            double y_coordinate = node_i.Y();
            double z_coordinate = node_i.Z();
            outputFile << std::setw(10) << node_label << std::setw(10) << exportCoordinateSystemNumber
                        << std::setw(10)
                        << displacementCoordinateSystemNumber << std::setw(10) << color << "\n";
            outputFile << std::setw(25) << x_coordinate << std::setw(25) << y_coordinate << std::setw(25)
                        << z_coordinate << "\n";
        }
        outputFile << std::setw(6) << "-1" << "\n";

        outputFile.close();
    }


    /**
     * @brief 写入模型部件关联的单元数据
     */
    void WriteElements(){
        std::ofstream outputFile;
        outputFile.open(mOutputFileName, std::ios::out | std::ios::app);

        const int physicalPropertyTableNumber = 1;
        const int materialPropertyTableNumber = 1;
        const int color = 0;

        outputFile << std::setw(6) << "-1" << "\n";
        outputFile << std::setw(6) << as_integer(DatasetID::ELEMENTS_DATASET) << "\n";

        for (auto &element : mrOutputModelPart.Elements()) {
            const int elementLabel = element.Id();
            Quest::ModelPart::ConditionType::GeometryType elementGeometry = element.GetGeometry();
            // Write triangles
            if (elementGeometry.size() == 3 && elementGeometry.WorkingSpaceDimension() == 2) {
                const int feDescriptorId = 41; // Plane Stress Linear Triangle
                const int numberOfNodes = 3;
                outputFile << std::setw(10) << elementLabel;
                outputFile << std::setw(10) << feDescriptorId;
                outputFile << std::setw(10) << physicalPropertyTableNumber;
                outputFile << std::setw(10) << materialPropertyTableNumber;
                outputFile << std::setw(10) << color;
                outputFile << std::setw(10) << numberOfNodes << "\n";
                outputFile << std::setw(10) << elementGeometry[0].Id();
                outputFile << std::setw(10) << elementGeometry[1].Id();
                outputFile << std::setw(10) << elementGeometry[2].Id() << "\n";
            }
                // Write tetrahedras
            else if (elementGeometry.size() == 4 && elementGeometry.WorkingSpaceDimension() == 3) {
                const int feDescriptorId = 111; // Solid linear tetrahedron
                const int numberOfNodes = 4;
                outputFile << std::setw(10) << elementLabel;
                outputFile << std::setw(10) << feDescriptorId;
                outputFile << std::setw(10) << physicalPropertyTableNumber;
                outputFile << std::setw(10) << materialPropertyTableNumber;
                outputFile << std::setw(10) << color;
                outputFile << std::setw(10) << numberOfNodes << "\n";
                outputFile << std::setw(10) << elementGeometry[0].Id();
                outputFile << std::setw(10) << elementGeometry[1].Id();
                outputFile << std::setw(10) << elementGeometry[2].Id();
                outputFile << std::setw(10) << elementGeometry[3].Id() << "\n";
            }
        }
        outputFile << std::setw(6) << "-1" << "\n";
        outputFile.close();
    }


    /**
     * @brief 写入给定时间步的变量值
     */
    void WriteNodalResults(const Variable<bool>& rVariable, const double timeStep) {
        WriteNodalResultRecords(rVariable, 1, timeStep);
    }

    void WriteNodalResults(const Variable<int>& rVariable, const double timeStep) {
        WriteNodalResultRecords(rVariable, 1, timeStep);
    }

    void WriteNodalResults(const Variable<double>& rVariable, const double timeStep) {
        WriteNodalResultRecords(rVariable, 1, timeStep);
    }

    void WriteNodalResults(const Variable<Array1d<double,3>>& rVariable, const double timeStep){
        WriteNodalResultRecords(rVariable, 3, timeStep);
    }

    void WriteNodalResults(const Variable<Vector>& rVariable, const double timeStep){
        QUEST_ERROR << "Dynamic Vector results are not yet supported in UNV" << std::endl;
    }

    void WriteNodalResults(const Variable<Matrix>& rVariable, const double timeStep) {
        QUEST_ERROR << "Matrix results are not yet supported in UNV" << std::endl;
    }


    /**
     * @brief 获取变量的数据结构类型
     */
    UnvOutput::DataCharacteristics GetDataType(const Variable<bool>&){
        return UnvOutput::DataCharacteristics::SCALAR;
    }

    UnvOutput::DataCharacteristics GetDataType(const Variable<int>&) {
        return UnvOutput::DataCharacteristics::SCALAR;
    }   

    UnvOutput::DataCharacteristics GetDataType(const Variable<double>&) {
        return UnvOutput::DataCharacteristics::SCALAR;
    }   

    UnvOutput::DataCharacteristics GetDataType(const Variable<Array1d<double,3>>&){
        return UnvOutput::DataCharacteristics::D3_DOF_GLOBAL_TRANSLATION_VECTOR;
    }

    UnvOutput::DataCharacteristics GetDataType(const Variable<Vector>&) {
        QUEST_ERROR << "Dynamic Vector results are not yet supported in UNV" << std::endl;
    } 

    UnvOutput::DataCharacteristics GetDataType(const Variable<Matrix>&){
        QUEST_ERROR << "Matrix results are not yet supported in UNV" << std::endl;
    }


    /**
     * @brief 写入一个节点的变量值
     */
    void WriteNodalResultValues(std::ofstream &outputFile, const Node& node, const Variable<bool>& rVariable){
        outputFile << std::setw(13) << node.FastGetSolutionStepValue(rVariable) << "\n";
    }

    void WriteNodalResultValues(std::ofstream &outputFile, const Node& node, const Variable<int>& rVariable){
        outputFile << std::setw(13) << node.FastGetSolutionStepValue(rVariable) << "\n";
    }

    void WriteNodalResultValues(std::ofstream &outputFile, const Node& node, const Variable<double>& rVariable){
        outputFile << std::setw(13) << node.FastGetSolutionStepValue(rVariable) << "\n";
    }

    void WriteNodalResultValues(std::ofstream &outputFile, const Node& node, const Variable<Array1d<double,3>>& rVariable){
        auto & temp = node.FastGetSolutionStepValue(rVariable);

        outputFile << std::setw(13) << temp[0];
        outputFile << std::setw(13) << temp[1];
        outputFile << std::setw(13) << temp[2];
        outputFile << "\n";
    }

    void WriteNodalResultValues(std::ofstream &outputFile, const Node& node, const Variable<Vector>& rVariable){
        QUEST_ERROR << "Dynamic Vector results are not yet supported by in UNV" << std::endl;
    }

    void WriteNodalResultValues(std::ofstream &outputFile, const Node& node, const Variable<Matrix>& rVariable){
        QUEST_ERROR << "Matrix results are not yet supported by in UNV" << std::endl;
    }

}