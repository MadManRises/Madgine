#include "../directx11lib.h"

#include "directx11pixelshader.h"

#include "../directx11rendercontext.h"

namespace Engine {
namespace Render {

    DirectX11PixelShader::DirectX11PixelShader(ID3DBlob *pShaderBlob)
    {
        sDevice->CreatePixelShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), nullptr, &mShader);
    }

    DirectX11PixelShader::DirectX11PixelShader(DirectX11PixelShader &&other)
        : mShader(std::exchange(other.mShader, nullptr))
    {
    }

    DirectX11PixelShader::~DirectX11PixelShader()
    {
        reset();
    }

    DirectX11PixelShader &DirectX11PixelShader::operator=(DirectX11PixelShader &&other)
    {
        std::swap(mShader, other.mShader);
        return *this;
    }

    void DirectX11PixelShader::reset()
    {
        if (mShader) {
            mShader->Release();
            mShader = nullptr;
        }
    }

    void DirectX11PixelShader::bind()
    {
        sDeviceContext->PSSetShader(mShader, nullptr, 0);
    }

}
}