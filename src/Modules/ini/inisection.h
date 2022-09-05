#pragma once

namespace Engine {
namespace Ini {

    struct MODULES_EXPORT IniSection {
        IniSection();

        std::string &operator[](const std::string &key);

        void save(Stream &stream) const;
        void load(Stream &stream);

        std::map<std::string, std::string>::iterator begin();
        std::map<std::string, std::string>::iterator end();
        std::map<std::string, std::string>::const_iterator begin() const;
        std::map<std::string, std::string>::const_iterator end() const;

    private:
        std::map<std::string, std::string> mValues;
    };

}
}