#pragma once

#include "Meta/math/vector4.h"

#include "render/attributedescriptor.h"

namespace Engine {
namespace Render {
    

    struct MADGINE_DIRECTX11_EXPORT DirectX11VertexArray {
        
        DirectX11VertexArray() = default;
        DirectX11VertexArray(const DirectX11VertexArray &) = delete;
        DirectX11VertexArray(DirectX11VertexArray &&);
        DirectX11VertexArray(const DirectX11Buffer &vertex, const DirectX11Buffer &index, std::array<AttributeDescriptor, 7> (*attributes)());
        ~DirectX11VertexArray();

        DirectX11VertexArray &operator=(DirectX11VertexArray &&other);

		explicit operator bool() const;

        void reset();

        void bind() const;

        UINT mStride = 0;
        uint8_t mFormat = 0;

        std::array<AttributeDescriptor, 7> (*mAttributes)();

    private:
        const DirectX11Buffer *mVBO = nullptr;
        const DirectX11Buffer *mEBO = nullptr;

        
    };

}
}