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

/**
 * @brief 用于在错误消息中获取正确的行号的宏
 */
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

/**
 * @brief 基于MPI_Reduce的方法，支持求和、最大值和最小值操作
 */
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

/**
 * @brief 基于MPI_Allreduce的方法，支持求和、最大值和最小值操作
 */
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

/**
 * @brief 基于MPI_Scan的方法，支持求和操作
 */
#ifndef QUEST_BASE_DATA_COMMUNICATOR_DECLARE_SCANSUM_INTERFACE_FOR_TYPE
#define QUEST_BASE_DATA_COMMUNICATOR_DECLARE_SCANSUM_INTERFACE_FOR_TYPE(...) \
    virtual __VA_ARGS__ ScanSum(const __VA_ARGS__& rLocalValue) const {return rLocalValue;}   \
    virtual std::vector<__VA_ARGS__> ScanSum(const std::vector<__VA_ARGS__>& rLocalValues) const {return rLocalValues;}   \
    virtual void ScanSum(const std::vector<__VA_ARGS__>& rLocalValues,std::vector<__VA_ARGS__>& rPartialSums) const {    \
        QUEST_DATA_COMMUNICATOR_DEBUG_SIZE_CHECK(rLocalValues.size(),rPartialSums.size(),"ScanSum");    \
        rPartialSums = ScanSum(rLocalValues);    \
    }
#endif

/**
 * @brief 基于MPI_Sendrecv、MPI_Send、MPI_Recv的方法，支持点对点通信
 */
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


/**
 * @brief 基于MPI_Bcast的方法，支持广播操作
 */
#ifndef QUEST_BASE_DATA_COMMUNICATOR_DECLARE_BROADCAST_INTERFACE_FOR_TYPE
#define QUEST_BASE_DATA_COMMUNICATOR_DECLARE_BROADCAST_INTERFACE_FOR_TYPE(...) \
    virtual void BroadcastImpl(__VA_ARGS__& rBuffer, const int SourceRank) const {}   \
    virtual void BroadcastImpl(std::vector<__VA_ARGS__>& rBuffer, const int SourceRank) const {}
#endif


/**
 * @brief 基于MPI_Scatter、MPI_Scatterv的方法，支持分发操作
 */
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


