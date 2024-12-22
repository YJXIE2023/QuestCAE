#ifndef QUEST_MPI_SERIALIZER_HPP
#define QUEST_MPI_SERIALIZER_HPP

// 系统头文件
#include <string>
#include <cstring>
#include <iostream>

// 项目头文件
#include "includes/define.hpp"
#include "includes/stream_serializer.hpp"

namespace Quest{

    /**
     * @brief MPI序列化器
     * @details 通过内存中的字符串缓冲区高效地进行对象序列化和反序列化，并支持 MPI 通信和浅拷贝指针序列化
     */
    class MpiSerializer:public StreamSerializer{
        public:
            QUEST_CLASS_POINTER_DEFINITION(MpiSerializer);

            explicit MpiSerializer(const TraceType& rTrace=SERIALIZER_NO_TRACE):StreamSerializer(rTrace){
                Set(Serializer::MPI);
                Set(Serializer::SHALLOW_GLOBAL_POINTER_SERIALIZATION);
            }

            MpiSerializer(const std::string& rString, const TraceType& rTrace=SERIALIZER_NO_TRACE):StreamSerializer(rString, rTrace){
                Set(Serializer::MPI);
                Set(Serializer::SHALLOW_GLOBAL_POINTER_SERIALIZATION);
            }

            virtual ~MpiSerializer(){}

        protected:

        private:
            MpiSerializer& operator=(const MpiSerializer& rhs) = delete;
            MpiSerializer(const MpiSerializer& rhs) = delete;
    };

} // namespace Quest

#endif // MPI_SERIALIZER_HPP