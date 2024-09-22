/*---------------------------------------
实现Fowler–Noll–Vo (FNV-1a) 哈希算法
适用于生成字符串、数据块等的唯一标识符
---------------------------------------*/

#ifndef FNV_1A_HASH_HPP
#define FNV_1A_HASH_HPP

// 系统头文件
#include <cstdint>


namespace Quest{

    class FNV1a32Hash{
        public:
            using HashType = std::uint32_t;

            FNV1a32Hash() = delete;

            virtual ~FNV1a32Hash() = delete;

            FNV1a32Hash& operator = (const FNV1a32Hash&) = delete;

            FNV1a32Hash(const FNV1a32Hash&) = delete;

            static constexpr HashType CalculateHash(const char* const TheString){
                return CalculateHash(mFNV320fsetBasis, TheString);
            }

        protected:

        private:
            static constexpr HashType mFNV320fsetBasis = 0x811c9dc5;
            static constexpr HashType mFNV32Prime = 0x01000193;

            static constexpr HashType CalculateHash(const HashType Value, const char* const TheString){
                return (TheString[0] == '\0') ? Value 
                        : CalculateHash((Value ^ static_cast<HashType>(TheString[0])) * mFNV32Prime, TheString + 1);
            }
    };

    class FNV1a64Hash{
        public:
            using HashType = std::uint64_t;

            FNV1a64Hash() = delete;

            virtual ~FNV1a64Hash() = delete;

            FNV1a64Hash& operator = (const FNV1a64Hash&) = delete;

            FNV1a64Hash(const FNV1a64Hash&) = delete;

            static constexpr HashType CalculateHash(const char* const TheString){
                return CalculateHash(mFNV640fsetBasis, TheString);
            }

        protected:

        private:
            static constexpr HashType mFNV640fsetBasis = 0xcbf29ce484222325;
            static constexpr HashType mFNV64Prime = 0x100000001b3;

            static constexpr HashType CalculateHash(const HashType Value, const char* const TheString){
                return (TheString[0] == '\0') ? Value 
                        : CalculateHash((Value ^ static_cast<HashType>(TheString[0])) * mFNV64Prime, TheString + 1);
            }
    };

}

#endif // FNV_1A_HASH_HPP