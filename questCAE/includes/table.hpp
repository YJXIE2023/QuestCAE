#ifndef QUEST_TABLE_HPP
#define QUEST_TABLE_HPP

// 系统头文件
#include <string>
#include <iostream>

// 项目头文件
#include "IO/logger.hpp"
#include "includes/define.hpp"
#include "container/variable.hpp"

namespace Quest{

    /**
     * @class Table
     * @brief 表格类
     * @details 表示一个变量随另一个变量的变化关系
     * 还提供了一个使用分段线性插值的 double到double类型的表，用于获取中间值
     * @tparam TArgumentType 自变量类型
     * @tparam TResultType 因变量类型
     * @tparam TResultsColumns 因变量的列数
     */
    template<typename TArgumentType, typename TResultType = TArgumentType, std::size_t TResultsColumns = 1>
    class Table{
        public:
            QUEST_CALSS_POINTER_DEFINITION(Table);

            using result_row_type = std::array<TResultType, TResultsColumns>;
            using RecordType = std::pair<TArgumentType, result_row_type>;
            using TableContainerType = std::vector<RecordType>;

        public:
            Table() = default;


            virtual ~Table() = default;

            /**
             * @brief 函数调用运算符重载，返回表中最接近自变量的第一个列的结果值
             */
            const TResultType& operator()(const TArgumentType& X) const{
                return GetNearestRow(X)[0];
            }

            /**
             * @brief 函数调用运算符重载，返回表中最接近自变量的第一个列的结果值
             */
            TResultType& operator()(const TArgumentType& X){
                return GetNearestRow(X)[0];
            }

            /**
             * @brief 函数调用运算符重载，返回表中最接近自变量的指定列的结果值
             */
            const TResultType& operator()(const TArgumentType& X, std::size_t J) const{
                return GetNearestRow(X)[J];
            }

            /**
             * @brief 函数调用运算符重载，返回表中最接近自变量的指定列的结果值
             */
            TResultType& operator()(const TArgumentType& X, std::size_t J){
                return GetNearestRow(X)[J];
            }

            /**
             * @brief 下标运算符重载，查找表中与指定变量值最接近的值，并返回整行值
             */
            const result_row_type& operator[](const TArgumentType& X) const{
                return GetNearestRow(X);
            }

            /**
             * @brief 下标运算符重载，查找表中与指定变量值最接近的值，并返回整行值
             */
            result_row_type& operator[](const TArgumentType& X){
                return GetNearestRow(X);
            }

            /**
             * @brief 查找表中与指定变量值最接近的值，并返回整行值
             */
            TResultType& GetNearestRow(const TArgumentType& X){
                std::size_t size = mData.size();

                QUEST_ERROR_IF(size == 0) << "Get value from empty table" << std::endl;

                if(size == 1){
                    return mData.begin()->second;
                }

                if(X <= mData[0].first){
                    return mData[0].second;
                }

                for(std::size_t i = 1; i < size; ++i){
                    if(X <= mData[i].first){
                        return ((X - mData[i-1].first) < (mData[i].first - X)) ? mData[i-1].second : mData[i].second;
                    }
                }

                return mData[size-1].second;
            }

            /**
             * @brief 查找表中与指定变量值最接近的值，并返回整行值
             */
            const TResultType& GetNearestRow(const TArgumentType& X) const{
                std::size_t size = mData.size();

                QUEST_ERROR_IF(size == 0) << "Get value from empty table" << std::endl;

                if(size == 1){
                    return mData.begin()->second;
                }

                if(X <= mData[0].first){
                    return mData[0].second;
                }

                for(std::size_t i = 1; i < size; ++i){
                    if(X <= mData[i].first){
                        return ((X - mData[i-1].first) < (mData[i].first - X)) ? mData[i-1].second : mData[i].second;
                    }
                }

                return mData[size-1].second;
            }

            /**
             * @brief 在排序位置插入一行，使得 Xi-1 < X < Xi+1，并将第一列填充为 Y
             */
            void insert(const TArgumentType& X, const TResultType& Y){
                result_row_type a = {{Y}};
                insert(X, a);
            }

            /**
             * @brief 在排序位置插入一行，使得 Xi-1 < X < Xi+1，并将第一列填充为 Y
             */
            template<typename TArrayType>
            void insert(const TArgumentType& X, const TArrayType& Y){
                result_row_type a;
                for(std::size_t i = 0; i < TResultsColumns; ++i){
                    a[i] = Y[i];
                }
                insert(X, a);
            }

