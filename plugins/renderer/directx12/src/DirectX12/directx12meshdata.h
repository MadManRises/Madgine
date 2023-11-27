#pragma once

#include "util/directx12buffer.h"
#include "util/directx12texture.h"

#include "Madgine/meshloader/gpumeshdata.h"

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX12_EXPORT DirectX12MeshData : GPUMeshData {

        DirectX12Buffer mVertices;
        DirectX12Buffer mIndices;
    };

}
}