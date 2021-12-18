#pragma once

namespace Engine {
namespace Render {

    struct DirectX12VertexShader {
        DirectX12VertexShader() = default;
        DirectX12VertexShader(Resources::ResourceBase *resource);
        DirectX12VertexShader(const DirectX12VertexShader &) = delete;
        DirectX12VertexShader(DirectX12VertexShader &&);
        ~DirectX12VertexShader();

        DirectX12VertexShader &operator=(DirectX12VertexShader &&);

        void reset();

        ID3DBlob *getInstance(const DirectX12VertexArray *format) const;

        Resources::ResourceBase *mResource = nullptr;

        mutable std::vector<ID3DBlob *> mInstances;
    };

}
}