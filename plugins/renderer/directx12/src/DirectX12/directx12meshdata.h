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
            mMaterials.clear();
            mVertices.reset();
            mIndices.reset();
            mVAO.reset();
        }

        DirectX12Buffer mVertices;
        DirectX12Buffer mIndices;
        size_t mElementCount;
        DirectX12VertexArray mVAO;
    };

}
}