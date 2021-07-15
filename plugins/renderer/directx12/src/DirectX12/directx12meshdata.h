#pragma once

#include "util/directx12buffer.h"
#include "util/directx12texture.h"
#include "util/directx12vertexarray.h"

#include "gpumeshdata.h"

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX12_EXPORT DirectX12MeshData : GPUMeshData {

        void reset()
        {
            mTexture.reset();
            mVertices.reset();
            mIndices.reset();
            mVAO.reset();
        }

        DirectX12Texture mTexture;
        DirectX12Buffer mVertices;
        DirectX12Buffer mIndices;
        size_t mGroupSize;
        size_t mElementCount;
        DirectX12VertexArray mVAO;
    };

}
}