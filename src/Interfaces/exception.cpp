#include "interfaceslib.h"

#include "exception.h"

namespace Engine{

    exception::exception(std::string what) :
        mWhat(std::move(what)),
        mStacktrace(Debug::StackTrace<10>::getCurrent(1)) {}

    const char *exception::what() const noexcept {
        return mWhat.c_str();
    }

    const Debug::StackTrace<10> &exception::stacktrace() const {
        return mStacktrace;
    }

}