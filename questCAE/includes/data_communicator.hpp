/*-------------------------------------
串行和并行情况下进行数据通信
删除GetDefault方法
-------------------------------------*/

#ifndef QUEST_DATA_COMMUNICATOR_HPP
#define QUEST_DATA_COMMUNICATOR_HPP

// 系统头文件
#include <string>
#include <iostream>
#include <type_traits>

// 项目头文件
#include "container/array_1d.hpp"
#include "container/flags.hpp"
#include "includes/define.hpp"
#include "includes/mpi_serializer.hpp"

// 宏定义
#ifndef QUEST_DATA_COMMUNICATOR_DEBUG_SIZE_CHECK
#define QUEST_DATA_COMMUNICATOR_DEBUG_SIZE_CHECK(Size1,Size2,CheckedFunction) \
    QUEST_DEBUG_ERROR_IF(Size1!=Size2)    \
    << "Input error in call to DataCommunicator::" << CheckedFunction    \
    << ": The sizes of the local and distributed buffers do not match." << std::endl;
#endif

#ifndef QUEST_BASE_DATA_COMMUNICATOR_DECLARE_SYNC_SHAPE_INTERFACE_FOR_TYPE
#define QUEST_BASE_DATA_COMMUNICATOR_DECLARE_SYNC_SHAPE_INTERFACE_FOR_TYPE(...) \
    virtual bool SynchronizeShape(__VA_ARGS__&) const {return false;}    \
    virtual bool SynchronizeShape(const __VA_ARGS__& rSendValue,const int SendDestination, const int SendTag,    \
    __VA_ARGS__& rRecvValue, const int RecvSource, const int RecvTag) const {return false;}    
#endif

// 针对标量和向量的三种归约操作（Sum、Min、Max）的虚函数接口，并提供默认的串行实现
#ifndef QUEST_BASE_DATA_COMMUNICATOR_DECLARE_REDUCE_INTERFACE_FOR_TYPE
#define QUEST_BASE_DATA_COMMUNICATOR_DECLARE_REDUCE_INTERFACE_FOR_TYPE(...) \
    virtual __VA_ARGS__ Sum(const __VA_ARGS__& rLocalValue,const int Root) const {return rLocalValue;}   \
    virtual std::vector<__VA_ARGS__> Sum(const std::vector<__VA_ARGS__>& rLocalValues,const int Root) const {return rLocalValues;}   \
    virtual void Sum(const std::vector<__VA_ARGS__>& rLocalValues,std::vector<__VA_ARGS__>& rGlobalValues,const int Root) const {    \
        QUEST_DATA_COMMUNICATOR_DEBUG_SIZE_CHECK(rLocalValues.size(),rGlobalValues.size(),"Sum");    \
        rGlobalValues = Sum(rLocalValues,Root);    \
    }   \
    virtual __VA_ARGS__ Min(const __VA_ARGS__& rLocalValue,const int Root) const {return rLocalValue;}   \
    virtual std::vector<__VA_ARGS__> Min(const std::vector<__VA_ARGS__>& rLocalValues,const int Root) const {return rLocalValues;}   \
    virtual void Min(const std::vector<__VA_ARGS__>& rLocalValues,std::vector<__VA_ARGS__>& rGlobalValues,const int Root) const {    \
        QUEST_DATA_COMMUNICATOR_DEBUG_SIZE_CHECK(rLocalValues.size(),rGlobalValues.size(),"Min");    \
        rGlobalValues = Min(rLocalValues,Root);    \
    }   \
    virtual __VA_ARGS__ Max(const __VA_ARGS__& rLocalValue,const int Root) const {return rLocalValue;}   \
    virtual std::vector<__VA_ARGS__> Max(const std::vector<__VA_ARGS__>& rLocalValues,const int Root) const {return rLocalValues;}   \
    virtual void Max(const std::vector<__VA_ARGS__>& rLocalValues,std::vector<__VA_ARGS__>& rGlobalValues,const int Root) const {    \
        QUEST_DATA_COMMUNICATOR_DEBUG_SIZE_CHECK(rLocalValues.size(),rGlobalValues.size(),"Max");    \
        rGlobalValues = Max(rLocalValues,Root);    \
    }   
#endif

