#include "../moduleslib.h"

#include "inisection.h"

namespace Engine {
namespace Ini {

    IniSection::IniSection(std::istream *stream)
    {
        if (stream)
            load(*stream);
    }

    std::string &IniSection::operator[](const std::string &key)
    {
        return mValues[key];
    }

    void IniSection::save(std::ostream &stream) const
    {
        for (const std::pair<const std::string, std::string> &p : mValues) {
            stream << p.first << "=" << p.second << '\n';
        }
    }

    void IniSection::load(std::istream &stream)
    {
        mValues.clear();
        std::streampos save = stream.tellg();
        std::string line;
        while (std::getline(stream, line)) {
            if (line.at(0) == '[') {
                stream.seekg(save);
                return;
            }
            size_t pos = line.find('=');
            if (pos == std::string::npos || line.rfind('=') != pos)
                std::terminate();
            mValues[line.substr(0, pos)] = line.substr(pos + 1);
            save = stream.tellg();
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

}
}