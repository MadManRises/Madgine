#pragma once

namespace Engine {
namespace StringUtil {

    inline bool startsWith(const std::string &s, const std::string &prefix)
    {
        return s.substr(0, prefix.size()) == prefix;
    }

    inline bool endsWith(const std::string &s, const std::string &suffix)
    {
        return s.size() >= suffix.size() ? (s.substr(s.size() - suffix.size(), suffix.size()) == suffix) : false;
    }

    inline std::string &replace(std::string &s, char old, char rep)
    {
        for (char &c : s) {
            if (c == old)
                c = rep;
        }
        return s;
    }

    inline std::string &replace(std::string &s, std::string_view old, std::string_view rep)
    {
        size_t index = 0;
        size_t oldLen = old.size();
        size_t repLen = rep.size();

        while (index + oldLen <= s.size()) {
            if (std::string_view { &s[index], oldLen } == old) {
                s.replace(index, oldLen, rep.data(), repLen);
                index += oldLen;
            } else {
                ++index;
            }
        }
        return s;
    }

    inline std::string replace(std::string &&s, char old, char rep)
    {
        replace(s, old, rep);
        return std::move(s);
    }

    inline std::string replace(std::string &&s, std::string_view old, std::string_view rep)
    {
        replace(s, old, rep);
        return std::move(s);
    }

    inline std::string replace(const std::string &s, char old, char rep)
    {
        return replace(std::string { s }, old, rep);
    }

    inline std::string replace(const std::string &s, std::string_view old, std::string_view rep)
    {
        return replace(std::string { s }, old, rep);
    }

    inline std::string toLower(std::string &&s)
    {
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        return std::move(s);
    }

    inline std::string toUpper(std::string &&s)
    {
        std::transform(s.begin(), s.end(), s.begin(), ::toupper);
        return std::move(s);
    }

}

constexpr size_t strlen(const char *s)
{
    size_t len = 0;
    while (s[len] != '\0')
        ++len;
    return len;
}

constexpr const char *strrchr(const char *s, char c)
{
    size_t len = strlen(s);
    const char *p = s + len;
    while (p != s) {
        --p;
        if (*p == c)
            return p;
    }
    return nullptr;
}

}