// 声明基于 MPI_Allreduce 的归约操作接口，支持标量和向量的求和、最小值、最大值操作。还提供了带有位置信息的最小值和最大值操作版本
#ifndef QUEST_BASE_DATA_COMMUNICATOR_DECLARE_ALLREDUCE_INTERFACE_FOR_TYPE
#define QUEST_BASE_DATA_COMMUNICATOR_DECLARE_ALLREDUCE_INTERFACE_FOR_TYPE(...) \
    virtual __VA_ARGS__ SumAll(const __VA_ARGS__& rLocalValue) const {return rLocalValue;}   \
    virtual std::vector<__VA_ARGS__> SumAll(const std::vector<__VA_ARGS__>& rLocalValues) const {return rLocalValues;}   \
    virtual void SumAll(const std::vector<__VA_ARGS__>& rLocalValues,std::vector<__VA_ARGS__>& rGlobalValues) const {    \
        QUEST_DATA_COMMUNICATOR_DEBUG_SIZE_CHECK(rLocalValues.size(),rGlobalValues.size(),"SumAll");    \
        rGlobalValues = SumAll(rLocalValues);    \
    }   \
    virtual __VA_ARGS__ MinAll(const __VA_ARGS__& rLocalValue) const {return rLocalValue;}   \
    virtual std::vector<__VA_ARGS__> MinAll(const std::vector<__VA_ARGS__>& rLocalValues) const {return rLocalValues;}   \
    virtual void MinAll(const std::vector<__VA_ARGS__>& rLocalValues,std::vector<__VA_ARGS__>& rGlobalValues) const {    \
        QUEST_DATA_COMMUNICATOR_DEBUG_SIZE_CHECK(rLocalValues.size(),rGlobalValues.size(),"MinAll");    \
        rGlobalValues = MinAll(rLocalValues);    \
    }   \
    virtual __VA_ARGS__ MaxAll(const __VA_ARGS__& rLocalValue) const {return rLocalValue;}   \
    virtual std::vector<__VA_ARGS__> MaxAll(const std::vector<__VA_ARGS__>& rLocalValues) const {return rLocalValues;}   \
    virtual void MaxAll(const std::vector<__VA_ARGS__>& rLocalValues,std::vector<__VA_ARGS__>& rGlobalValues) const {    \
        QUEST_DATA_COMMUNICATOR_DEBUG_SIZE_CHECK(rLocalValues.size(),rGlobalValues.size(),"MaxAll");    \
        rGlobalValues = MaxAll(rLocalValues);    \
    }
#endif

#ifndef QUEST_BASE_DATA_COMMUNICATOR_DECLARE_ALLREDUCE_LOC_INTERFACE_FOR_TYPE
#define QUEST_BASE_DATA_COMMUNICATOR_DECLARE_ALLREDUCE_LOC_INTERFACE_FOR_TYPE(...) \
    virtual std::pair<__VA_ARGS__,int> MinLocAll(const __VA_ARGS__& rLocalValue) const {return std::pair<__VA_ARGS__,int>(rLocalValue,0);}   \  
    virtual std::pair<__VA_ARGS__,int> MaxLocAll(const __VA_ARGS__& rLocalValue) const {return std::pair<__VA_ARGS__,int>(rLocalValue,0);}   
#endif

// 用于处理并行计算中归并求和操作的包装器
#ifndef QUEST_BASE_DATA_COMMUNICATOR_DECLARE_SCANSUM_INTERFACE_FOR_TYPE
#define QUEST_BASE_DATA_COMMUNICATOR_DECLARE_SCANSUM_INTERFACE_FOR_TYPE(...) \
    virtual __VA_ARGS__ ScanSum(const __VA_ARGS__& rLocalValue) const {return rLocalValue;}   \
    virtual std::vector<__VA_ARGS__> ScanSum(const std::vector<__VA_ARGS__>& rLocalValues) const {return rLocalValues;}   \
    virtual void ScanSum(const std::vector<__VA_ARGS__>& rLocalValues,std::vector<__VA_ARGS__>& rPartialSums) const {    \
        QUEST_DATA_COMMUNICATOR_DEBUG_SIZE_CHECK(rLocalValues.size(),rPartialSums.size(),"ScanSum");    \
        rPartialSums = ScanSum(rLocalValues);    \
    }
#endif

// 对 MPI_Sendrecv、MPI_Send 和 MPI_Recv 等 MPI 函数的包装器
#ifndef QUEST_BASE_DATA_COMMUNICATOR_DECLARE_SENDRECV_INTERFACE_FOR_TYPE
#define QUEST_BASE_DATA_COMMUNICATOR_DECLARE_SENDRECV_INTERFACE_FOR_TYPE(...) \
    virtual __VA_ARGS__ SendRecvImpl(const __VA_ARGS__& rSendValue, const int SendDestination, const int SendTag,    \
        const int RecvSource, const int RecvTag) const {    \
        QUEST_ERROR_IF((Rank() != SendDestination) || (Rank() != RecvSource))    \
        << "Communication between different ranks is not possible with a serial DataCommunicator." << std::endl;    \
        return rSendValue;    \
        } \
    virtual std::vector<__VA_ARGS__> SendRecvImpl(    \
        const std::vector<__VA_ARGS__>& rSendValues, const int SendDestination, const int SendTag,    \
        const int RecvSource, const int RecvTag) const {    \
        QUEST_ERROR_IF((Rank() != SendDestination) || (Rank() != RecvSource))    \
        << "Communication between different ranks is not possible with a serial DataCommunicator." << std::endl;    \
        return rSendValues;    \
        } \
    virtual void SendRecvImpl(    \
        const __VA_ARGS__& rSendValues, const int SendDestination, const int SendTag,    \
        __VA_ARGS__& rRecvValues, const int RecvSource, const int RecvTag) const {    \
        rRecvValues = SendRecvImpl(rSendValues, SendDestination, SendTag, RecvSource, RecvTag);    \
    } \
    virtual void SendRecvImpl(    \
        const std::vector<__VA_ARGS__>& rSendValues, const int SendDestination, const int SendTag,    \
        std::vector<__VA_ARGS__>& rRecvValues, const int RecvSource, const int RecvTag) const {    \
        QUEST_DATA_COMMUNICATOR_DEBUG_SIZE_CHECK(rSendValues.size(),rRecvValues.size(),"SendRecv");    \
        rRecvValues = SendRecvImpl(rSendValues, SendDestination, SendTag, RecvSource, RecvTag);    \
    } \
    virtual void SendImpl(const __VA_ARGS__& rSendValue, const int SendDestination, const int SendTag = 0) const {    \
        QUEST_ERROR_IF(Rank() != SendDestination)    \
        << "Communication between different ranks is not possible with a serial DataCommunicator." << std::endl;    \
    } \
    virtual void SendImpl(const std::vector<__VA_ARGS__>& rSendValues, const int SendDestination, const int SendTag = 0) const {    \
        QUEST_ERROR_IF(Rank() != SendDestination)    \
        << "Communication between different ranks is not possible with a serial DataCommunicator." << std::endl;    \
    } \
    virtual void RecvImpl(__VA_ARGS__& rRecvValues, const int RecvSource, const int RecvTag = 0) const {    \
        QUEST_ERROR << "Calling serial DataCommunicator::Recv, which has no meaningful return." << std::endl;    \
    } \
    virtual void RecvImpl(std::vector<__VA_ARGS__>& rRecvValues, const int RecvSource, const int RecvTag = 0) const {    \
        QUEST_ERROR << "Calling serial DataCommunicator::Recv, which has no meaningful return." << std::endl;    \
    }
