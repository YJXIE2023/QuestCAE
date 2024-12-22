#ifndef QUEST_INITIAL_STATE_HPP
#define QUEST_INITIAL_STATE_HPP

// 系统头文件
#include <atomic>

// 项目头文件
#include "includes/define.hpp"
#include "includes/variables.hpp"

namespace Quest{

    /**
     * @class InitialState
     * @brief 定义材料的初始状态，包括初始应力、应变、变形梯度等
     * @details 存储关于初始应力、应变、变形梯度等的变量，并提供相应的接口
     */
    class QUEST_API(QUEST_CORE) InitialState{
        public:
            QUEST_CLASS_INTRUSIVE_POINTER_DEFINITION(InitialState);

            using SizeType = std::size_t;

            enum class InitialImposingType{
                STRAIN_ONLY = 0,
                STRESS_ONLY = 1,
                DEFORMATION_GRADIENT_ONLY = 2,
                STRAIN_AND_STRESS = 3,
                DEFORMATION_GRADIENT_AND_STRESS = 4
            };

        public:
            /**
             * @brief 默认构造函数
             */
            InitialState(){}

            /**
             * @brief 构造函数
             * @param Dimension 维度
             */
            InitialState(const SizeType Dimension);

            /**
             * @brief 构造函数
             * @param rInitialStrainVector 初始应力向量
             * @param rInitialStressVector 初始应力向量
             * @param rInitialDeformationGradientMatrix 初始变形梯度矩阵
             */
            InitialState(
                const Vector& rInitialStrainVector,
                const Vector& rInitialStressVector,
                const Matrix& rInitialDeformationGradientMatrix
            );
            

            /**
             * @brief 构造函数
             * @param rImposingEntity 应力应变实体
             * @param InitialImposition 初始状态类型
             */
            InitialState(
                const Vector& rImposingEntity,
                const InitialImposingType InitialImposition = InitialImposingType::STRAIN_ONLY
            );

            /**
             * @brief 构造函数
             * @param rInitialStrainVector 初始应力向量
             * @param rInitialStressVector 初始应力向量
             */
            InitialState(
                const Vector& rInitialStrainVector,
                const Vector& rInitialStressVector
            );

            /**
             * @brief 复制构造函数
             */
            InitialState(const Matrix& rInitialDeformationGradientMatrix);

            /**
             * @brief 析构函数
             */
            virtual ~InitialState(){}

            /**
             * @brief 获取引用计数
             */
            unsigned int use_count() const noexcept{
                return mReferenceCount;
            }

            /**
             * @brief 设置初始应变向量
             */
            void SetInitialStrainVector(const Vector& rInitialStrainVector);

            /**
             * @brief 设置初始应力向量
             */
            void SetInitialStressVector(const Vector& rInitialStressVector);

            /**
             * @brief 设置初始变形梯度矩阵
             */
            void SetInitialDeformationGradientMatrix(const Matrix& rInitialDeformationGradientMatrix);

            /**
             * @brief 获取初始应变向量
             */
            const Vector& GetInitialStrainVector() const;

            /**
             * @brief 获取初始应力向量
             */
            const Vector& GetInitialStressVector() const;

            /**
             * @brief 获取初始变形梯度矩阵
             */
            const Matrix& GetInitialDeformationGradientMatrix() const;


            virtual std::string Info() const{
                std::stringstream buffer;
                buffer << "InitialState";
                return buffer.str();
            }


            virtual void PrintInfo(std::ostream& rOstream) const{
                rOstream << this->Info();
            }


            virtual void PrintData(std::ostream& rOstream) const{}

        protected:

        private:
            mutable std::atomic<int> mReferenceCount{0};


            friend void intrusive_ptr_add_ref(const InitialState* x){
                x->mReferenceCount.fetch_add(1, std::memory_order_relaxed);
            }


            friend void intrusive_ptr_release(const InitialState* x){
                if(x->mReferenceCount.fetch_sub(1, std::memory_order_release) == 1){
                    delete x;
                }
            }


            friend class Serializer;


            void save(Serializer& rSerializer) const{
                rSerializer.save("InitialStrainVector", mInitialStrainVector);
                rSerializer.save("InitialStressVector", mInitialStressVector);
                rSerializer.save("InitialDeformationGradientMatrix", mInitialDeformationGradientMatrix);
            }


            void load(Serializer& rSerializer){
                rSerializer.load("InitialStrainVector", mInitialStrainVector);
                rSerializer.load("InitialStressVector", mInitialStressVector);
                rSerializer.load("InitialDeformationGradientMatrix", mInitialDeformationGradientMatrix);
            }

        private:
            /**
             * @brief 初始应力向量
             */
            Vector mInitialStrainVector;

            /**
             * @brief 初始应力向量
             */
            Vector mInitialStressVector;

            /**
             * @brief 初始变形梯度矩阵
             */
            Matrix mInitialDeformationGradientMatrix;

    };

} // namespace Quest

#endif //QUEST_INITIAL_STATE_HPP