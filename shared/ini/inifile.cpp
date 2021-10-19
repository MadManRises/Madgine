#include "inilib.h"

#include "inifile.h"

#include "inisection.h"

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
        std::ofstream stream(path.str());
        assert(stream);
        for (const std::pair<const std::string, IniSection> &p : mSections) {
            stream << "[" << p.first << "]\n";
            p.second.save(stream);
        }
    }

    bool IniFile::loadFromDisk(const Filesystem::Path &path)
    {
        std::ifstream stream(path.str());
        if (!stream)
            return false;
        mSections.clear();
        std::string line;
        while (std::getline(stream, line)) {
            if (!StringUtil::startsWith(line, "[") || !StringUtil::endsWith(line, "]"))
                std::terminate();
            std::string sectionName = line.substr(1, line.size() - 2);
            auto pib = mSections.try_emplace(sectionName, &stream);
            if (!pib.second) {
                LOG_WARNING("Ini-File '" << path.c_str() << "' contains section '" << sectionName << "' twice. Second instance is ignored!");
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