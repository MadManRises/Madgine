#pragma once

namespace Engine {
namespace Scripting {
namespace Parsing {

class ParseException : public std::exception {
public:
    ParseException(const std::string &msg) :
        m_msg(msg)
    {

    }

    virtual const char *what() const throw()
    {
        return m_msg.c_str();
    }

private:
    std::string m_msg;
};

} // namespace Parsing
} // namespace Scripting
}

