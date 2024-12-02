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

    /**
     * @brief 处理与文件系统相关操作的工具类
     */
    class QUEST_API(QUEST_CORE) FilesystemExtensions{
        public:
            FilesystemExtensions() = delete;

            FilesystemExtensions(const FilesystemExtensions&) = delete;

            FilesystemExtensions& operator=(const FilesystemExtensions&) = delete;


            /**
             * @brief 展开路径
             * @details 该函数接受一个路径 rPath，并返回该路径的绝对路径
             */
            [[nodiscard]] static std::vector<std::filesystem::path> ListDirectory(const std::filesystem::path& rPath);
            

            /**
             * @brief 分布式计算环境安全地创建目录
             */
            static void MPISafeCreateDirectories(const std::filesystem::path& rPath);


            /**
             * @brief 递归解析目录
             * @details 如果 rPath 是一个符号链接，函数将一直解析该链接，直到找到实际的文件路径（定位symlink标志）
             * ln -s /home/user/documents/project /shortcut/project
             * lrwxrwxrwx 1 user group 11 Nov 24 12:00 symlink -> /target/path （转换后的信息，symlink后指向的路径才为实际路径）
             */
            [[nodiscard]] static std::filesystem::path ResolveSymlinks(const std::filesystem::path& rPath);

        protected:

        private:

    };


}  // namespace Quest


#endif  // QUEST_FILESYSTEM_HPP