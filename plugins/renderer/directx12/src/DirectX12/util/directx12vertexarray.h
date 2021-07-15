#pragma once

#include "Meta/math/vector4.h"

#include "render/attributedescriptor.h"

namespace Engine {
namespace Render {
    

    struct MADGINE_DIRECTX12_EXPORT DirectX12VertexArray {
        
        DirectX12VertexArray() = default;
        DirectX12VertexArray(const DirectX12VertexArray &) = delete;
        DirectX12VertexArray(DirectX12VertexArray &&);
        DirectX12VertexArray(const std::array<AttributeDescriptor, 7> &attributes);
        ~DirectX12VertexArray();

        DirectX12VertexArray &operator=(DirectX12VertexArray &&other);

		explicit operator bool() const;

        static DirectX12VertexArray *getCurrent();

        void reset();

        void bind(DirectX12Program *program = nullptr);
        void unbind();

        static void onBindVBO(const DirectX12Buffer *buffer);
        static void onBindEBO(const DirectX12Buffer *buffer);

        static std::pair<const DirectX12Buffer *, const DirectX12Buffer *> getCurrentBindings();

        UINT mStride = 0;
        uint8_t mFormat = 0;

        std::vector<AttributeDescriptor> mAttributes;

    private:
        const DirectX12Buffer *mVBO = nullptr;
        const DirectX12Buffer *mEBO = nullptr;

        
    };

}
}