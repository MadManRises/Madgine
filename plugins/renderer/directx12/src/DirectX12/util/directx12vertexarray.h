#pragma once

#include "Meta/math/vector4.h"

#include "render/attributedescriptor.h"

namespace Engine {
namespace Render {
    

    struct MADGINE_DIRECTX12_EXPORT DirectX12VertexArray {
        
        DirectX12VertexArray() = default;
        DirectX12VertexArray(const DirectX12VertexArray &) = delete;
        DirectX12VertexArray(DirectX12VertexArray &&);
        DirectX12VertexArray(size_t groupSize, const DirectX12Buffer &vertex, const DirectX12Buffer &index, const std::array<AttributeDescriptor, 7> &attributes);
        ~DirectX12VertexArray();

        DirectX12VertexArray &operator=(DirectX12VertexArray &&other);

		explicit operator bool() const;

        void reset();

        void bind(const DirectX12Program *program, const DirectX12VertexShader *vertexShader, const DirectX12PixelShader *pixelShader) const;

        UINT mStride = 0;
        uint8_t mFormat = 0;

        size_t mGroupSize = 0;

        std::vector<AttributeDescriptor> mAttributes;

    private:
        const DirectX12Buffer *mVBO = nullptr;
        const DirectX12Buffer *mEBO = nullptr;

        mutable std::map<const DirectX12Program *, ID3D12PipelineState *> mInstances;        
    };

}
}