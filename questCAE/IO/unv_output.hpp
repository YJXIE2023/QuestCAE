#ifndef QUEST_UNV_OUTPUT_HPP
#define QUEST_UNV_OUTPUT_HPP

// 系统头文件
#include <iostream>
#include <fstream>
#include <string>

// 项目头文件
#include "includes/model_part.hpp"
#include "includes/exceptions.hpp"

namespace Quest{

    /**
     * @brief 提供一个工具来写入UNV文件
     * 支持以下三个数据集：
     * 2411  -  节点数据集
     * 2412  -  单元数据集
     * 2414  -  结果数据集
     */
    class QUEST_API(QUEST_CORE) UnvOutput{
        public:
            /**
             * @brief 数据集类型枚举类
             */
            enum class DatasetID {
                NODES_DATASET               = 2411,
                ELEMENTS_DATASET            = 2412,
                RESULTS_DATASET             = 2414
            };


            /**
             * @brief 数据集所属对象的枚举类
             */
            enum class DatasetLocation {
                DATA_AT_NODES               = 1,
                DATA_AT_ELEMENTS            = 2,
                DATA_AT_NODES_ON_ELEMENTS   = 3,
                DATA_AT_POINTS              = 5,
                DATA_ON_ELEMENTS_AT_NODES   = 6
            };


            /**
             * @brief 模型类型的枚举类
             */
            enum class ModelType {
                UNKNOWN         = 0,
                STRUCTURAL      = 1,
                HEAT_TRANSFER   = 2,
                FLUID_FLOW      = 3
            };


            /**
             * @brief 有限元分析类型枚举类
             */
            enum class AnalysisType {
                UNKNOWN                             = 0,
                STATIC                              = 1,
                NORMAL_MODE                         = 2,
                COMPLEX_EIGENVALUE_FIRST_ORDER      = 3,
                TRANSIENT                           = 4,
                FREQUENCY_RESPONSE                  = 5,
                BUCKLING                            = 6,
                COMPLEX_EIGENVALUE_SECOND_ORDER     = 7,
                STATIC_NON_LINEAR                   = 9,
                CRAIG_BAMPTON_CONSTRAINT_MODES      = 10,
                EQUIVALENT_ATTACHMENT_MODES         = 11,
                EFFECTIVE_MASS_MODES                = 12,
                EFFECTIVE_MASS_MATRIX               = 13,
                EFFECTIVE_MASS_MATRIX_COPY          = 14,  
                DISTRIBUTED_LOAD_LOAD_DISTRIBUTION  = 15,
                DISTRIBUTED_LOAD_ATTACHMENT_MODES   = 16
            };


            /**
             * @brief 数据结构类型
             * @details 如一维数据、二维数据等
             */
            enum class DataCharacteristics {
                UNKNOWN = 0,
                SCALAR = 1,
                D3_DOF_GLOBAL_TRANSLATION_VECTOR = 2,
                D3_DOF_GLOBAL_TRANSLATION_ROTATION_VECTOR = 3,
                SYMMETRIC_GLOBAL_TENSOR = 4,
                STRESS_RESULTANTS = 6
            };


            /**
             * @brief 数据类型，如整数、浮点数等
             */
            enum class DataType {
                INTEGER = 1,
                SINGLE_PRECISION_FLOATING_POINT = 2,
                DOUBLE_PRECISION_FLOATING_POINT = 4,
                SINGLE_PRECISION_COMPLEX = 5,
                DOUBLE_PRECISION_COMPLEX = 6
            };

            QUEST_CLASS_POINTER_DEFINITION(UnvOutput);


            /**
             * @brief 将枚举类型转换为底层的整数类型
             */
            template <typename Enumeration>
            auto as_integer(Enumeration const value)
                -> typename std::underlying_type<Enumeration>::type
            {
                return static_cast<typename std::underlying_type<Enumeration>::type>(value);
            }

        public:
            /**
             * @brief 构造函数
             */
            UnvOutput(Quest::ModelPart &modelPart, const std::string &outFileWithoutExtension);


            /**
             * @brief 初始化输出文件
             */
            void InitializeOutputFile();


            /**
             * @brief 写入模型部件关联的网格数据
             */
            void WriteMesh();


            /**
             * @brief 写入模型部件关联的节点数据
             */
            void WriteNodes();


            /**
             * @brief 写入模型部件关联的单元数据
             */
            void WriteElements();


            /**
             * @brief 写入给定时间步的变量值
             */
            void WriteNodalResults(const Variable<bool>& rVariable, const double timeStep);
            void WriteNodalResults(const Variable<int>& rVariable, const double timeStep);
            void WriteNodalResults(const Variable<double>& rVariable, const double timeStep);
            void WriteNodalResults(const Variable<Array1d<double,3>>& rVariable, const double timeStep);
            void WriteNodalResults(const Variable<Vector>& rVariable, const double timeStep);
            void WriteNodalResults(const Variable<Matrix>& rVariable, const double timeStep);


            /**
             * @brief 获取变量的数据结构类型
             */
            UnvOutput::DataCharacteristics GetDataType(const Variable<bool>&);
            UnvOutput::DataCharacteristics GetDataType(const Variable<int>&);
            UnvOutput::DataCharacteristics GetDataType(const Variable<double>&);
            UnvOutput::DataCharacteristics GetDataType(const Variable<Array1d<double,3>>&);
            UnvOutput::DataCharacteristics GetDataType(const Variable<Vector>&);
            UnvOutput::DataCharacteristics GetDataType(const Variable<Matrix>&);


