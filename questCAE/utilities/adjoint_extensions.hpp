/*-----------------------------------------
为伴随问题提供统一的扩展接口。
帮助实现伴随元素和条件的灵敏度分析和反向传播。
用于优化问题、灵敏度分析和误差反向传播
------------------------------------------*/

#ifndef QUEST_ADJOINT_EXTENSIONS_HPP
#define QUEST_ADJOINT_EXTENSIONS_HPP

// 系统头文件
#include <iosfwd>
#include <vector>

// 项目头文件
#include "includes/define.hpp"
#include "includes/serializer.hpp"
#include "utilities/indirect_scalar_fwd.hpp"

namespace Quest{

    class AdjointExtensions{
        public:
            QUEST_CLASS_POINTER_DEFINITION(AdjointExtensions);

            virtual ~AdjointExtensions(){}

            virtual void GetFirstDerivativesVector(std::size_t NodeId, std::vector<IndirectScalar<double>>& rVector, std::size_t Step){}

            virtual void GetSecondDerivativesVector(std::size_t NodeId, std::vector<IndirectScalar<double>>& rVector, std::size_t Step){}   

            virtual void GetAuxiliaryVector(std::size_t NodeId, std::vector<IndirectScalar<double>>& rVector, std::size_t Step){}

            virtual void GetFirstDerivativesVariables(std::vector<const VariableData*>& rVariables) const {}

            virtual void GetSecondDerivativesVariables(std::vector<const VariableData*>& rVariables) const {}

            virtual void GetAuxiliaryVariables(std::vector<const VariableData*>& rVariables) const {}

            virtual std::ostream& Print(std::ostream& rOStream) const{
                return rOStream;
            }

        protected:


        private:
            friend class Serializer;

            void save(Serializer& serializer) const{}

            void load(Serializer& serializer){}

        private:


    }; // class AdjointExtensions

} // namespace Quest

#endif //QUEST_ADJOINT_EXTENSIONS_HPP