#include "../directx11lib.h"

#include "directx11geometryshader.h"

#include "../directx11rendercontext.h"

namespace Engine {
namespace Render {

    DirectX11GeometryShader::DirectX11GeometryShader(ReleasePtr<ID3DBlob> pShaderBlob)
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
        mShader.reset();
    }

    void DirectX11GeometryShader::bind() const 
    {
        sDeviceContext->GSSetShader(mShader, nullptr, 0);
    }

}
}