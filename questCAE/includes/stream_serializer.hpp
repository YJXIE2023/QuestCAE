#ifndef QUEST_STREAM_SERIALIZER_HPP
#define QUEST_STREAM_SERIALIZER_HPP

// 系统头文件
#include <string>
#include <cstring>
#include <iostream>

// 项目头文件
#include "includes/define.hpp"
#include "includes/serializer.hpp"

namespace Quest{

    /**
     * @class StreamSerializer
     * @brief 基于Serializer类，将对象序列化为字符串
     */
    class StreamSerializer : public Serializer{
        public:
            QUEST_CLASS_POINTER_DEFINITION(StreamSerializer);

            explicit StreamSerializer(const TraceType& rTrace = SERIALIZER_NO_TRACE):
                Serializer(new std::stringstream(std::ios::binary|std::ios::in|std::ios::out), rTrace)
            {}


            StreamSerializer(const std::string& data, const TraceType& rTrace = SERIALIZER_NO_TRACE):
                Serializer(new std::stringstream(data, std::ios::binary|std::ios::in|std::ios::out), rTrace){
                *(this->pGetBuffer()) << data << std::endl;
            }


            std::string GetStringRepresentation(){
                return ((std::stringstream*)(this->pGetBuffer()))->str();
            }

            virtual ~StreamSerializer(){}


        protected:

        private:
            StreamSerializer& operator = (const StreamSerializer& rhs) = delete;

            StreamSerializer(const StreamSerializer& rhs) = delete;
    };

} // namespace Quest


#endif // STREAM_SERIALIZER_HPP