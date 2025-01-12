#ifndef QUEST_ENTITIES_UTILITIES_HPP
#define QUEST_ENTITIES_UTILITIES_HPP

// 项目头文件
#include "includes/model_part.hpp"
#include "utilities/parallel_utilities.hpp"
#include "utilities/string_utilities.hpp"

namespace Quest{

    /**
     * @brief 用于高效计算实体计算相关的工具类
     */
    namespace EntitiesUtilities{

        /**
         * @brief 用于根据实体的名称和定义识别并检索实体类型
         */
        template<typename TEntity>
        class QUEST_API(QUEST_CORE) EntitityIdentifier{
            public:
                using GeometryType = typename TEntity::GeometryType;

                constexpr static std::size_t LengthArray = static_cast<std::size_t>(GeometryData::QuestGeometryType::NumberOfGeometryTypes);
                
                QUEST_CLASS_POINTER_DEFINITION(EntitityIdentifier);

            public:
                /**
                 * @brief 默认构造函数
                 */
                EntitityIdentifier() = default;


                /**
                 * @brief 构造函数
                 */
                EntitityIdentifier(const std::string& rName);


                /**
                 * @Brief 复制构造函数
                 */
                EntitityIdentifier(const EntitityIdentifier& rOther)
                : mTypes(rOther.mTypes),
                  mIsInitialized(rOther.mIsInitialized)
                {}


                /**
                 * @brief 赋值运算符重载
                 */
                EntitityIdentifier& operator=(const EntitityIdentifier& rOther)
                {
                    mTypes = rOther.mTypes;
                    mIsInitialized = rOther.mIsInitialized;

                    return *this;
                }


                /**
                 * @brief 检查对象是否已初始化
                 */
                bool IsInitialized() const{}


                /**
                 * @brief 获取原型实体。
                 * @param rGeometry 指向几何体的引用。
                 * @return true 如果存在提供实体的原型。
                 * @return false 如果不存在提供实体的原型。
                 */
                bool HasPrototypeEntity(const GeometryType& rGeometry) const{}


                /**
                 * @brief 获取原型实体
                 */
                const TEntity& GetPrototypeEntity(typename GeometryType::Pointer pGeometry) const{}


                /**
                 * @brief 获取实体类型
                 */
                const TEntity& GetPrototypeEntity(const GeometryType& rGeometry) const{}


                std::string Info() const
                {
                    return "EntitityIdentifier";
                }


                void PrintInfo(std::ostream& rOStream) const
                {
                    rOStream << "EntitityIdentifier";
                }


                void PrintData(std::ostream& rOStream) const;

            protected:

            private:
                /**
                 * @brief 返回给定实体的实体类型名称
                 * @return 实体类型名称的字符串
                 * @throws std::logic_error 如果实体类型不被支持
                 */
                std::string GetEntityTypeName() const{}


                /**
                 * @brief 生成单一类型的原型实体
                 * @param rName 用于生成单一类型的实体名称
                 * @throws std::runtime_error 如果在 QuestComponents 中未找到实体名称
                 */
                void GenerateSingleType(const std::string& rName){}


                /**
                 * @brief 生成多类型实体映射。
                 * @param rName 用于生成多类型的实体名称
                 * @throws std::runtime_error 如果在 QuestComponents 中未找到实体名称
                 */
                void GenerateMultipleTypes(const std::string& rName){}


                /**
                 * @brief 生成模板类型的实体映射。
                 * @param rName 用于生成模板类型的实体名称
                 * @throws std::runtime_error 如果在 QuestComponents 中未找到实体名称
                 */
                void GenerateTemplatedTypes(const std::string& rName){}

            private:
                /**
                 * @brief 实体设置
                 */
                std::array<const TEntity*, LengthArray> mTypes;

                /**
                 * @brief 对象是否初始化的标志
                 */
                bool mIsInitialized = false;
        };


        template<typename TEntity>
        inline std::istream& operator >> (std::istream& rIStream, EntitityIdentifier<TEntity>& rThis){}


        template<typename TEntity>
        inline std::ostream& operator << (std::ostream& rOStream, const EntitityIdentifier<TEntity>& rThis){
            rThis.PrintInfo(rOStream);
            rOStream << std::endl;
            rThis.PrintData(rOStream);

            return rOStream;
        }


        /**
         * @brief 此方法初始化所有已激活实体（条件、单元、约束）。
         * @param rModelPart 要求解的问题的模型部分
         */
        void QUEST_API(QUEST_CORE) InitializeAllEntities(ModelPart& rModelPart){}


        /**
         * @brief 此方法为所有实体（条件、单元、约束）调用 InitializeSolution。
         * @param rModelPart 要求解的问题的模型部分
         */
        void QUEST_API(QUEST_CORE) InitializeSolutionStepAllEntities(ModelPart& rModelPart){}