            /**
             * @brief 写入一个节点的变量值
             */
            void WriteNodalResultValues(std::ofstream &outputFile, const Node& node, const Variable<bool>& rVariable);
            void WriteNodalResultValues(std::ofstream &outputFile, const Node& node, const Variable<int>& rVariable);
            void WriteNodalResultValues(std::ofstream &outputFile, const Node& node, const Variable<double>& rVariable);
            void WriteNodalResultValues(std::ofstream &outputFile, const Node& node, const Variable<Array1d<double,3>>& rVariable);
            void WriteNodalResultValues(std::ofstream &outputFile, const Node& node, const Variable<Vector>& rVariable);
            void WriteNodalResultValues(std::ofstream &outputFile, const Node& node, const Variable<Matrix>& rVariable);


            /**
             * @brief 获取与 rVariable 对应的 UNV 变量名称的 ID。如果未找到对应，返回 1000+
             */
            template<class TVariablebleType>
            int GetUnvVariableName(const TVariablebleType& rVariable) {
                if(rVariable == VELOCITY)       return 11;
                if(rVariable == TEMPERATURE)    return 5;
                if(rVariable == PRESSURE)       return 117;

                return 1000;
            }


            /**
             * @brief 输出节点结果数据集
             */
            template<class TVariablebleType>
            void WriteNodalResultRecords(const TVariablebleType& rVariable, const int numComponents, const double timeStep) {
                std::ofstream outputFile;
                outputFile.open(mOutputFileName, std::ios::out | std::ios::app);

                std::string dataSetName = "NodalResults";
                const std::string& dataSetLabel = rVariable.Name();

                outputFile << std::setw(6)  << "-1" << "\n";                                                // 开始
                outputFile << std::setw(6)  << as_integer(DatasetID::RESULTS_DATASET) << "\n";              // 数据集ID

                outputFile << std::setw(10) << dataSetLabel << "\n";                                        // Record 1 - Label
                outputFile << std::setw(6)  << dataSetName << "\n";                                         // Record 2 - Name
                outputFile << std::setw(10) << as_integer(DatasetLocation::DATA_AT_NODES) << "\n";          // Record 3

                // 开始输出Record
                outputFile << "" << "\n";                                                                   // Record 4
                outputFile << "" << "\n";                                                                   // Record 5
                outputFile << "" << "\n";                                                                   // Record 6
                outputFile << "" << "\n";                                                                   // Record 7
                outputFile << "" << "\n";                                                                   // Record 8
                
                // ModelType, AnalysisType, DataCharacteristic, ResultType, DataType, NumberOfDataValues    // Record 9
                outputFile << std::setw(10) << as_integer(ModelType::STRUCTURAL); 
                outputFile << std::setw(10) << as_integer(AnalysisType::TRANSIENT);
                outputFile << std::setw(10) << as_integer(GetDataType(rVariable));
                outputFile << std::setw(10) << GetUnvVariableName(rVariable);
                outputFile << std::setw(10) << as_integer(DataType::SINGLE_PRECISION_FLOATING_POINT);
                outputFile << std::setw(10) << numComponents; 
                outputFile << "\n";

                // DesignSetId, IterationNumber, SolutionSetId, BoundaryCondition, LoadSet, ModeNumber, TimeStampNumber, FrequencyNumber
                outputFile << std::setw(10) << 0;                                                           // Record 10
                outputFile << std::setw(10) << timeStep;
                outputFile << std::setw(10) << 0;
                outputFile << std::setw(10) << 0;
                outputFile << std::setw(10) << 0;
                outputFile << std::setw(10) << 1;
                outputFile << std::setw(10) << timeStep;
                outputFile << std::setw(10) << 0;
                outputFile << "\n";

                // CreationOption, (Unknown)*7
                outputFile << std::setw(10) << 0;                                                           // Record 11
                outputFile << std::setw(10) << 0;
                outputFile << "\n";

                // Time, Frequency Eigenvalue NodalMass ViscousDampingRatio, HystereticDampingRatio
                outputFile << std::setw(13) << timeStep * 0.1;                                              // Record 12
                outputFile << std::setw(13) << "0.00000E+00";
                outputFile << std::setw(13) << "0.00000E+00";
                outputFile << std::setw(13) << "0.00000E+00";
                outputFile << std::setw(13) << "0.00000E+00";
                outputFile << std::setw(13) << "0.00000E+00";
                outputFile << "\n";

                // Eigenvalue_re, Eigenvalue_im, ModalA_re, ModalA_im, ModalB_re, ModalB_im
                outputFile << std::setw(13) << "0.00000E+00";                                               // Record 13
                outputFile << std::setw(13) << "0.00000E+00";
                outputFile << std::setw(13) << "0.00000E+00";
                outputFile << std::setw(13) << "0.00000E+00";
                outputFile << std::setw(13) << "0.00000E+00";
                outputFile << std::setw(13) << "0.00000E+00";
                outputFile << "\n";

                // 节点上的数据
                for (auto &node_i : mrOutputModelPart.Nodes()) {
                    int node_label = node_i.Id();
                    outputFile << std::setw(6) << node_label << "\n";                                       // Record 14 - Node Number
                    WriteNodalResultValues(outputFile, node_i, rVariable);                                  // Record 15 - NumberOfDataValues' data of the node
                }
                
                outputFile << std::setw(6) << "-1" << "\n";
                outputFile.close();
            }

        protected:

        private:

        private:
            /**
             * @brief 要输出的模型部件
             */
            Quest::ModelPart &mrOutputModelPart;

            /**
             * @brief 输出文件名
             */
            std::string mOutputFileName;

    };

}


#endif //QUEST_UNV_OUTPUT_HPP