#endif


// 提供对 MPI_Bcast（广播操作）的封装
#ifndef QUEST_BASE_DATA_COMMUNICATOR_DECLARE_BROADCAST_INTERFACE_FOR_TYPE
#define QUEST_BASE_DATA_COMMUNICATOR_DECLARE_BROADCAST_INTERFACE_FOR_TYPE(...) \
    virtual void BroadcastImpl(__VA_ARGS__& rBuffer, const int SourceRank) const {}   \
    virtual void BroadcastImpl(std::vector<__VA_ARGS__>& rBuffer, const int SourceRank) const {}
#endif


// 提供了对 MPI_Scatter 和 MPI_Scatterv 的封装接口，用于在并行计算中将数据从一个进程分发到其他进程
#ifndef QUEST_BASE_DATA_COMMUNICATOR_DECLARE_SCATTER_INTERFACE_FOR_TYPE
#define QUEST_BASE_DATA_COMMUNICATOR_DECLARE_SCATTER_INTERFACE_FOR_TYPE(...) \
    virtual std::vector<__VA_ARGS__> Scatter(const std::vector<__VA_ARGS__>& rSendValues, const int SourceRank) const {    \
        QUEST_ERROR_IF(Rank() != SourceRank)    \
        << "Communication between different ranks is not possible with a serial DataCommunicator." << std::endl;    \
        return rSendValues;    \
    } \
    virtual void Scatter(const std::vector<__VA_ARGS__>& rSendValues, std::vector<__VA_ARGS__>& rRecvValues, const int SourceRank) const {    \
        QUEST_DATA_COMMUNICATOR_DEBUG_SIZE_CHECK(rSendValues.size(),rRecvValues.size(),"Scatter");    \
        rRecvValues = Scatter(rSendValues, SourceRank);    \
    } \
    virtual std::vector<__VA_ARGS__> Scatterv(const std::vector<std::vector<__VA_ARGS__>>& rSendValues, const int SourceRank) const {    \
        QUEST_ERROR_IF(Rank() != SourceRank)    \
        << "Communication between different ranks is not possible with a serial DataCommunicator." << std::endl;    \
        QUEST_ERROR_IF( static_cast<unsigned int>(Size()) != rSendValues.size())    \
        << "Unexpected number of sends in DataCommunicator::Scatterv (serial DataCommunicator always assumes a single process)." << std::endl;    \
        return rSendValues[0];    \
    } \
    virtual void Scatterv( const std::vector<__VA_ARGS__>& rSendValues, const std::vector<int>& rSendCounts,     \
        const std::vector<int>& rSendOffsets, std::vector<__VA_ARGS__>& rRecvValues, const int SourceRank) const {    \
        QUEST_DATA_COMMUNICATOR_DEBUG_SIZE_CHECK(rSendValues.size(),rRecvValues.size(),"Scatterv (values check)");    \
        QUEST_DATA_COMMUNICATOR_DEBUG_SIZE_CHECK(rSendCounts.size(),1,"Scatterv (counts check)");    \
        QUEST_DATA_COMMUNICATOR_DEBUG_SIZE_CHECK(rSendOffsets.size(),1,"Scatterv (offsets check)");    \
        QUEST_ERROR_IF(Rank() != SourceRank)    \
        << "Communication between different ranks is not possible with a serial DataCommunicator." << std::endl;    \
        rRecvValues = rSendValues;    \
    }
#endif


