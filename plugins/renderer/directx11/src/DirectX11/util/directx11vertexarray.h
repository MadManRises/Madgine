#pragma once

#include "Modules/math/vector4.h"

#include "render/attributedescriptor.h"

namespace Engine {
namespace Render {
    

    struct MADGINE_DIRECTX11_EXPORT DirectX11VertexArray {
        
        DirectX11VertexArray() = default;
        DirectX11VertexArray(const DirectX11VertexArray &) = delete;
        DirectX11VertexArray(DirectX11VertexArray &&);
        DirectX11VertexArray(const std::vector<std::optional<AttributeDescriptor>> &attributes);
        ~DirectX11VertexArray();

        DirectX11VertexArray &operator=(DirectX11VertexArray &&other);

		operator bool() const;

        static DirectX11VertexArray *getCurrent();

        void reset();

        void bind(DirectX11Program *program = nullptr);
        void unbind();

        static void onBindVBO(const DirectX11Buffer *buffer);
        static void onBindEBO(const DirectX11Buffer *buffer);

        static std::pair<const DirectX11Buffer *, const DirectX11Buffer *> getCurrentBindings();

        UINT mStride = 0;
        uint8_t mFormat = 0;

        std::vector<AttributeDescriptor> mAttributes;

    private:
        const DirectX11Buffer *mVBO = nullptr;
        const DirectX11Buffer *mEBO = nullptr;

        
    };

}
}