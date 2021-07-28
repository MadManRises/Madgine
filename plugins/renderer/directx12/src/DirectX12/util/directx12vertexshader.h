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

        ID3DBlob *getInstance(DirectX12VertexArray *format);

        Resources::ResourceBase *mResource = nullptr;

        std::vector<ID3DBlob *> mInstances;
    };

}
}