// 提供了对 MPI 中常用的聚集（Gather）、分组聚集（Gatherv）以及全局聚集（Allgather 和 Allgatherv）操作的封装接口
#ifndef QUEST_BASE_DATA_COMMUNICATOR_DECLARE_GATHER_INTERFACE_FOR_TYPE
#define QUEST_BASE_DATA_COMMUNICATOR_DECLARE_GATHER_INTERFACE_FOR_TYPE(...) \
    virtual std::vector<__VA_ARGS__> Gather(const std::vector<__VA_ARGS__>& rSendValues, const int DestinationRank) const {    \
        QUEST_ERROR_IF(Rank() != DestinationRank)    \
        << "Communication between different ranks is not possible with a serial DataCommunicator." << std::endl;    \
        return rSendValues;    \
    } \
    virtual void Gather(const std::vector<__VA_ARGS__>& rSendValues, std::vector<__VA_ARGS__>& rRecvValues, const int DestinationRank) const {    \
        QUEST_DATA_COMMUNICATOR_DEBUG_SIZE_CHECK(rSendValues.size(),rRecvValues.size(),"Gather");    \
        rRecvValues = Gather(rSendValues, DestinationRank);    \
    } \
    virtual std::vector<std::vector<__VA_ARGS__>> Gatherv(const std::vector<__VA_ARGS__>& rSendValues, const int DestinationRank) const {    \
        QUEST_ERROR_IF(Rank() != DestinationRank)    \
        << "Communication between different ranks is not possible with a serial DataCommunicator." << std::endl;    \
        return std::vector<std::vector<__VA_ARGS__>>{rSendValues};    \
    } \
    virtual void Gatherv(    \
        const std::vector<__VA_ARGS__>& rSendValues, const std::vector<int>& rRecvCounts,    \
        const std::vector<int>& rRecvOffsets, std::vector<__VA_ARGS__>& rRecvValues, const int DestinationRank) const {    \
        QUEST_DATA_COMMUNICATOR_DEBUG_SIZE_CHECK(rSendValues.size(),rRecvValues.size(),"Gatherv (values check)");    \
        QUEST_DATA_COMMUNICATOR_DEBUG_SIZE_CHECK(rRecvCounts.size(),1,"Gatherv (counts check)");    \
        QUEST_DATA_COMMUNICATOR_DEBUG_SIZE_CHECK(rRecvOffsets.size(),1,"Gatherv (offsets check)");    \
        QUEST_ERROR_IF(Rank() != DestinationRank)    \
        << "Communication between different ranks is not possible with a serial DataCommunicator." << std::endl;    \
        rRecvValues = rSendValues;    \
    } \
    virtual std::vector<__VA_ARGS__> AllGather(const std::vector<__VA_ARGS__>& rSendValues) const {return rSendValues;} \
    virtual void AllGather(const std::vector<__VA_ARGS__>& rSendValues, std::vector<__VA_ARGS__>& rRecvValues) const {    \
        QUEST_DATA_COMMUNICATOR_DEBUG_SIZE_CHECK(rSendValues.size(),rRecvValues.size(),"AllGather");    \
        rRecvValues = AllGather(rSendValues);    \
    } \
    virtual std::vector<std::vector<__VA_ARGS__>> AllGatherv(const std::vector<__VA_ARGS__>& rSendValues) const {    \
        return std::vector<std::vector<__VA_ARGS__>>{rSendValues};    \
    } \
    virtual void AllGatherv(const std::vector<__VA_ARGS__>& rSendValues, std::vector<__VA_ARGS__>& rRecvValues,    \
        const std::vector<int>& rRecvCounts, const std::vector<int>& rRecvOffsets) const {    \
        QUEST_DATA_COMMUNICATOR_DEBUG_SIZE_CHECK(rSendValues.size(),rRecvValues.size(),"AllGatherv (values check)");    \
        QUEST_DATA_COMMUNICATOR_DEBUG_SIZE_CHECK(rRecvCounts.size(),1,"AllGatherv (counts check)");    \
        QUEST_DATA_COMMUNICATOR_DEBUG_SIZE_CHECK(rRecvOffsets.size(),1,"AllGatherv (offsets check)");    \
        rRecvValues = rSendValues;    \
    }
#endif


#ifndef QUEST_BASE_DATA_COMMUNICATOR_DECLARE_PUBLIC_INTERFACE_FOR_TYPE
#define QUEST_BASE_DATA_COMMUNICATOR_DECLARE_PUBLIC_INTERFACE_FOR_TYPE(...) \
    QUEST_BASE_DATA_COMMUNICATOR_DECLARE_REDUCE_INTERFACE_FOR_TYPE(__VA_ARGS__) \
    QUEST_BASE_DATA_COMMUNICATOR_DECLARE_ALLREDUCE_INTERFACE_FOR_TYPE(__VA_ARGS__) \
    QUEST_BASE_DATA_COMMUNICATOR_DECLARE_SCANSUM_INTERFACE_FOR_TYPE(__VA_ARGS__) \
    QUEST_BASE_DATA_COMMUNICATOR_DECLARE_SCATTER_INTERFACE_FOR_TYPE(__VA_ARGS__) \
    QUEST_BASE_DATA_COMMUNICATOR_DECLARE_GATHER_INTERFACE_FOR_TYPE(__VA_ARGS__) \
    QUEST_BASE_DATA_COMMUNICATOR_DECLARE_SYNC_SHAPE_INTERFACE_FOR_TYPE(__VA_ARGS__) 
