#include "processes/integration_values_extrapolation_to_nodes_process.hpp"

namespace Quest{

    /**
     * @brief 构造函数
     */
    IntegrationValuesExtrapolationToNodesProcess(
        Model& rModel,
        Parameters ThisParameters = Parameters(R"({})")
    ){}


    /**
     * @brief 构造函数
     */
    IntegrationValuesExtrapolationToNodesProcess(
        ModelPart& rMainModelPart,
        Parameters ThisParameters = Parameters(R"({})")
    ){}


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
    void Execute(){}


    /**
     * @brief 在求解循环之前执行一次，但在所有求解器构建之后执行
     */
    void ExecuteBeforeSolutionLoop(){}


    /**
     * @brief 该函数将在每个时间步后执行，执行顺序是在求解阶段之后
     */
    void ExecuteFinalizeSolutionStep(){}


    /**
     * @brief 该函数设计为在读取模型和组之后，在计算结束时调用
     */
    void ExecuteFinalize(){}


    /**
     * @brief 该方法提供默认参数，以避免不同构造函数之间的冲突
     */
    const Parameters GetDefaultParameters() const{}


    /**
     * @brief 该方法初始化map
     */
    void InitializeMaps(){}


    /**
     * @brief 该方法初始化变量
     */
    void InitializeVariables(){}

}