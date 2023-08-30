#pragma once

namespace Engine {
namespace Render {

    template <typename T, size_t alignment>
    struct alignas(alignment) Aligned {

        Aligned& operator=(T data) {
            mData = data;
            return *this;
        }

        T mData;
    };

}
}