#endif

#ifndef QUEST_BASE_DATA_COMMUNICATOR_DECLARE_IMPLEMENTATION_FOR_TYPE
#define QUEST_BASE_DATA_COMMUNICATOR_DECLARE_IMPLEMENTATION_FOR_TYPE(...) \
    QUEST_BASE_DATA_COMMUNICATOR_DECLARE_SENDRECV_INTERFACE_FOR_TYPE(__VA_ARGS__) \
    QUEST_BASE_DATA_COMMUNICATOR_DECLARE_BROADCAST_INTERFACE_FOR_TYPE(__VA_ARGS__) 
#endif


namespace Quest{

    class QUEST_API DataCommunicator{
        private:
            template<typename T>
            class serialization_is_required{
                public: 
                    constexpr static bool value = std::is_compound<T>::value && !is_vector_of_directly_communicable_type;

                protected:

                private:
                    template<typename U>
                    struct serialization_traits{
                        constexpr static bool is_std_vactor = false;
                        constexpr static bool value_type_is_compound = false;
                        constexpr static bool value_type_is_bool = false;
                    };

                    template<typename U>
                    struct serialization_traits<std::vector<U>>{
                        constexpr static bool is_std_vactor = true;
                        constexpr static bool value_type_is_compound = std::is_compound<U>::value;
                        constexpr static bool value_type_is_bool = std::is_same<U, bool>::value;
                    };

                    constexpr static bool is_vector_of_simple_types = serialization_traits<T>::is_std_vactor &&!serialization_traits<T>::value_type_is_compound;
                    constexpr static bool is_vector_of_bools = Serialization_traits<T>::is_std::vactor && serialization_traits<T>::value_type_is_bool;

                    constexpr static bool is_vector_of_directly_communicable_type = is_vector_of_simple_types || !is_vector_of_bools;

            };

            template<bool value> struct TypeFromBool {};

            template<typename T> void CheckSerializationForSimpleType(const T& rSerializedType, TypeFromBool<true>) const {}

            template<typename T>
            QUEST_DEPRECATED_MSG("Calling serialization-based communication for a simple type. Please implement direct communication support for this type.")
            void CheckSerializationForSimpleType(const T& rSerializedType, TypeFromBool<false>) const {}

        public:

            QUEST_CLASS_POINTER_DEFINITION(DataCommunicator);

            DataCommunicator() {}

            virtual ~DataCommunicator() {}

            // 工厂方法，创建并返回一个 DataCommunicator 对象
            static DataCommunicator::UniquePointer Create(){
                return Quest::make_unique<DataCommunicator>();
            }


            // 用于暂停程序执行，直到所有线程或进程都到达此调用点
            // 对 MPI 的 MPI_Barrier 函数的封装
            virtual void Barrier() const {}


            // 基本类型接口
            QUEST_BASE_DATA_COMMUNICATOR_DECLARE_PUBLIC_INTERFACE_FOR_TYPE(char)
            QUEST_BASE_DATA_COMMUNICATOR_DECLARE_PUBLIC_INTERFACE_FOR_TYPE(int)
            QUEST_BASE_DATA_COMMUNICATOR_DECLARE_PUBLIC_INTERFACE_FOR_TYPE(unsigned int)
            QUEST_BASE_DATA_COMMUNICATOR_DECLARE_PUBLIC_INTERFACE_FOR_TYPE(long unsigned int)
            QUEST_BASE_DATA_COMMUNICATOR_DECLARE_PUBLIC_INTERFACE_FOR_TYPE(double)
            QUEST_BASE_DATA_COMMUNICATOR_DECLARE_PUBLIC_INTERFACE_FOR_TYPE(Array1d<double, 3>)
            QUEST_BASE_DATA_COMMUNICATOR_DECLARE_PUBLIC_INTERFACE_FOR_TYPE(Array1d<double, 4>)
            QUEST_BASE_DATA_COMMUNICATOR_DECLARE_PUBLIC_INTERFACE_FOR_TYPE(Array1d<double, 6>)
            QUEST_BASE_DATA_COMMUNICATOR_DECLARE_PUBLIC_INTERFACE_FOR_TYPE(Array1d<double, 9>)
            QUEST_BASE_DATA_COMMUNICATOR_DECLARE_PUBLIC_INTERFACE_FOR_TYPE(Vector)
            QUEST_BASE_DATA_COMMUNICATOR_DECLARE_PUBLIC_INTERFACE_FOR_TYPE(Matrix)

            // MinLoc 和 MaxLoc 操作，这些是 MPI 中的规约操作，用于处理分布式计算中的数据汇总和同步
            QUEST_BASE_DATA_COMMUNICATOR_DECLARE_ALLREDUCE_LOC_INTERFACE_FOR_TYPE(char)
            QUEST_BASE_DATA_COMMUNICATOR_DECLARE_ALLREDUCE_LOC_INTERFACE_FOR_TYPE(int)
            QUEST_BASE_DATA_COMMUNICATOR_DECLARE_ALLREDUCE_LOC_INTERFACE_FOR_TYPE(unsigned int)
            QUEST_BASE_DATA_COMMUNICATOR_DECLARE_ALLREDUCE_LOC_INTERFACE_FOR_TYPE(long unsigned int)
            QUEST_BASE_DATA_COMMUNICATOR_DECLARE_ALLREDUCE_LOC_INTERFACE_FOR_TYPE(double)

