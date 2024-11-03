/*-----------------------------------------------
定义材料的初始状态
记录材料的初始应力、应变和变形梯度 𝐹 等信息
-----------------------------------------------*/

#ifndef QUEST_INITIAL_STATE_HPP
#define QUEST_INITIAL_STATE_HPP

// 系统头文件
#include <atomic>

// 项目头文件
#include "includes/define.hpp"
#include "includes/variables.hpp"

namespace Quest{

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
            InitialState(){}


            InitialState(const SizeType Dimension);


            InitialState(
                const Vector& rInitialStrainVector,
                const Vector& rInitialStressVector,
                const Matrix& rInitialDeformationGradientMatrix
            );
            


            InitialState(
                const Vector& rImposingEntity,
                const InitialImposingType InitialImposition = InitialImposingType::STRAIN_ONLY
            );


            InitialState(
                const Vector& rInitialStrainVector,
                const Vector& rInitialStressVector
            );


            InitialState(const Matrix& rInitialDeformationGradientMatrix);


            virtual ~InitialState(){}


            unsigned int use_count() const noexcept{
                return mReferenceCount;
            }


            void SetInitialStrainVector(const Vector& rInitialStrainVector);


            void SetInitialStressVector(const Vector& rInitialStressVector);


            void SetInitialDeformationGradientMatrix(const Matrix& rInitialDeformationGradientMatrix);


            const Vector& GetInitialStrainVector() const;


            const Vector& GetInitialStressVector() const;


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
            Vector mInitialStrainVector;
            Vector mInitialStressVector;
            Matrix mInitialDeformationGradientMatrix;

    };

} // namespace Quest

#endif //QUEST_INITIAL_STATE_HPP