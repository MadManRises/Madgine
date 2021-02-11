#pragma once

#include "Interfaces/filesystem/path.h"

namespace Engine {
namespace Ini {

    struct INI_EXPORT IniFile {

        IniFile();        
        ~IniFile();

		IniFile &operator=(const IniFile &) = delete;

        IniSection &operator[](const std::string &key);

        void clear();

        void saveToDisk(const Filesystem::Path &path) const;
        bool loadFromDisk(const Filesystem::Path &path);

        std::map<std::string, IniSection>::iterator begin();
        std::map<std::string, IniSection>::iterator end();
        std::map<std::string, IniSection>::const_iterator begin() const;
        std::map<std::string, IniSection>::const_iterator end() const;

    private:
        std::map<std::string, IniSection> mSections;
    };

}
}