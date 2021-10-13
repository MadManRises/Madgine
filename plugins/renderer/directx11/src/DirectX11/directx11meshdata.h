#pragma once

#include "util/directx11buffer.h"
#include "util/directx11texture.h"
#include "util/directx11vertexarray.h"

#include "gpumeshdata.h"

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX11_EXPORT DirectX11MeshData : GPUMeshData {

        void reset()
        {
            mMaterials.clear();
            mVertices.reset();
            mIndices.reset();
            mVAO.reset();
        }

        DirectX11Buffer mVertices = D3D11_BIND_VERTEX_BUFFER;
        DirectX11Buffer mIndices = D3D11_BIND_INDEX_BUFFER;
        size_t mGroupSize;
        size_t mElementCount;
        DirectX11VertexArray mVAO;
    };

}
}