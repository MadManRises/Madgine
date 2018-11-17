#pragma once

#include "../debug/stacktrace.h"

namespace Engine{

    struct INTERFACES_EXPORT exception : std::exception {
        
        exception(std::string what);

        const char *what() const noexcept override;

        const Debug::StackTrace<10> &stacktrace() const;

    private:
        std::string mWhat;
		Debug::StackTrace<10> mStacktrace;
    };

}