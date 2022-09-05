#include "../moduleslib.h"

#include "inifile.h"

#include "inisection.h"

#include "Interfaces/filesystem/api.h"

namespace Engine {
namespace Ini {
    IniFile::IniFile()
    {
    }

    IniFile::~IniFile()
    {
    }

    IniSection &IniFile::operator[](const std::string &key)
    {
        return mSections[key];
    }

    void IniFile::clear()
    {
        mSections.clear();
    }

    void IniFile::saveToDisk(const Filesystem::Path &path) const
    {
        Stream stream = Filesystem::openFileWrite(path);
        assert(stream);
        for (const std::pair<const std::string, IniSection> &p : mSections) {
            stream << "[" << p.first << "]\n";
            p.second.save(stream);
        }
    }

    bool IniFile::loadFromDisk(const Filesystem::Path &path)
    {
        Stream stream = Filesystem::openFileRead(path);
        if (!stream)
            return false;
        mSections.clear();
        std::string line;
        while (std::getline(stream.stream(), line)) {
            if (!StringUtil::startsWith(line, "[") || !StringUtil::endsWith(line, "]"))
                std::terminate();
            std::string sectionName { StringUtil::substr(line, 1, -1) };
            auto pib = mSections.try_emplace(sectionName);
            if (!pib.second) {
                LOG_WARNING("Ini-File '" << path.c_str() << "' contains section '" << sectionName << "' twice. Second instance is ignored!");
            } else {
                pib.first->second.load(stream);
            }
        }
        return true;
    }

    std::map<std::string, IniSection>::iterator IniFile::begin()
    {
        return mSections.begin();
    }

    std::map<std::string, IniSection>::iterator IniFile::end()
    {
        return mSections.end();
    }

    std::map<std::string, IniSection>::const_iterator IniFile::begin() const
    {
        return mSections.begin();
    }

    std::map<std::string, IniSection>::const_iterator IniFile::end() const
    {
        return mSections.end();
    }

}
}