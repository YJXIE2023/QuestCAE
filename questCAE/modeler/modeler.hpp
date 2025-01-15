#ifndef QUEST_MODELER_HPP
#define QUEST_MODELER_HPP

// 项目头文件
#include "includes/define.hpp"
#include "container/model.hpp"
#include "includes/quest_components.hpp"

namespace Quest{

    /**
     * @brief 与模型部件交互的建模器，用于交互、创建和更新模型部件
     */
    class Modeler{
        public:
            QUEST_CLASS_POINTER_DEFINITION(Modeler);

            using SizeType = std::size_t;
            using IndexType = std::size_t;

        public:
            /**
             * @brief 构造函数
             */
            Modeler(Parameters ModelerParameters = Parameters()): 
                mParameters(ModelerParameters),
                mEchoLevel(
                    ModelerParameters.Has("echo_level")
                    ? ModelerParameters["echo_level"].GetInt()
                    : 0)
            {}


            /**
             * @brief 构造函数
             */
            Modeler(
                Model& rModel,
                Parameters ModelerParameters = Parameters()
            ): mParameters(ModelerParameters)
                , mEchoLevel(
                    ModelerParameters.Has("echo_level")
                    ? ModelerParameters["echo_level"].GetInt()
                    : 0)
            {}


            /**
             * @brief 析构函数
             */
            virtual ~Modeler() = default;


            /**
             * @brief　创建并返回一个modeler对象指针
             */
            virtual Modeler::Pointer Create(Model& rModel, const Parameters ModelParameters) const
            {
                QUEST_ERROR << "Trying to Create Modeler. Please check derived class 'Create' definition." << std::endl;
            }


            /**
             * @brief 从外部输入导入或生成几何模型
             */
            virtual void SetupGeometryModel(){}


            /**
             * @brief 准备或更新几何模型部件
             */
            virtual void PrepareGeometryModel(){}


            /**
             * @brief 转换几何模型或导入适合分析的模型
             */
            virtual void SetupModelPart(){}


            /**
             * @brief 该方法提供默认参数，以避免不同构造函数之间的冲突
             * @return 默认参数
             */
            virtual const Parameters GetDefaultParameters() const
            {
                QUEST_ERROR << "Calling the base Modeler class GetDefaultParameters. Please implement the GetDefaultParameters in your derived model class." << std::endl;
                const Parameters default_parameters = Parameters(R"({})");
                return default_parameters;
            }


            /**
             * @brief 生成模型部件
             */
            virtual void GenerateModelPart(ModelPart& rOriginModelPart, ModelPart& rDestinationModelPart, Element const& rReferenceElement, Condition const& rReferenceBoundaryCondition)
            {
                QUEST_ERROR << "This modeler CAN NOT be used for mesh generation." << std::endl;
            }


            /**
             * @brief 生成网格
             */
            virtual void GenerateMesh(ModelPart& ThisModelPart, Element const& rReferenceElement, Condition const& rReferenceBoundaryCondition)
            {
                QUEST_ERROR << "This modeler CAN NOT be used for mesh generation." << std::endl;
            }


            /**
             * @brief 生成节点
             */
            virtual void GenerateNodes(ModelPart& ThisModelPart)
            {
                QUEST_ERROR << "This modeler CAN NOT be used for node generation." << std::endl;
            }


            virtual std::string Info() const
            {
                return "Modeler";
            }


            virtual void PrintInfo(std::ostream& rOStream) const
            {
                rOStream << Info();
            }


            virtual void PrintData(std::ostream& rOStream) const{}

        protected:

        private:

        private:
            /**
             * @brief 输入参数
             */
            Parameters mParameters;

            /**
             * @brief 信息输出等级
             */
            SizeType mEchoLevel;

    };

    inline std::istream& operator >> (std::istream& rIstream, Modeler& rThis);

    inline std::ostream& operator << (std::ostream& rOstream, const Modeler& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }

    QUEST_API_EXTERN template class QUEST_API(QUEST_CORE) QuestComponents<Modeler>;

    void QUEST_API(QUEST_CORE) AddQuestComponent(const std::string& Name, const Modeler& ThisComponent);

}

#endif //QUEST_MODELER_HPP