            // 归约操作（Reduce）
            virtual bool AndReduce(const bool Value, const int Root) const {
                return Value;
            }

            virtual Quest::Flags AndReduce(const Quest::Flags Values, const Quest::Flags Mask, const int Root) const{
                return Values;
            }

            virtual bool OrReduce(const bool Value, const int Root) const {
                return Value;
            }

            virtual Quest::Flags OrReduce(const Quest::Flags Values, const Quest::Flags Mask, const int Root) const{
                return Values;
            }

            // 归约操作（AllReduce）
            virtual bool AndReduceAll(const bool Value) const {
                return Value;
            }

            virtual Quest::Flags AndReduceAll(const Quest::Flags Values, const Quest::Flags Mask) const{
                return Values;
            }

            virtual bool OrReduceAll(const bool Value) const {
                return Value;
            }

            virtual Quest::Flags OrReduceAll(const Quest::Flags Values, const Quest::Flags Mask) const{
                return Values;
            }

            // 用于在并行计算中通过 MPI 进行广播操作
            template<typename TObject>
            void Broadcast(TObject& rBroadcastObject, const int SourceRank) const{
                this->BroadcastImpl(rBroadcastObject, SourceRank);
            }

            // 通过 SendRecvImpl 实现了数据的发送和接收
            template<typename TObject>
            TObject SendRecv(
                const TObject& rSendObject, const int SendDestination, const int SendTag,
                const int RecvSource, const int RecvTag) const
            {
                return this->SendRecvImpl(rSendObject, SendDestination, SendTag, RecvSource, RecvTag);
            }
            
            // 用于在并行计算中进行数据的交换
            template<typename TObject>
            void SendRecv(const TObject& rSendObject, const int SendDestination, const int RecvSource) const{
                return this->SendRecvImpl(rSendObject, SendDestination, 0, RecvSource, 0);
            }

            // 封装了 MPI 的 MPI_Sendrecv 操作
            // 通过 MPI 通信操作同时发送数据到一个指定的进程，并从另一个指定的进程接收数据
            template<typename TObject>
            void SendRecv(const TObject& rSendObject, const int SendDestination, const int SendTag,
                TObject& rRecvObject, const int RecvSource, const int RecvTag) const
            {
                this->SendRecvImpl(rSendObject, SendDestination, SendTag, rRecvObject, RecvSource, RecvTag);
            }


            // 通过 MPI 的 Sendrecv 操作实现数据的同步发送和接收
            template<typename TObject>
            void SendRecv(const TObject& rSendObject, const int SendDestination, TObject& rRecvObject,
                const int RecvSource) const
            {
                this->SendRecvImpl(rSendObject, SendDestination, 0, rRecvObject, RecvSource, 0);
            }

            // 用于在 MPI 环境中发送数据
            template<typename TObject>
            void Send(const TObject& rSendObject, const int Destination, const int Tag = 0) const{
                this->SendImpl(rSendObject, Destination, Tag);
            }

            // 用于在 MPI 环境中接收数据
            template<typename TObject>
            void Recv(TObject& rRecvObject, const int Source, const int Tag = 0) const{
                this->RecvImpl(rRecvObject, Source, Tag);
            }
            
            // 获取当前 MPI 进程的编号
            virtual int Rank() const{
                return 0;
            }

            // 获取 DataCommunicator 的并行大小
            virtual int Size() const{
                return 1;
            }

            // 判断当前 DataCommunicator 是否处于分布式环境中
            virtual bool IsDistrubuted() const{
                return false;
            }

            // 检查当前 DataCommunicator 是否在当前进程（rank）上定义
            virtual bool IsDefinedOnThisRank() const{
                return true;
            }

            // 检查 DataCommunicator 是否在当前进程上是 MPI_COMM_NULL
            // MPI_COMM_NULL 是一个特殊的 MPI 通信上下文，表示当前进程不参与通信
            virtual bool IsNullOnThisRank() const{
                return false;
            }

            // 获取一个基于指定进程（rank）和新通信器名称的子通信器
            virtual const DataCommunicator& GetSubDataCommunicator(
                const std::vector<int>& rRanks, const std::string& rName) const
            {
                return *this;
            }

            // 用于在并行计算中处理错误并进行广播
            virtual bool BroadcastErrorIfTrue(bool Condition, const int SourceRank) const{
                return Condition;
            }

            // 检测某个条件，如果条件为假，在特定的源进程（SourceRank）触发错误，并将错误信息广播到所有其他进程
            virtual bool BroadcastErrorIfFalse(bool Condition, const int SourceRank) const{
                return Condition;
            }

            // 在所有进程中检查条件是否为真，并在所有进程中广播错误信息，以确保错误的一致性处理
            virtual bool ErrorIfTrueOnAnyRank(bool Condition) const{
                return Condition;
            }

            // 在所有进程中检查条件是否为假，并在所有进程中广播错误信息，以确保错误的一致性处理
            virtual bool ErrorIfFalseOnAnyRank(bool Condition) const{
                return Condition;
            }


