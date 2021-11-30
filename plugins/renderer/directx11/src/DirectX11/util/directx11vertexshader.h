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

        void bind(const DirectX11VertexArray *format, size_t instanceDataSize) const;

        Resources::ResourceBase *mResource = nullptr;

        mutable std::vector<std::pair<ReleasePtr<ID3D11VertexShader>, ReleasePtr<ID3D11InputLayout>>> mInstances;
    };

}
}