#pragma once

namespace Engine {
namespace Scene {

class EntityException : public std::exception {
public:
    EntityException(const std::string &msg) :
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

}
}


