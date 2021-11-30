#pragma once

namespace Engine {
namespace Render {

    struct DirectX11PixelShader {
        DirectX11PixelShader() = default;
        DirectX11PixelShader(ReleasePtr<ID3DBlob> pShaderBlob);
        DirectX11PixelShader(const DirectX11PixelShader &) = delete;
        DirectX11PixelShader(DirectX11PixelShader &&);
        ~DirectX11PixelShader();

        DirectX11PixelShader &operator=(DirectX11PixelShader &&);

        void reset();

        void bind() const;

        ReleasePtr<ID3D11PixelShader> mShader;        
    };

}
}