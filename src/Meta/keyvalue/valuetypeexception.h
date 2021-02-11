#pragma once

namespace Engine {
struct ValueTypeException : std::exception {
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