#include "../directx11lib.h"

#include "directx11geometryshader.h"

#include "../directx11rendercontext.h"

namespace Engine {
namespace Render {

    DirectX11GeometryShader::DirectX11GeometryShader(ID3DBlob *pShaderBlob)
    {
        sDevice->CreateGeometryShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), nullptr, &mShader);
    }

    DirectX11GeometryShader::DirectX11GeometryShader(DirectX11GeometryShader &&other)
        : mShader(std::exchange(other.mShader, nullptr))
    {
    }

    DirectX11GeometryShader::~DirectX11GeometryShader()
    {
        reset();
    }

    DirectX11GeometryShader &DirectX11GeometryShader::operator=(DirectX11GeometryShader &&other)
    {
        std::swap(mShader, other.mShader);
        return *this;
    }

    void DirectX11GeometryShader::reset()
    {
        if (mShader) {
            mShader->Release();
            mShader = nullptr;
        }
    }

    void DirectX11GeometryShader::bind()
    {
        sDeviceContext->GSSetShader(mShader, nullptr, 0);
    }

}
}