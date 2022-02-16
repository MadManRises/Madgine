#include "inilib.h"

#include "inisection.h"

#include "Generic/stream.h"

namespace Engine {
namespace Ini {

    IniSection::IniSection()
    {
    }

    std::string &IniSection::operator[](const std::string &key)
    {
        return mValues[key];
    }

    void IniSection::save(Stream &stream) const
    {
        for (const std::pair<const std::string, std::string> &p : mValues) {
            stream << p.first << "=" << p.second << '\n';
        }
    }

    void IniSection::load(Stream &stream)
    {
        mValues.clear();
        std::streampos save = stream.tell();
        std::string line;
        while (std::getline(stream.stream(), line)) {
            if (line.at(0) == '[') {
                stream.seek(save);
                return;
            }
            size_t pos = line.find('=');
            if (pos == std::string::npos || line.rfind('=') != pos)
                std::terminate();
            mValues[line.substr(0, pos)] = line.substr(pos + 1);
            save = stream.tell();
        }
    }

    std::map<std::string, std::string>::iterator IniSection::begin()
    {
        return mValues.begin();
    }

    std::map<std::string, std::string>::iterator IniSection::end()
    {
        return mValues.end();
    }

    std::map<std::string, std::string>::const_iterator IniSection::begin() const
    {
        return mValues.begin();
    }

    std::map<std::string, std::string>::const_iterator IniSection::end() const
    {
        return mValues.end();
    }

}
}