            virtual std::string Info() const{
                std::stringstream buffer;
                PrintInfo(buffer);
                return buffer.str();
            }

            virtual void PrintInfo(std::ostream& rOstream) const{
                rOstream << "DataCommunicator";
            }

            virtual void PrintData(std::ostream& rOstream) const{
                rOstream << "Serial do-nothing version of the Quest wrapper for MPI communication." << std::endl
                         << "Rank 0 of 1 assumed." << std::endl;
            }

        protected:

            QUEST_BASE_DATA_COMMUNICATOR_DECLARE_IMPLEMENTATION_FOR_TYPE(char)
            QUEST_BASE_DATA_COMMUNICATOR_DECLARE_IMPLEMENTATION_FOR_TYPE(int)
            QUEST_BASE_DATA_COMMUNICATOR_DECLARE_IMPLEMENTATION_FOR_TYPE(unsigned int)
            QUEST_BASE_DATA_COMMUNICATOR_DECLARE_IMPLEMENTATION_FOR_TYPE(long unsigned int)
            QUEST_BASE_DATA_COMMUNICATOR_DECLARE_IMPLEMENTATION_FOR_TYPE(double)
            QUEST_BASE_DATA_COMMUNICATOR_DECLARE_IMPLEMENTATION_FOR_TYPE(Array1d<double, 3>)
            QUEST_BASE_DATA_COMMUNICATOR_DECLARE_IMPLEMENTATION_FOR_TYPE(Array1d<double, 4>)
            QUEST_BASE_DATA_COMMUNICATOR_DECLARE_IMPLEMENTATION_FOR_TYPE(Array1d<double, 6>)
            QUEST_BASE_DATA_COMMUNICATOR_DECLARE_IMPLEMENTATION_FOR_TYPE(Array1d<double, 9>)
            QUEST_BASE_DATA_COMMUNICATOR_DECLARE_IMPLEMENTATION_FOR_TYPE(Vector)
            QUEST_BASE_DATA_COMMUNICATOR_DECLARE_IMPLEMENTATION_FOR_TYPE(Matrix)


            // MPI_Bcast 函数的封装
            virtual void BroadcastImpl(std::string& rBuffer, const int SourceRank) const {}


            // 通过序列化技术将复杂对象打包为字符串，再使用 MPI_Bcast 实现广播
            template<typename TObject>
            void BroadcastImpl(TObject& rBroadcastObject, const int SourceRank) const{
                CheckSerializationForSimpleType(rBroadcastObject, TypeFromBool<serialization_is_required<TObject>::value>());
                if(this->IsDistrubuted()){
                    unsigned int message_size;
                    std::string broadcast_message;
                    int rank = this->Rank();
                    if(rank == SourceRank){
                        MpiSerializer send_serializer;
                        send_serializer.save("data", rBroadcastObject);
                        broadcast_message = send_serializer.GetStringRepresentation();

                        message_size = broadcast_message.size();
                    }

                    this->BroadcastImpl(message_size, SourceRank);

                    if(rank != SourceRank){
                        broadcast_message.resize(message_size);
                    }

                    this->Broadcast(broadcast_message, SourceRank);

                    if(rank != SourceRank){
                        MpiSerializer recv_serializer(broadcast_message);
                        recv_serializer.load("data", rBroadcastObject);
                    }
                }
            }


            // 封装了 MPI_Sendrecv 的方法，用于在并行计算中在不同的 MPI 进程之间交换字符串数据
            virtual void SendRecvImpl(
                const std::string& rSendValues, const int SendDestination, const int SendTag,
                std::string& rRecvValues, const int RecvSource, const int RecvTag) const 
            {
                QUEST_DATA_COMMUNICATOR_DEBUG_SIZE_CHECK(rSendValues.size(),rRecvValues.size(),"SendRecv");
                rRecvValues = SendRecvImpl(rSendValues, SendDestination, SendTag, RecvSource, RecvTag);
            }


            // 封装了 MPI 并行编程中的 MPI_Sendrecv 函数，用于在不同的进程之间发送和接收字符串数据
            // 这段代码的实现是一个串行版本
            virtual std::string SendRecvImpl(
                const std::string& rSendValues, const int SendDestination, const int SendTag,
                const int RecvSource, const int RecvTag) const
            {
                QUEST_ERROR_IF(Rank() != SendDestination || Rank() != RecvSource)
                << "Communication between different ranks is not possible with a serial DataCommunicator." << std::endl;

                return rSendValues;
            }


            // 封装了 MPI 中的 MPI_Sendrecv 函数，用于在不同进程间发送和接收任意可序列化对象
            template<typename TObject>
            TObject SendRecvImpl(
                const TObject& rSendObject, const int SendDestination, const int SendTag,
                const int RecvSource, const int RecvTag) const
            {
                CheckSerializationForSimpleType(rSendObject, TypeFromBool<serialization_is_required<TObject>::value>());
                if(this->IsDistrubuted()){
                    MpiSerializer send_serializer;
                    send_serializer.save("data", rSendObject);
                    std::string send_message = send_serializer.GetStringRepresentation();

                    std::string recv_message = this->SendRecvImpl(send_message, SendDestination, RecvSource);

                    MpiSerializer recv_serializer(recv_message);
                    TObject recv_object;
                    recv_serializer.load("data", recv_object);

                    return recv_object;
                } else {
                    QUEST_ERROR_IF(Rank() != SendDestination || Rank() != RecvSource)
                    << "Communication between different ranks is not possible with a serial DataCommunicator." << std::endl;

                    return rSendObject;
                }
            }


