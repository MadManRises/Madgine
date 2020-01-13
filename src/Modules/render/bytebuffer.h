#pragma once

#include "../generic/any.h"

namespace Engine {
namespace Render {

    struct ByteBuffer {

		ByteBuffer() = default;

        template <typename T>
        ByteBuffer(std::vector<T> v)
            : mSize(v.size() * sizeof(T))
            , mKeep(std::move(v))            
            , mData(mKeep.as<std::vector<T>>().data())
        {
        }
    
        size_t mSize = 0;

    private:
        Any mKeep;

    public:
        const void *mData = nullptr;
    };

}
}