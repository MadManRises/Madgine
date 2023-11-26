#pragma once

#include "ptr.h"

namespace Engine {
namespace Render {

    struct ResourceBlock {

        template <typename T>
        operator T() const
        {
            return (T)(mPtr);
        }

        auto operator<=>(const ResourceBlock &other) const = default;

        explicit operator bool() const
        {
            return mPtr != 0;
        }

        uintptr_t mPtr;
    };

    struct UniqueResourceBlock : UniqueOpaquePtr {        

        operator ResourceBlock() const
        {
            return { get() };
        }

    };

}
}