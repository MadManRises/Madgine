#pragma once

#include "lambda.h"

namespace Engine {

struct Guard {
    Guard(Lambda<void()> &&init, Lambda<void()> &&finalize = {})
        : mFinalize(std::move(finalize))
    {
        if (init)
            init();
    }

    ~Guard()
    {
        if (mFinalize)
            mFinalize();
    }

private:
    Lambda<void()> mFinalize;
};

}