/*-------------------------------
accessor.hpp头文件的实现代码
-------------------------------*/

// 项目头文件
#include "includes/accessor.hpp"

namespace Quest{
    double Accessor::GetValue(
        const Variable<double>& rVariable,
        const Properties& rProperties,
        const GeometryType& rGeometry,
        const Vector& rShapeFunctionVector,
        const ProcessInfo& rProcessInfo
    ) const {
        QUEST_ERROR << "You are calling the GetValue of the virtual Accessor class..." << std::endl;
        return 0.0;
    }


    Vector Accessor::GetValue(
        const Variable<Vector>& rVariable,
        const Properties& rProperties,
        const GeometryType& rGeometry,
        const Vector& rShapeFunctionVector,
        const ProcessInfo& rProcessInfo
    ) const {
        QUEST_ERROR << "You are calling the GetValue of the virtual Accessor class..." << std::endl;
        return Vector();
    }


    bool Accessor::GetValue(
        const Variable<bool>& rVariable,
        const Properties& rProperties,
        const GeometryType& rGeometry,
        const Vector& rShapeFunctionVector,
        const ProcessInfo& rProcessInfo
    ) const{
        QUEST_ERROR << "You are calling the GetValue of the virtual Accessor class..." << std::endl;
        return false;
    }


    int Accessor::GetValue(
        const Variable<int>& rVariable,
        const Properties& rProperties,
        const GeometryType& rGeometry,
        const Vector& rShapeFunctionVector,
        const ProcessInfo& rProcessInfo
    ) const{
        QUEST_ERROR << "You are calling the GetValue of the virtual Accessor class..." << std::endl;
        return 0;
    }


    Matrix Accessor::GetValue(
        const Variable<Matrix>& rVariable,
        const Properties& rProperties,
        const GeometryType& rGeometry,
        const Vector& rShapeFunctionVector,
        const ProcessInfo& rProcessInfo
    ) const{    
        QUEST_ERROR << "You are calling the GetValue of the virtual Accessor class..." << std::endl;
        return Matrix();
    }


    Array1d<double,3> Accessor::GetValue(
        const Variable<Array1d<double,3>>& rVariable,
        const Properties& rProperties,
        const GeometryType& rGeometry,
        const Vector& rShapeFunctionVector,
        const ProcessInfo& rProcessInfo
    ) const{
        QUEST_ERROR << "You are calling the GetValue of the virtual Accessor class..." << std::endl;
        return Array1d<double,3>();
    }


    Array1d<double,6> Accessor::GetValue(
        const Variable<Array1d<double,6>>& rVariable,
        const Properties& rProperties,
        const GeometryType& rGeometry,
        const Vector& rShapeFunctionVector,
        const ProcessInfo& rProcessInfo
    ) const{
        QUEST_ERROR << "You are calling the GetValue of the virtual Accessor class..." << std::endl;
        return Array1d<double,6>();
    }


    Array1d<double,4> Accessor::GetValue(
        const Variable<Array1d<double,4>>& rVariable,
        const Properties& rProperties,
        const GeometryType& rGeometry,
        const Vector& rShapeFunctionVector,
        const ProcessInfo& rProcessInfo
    ) const{
        QUEST_ERROR << "You are calling the GetValue of the virtual Accessor class..." << std::endl;
        return Array1d<double,4>();
    }


    Array1d<double,9> Accessor::GetValue(
        const Variable<Array1d<double,9>>& rVariable,
        const Properties& rProperties,
        const GeometryType& rGeometry,
        const Vector& rShapeFunctionVector,
        const ProcessInfo& rProcessInfo
    ) const{
        QUEST_ERROR << "You are calling the GetValue of the virtual Accessor class..." << std::endl;
        return Array1d<double,9>();
    }


    std::string Accessor::GetValue(
        const Variable<std::string>& rVariable,
        const Properties& rProperties,
        const GeometryType& rGeometry,
        const Vector& rShapeFunctionVector,
        const ProcessInfo& rProcessInfo
    ) const{
        QUEST_ERROR << "You are calling the GetValue of the virtual Accessor class..." << std::endl;
        return std::string();
    }


    Accessor::UniquePointer Accessor::Clone() const{
        return Quest::make_unique<Accessor>(*this);
    }

}