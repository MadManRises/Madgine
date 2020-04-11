#pragma once

namespace Engine {
namespace Render {

    enum ShaderType {
        NoShader,
        VertexShader,
        PixelShader
    };

    struct DirectX11VertexShader {
        DirectX11VertexShader() = default;
        DirectX11VertexShader(Resources::ResourceBase *resource);
        DirectX11VertexShader(const DirectX11VertexShader &) = delete;
        DirectX11VertexShader(DirectX11VertexShader &&);
        ~DirectX11VertexShader();

        DirectX11VertexShader &operator=(DirectX11VertexShader &&);

        void reset();

        void bind(DirectX11VertexArray *format);

        Resources::ResourceBase *mResource = nullptr;

        std::vector<std::pair<ID3D11VertexShader *, ID3D11InputLayout *>> mInstances;
    };

}
}