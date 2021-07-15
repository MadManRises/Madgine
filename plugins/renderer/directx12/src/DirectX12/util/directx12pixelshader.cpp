#include "../directx12lib.h"

#include "directx12pixelshader.h"

#include "../directx12rendercontext.h"

namespace Engine {
namespace Render {

    DirectX12PixelShader::DirectX12PixelShader(ID3DBlob *pShaderBlob)
        : mShader(pShaderBlob)
    {
    }

    DirectX12PixelShader::DirectX12PixelShader(DirectX12PixelShader &&other)
        : mShader(std::exchange(other.mShader, nullptr))
    {
    }

    DirectX12PixelShader::~DirectX12PixelShader()
    {
        reset();
    }

    DirectX12PixelShader &DirectX12PixelShader::operator=(DirectX12PixelShader &&other)
    {
        std::swap(mShader, other.mShader);
        return *this;
    }

    void DirectX12PixelShader::reset()
    {
        if (mShader) {
            mShader->Release();
            mShader = nullptr;
        }
    }

}
}