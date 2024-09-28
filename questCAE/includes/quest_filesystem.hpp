/*---------------------------------
处理与文件系统相关的操作
（删除原文件中的filesystem命名空间）
-----------------------------------*/

#ifndef QUEST_QUEST_FILESYSTEM_HPP
#define QUEST_QUEST_FILESYSTEM_HPP

// 系统头文件
#include <filesystem>
#include <string>
#include <vector>

// 项目头文件
#include "includes/define.hpp"

namespace Quest{

    class QUEST_API FilesystemExtensions{
        public:
            FilesystemExtensions() = delete;

            FilesystemExtensions(const FilesystemExtensions&) = delete;

            FilesystemExtensions& operator=(const FilesystemExtensions&) = delete;

            //该函数接受一个路径 rPath，并返回该路径下所有文件和目录的列表
            [[nodiscard]] static std::vector<std::filesystem::path> ListDirectory(const std::filesystem::path& rPath);
            
            //该函数用于在分布式计算环境（如 MPI 中）安全地创建目录
            static void MPISafeCreateDirectories(const std::filesystem::path& rPath);

            //该函数用于递归解析符号链接（symlink）。如果 rPath 是一个符号链接，函数将一直解析该链接，直到找到实际的文件路径
            [[nodiscard]] static std::filesystem::path ResolveSymlinks(const std::filesystem::path& rPath);

        protected:

        private:

    };


}  // namespace Quest


#endif  // QUEST_FILESYSTEM_HPP