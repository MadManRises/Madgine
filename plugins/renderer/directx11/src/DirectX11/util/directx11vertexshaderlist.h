#pragma once

#include "render/vertexformat.h"

namespace Engine {
namespace Render {

    struct DirectX11VertexShaderList {
        
        
        void reset();

        ReleasePtr<ID3D11VertexShader> &get(VertexFormat format, size_t instanceDataSize);
        const ReleasePtr<ID3D11VertexShader> *ptr() const;

        Resources::ResourceBase *mResource = nullptr;
        std::array<ReleasePtr<ID3D11VertexShader>, 128> mShaders;
    };

}
}