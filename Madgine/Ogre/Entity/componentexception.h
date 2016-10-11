#pragma once

#include <exception>
#include <string>

namespace Engine{
namespace OGRE{
namespace Entity{

class ComponentException : public std::exception {
public:
    ComponentException(const std::string &msg) :
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
}


