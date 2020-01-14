#pragma once

#include "../serialize/serializemanager.h"
#include "Interfaces/filesystem/path.h"

namespace Engine {
namespace Filesystem {
    struct MODULES_EXPORT FileManager : Serialize::SerializeManager {    
        FileManager(const std::string &name);
        FileManager(const FileManager &) = delete;
        FileManager(FileManager &&) noexcept = default; 
        virtual ~FileManager() = default;

        void operator=(const FileManager &) = delete;

        std::optional<Serialize::SerializeInStream> openRead(const Path &path, std::unique_ptr<Serialize::Formatter> format);
        std::optional<Serialize::SerializeOutStream> openWrite(const Path &path, std::unique_ptr<Serialize::Formatter> format);
    };
}
}