        /**
         * @brief 此方法为所有实体（条件、单元、约束）调用 FinalizeSolutionStep。
         * @param rModelPart 要求解的问题的模型部分
         */
        void QUEST_API(QUEST_CORE) FinalizeSolutionStepAllEntities(ModelPart& rModelPart){}


        /**
         * @brief 此方法为所有实体（条件、单元、约束）调用 InitializeNonLinearIteration。
         * @param rModelPart 要求解的问题的模型部分
         */
        void QUEST_API(QUEST_CORE) InitializeNonLinearIterationAllEntities(ModelPart& rModelPart){}


        /**
         * @brief 此方法为所有实体（条件、单元、约束）调用 FinalizeNonLinearIteration。
         * @param rModelPart 要求解的问题的模型部分
         */
        void QUEST_API(QUEST_CORE) FinalizeNonLinearIterationAllEntities(ModelPart& rModelPart){}


        /**
         * @brief 此方法返回模型部件中的 TEntitytype 容器（单元、条件和节点）。
         * @param rModelPart 要求解的问题的模型部分
         */
        template<class TEntityType>
        QUEST_API(QUEST_CORE) PointerVectorSet<TEntityType, IndexedObject>& GetEntities(ModelPart& rModelPart){}


        /**
         * @brief 此方法初始化所有激活的实体。
         * @param rModelPart 要求解的问题的模型部分
         */
        template<class TEntityType>
        void InitializeEntities(ModelPart& rModelPart)
        {
            QUEST_TRY

            auto& r_entities_array = GetEntities<TEntityType>(rModelPart);
o
            const ProcessInfo& r_current_process_info = rModelPart.GetProcessInfo();

            block_for_each(
                r_entities_array,
                [&r_current_process_info](TEntityType& rEntity) {
                    if (rEntity.IsActive()) {
                        rEntity.Initialize(r_current_process_info);
                    }
                }
            );

            QUEST_CATCH("")
        }


        /**
         * @brief 此方法为所有实体调用 InitializeSolutionStep。
         * @param rModelPart 要求解的问题的模型部分
         */
        template<class TEntityType>
        void InitializeSolutionStepEntities(ModelPart& rModelPart)
        {
            QUEST_TRY

            const ProcessInfo& r_current_process_info = rModelPart.GetProcessInfo();

            block_for_each(
                GetEntities<TEntityType>(rModelPart),
                [&r_current_process_info](TEntityType& rEntity){
                    rEntity.InitializeSolutionStep(r_current_process_info);
                }
            );

            QUEST_CATCH("")
        }


        /**
         * @brief 此方法为所有实体调用 FinalizeSolutionStep。
         * @param rModelPart 要求解的问题的模型部分
         */
        template<class TEntityType>
        void FinalizeSolutionStepEntities(ModelPart& rModelPart)
        {
            QUEST_TRY

            const ProcessInfo& r_current_process_info = rModelPart.GetProcessInfo();

            block_for_each(
                GetEntities<TEntityType>(rModelPart),
                [&r_current_process_info](TEntityType& rEntity){
                    rEntity.FinalizeSolutionStep(r_current_process_info);
                }
            );

            QUEST_CATCH("")
        }


        /**
         * @brief 此方法为所有实体调用 InitializeNonLinearIteration。
         * @param rModelPart 要求解的问题的模型部分
         */
        template<class TEntityType>
        void InitializeNonLinearIterationEntities(ModelPart& rModelPart)
        {
            QUEST_TRY

            const ProcessInfo& r_current_process_info = rModelPart.GetProcessInfo();

            block_for_each(
                GetEntities<TEntityType>(rModelPart),
                [&r_current_process_info](TEntityType& rEntity){
                    rEntity.InitializeNonLinearIteration(r_current_process_info);
                }
            );

            QUEST_CATCH("")
        }


        /**
         * @brief 此方法为所有实体调用 FinalizeNonLinearIteration。
         * @param rModelPart 要求解的问题的模型部分
         */
        template<class TEntityType>
        void FinalizeNonLinearIterationEntities(ModelPart& rModelPart)
        {
            QUEST_TRY

            const ProcessInfo& r_current_process_info = rModelPart.GetProcessInfo();

            block_for_each(
                GetEntities<TEntityType>(rModelPart),
                [&r_current_process_info](TEntityType& rEntity){
                    rEntity.FinalizeNonLinearIteration(r_current_process_info);
                }
            );

            QUEST_CATCH("")
        }

    }; // namespace EntitiesUtilities

}


#endif //QUEST_ENTITIES_UTILITIES_HPP