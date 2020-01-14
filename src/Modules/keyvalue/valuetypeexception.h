#pragma once

namespace Engine {
class ValueTypeException : public std::exception {
public:
    ValueTypeException(const std::string &msg)
        : mMsg(msg)
    {
    }

    const char *what() const noexcept override
    {
        return mMsg.c_str();
    }

private:
    std::string mMsg;
};
}