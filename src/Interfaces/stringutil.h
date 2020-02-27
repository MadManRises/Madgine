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

	template <typename String>
    inline String &replace(String &s, std::string_view old, std::string_view rep)
    {
        size_t pos = s.find(old);
        size_t oldLen = old.size();
        size_t repLen = rep.size();

        while (pos != std::string::npos) {
            s.replace(pos, oldLen, rep);            
            pos = s.find(old, pos + repLen);
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

	template <typename Cont>
    std::string join(Cont &&cont, const char *sep)
    {
        std::stringstream ss;
        bool first = true;
        for (auto &&element : cont) {
            if (first)
                first = false;
            else
                ss << sep;
            ss << element;
        }
        return ss.str();
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

constexpr bool streq(const char *lhs, const char *rhs)
{
    int i = -1;
    do {
        ++i;
        if (lhs[i] != rhs[i])
            return false;
    } while (lhs[i]);
    return true;
}

}