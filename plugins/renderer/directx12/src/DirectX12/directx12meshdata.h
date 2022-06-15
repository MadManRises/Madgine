#pragma once

#include "util/directx12buffer.h"
#include "util/directx12texture.h"

#include "gpumeshdata.h"

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX12_EXPORT DirectX12MeshData : GPUMeshData {


        void reset()
        {
            mMaterials.clear();
            mVertices.reset();
            mIndices.reset();
        }

        DirectX12Buffer mVertices;
        DirectX12Buffer mIndices;
    };

}
}