/**
 * @brief 基于MPI_Gather、MPI_Gatherv和MPI_Allgather的方法，支持收集操作
 */
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

    /**
     * @brief 用于MPI通信的串行版本的封装类
     * @details 该类不执行任何实际的通信操作，作用相当于一个占位符
     */
    class QUEST_API(QUEST_CORE) DataCommunicator{
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

            /**
             * @brief 创建并返回一个 DataCommunicator 实例
             */
            static DataCommunicator::UniquePointer Create(){
                return Quest::make_unique<DataCommunicator>();
            }


            /**
             * @brief 暂停程序执行，知道所有线程都到达此调用
             * @details MPI_Barrier的封装
             */
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

            /**
             * @brief Reduce 操作，用于在分布式计算中对数据进行归约操作
             */
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

            /**
             * @brief AllReduce 操作，用于在分布式计算中对数据进行归约操作
             */
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

            /**
             * @brief MPI_Bcast 的封装，将缓冲区同步到广播进程持有的值
             */
            template<typename TObject>
            void Broadcast(TObject& rBroadcastObject, const int SourceRank) const{
                this->BroadcastImpl(rBroadcastObject, SourceRank);
            }

            /**
             * @brief MPI_Sendrecv 的封装,与其他进程交换数据
             */
            template<typename TObject>
            TObject SendRecv(
                const TObject& rSendObject, const int SendDestination, const int SendTag,
                const int RecvSource, const int RecvTag) const
            {
                return this->SendRecvImpl(rSendObject, SendDestination, SendTag, RecvSource, RecvTag);
            }
            
            /**
             * @brief MPI_Sendrecv 的封装,与其他进程交换数据
             */
            template<typename TObject>
            void SendRecv(const TObject& rSendObject, const int SendDestination, const int RecvSource) const{
                return this->SendRecvImpl(rSendObject, SendDestination, 0, RecvSource, 0);
            }

            /**
             * @brief MPI_Sendrecv 的封装,与其他进程交换数据
             */
            template<typename TObject>
            void SendRecv(const TObject& rSendObject, const int SendDestination, const int SendTag,
                TObject& rRecvObject, const int RecvSource, const int RecvTag) const
            {
                this->SendRecvImpl(rSendObject, SendDestination, SendTag, rRecvObject, RecvSource, RecvTag);
            }


            /**
             * @brief MPI_Sendrecv 的封装,与其他进程交换数据
             */
            template<typename TObject>
            void SendRecv(const TObject& rSendObject, const int SendDestination, TObject& rRecvObject,
                const int RecvSource) const
            {
                this->SendRecvImpl(rSendObject, SendDestination, 0, rRecvObject, RecvSource, 0);
            }

            /**
             * @brief MPI_Send 函数的封装，用于在分布式计算中发送数据
             */
            template<typename TObject>
            void Send(const TObject& rSendObject, const int Destination, const int Tag = 0) const{
                this->SendImpl(rSendObject, Destination, Tag);
            }

            /**
             * @brief MPI_Recv 函数的封装，用于在分布式计算中接收数据
             */
            template<typename TObject>
            void Recv(TObject& rRecvObject, const int Source, const int Tag = 0) const{
                this->RecvImpl(rRecvObject, Source, Tag);
            }
            
            /**
             * @brief 返回进程号
             */
            virtual int Rank() const{
                return 0;
            }

            /**
             * @brief 返回进程数
             */
            virtual int Size() const{
                return 1;
            }

            /**
             * @brief 判断当前 DataCommunicator 是否处于分布式环境中
             */
            virtual bool IsDistrubuted() const{
                return false;
            }

            /**
             * @brief 判断当前 DataCommunicator 是否在当前进程（rank）上定义
             */
            virtual bool IsDefinedOnThisRank() const{
                return true;
            }

            /**
             * @brief 检查当前进程是否使用的是 MPI_COMM_NULL
             * @details 在 MPI 中，如果某个进程没有参与通信，则该进程的通信器是 MPI_COMM_NULL
             */
            virtual bool IsNullOnThisRank() const{
                return false;
            }

            /**
             * @brief 获取子通信器
             */
            virtual const DataCommunicator& GetSubDataCommunicator(
                const std::vector<int>& rRanks, const std::string& rName) const
            {
                return *this;
            }

            /**
             * @brief 检测到某个进程出现错误时，强制所有进程停止
             * @details 检查给定的条件，如果条件为 true，在根进程广播错误信息，并强制其他进程停止
             */
            virtual bool BroadcastErrorIfTrue(bool Condition, const int SourceRank) const{
                return Condition;
            }

            /**
             * @brief 检测到某个进程出现错误时，强制所有进程停止
             * @details 检查给定的条件，如果条件为假，在根进程广播错误信息，并强制其他进程停止
             */
            virtual bool BroadcastErrorIfFalse(bool Condition, const int SourceRank) const{
                return Condition;
            }

            /**
             * @brief 如果条件在某些进程中为真，且在其他进程中为假，则在条件为假的进程上抛出错误
             */
            virtual bool ErrorIfTrueOnAnyRank(bool Condition) const{
                return Condition;
            }

            /**
             * @brief 如果条件在某些进程中为假，且在其他进程中为真，则在条件为真的进程上抛出错误
             */
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


            /**
             * @brief MPI_Bcast 的封装，将缓冲区同步到广播进程持有的值
             */
            virtual void BroadcastImpl(std::string& rBuffer, const int SourceRank) const {}


            /**
             * @brief MPI_Bcast 的封装，将缓冲区同步到广播进程持有的值
             */
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


            /**
             * @brief MPI_Sendrecv 的封装,与其他进程交换数据
             */
            virtual void SendRecvImpl(
                const std::string& rSendValues, const int SendDestination, const int SendTag,
                std::string& rRecvValues, const int RecvSource, const int RecvTag) const 
            {
                QUEST_DATA_COMMUNICATOR_DEBUG_SIZE_CHECK(rSendValues.size(),rRecvValues.size(),"SendRecv");
                rRecvValues = SendRecvImpl(rSendValues, SendDestination, SendTag, RecvSource, RecvTag);
            }


            /**
             * @brief MPI_Sendrecv 的封装,与其他进程交换数据
             */
            virtual std::string SendRecvImpl(
                const std::string& rSendValues, const int SendDestination, const int SendTag,
                const int RecvSource, const int RecvTag) const
            {
                QUEST_ERROR_IF(Rank() != SendDestination || Rank() != RecvSource)
                << "Communication between different ranks is not possible with a serial DataCommunicator." << std::endl;

                return rSendValues;
            }


            /**
             * @brief MPI_Sendrecv 的封装,与其他进程交换数据
             */
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


            /**
             * @brief MPI_Send 函数的封装，用于在分布式计算中发送数据
             */
            virtual void SendImpl(const std::string& rSendValues, const int SendDestination, const int SendTag) const{
                QUEST_ERROR_IF(Rank() != SendDestination)
                << "Communication between different ranks is not possible with a serial DataCommunicator." << std::endl;
            }


            /**
             * @brief MPI_Send 函数的封装，用于在分布式计算中发送数据
             */
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


            /**
             * @brief MPI_Recv 函数的封装，用于在分布式计算中接收数据
             */
            virtual void RecvImpl(std::string& rRecvValues, const int RecvSource, const int RecvTag = 0) const{
                QUEST_ERROR << "Calling serial DataCommunicator::Recv, which has no meaningful return." << std::endl;
            }


            /**
             * @brief MPI_Recv 函数的封装，用于在分布式计算中接收数据
             */
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