            /**
             * @brief 在排序位置插入一行，使得 Xi-1 < X < Xi+1，并将该行填充为 Y
             */
            void insert(const TArgumentType& X, const result_row_type& Y){
                std::size_t size = mData.size();

                if(size == 0){
                    mData.push_back(RecordType(X, Y));
                } else if(X <= mData[0].first){
                    mData.insert(mData.begin(), RecordType(X, Y));
                } else if(X >= mData.back().first){
                    mData.push_back(RecordType(X, Y));
                } else {
                    for(std::size_t i = 1; i < size; ++i){
                        if((X > mData[i-1].first) && (X < mData[i].first)){
                            mData.insert(mData.begin() + i, RecordType(X, Y));
                            break;
                        }
                    }
                }
            }

            /**
             * @brief 在表尾插入一行，并将第一列填充为 Y
             */
            void PushBack(const TArgumentType& X, const TResultType& Y){
                result_row_type a = {{Y}};
                mData.push_back(RecordType(X, a));
            }

            /**
             * @brief 在表尾插入一行，并将第一列填充为 Y
             */
            template<typename TArrayType>
            void PushBack(const TArgumentType& X, const TArrayType& Y){
                result_row_type a;                
                for(std::size_t i = 0; i < TResultsColumns; ++i){
                    a[i] = Y[i];
                }
                mData.push_back(RecordType(X, a));
            }

            /**
             * @brief 在表尾插入一行，并将该行填充为 Y
             */
            template<typename TArrayType>
            void PushBack(const TArgumentType& X, const result_row_type& Y){
                mData.push_back(RecordType(X, Y));
            }

            /**
             * @brief 清空表
             */
            void Clear(){
                mData.clear();
            }

            /**
             * @brief 获取表数据
             */
            TableContainerType& Data(){
                return mData;
            }

            /**
             * @brief 获取表数据
             */
            const TableContainerType& Data() const{
                return mData;
            }


            virtual std::string Info() const{
                std::stringstream buffer;
                buffer << "Table";
                return buffer.str();
            }


            virtual void PrintInfo(std::ostream& rOstream) const{
                rOstream << Info();
            }


            virtual void PrintData(std::ostream& rOstream) const{}


            const std::string& NameOfX() const{
                return mNameOfX;
            }


            const std::string& NameOfY() const{
                return mNameOfY;
            }


            void SetNameOfX(const std::string& rName){
                mNameOfX = rName;
            }


            void SetNameOfY(const std::string& rName){
                mNameOfY = rName;
            }

        protected:

        private:
            friend class Serializer;


            virtual void save(Serializer& rSerializer)const{
                std::size_t local_size = mData.size();

                rSerializer.save("size", local_size);

                for(auto i_row = mData.begin(); i_row!= mData.end(); ++i_row){
                    rSerializer.save("Argument", i_row->first);
                    for(auto j = i_row->second.begin(); j!= i_row->second.end(); ++j){
                        rSerializer.save("Column",j);
                    }
                }
            }


            virtual void load(Serializer& rSerializer){
                std::size_t local_size;
                rSerializer.load("size", local_size);

                mData.resize(local_size);

                for(auto i_row = mData.begin(); i_row!= mData.end(); ++i_row){
                    rSerializer.load("Argument", i_row->first);
                    for(auto j = i_row->second.begin(); j!= i_row->second.end(); ++j){
                        rSerializer.load("Column",j);
                    }
                }
            }

        private:
            /**
             * @brief 表格数据
             * @details std::vector<std::pair<TArgumentType, std::array<TResultType, TResultsColumns>>>
             */
            TableContainerType mData;

            /**
             * @brief 自变量名称
             */
            std::string mNameOfX;

            /**
             * @brief 因变量名称
             */
            std::string mNameOfY;

    };


    template<>
    class Table<double, double>{
        public:
            QUEST_CLASS_POINTER_DEFINITION(Table);

            using TResultType = double;
            using TArgumentType = double;
            using result_row_type = std::array<TResultType, 1>;
            using RecordType = std::pair<TArgumentType, result_row_type>;
            using TableContainerType = std::vector<RecordType>;

            using XVariableType = Variable<TArgumentType>;
            using YVariableType = Variable<TResultType>;

