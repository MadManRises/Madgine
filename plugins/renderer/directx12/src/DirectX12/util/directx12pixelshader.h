#pragma once

namespace Engine {
namespace Render {

    struct DirectX12PixelShader {
        DirectX12PixelShader() = default;
        DirectX12PixelShader(ID3DBlob *pShaderBlob);
        DirectX12PixelShader(const DirectX12PixelShader &) = delete;
        DirectX12PixelShader(DirectX12PixelShader &&);
        ~DirectX12PixelShader();

        DirectX12PixelShader &operator=(DirectX12PixelShader &&);

        void reset();

        ID3DBlob *mShader = nullptr;        
    };

}
}