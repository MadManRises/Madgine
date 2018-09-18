#pragma once

#include "stacktrace.h"

namespace Engine{

    struct exception : std::exception {
        
        exception(std::string what);

        const char *what() const noexcept override;

        const Util::StackTrace<10> &stacktrace() const;

    private:
        std::string mWhat;
        Util::StackTrace<10> mStacktrace;
    };

}