        public:
            Table() = default;


            template<typename TMatrixType>
            explicit Table(const TMatrixType& ThisMatrix):mData(){
                for(unsigned int i = 0; i < ThisMatrix.size1(); ++i){
                    PushBack(ThisMatrix(i, 0), ThisMatrix(i, 1));
                }
            }


            virtual ~Table() = default;


            TResultType operator()(const TArgumentType& X) const{
                return GetValue(X);
            }


            const TResultType& operator[](const TArgumentType& X) const{
                return GetNearestValue(X);
            }


            TResultType& operator()(const TArgumentType& X){
                return GetNearestValue(X);
            }


            TResultType GetValue(const TArgumentType& X) const{
                std::size_t size = mData.size();

                QUEST_ERROR_IF(size == 0) << "Get value from empty table" << std::endl;

                if(size == 1){
                    return mData.begin()->second[0];
                }

                TResultType result;
                if(X <= mData[0].first){
                    return Interpolate(X, mData[0].first, mData[0].second[0], mData[1].first, mData[1].second[0], result);
                }

                for(std::size_t i = 1; i < size; ++i){
                    if(X <= mData[i].first){
                        return Interpolate(X, mData[i-1].first, mData[i-1].second[0], mData[i].first, mData[i].second[0], result);
                    }
                }

                return Interpolate(X, mData[size-2].first, mData[size-2].second[0], mData[size-1].first, mData[size-1].second[0], result);
            }


            result_row_type GetNearestRow(const TArgumentType& X){
                std::size_t size = mData.size();

                QUEST_ERROR_IF(size == 0) << "Get value from empty table" << std::endl;

                if(size == 1){
                    return mData[0].second;
                }

                if(X <= mData[0].first){
                    return mData[0].second;
                }

                for(std::size_t i = 1; i < size; ++i){
                    if(X <= mData[i].first){
                        return ((X - mData[i-1].first) < (mData[i].first - X)) ? mData[i-1].second : mData[i].second;
                    }
                }

                return mData[size-1].second;
            }


            const TResultType GetNearestValue(const TArgumentType& X) const{
                std::size_t size = mData.size();

                QUEST_ERROR_IF(size == 0) << "Get value from empty table" << std::endl;

                if(size == 1){
                    return mData.begin()->second[0];
                }

                if(X <= mData[0].first){
                    return mData[0].second[0];
                }

                for(std::size_t i = 1; i < size; ++i){
                    if(X <= mData[i].first){
                        return ((X - mData[i-1].first) < (mData[i].first - X)) ? mData[i-1].second[0] : mData[i].second[0];
                    }
                }

                return mData[size-1].second[0];
            }


            TResultType& GetNearestValue(const TArgumentType& X){
                std::size_t size = mData.size();

                QUEST_ERROR_IF(size == 0) << "Get value from empty table" << std::endl;

                if(size == 1){
                    return mData.begin()->second[0];
                }

                if(X <= mData[0].first){
                    return mData[0].second[0];
                }

                for(std::size_t i = 1; i < size; ++i){
                    if(X <= mData[i].first){
                        return ((X - mData[i-1].first) < (mData[i].first - X)) ? mData[i-1].second[0] : mData[i].second[0];
                    }
                }

                return mData[size-1].second[0];
            }


            TResultType& Interpolate(const TArgumentType& X, const TArgumentType& X1, const TResultType& Y1, const TArgumentType& X2, const TResultType& Y2, TResultType& Result) const{
                const double epsilon = 1e-12;

                double dx = X2 - X1;
                TResultType dy = Y2 - Y1;

                double scale = 0.00;

                if(dx > epsilon){
                    scale = (X-X1)/dx;
                }

                Result = Y1 + dy*scale;

                return Result;
            }


            void insert(const TArgumentType& X, const TResultType& Y){
                result_row_type a = {{Y}};
                insert(X, a);
            }


            void insert(const TArgumentType& X, const result_row_type& Y){
                std::size_t size = mData.size();

                if(size == 0){
                    mData.push_back(RecordType(X, Y));
                } else if(X <= mData[0].first){
                    mData.insert(mData.begin(), RecordType(X, Y));
                } else if(X > mData.back().first){
                    mData.push_back(RecordType(X, Y));
                } else {
                    for(std::size_t i = 1; i < size; ++i){
                        if((X > mData[i-1].first) && (X <= mData[i].first)){
                            mData.insert(mData.begin() + i, RecordType(X, Y));
                            break;
                        }
                    }
                }
            }


