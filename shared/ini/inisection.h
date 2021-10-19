#pragma once

namespace Engine {
namespace Ini {

    struct INI_EXPORT IniSection {
        IniSection(std::istream *stream = nullptr);

        std::string &operator[](const std::string &key);

        void save(std::ostream &stream) const;
        void load(std::istream &stream);

        std::map<std::string, std::string>::iterator begin();
        std::map<std::string, std::string>::iterator end();
        std::map<std::string, std::string>::const_iterator begin() const;
        std::map<std::string, std::string>::const_iterator end() const;

    private:
        std::map<std::string, std::string> mValues;
    };

}
}