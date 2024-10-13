/*-------------------------------------------------------------
封装了 TableStream 类的功能，提供了多种与表格相关的操作，
如添加列、添加数据、打印表格的头和尾
-------------------------------------------------------------*/

#ifndef QUEST_TABLE_STREAM_UTILITY_HPP
#define QUEST_TABLE_STREAM_UTILITY_HPP

// 项目头文件
#include "includes/serializer.hpp"
#include "includes/table_stream.hpp"

namespace Quest{

    using TableStreamType = TableStream;

    class TableStreamUtility{
        public:
            QUEST_CLASS_POINTER_DEFINITION(TableStreamUtility);

            explicit TableStreamUtility(const bool UseBoldFont = true):
                mTable(&std::cout,"|",UseBoldFont){}

            virtual ~TableStreamUtility() = default;

            void PrintHeader(){
                mTable.PrintHeader();
            }

            void PrintFooter(){
                mTable.PrintFooter();
            }

            template<typename TClass>
            void AddToRow(TClass ThisClass){
                mTable << ThisClass;
            }

            void AddColumn(const std::string& ThisName, unsigned int ThisSpace){
                mTable.AddColumn(ThisName,ThisSpace);
            }

            TableStreamType& GetTable(){
                return mTable;
            }

            virtual std::string Info() const{
                return "TableStreamUtility";
            }

            virtual void PrintInfo(std::ostream& rOstream) const{
                rOstream << Info() << std::endl;
            }

            virtual void PrintData(std::ostream& rOstream) const{
                rOstream << Info() << std::endl;
            }

        protected:

        private:
            friend class Serializer;

            void save(Serializer& rSerializer) const{
                rSerializer.save("Table", mTable);
            }

            void load(Serializer& rSerializer){ 
                rSerializer.load("Table", mTable);
            }

        private:
            TableStreamType mTable;

    };

};


#endif //QUEST_TABLE_STREAM_UTILITY_HPP