#pragma once

#include "util/directx11buffer.h"
#include "util/directx11texture.h"
#include "util/directx11vertexarray.h"

#include "meshdata.h"

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX11_EXPORT DirectX11MeshData : MeshData {

        void reset()
        {
            mTexture.reset();
            mVertices.reset();
            mIndices.reset();
            mVAO.reset();
        }

        DirectX11Texture mTexture;
        DirectX11Buffer mVertices;
        DirectX11Buffer mIndices;
        size_t mGroupSize;
        size_t mElementCount;
        DirectX11VertexArray mVAO;
    };

}
}