#ifndef QUEST_TABLE_STREAM_HPP
#define QUEST_TABLE_STREAM_HPP

// 系统头文件
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>

// 项目头文件
#include "includes/serializer.hpp"
#include "includes/exceptions.hpp"

namespace Quest{

    class endl{};

    /**
     * @class TableStream
     * @brief 用于以结构化和美观的方式将数据流输出为表格形式的工具
     */
    class TableStream{
        public:
            QUEST_CLASS_POINTER_DEFINITION(TableStream);

            /**
             * @brief 构造函数
             */
            TableStream(std::ostream* pOutStream,const std::string& Separator = "|",const bool UseBoldFont = true):
                mOutStream(pOutStream),
                mSeparator(Separator),
                mBoldFont(UseBoldFont)
            {
                mIndexRow = 0;
                mIndexColumn = 0;
                mTableWidth = 0;
                mFlushLeft = false;
            }

            /**
             * @brief 析构函数
             */
            virtual ~TableStream() = default;

            /**
             * @brief 重载输出流操作符
             */
            template<typename TClass>
            TableStream& operator<<(TClass Input){
                if(typeid(Input) == typeid(endl)){
                    while(mIndexColumn != 0){
                        *this << "";
                    }
                } else {
                    if(mIndexColumn == 0){
                        *mOutStream << "|";
                    }

                    if(mFlushLeft){
                        *mOutStream << std::left;
                    } else {
                        *mOutStream << std::right;
                    }

                    *mOutStream << std::setw(mColumnWidths.at(mIndexColumn)) << Input;

                    if(mIndexColumn == GetNumColumns()-1){
                        *mOutStream << "|\n";
                        mIndexRow = mIndexRow + 1;
                        mIndexColumn = 0;
                    } else {
                        *mOutStream << mSeparator;
                        mIndexColumn = mIndexColumn + 1;
                    }
                }
                return *this;
            }

            TableStream& operator << (float Input){
                OutputDecimalNumber<float>(Input);
                return *this;
            }

            TableStream& operator << (double Input){
                OutputDecimalNumber<double>(Input);
                return *this;
            }

            /**
             * @brief 获取当前列数
             */
            unsigned int GetNumColumns() const{
                return mColumnHeaders.size();
            }

            /**
             * @brief 获取表格的总宽度
             */
            unsigned int GetTableWidth() const{
                return mTableWidth;
            }

            /**
             * @brief 设置分隔符
             */
            void SetSeparator(const std::string& Separator){
                mSeparator = Separator;
            }

            /**
             * @brief 设置是否使用粗体字
             */
            void SetBold(const bool& UseBoldFont){
                mBoldFont = UseBoldFont;
            }

            /**
             * @brief 设置左对齐
             */
            void SetFlushLeft(){
                mFlushLeft = true;
            }

            /**
             * @brief 设置右对齐
             */
            void SetFlushRight(){
                mFlushLeft = false;
            }

            /**
             * @brief 添加一列
             */
            void AddColumn(const std::string& HeaderName, const int ColumnWidth){
                QUEST_ERROR_IF(ColumnWidth < 4) << "Column width must be at least 4" << std::endl;

                mColumnHeaders.push_back(HeaderName);
                mColumnWidths.push_back(ColumnWidth);
                mTableWidth += ColumnWidth + mSeparator.size();
            }

            /**
             * @brief 打印表头
             */
            void PrintHeader(){
                PrintHorizontalLine();

                if(mBoldFont == true){
                    #ifndef _WIN32
                        *mOutStream << "\x1B[1m";
                    #endif
                }

                *mOutStream << "|";

                for(unsigned int i = 0; i < GetNumColumns(); ++i){
                    if(mFlushLeft){
                        *mOutStream << std::left;
                    } else {
                        *mOutStream << std::right;
                    }

                    *mOutStream << std::setw(mColumnWidths.at(i)) << mColumnHeaders.at(i).substr(0, mColumnWidths.at(i));

                    if(i != GetNumColumns()-1){
                        *mOutStream << mSeparator;
                    }
                }

                *mOutStream << "|";

                if(mBoldFont == true){
                    #ifndef _WIN32
                        *mOutStream << "\x1B[0m";
                    #endif
                }

                *mOutStream << "\n";
                PrintHorizontalLine();
            }

