/*----------------------------------
quest_filesystem.hpp头文件的实现
------------------------------------*/

// 系统头文件
#include <algorithm>
#include <chrono>
#include <set>
#include <thread>

// 项目头文件
#include "includes/quest_filesystem.hpp"

namespace Quest{

    std::vector<std::filesystem::path> FilesystemExtensions::ListDirectory(const std::filesystem::path& rPath){
        std::vector<std::filesystem::path> result;
        for(const auto& entry : std::filesystem::directory_iterator(rPath)){
            result.push_back(entry.path());
        }
            
        return result;
    }

    void FilesystemExtensions::MPISafeCreateDirectories(const std::filesystem::path& rPath){
        if(!std::filesystem::exists(rPath)){
            std::filesystem::create_directories(rPath);
        }
        if(std::filesystem::is_directory(rPath)){
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }

    std::filesystem::path FilesystemExtensions::ResolveSymlinks(const std::filesystem::path& rPath){
        auto status = std::filesystem::symlink_status(rPath);
        QUEST_ERROR_IF(status.type() == std::filesystem::file_type::not_found) << "File not found: " << rPath;

        std::filesystem::path path = rPath;
        std::set<std::filesystem::path> symlinks;

        while(status.type() == std::filesystem::file_type::symlink){
            const auto insert_result = symlinks.insert(path);
            QUEST_ERROR_IF_NOT(insert_result.second) << rPath << " leads to cyclic symlinks";
            path = std::filesystem::read_symlink(path);
            status = std::filesystem::symlink_status(path);
        }

        return path;
    }

} // namespace Quest