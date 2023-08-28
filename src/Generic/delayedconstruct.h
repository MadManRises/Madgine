#pragma once

#include "lambda.h"

namespace Engine {

template <typename T>
struct DelayedConstruct {

    operator T() {
        return mConstructor();
    }

    Lambda<T()> mConstructor;
};

}