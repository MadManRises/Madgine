#pragma once

namespace Engine {
namespace Render {

    struct DirectX11GeometryShader {
        DirectX11GeometryShader() = default;
        DirectX11GeometryShader(ReleasePtr<ID3DBlob> pShaderBlob);
        DirectX11GeometryShader(const DirectX11GeometryShader &) = delete;
        DirectX11GeometryShader(DirectX11GeometryShader &&);
        ~DirectX11GeometryShader();

        DirectX11GeometryShader &operator=(DirectX11GeometryShader &&);

        void reset();

        void bind() const;

        ReleasePtr<ID3D11GeometryShader> mShader;        
    };

}
}