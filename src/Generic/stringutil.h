#pragma once

namespace Engine {
namespace StringUtil {

    constexpr bool isspace(char c) {
        return c == ' ';
    }

    constexpr bool startsWith(const std::string_view &s, const std::string_view &prefix)
    {
        return s.substr(0, prefix.size()) == prefix;
    }

    constexpr bool endsWith(const std::string_view &s, const std::string_view &suffix)
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

    template <size_t S>
    constexpr std::array<std::string_view, S> tokenize(std::string_view string, char token) {
        std::array<std::string_view, S> result = {};
        size_t pivot = 0;
        for (size_t i = 0; i < S; ++i) {
            while (isspace(string[pivot]))
                ++pivot;
            size_t newPivot = string.find(token, pivot);
            if ((i == S - 1) != (newPivot == std::string_view::npos))
                throw 0;
            if (newPivot == std::string_view::npos)
                newPivot = string.size();
            size_t actualEnd = newPivot;
            while (actualEnd > pivot && isspace(string[actualEnd - 1]))
                --actualEnd;
            result[i] = string.substr(pivot, actualEnd - pivot);
            pivot = newPivot + 1;
        }
        return result;
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