/*-----------------------------------
mpi中将对象序列化为字符串
------------------------------------*/

#ifndef MPI_SERIALIZER_HPP
#define MPI_SERIALIZER_HPP

// 系统头文件
#include <string>
#include <cstring>
#include <iostream>

// 项目头文件
#include "includes/define.hpp"
#include "includes/stream_serializer.hpp"

namespace Quest{

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