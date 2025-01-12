#ifndef QUEST_INTEGRATION_VALUES_EXTRAPOLATION_TO_NODES_PROCESS_HPP
#define QUEST_INTEGRATION_VALUES_EXTRAPOLATION_TO_NODES_PROCESS_HPP

// 系统头文件
#include <unordered_map>

// 项目头文件
#include "processes/process.hpp"
#include "container/model.hpp"
#include "includes/key_hash.hpp"
#include "includes/quest_parameters.hpp"

namespace Quest{

    /**
     * @brief 该过程将积分点的值外推到节点
     * @details 通过求解局部问题，将值从高斯点外推到节点。
     * 对于节点和高斯点数量相同的情况，使用逆矩阵；而对于高斯点数量多于节点数量的情况，使用广义逆矩阵
     */
    class QUEST_API(QUEST_CORE) IntegrationValuesExtrapolationToNodesProcess : public Process{
        public:
            using NodeType = Node;
            using GeometryType = Geometry<NodeType>;
            using SizeType = std::size_t;
            using IndexType = std::size_t;

            QUEST_CLASS_POINTER_DEFINITION(IntegrationValuesExtrapolationToNodesProcess);

        public:
            /**
             * @brief 构造函数
             */
            IntegrationValuesExtrapolationToNodesProcess(
                Model& rModel,
                Parameters ThisParameters = Parameters(R"({})")
            );


            /**
             * @brief 构造函数
             */
            IntegrationValuesExtrapolationToNodesProcess(
                ModelPart& rMainModelPart,
                Parameters ThisParameters = Parameters(R"({})")
            );


            /**
             * @brief 析构函数
             */
            ~IntegrationValuesExtrapolationToNodesProcess() override= default;


            /**
             * @brief 函数调用运算符重载，调用Execute函数
             */
            void operator()()
            {
                Execute();
            }


            /**
             * @brief 执行过程的算法
             */
            void Execute() override;


            /**
             * @brief 在求解循环之前执行一次，但在所有求解器构建之后执行
             */
            void ExecuteBeforeSolutionLoop() override;


            /**
             * @brief 该函数将在每个时间步后执行，执行顺序是在求解阶段之后
             */
            void ExecuteFinalizeSolutionStep() override;


            /**
             * @brief 该函数设计为在读取模型和组之后，在计算结束时调用
             */
            void ExecuteFinalize() override;


            /**
             * @brief 该方法提供默认参数，以避免不同构造函数之间的冲突
             */
            const Parameters GetDefaultParameters() const override;


            std::string Info() const override
            {
                return "IntegrationValuesExtrapolationToNodesProcess";
            }


            void PrintInfo(std::ostream& rOStream) const override
            {
                rOStream << Info();
            }

        protected:

        private:
            /**
             * @brief 该方法初始化map
             */
            void InitializeMaps();


            /**
             * @brief 该方法初始化变量
             */
            void InitializeVariables();

        private:
            /**
             * @brief 主模型部件
             */
            ModelPart& mrModelPart;

            /**
             * @brief 是否外推非历史值
             */
            bool mExtrapolateNonHistorical;  

            /**
             * @brief 是否对区域进行平均
             */
            bool mAreaAverage;                                      

            /**
             * @brief 双精度变量
             */
            std::vector<const Variable<double>*> mDoubleVariable;    

            /**
             * @brief 要计算的数组变量
             */
            std::vector<const Variable<Array1d<double, 3>>*> mArrayVariable; 

            /**
             * @brief 要计算的向量变量
             */
            std::vector<const Variable<Vector>*> mVectorVariable; 

            /**
             * @brief 要计算的矩阵变量
             */
            std::vector<const Variable<Matrix>*> mMatrixVariable;      

            /**
             * @brief 向量变量的大小
             */
            std::unordered_map<const Variable<Vector>*, SizeType, pVariableHasher, pVariableComparator> mSizeVectors;

            /**
             * @brief 矩阵变量的大小
             */
            std::unordered_map<const Variable<Matrix>*, std::pair<SizeType, SizeType>, pVariableHasher, pVariableComparator> mSizeMatrixes; 

            /**
             * @brief 用于计算平均权重的变量
             */
            const Variable<double>* mpAverageVariable;      

            /**
             * @brief 输出详细信息的级别
             */
            SizeType mEchoLevel;                              

    };


    inline std::istream& operator >> (std::istream& rIstream, IntegrationValuesExtrapolationToNodesProcess& rThis);


    inline std::ostream& operator << (std::ostream& rOstream, const IntegrationValuesExtrapolationToNodesProcess& rThis){
        return rOstream;
    }

}


#endif //QUEST_INTEGRATION_VALUES_EXTRAPOLATION_TO_NODES_PROCESS_HPP