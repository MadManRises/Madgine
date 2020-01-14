#pragma once

namespace Engine {
namespace Serialize {
    class FileException : public std::exception {
    public:
        FileException(const std::string &msg)
            : m_msg(msg)
        {
        }

        const char *what() const throw() override
        {
            return m_msg.c_str();
        }

    private:
        std::string m_msg;
    };
}
}
