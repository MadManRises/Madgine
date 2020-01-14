#include "../moduleslib.h"

#include "inifile.h"

#include "Interfaces/stringutil.h"

#include "inisection.h"

namespace Engine {
namespace Ini {

    IniFile::IniFile(const Filesystem::Path &path)
        : mPath(path)
    {
        loadFromDisk();
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

    void IniFile::saveToDisk() const
    {
        std::ofstream stream(mPath.str());
        assert(stream);
        for (const std::pair<const std::string, IniSection> &p : mSections) {
            stream << "[" << p.first << "]\n";
            p.second.save(stream);
        }
    }

    void IniFile::loadFromDisk()
    {
        mSections.clear();
        std::ifstream stream(mPath.str());
        std::string line;
        while (std::getline(stream, line)) {
            if (!StringUtil::startsWith(line, "[") || !StringUtil::endsWith(line, "]"))
                std::terminate();
            std::string sectionName = line.substr(1, line.size() - 2);
            auto pib = mSections.try_emplace(sectionName, &stream);
            if (!pib.second) {
                LOG_WARNING("Ini-File '" << mPath.c_str() << "' contains section '" << sectionName << "' twice. Second instance is ignored!");
            }
        }
    }

    const Filesystem::Path &IniFile::path()
    {
        return mPath;
    }

    std::map<std::string, IniSection>::iterator IniFile::begin()
    {
        return mSections.begin();
    }

    std::map<std::string, IniSection>::iterator IniFile::end()
    {
        return mSections.end();
    }

}
}