/*--------------------------------------------
integration_info.hpp类的实现代码
--------------------------------------------*/

// 项目头文件
#include "integration/integration_info.hpp"

namespace Quest{

    QUEST_CREATE_LOCAL_FLAG(IntegrationInfo, DO_NOT_CREATE_TESSELLATION_ON_SLAVE,0);

    IntegrationInfo::IntegrationInfo(SizeType LocalSpaceDimension, IntegrationMethod ThisIntegrationMethod){
        mNumberOfIntegrationPointsPerSpanVector = std::vector<SizeType>(LocalSpaceDimension);
        mQuadratureMethodVector = std::vector<QuadratureMethod>(LocalSpaceDimension);

        for(IndexType i=0; i<LocalSpaceDimension; i++){
            SetIntegrationMethod(i, ThisIntegrationMethod);
        }
    }


    IntegrationInfo::IntegrationInfo(
        SizeType LocalSpaceDimension,
        SizeType NumberOfIntegrationPointsPerSpan,
        QuadratureMethod ThisQuadratureMethod
    ){
        mNumberOfIntegrationPointsPerSpanVector = std::vector<SizeType>(LocalSpaceDimension);
        mQuadratureMethodVector = std::vector<QuadratureMethod>(LocalSpaceDimension);

        for(IndexType i=0; i<LocalSpaceDimension; i++){
            mNumberOfIntegrationPointsPerSpanVector[i] = NumberOfIntegrationPointsPerSpan;
            mQuadratureMethodVector[i] = ThisQuadratureMethod;
        }
    }


    IntegrationInfo::IntegrationInfo(
        const std::vector<SizeType>& NumberOfIntegrationPointsPerSpanVector,
        const std::vector<QuadratureMethod>& ThisQuadratureMethodVector
    ){
        QUEST_ERROR_IF(NumberOfIntegrationPointsPerSpanVector.size() != ThisQuadratureMethodVector.size())
            << "The sizes of the NumberOfIntegrationPointsPerSpanVector: " << NumberOfIntegrationPointsPerSpanVector.size()
            << " and the ThisQuadratureMethodVector: " << ThisQuadratureMethodVector.size() << " does not coincide." << std::endl;  
    }


    void IntegrationInfo::SetIntegrationMethod(
        IndexType DimensionIndex,
        IntegrationMethod ThisIntegrationMethod
    ){
        switch(ThisIntegrationMethod){
            case IntegrationMethod::GI_GAUSS_1:
                mNumberOfIntegrationPointsPerSpanVector[DimensionIndex] = 1;
                mQuadratureMethodVector[DimensionIndex] = QuadratureMethod::GAUSS;
                break;
            case IntegrationMethod::GI_GAUSS_2:
                mNumberOfIntegrationPointsPerSpanVector[DimensionIndex] = 2;
                mQuadratureMethodVector[DimensionIndex] = QuadratureMethod::GAUSS;
                break;
            case IntegrationMethod::GI_GAUSS_3:
                mNumberOfIntegrationPointsPerSpanVector[DimensionIndex] = 3;
                mQuadratureMethodVector[DimensionIndex] = QuadratureMethod::GAUSS;
                break;  
            case IntegrationMethod::GI_GAUSS_4:
                mNumberOfIntegrationPointsPerSpanVector[DimensionIndex] = 4;
                mQuadratureMethodVector[DimensionIndex] = QuadratureMethod::GAUSS;
                break;
            case IntegrationMethod::GI_GAUSS_5:
                mNumberOfIntegrationPointsPerSpanVector[DimensionIndex] = 5;
                mQuadratureMethodVector[DimensionIndex] = QuadratureMethod::GAUSS;
                break;
            case IntegrationMethod::GI_EXTENDED_GAUSS_1:
                mNumberOfIntegrationPointsPerSpanVector[DimensionIndex] = 1;
                mQuadratureMethodVector[DimensionIndex] = QuadratureMethod::EXTENDED_GAUSS;
                break;
            case IntegrationMethod::GI_EXTENDED_GAUSS_2:
                mNumberOfIntegrationPointsPerSpanVector[DimensionIndex] = 2;
                mQuadratureMethodVector[DimensionIndex] = QuadratureMethod::EXTENDED_GAUSS;
                break;
            case IntegrationMethod::GI_EXTENDED_GAUSS_3:
                mNumberOfIntegrationPointsPerSpanVector[DimensionIndex] = 3;
                mQuadratureMethodVector[DimensionIndex] = QuadratureMethod::EXTENDED_GAUSS;
                break;
            case IntegrationMethod::GI_EXTENDED_GAUSS_4:
                mNumberOfIntegrationPointsPerSpanVector[DimensionIndex] = 4;
                mQuadratureMethodVector[DimensionIndex] = QuadratureMethod::EXTENDED_GAUSS;
                break;
            case IntegrationMethod::GI_EXTENDED_GAUSS_5:
                mNumberOfIntegrationPointsPerSpanVector[DimensionIndex] = 5;
                mQuadratureMethodVector[DimensionIndex] = QuadratureMethod::EXTENDED_GAUSS;
                break;
            case IntegrationMethod::NumberOfIntegrationMethods:
                mNumberOfIntegrationPointsPerSpanVector[DimensionIndex] = 0;
                mQuadratureMethodVector[DimensionIndex] = QuadratureMethod::Default;
                break;
        }
    }


    IntegrationInfo::SizeType IntegrationInfo::GetNumberOfIntegrationPointsPerSpan(IndexType DimensionIndex) const{
        return mNumberOfIntegrationPointsPerSpanVector[DimensionIndex];
    }


    void IntegrationInfo::SetNumberOfIntegrationPointsPerSpan(IndexType DimensionIndex, SizeType NumberOfIntegrationPointsPerSpan){
        mNumberOfIntegrationPointsPerSpanVector[DimensionIndex] = NumberOfIntegrationPointsPerSpan;
    }


    void IntegrationInfo::SetQuadratureMethod(IndexType DimensionIndex, QuadratureMethod ThisQuadratureMethod){
        mQuadratureMethodVector[DimensionIndex] = ThisQuadratureMethod;
    }


    IntegrationInfo::IntegrationMethod IntegrationInfo::GetIntegrationMethod(IndexType DimensionIndex) const{
        return GetIntegrationMethod(GetNumberOfIntegrationPointsPerSpan(DimensionIndex), GetQuadratureMethod(DimensionIndex));
    }

} // namespace Quest