#pragma once

#include "directx12descriptorheap.h"

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX12_EXPORT DirectX12Buffer {

        DirectX12Buffer() = default;
        DirectX12Buffer(size_t size);
        DirectX12Buffer(const ByteBuffer &data);
        DirectX12Buffer(const DirectX12Buffer &) = delete;
        DirectX12Buffer(DirectX12Buffer &&);
        ~DirectX12Buffer();

        DirectX12Buffer &operator=(const DirectX12Buffer &) = delete;
        DirectX12Buffer &operator=(DirectX12Buffer &&);

        explicit operator bool() const;

        void bindVertex(UINT stride) const;
        void bindIndex() const;

        void reset();
        void setData(const ByteBuffer &data);
        void resize(size_t size);
        WritableByteBuffer mapData();

        OffsetPtr handle();

    private:
        size_t mSize = 0;              
        OffsetPtr mOffset;
        OffsetPtr mHandle;
    };

}
}