            /**
             * @brief 打印表尾
             */
            void PrintFooter(){
                PrintHorizontalLine();
            }

        protected:

        private:
            /**
             * @brief 打印水平线
             */
            void PrintHorizontalLine(){
                *mOutStream << "+";

                for(unsigned int i = 0; i < mTableWidth-1; ++i){
                    *mOutStream << "-";
                }

                *mOutStream << "+";
                *mOutStream << "\n";
            }

            /**
             * @brief 输出小数
             */
            template<typename TClass>
            void OutputDecimalNumber(TClass Input){
                if (Input < 10*(mColumnWidths.at(mIndexColumn)-1) || Input > 10*(mColumnWidths.at(mIndexColumn))){
                    std::stringstream string_out;
                    string_out
                    << std::setiosflags(std::ios::scientific)
                    << std::setprecision(3)
                    << std::uppercase
                    << std::setw(mColumnWidths.at(mIndexColumn))
                    << Input;

                    std::string string_to_print = string_out.str();
                    *mOutStream << string_to_print;
                } else {
                    *mOutStream
                    << std::setiosflags(std::ios::scientific)
                    << std::setprecision(3)
                    << std::uppercase
                    << std::setw(mColumnWidths.at(mIndexColumn))
                    << Input;
                }

                if(mIndexColumn == GetNumColumns()-1){
                    *mOutStream << "|\n";
                    mIndexRow = mIndexRow + 1;
                    mIndexColumn = 0;
                } else {
                    *mOutStream << mSeparator;
                    mIndexColumn = mIndexColumn + 1;
                }
            }


            friend class Serializer;

            void save(Serializer& rSerializer) const{
                rSerializer.save("ColumnHeaders", mColumnHeaders);
                rSerializer.save("ColumnWidths", mColumnWidths);
                rSerializer.save("Separator", mSeparator);
                rSerializer.save("IndexRow", mIndexRow);
                rSerializer.save("IndexColumn", mIndexColumn);
                rSerializer.save("TableWidth", mTableWidth);
                rSerializer.save("FlushLeft", mFlushLeft);
                rSerializer.save("BoldFont", mBoldFont);
            }

            void load(Serializer& rSerializer){
                rSerializer.load("ColumnHeaders", mColumnHeaders);
                rSerializer.load("ColumnWidths", mColumnWidths);
                rSerializer.load("Separator", mSeparator);
                rSerializer.load("IndexRow", mIndexRow);
                rSerializer.load("IndexColumn", mIndexColumn);
                rSerializer.load("TableWidth", mTableWidth);
                rSerializer.load("FlushLeft", mFlushLeft);
                rSerializer.load("BoldFont", mBoldFont);    
            }

        private:
            /**
             * @brief 输出流指针
             */
            std::ostream* mOutStream;

            /**
             * @brief 表头信息
             */
            std::vector<std::string> mColumnHeaders;

            /**
             * @brief 列宽信息
             */
            std::vector<int> mColumnWidths;

            /**
             * @brief 分隔符
             */
            std::string mSeparator;

            /**
             * @brief 当前行索引
             */
            unsigned int mIndexRow;

            /**
             * @brief 当前列索引
             */
            unsigned int mIndexColumn;

            /**
             * @brief 表格的总宽度
             */
            unsigned int mTableWidth;

            /**
             * @brief 是否左对齐
             */
            bool mFlushLeft;

            /**
             * @brief 是否使用粗体字
             */
            bool mBoldFont;

    };

} // namespace Quest


#endif //QUEST_TABLE_STREAM_HPP