            void PushBack(const TArgumentType& X, const TResultType& Y){
                result_row_type a = {{Y}};
                mData.push_back(RecordType(X, a));
            }


            TResultType GetDerivative(const TArgumentType& X) const{
                std::size_t size = mData.size();

                QUEST_ERROR_IF(size == 0) << "Get value from empty table" << std::endl;

                if(size == 1){
                    return 0.0;
                }

                TResultType result;
                if(X <= mData[0].first){
                    return 0.0;
                }

                for(std::size_t i = 1; i < size; ++i){
                    if(X <= mData[i].first){
                        return InterpolateDerivative(mData[i-1].first, mData[i-1].second[0], mData[i].first, mData[i].second[0], result);
                    }
                }

                return 0.0;
            }


            TResultType& InterpolateDerivative(const TArgumentType& X1, const TResultType& Y1, const TArgumentType& X2, const TResultType& Y2, TResultType& Result) const{
                const double epsilon = 1e-12;
                TArgumentType dx = X2 - X1;
                TResultType dy = Y2 - Y1;
                
                if(dx < epsilon){
                    dx = epsilon;
                    QUEST_WARNING("") 
                    << "*******************************************\n"
                    << "*** ATTENTION: SMALL dX WHEN COMPUTING  ***\n"
                    << "*** DERIVATIVE FROM TABLE. SET TO 1E-12 ***\n"
                    << "*******************************************" <<std::endl;
                }
                Result = dy/dx;
                return Result;
            }


            void Clear(){
                mData.clear();
            }


            TableContainerType& Data(){
                return mData;
            }


            const TableContainerType& Data() const{
                return mData;
            }


            virtual std::string Info() const{
                std::stringstream buffer;
                buffer << "Piceise Linear Table";
                return buffer.str();
            }


            virtual void PrintInfo(std::ostream& rOstream) const{
                rOstream << Info();
            }


            virtual void PrintData(std::ostream& rOstream) const{
                for(std::size_t i = 0; i < mData.size(); ++i){
                    rOstream << mData[i].first << "\t\t" << mData[i].second[0] << std::endl;
                }
            }


            const std::string& NameOfX() const{
                return mNameOfX;
            }


            const std::string& NameOfY() const{
                return mNameOfY;
            }


            void SetNameOfX(const std::string& rName){
                mNameOfX = rName;
            }


            void SetNameOfY(const std::string& rName){
                mNameOfY = rName;
            }

        protected:

        private:
            friend class Serializer;


            virtual void save(Serializer& rSerializer)const{
                std::size_t local_size = mData.size();

                rSerializer.save("size", local_size);

                for(auto i_row = mData.begin(); i_row!= mData.end(); ++i_row){
                    rSerializer.save("Argument", i_row->first);
                    for(auto j = i_row->second.begin(); j!= i_row->second.end(); ++j){
                        rSerializer.save("Column",*j);
                    }
                }
            }


            virtual void load(Serializer& rSerializer){
                std::size_t local_size;
                rSerializer.load("size", local_size);

                mData.resize(local_size);

                for(auto i_row = mData.begin(); i_row!= mData.end(); ++i_row){
                    rSerializer.load("Argument", i_row->first);
                    for(auto j = i_row->second.begin(); j!= i_row->second.end(); ++j){
                        rSerializer.load("Column",*j);
                    }
                }
            }

        private:
            /**
             * @brief 表格数据
             * @details std::vector<std::pair<TArgumentType, std::array<TResultType, TResultsColumns>>>
             */
            TableContainerType mData;

            /**
             * @brief 自变量名称
             */
            std::string mNameOfX;

            /**
             * @brief 因变量名称
             */
            std::string mNameOfY;

    };


    template<typename TArgumentType, typename TResultType>
    inline std::istream& operator>>(std::istream& rIstream, Table<TArgumentType, TResultType>& rThis){}


    template<typename TArgumentType, typename TResultType>
    inline std::ostream& operator<<(std::ostream& rOstream, const Table<TArgumentType, TResultType>& rThis){
        rThis.PrintData(rOstream);
        rOstream << std::endl;
        rThis.PrintInfo(rOstream);

        return rOstream;
    }

} // namespace Quest

#endif //QUEST_TABLE_HPP