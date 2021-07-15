#pragma once

namespace Engine {
namespace Render {

    enum ShaderType {
        NoShader,
        VertexShader,
        PixelShader
    };

    struct DirectX12VertexShader {
        DirectX12VertexShader() = default;
        DirectX12VertexShader(Resources::ResourceBase *resource);
        DirectX12VertexShader(const DirectX12VertexShader &) = delete;
        DirectX12VertexShader(DirectX12VertexShader &&);
        ~DirectX12VertexShader();

        DirectX12VertexShader &operator=(DirectX12VertexShader &&);

        void reset();

        //void bind(DirectX12VertexArray *format);

        Resources::ResourceBase *mResource = nullptr;

        std::vector<std::pair<ID3DBlob *, std::vector<D3D12_INPUT_ELEMENT_DESC>>> mInstances;
    };

}
}