            // 封装了 MPI_Send，但由于当前环境是串行的，它不会执行真正的跨进程通信，并且会在尝试跨进程通信时抛出错误
            virtual void SendImpl(const std::string& rSendValues, const int SendDestination, const int SendTag) const{
                QUEST_ERROR_IF(Rank() != SendDestination)
                << "Communication between different ranks is not possible with a serial DataCommunicator." << std::endl;
            }


            // 实现了一个泛型版本的 SendImpl 函数，用于在分布式环境中通过 MPI 序列化并发送任意对象
            // 将非简单数据对象（如 STL 容器中的对象）序列化为字符串，并通过 MPI 发送到指定的目标进程
            template<typename TObject>
            void SendImpl(const TObject& rSendObject, const int SendDestination, const int SendTag) const{
                CheckSerializationForSimpleType(rSendObject, TypeFromBool<serialization_is_required<TObject>::value>());
                if(this->IsDistrubuted()){
                    MpiSerializer send_serializer;
                    send_serializer.save("data", rSendObject);
                    std::string send_message = send_serializer.GetStringRepresentation();

                    this->SendImpl(send_message, SendDestination, SendTag);
                } else {
                    QUEST_ERROR_IF(Rank() != SendDestination)
                    << "Communication between different ranks is not possible with a serial DataCommunicator." << std::endl;
                }
            }


            // 实现了一个虚函数 RecvImpl，用于接收来自其他进程的字符串数据
            // 这段代码主要用于串行环境，在串行环境中调用 RecvImpl 是没有意义的
            virtual void RecvImpl(std::string& rRecvValues, const int RecvSource, const int RecvTag = 0) const{
                QUEST_ERROR << "Calling serial DataCommunicator::Recv, which has no meaningful return." << std::endl;
            }


            // 实现了一个模板函数 RecvImpl，用于接收任意类型的对象数据
            // 序列化技术来传输复杂对象，并适用于分布式计算环境。在串行环境中，如果试图调用它会抛出错误
            template<typename TObject>
            void RecvImpl(TObject& rRecvObject, const int RecvSource, const int RecvTag = 0) const{
                CheckSerializationForSimpleType(rRecvObject, TypeFromBool<serialization_is_required<TObject>::value>());
                if(this->IsDistrubuted()){
                    std::string recv_message;

                    this->Recv(recv_message, RecvSource, RecvTag);

                    MpiSerializer recv_serializer(recv_message);
                    recv_serializer.load("data", rRecvObject);
                } else {
                    QUEST_ERROR_IF(Rank() != RecvSource)
                    << "Communication between different ranks is not possible with a serial DataCommunicator." << std::endl;
                }
            }


        private:
            DataCommunicator(const DataCommunicator&) = delete;
            DataCommunicator& operator=(const DataCommunicator&) = delete;

    };

    inline std::istream& operator >> (std::istream& rIstream, DataCommunicator& rThis){
        return rIstream;
    }

    inline std::ostream& operator << (std::ostream& rOstream, const DataCommunicator& rThis){
        rThis.PrintInfo(rOstream);
        rOstream << std::endl;
        rThis.PrintData(rOstream);

        return rOstream;
    }


} // namespace Quest


#undef QUEST_DATA_COMMUNICATOR_DEBUG_SIZE_CHECK

#undef QUEST_BASE_DATA_COMMUNICATOR_DECLARE_SYNC_SHAPE_INTERFACE_FOR_TYPE
#undef QUEST_BASE_DATA_COMMUNICATOR_DECLARE_REDUCE_INTERFACE_FOR_TYPE
#undef QUEST_BASE_DATA_COMMUNICATOR_DECLARE_ALLREDUCE_INTERFACE_FOR_TYPE
#undef QUEST_BASE_DATA_COMMUNICATOR_DECLARE_SCANSUM_INTERFACE_FOR_TYPE
#undef QUEST_BASE_DATA_COMMUNICATOR_DECLARE_SENDRECV_INTERFACE_FOR_TYPE
#undef QUEST_BASE_DATA_COMMUNICATOR_DECLARE_BROADCAST_INTERFACE_FOR_TYPE
#undef QUEST_BASE_DATA_COMMUNICATOR_DECLARE_SCATTER_INTERFACE_FOR_TYPE
#undef QUEST_BASE_DATA_COMMUNICATOR_DECLARE_GATHER_INTERFACE_FOR_TYPE
#undef QUEST_BASE_DATA_COMMUNICATOR_DECLARE_PUBLIC_INTERFACE_FOR_TYPE
#undef QUEST_BASE_DATA_COMMUNICATOR_DECLARE_IMPLEMENTATION_FOR_TYPE


#endif // DATA_COMMUNICATOR_HPP