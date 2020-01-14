#pragma once

#include "Interfaces/filesystem/path.h"

namespace Engine {
namespace Ini {

    struct MODULES_EXPORT IniFile {

        IniFile(const Filesystem::Path &path);
        IniFile(const IniFile &) = delete;
        ~IniFile();

		IniFile &operator=(const IniFile &) = delete;

        IniSection &operator[](const std::string &key);

        void clear();

        void saveToDisk() const;
        void loadFromDisk();

        const Filesystem::Path &path();

        std::map<std::string, IniSection>::iterator begin();
        std::map<std::string, IniSection>::iterator end();

    private:
        Filesystem::Path mPath;

        std::map<std::string, IniSection> mSections;
    };

}
}