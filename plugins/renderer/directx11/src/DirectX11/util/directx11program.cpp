#include "../directx11lib.h"

#include "directx11pixelshader.h"
#include "directx11program.h"
#include "directx11vertexshader.h"
#include "directx11vertexarray.h"

#include "Meta/math/matrix4.h"

#include "../directx11rendercontext.h"

#include "Generic/bytebuffer.h"

namespace Engine {
namespace Render {

    DirectX11Program::~DirectX11Program()
    {
        reset();
    }

    DirectX11Program::DirectX11Program(DirectX11Program &&other)
        : mVertexShader(std::move(other.mVertexShader))
        , mPixelShader(std::move(other.mPixelShader))
        , mGeometryShader(std::move(other.mGeometryShader))
    {
    }

    DirectX11Program &DirectX11Program::operator=(DirectX11Program &&other)
    {
        std::swap(mVertexShader, other.mVertexShader);
        std::swap(mPixelShader, other.mPixelShader);
        std::swap(mGeometryShader, other.mGeometryShader);
        return *this;
    }

    bool DirectX11Program::link(typename DirectX11VertexShaderLoader::HandleType vertexShader, typename DirectX11PixelShaderLoader::HandleType pixelShader, typename DirectX11GeometryShaderLoader::HandleType geometryShader)
    {
        reset();

        mVertexShader = std::move(vertexShader);
        mPixelShader = std::move(pixelShader);
        mGeometryShader = std::move(geometryShader);

        return true;
    }

    void DirectX11Program::reset()
    {
        mVertexShader.reset();
        mPixelShader.reset();
        mGeometryShader.reset();

        mConstantBuffers.clear();
    }

    void DirectX11Program::bind(const DirectX11VertexArray *format) const 
    {
        format->bind();
        mVertexShader->bind(format, mInstanceDataSize);
        mPixelShader->bind();
        if (mGeometryShader)
            mGeometryShader->bind();
        else
            sDeviceContext->GSSetShader(nullptr, nullptr, 0);

        if (mInstanceBuffer)
            mInstanceBuffer.bindVertex(mInstanceDataSize, 1);

        ID3D11Buffer *buffers[20];
        assert(mConstantBuffers.size() <= 20);
        std::ranges::transform(mConstantBuffers, buffers, [](const DirectX11Buffer &buf) { return buf.handle(); });

        sDeviceContext->VSSetConstantBuffers(0, mConstantBuffers.size(), buffers);
        sDeviceContext->PSSetConstantBuffers(0, mConstantBuffers.size(), buffers);
        sDeviceContext->GSSetConstantBuffers(0, mConstantBuffers.size(), buffers);

        assert(mDynamicBuffers.size() <= 20);
        std::ranges::transform(mDynamicBuffers, buffers, [](const DirectX11Buffer &buf) { return buf.handle(); });

        sDeviceContext->VSSetConstantBuffers(3, mDynamicBuffers.size(), buffers);
        sDeviceContext->PSSetConstantBuffers(3, mDynamicBuffers.size(), buffers);
        sDeviceContext->GSSetConstantBuffers(3, mDynamicBuffers.size(), buffers);
    }

    void DirectX11Program::setParametersSize(size_t index, size_t size)
    {
        if (mConstantBuffers.size() <= index) {
            mConstantBuffers.reserve(index + 1);
            while (mConstantBuffers.size() <= index)
                mConstantBuffers.emplace_back(D3D11_BIND_CONSTANT_BUFFER);
        }

        mConstantBuffers[index].resize(size);
    }

    WritableByteBuffer DirectX11Program::mapParameters(size_t index)
    {
        return mConstantBuffers[index].mapData();
    }

    void DirectX11Program::setInstanceDataSize(size_t size)
    {
        mInstanceDataSize = size;
    }

    void DirectX11Program::setInstanceData(const ByteBuffer &data)
    {
        mInstanceBuffer.setData(data);
    }

    void DirectX11Program::setDynamicParameters(size_t index, const ByteBuffer &data)
    {
        if (mDynamicBuffers.size() <= index) {
            mDynamicBuffers.reserve(index + 1);
            while (mDynamicBuffers.size() <= index)
                mDynamicBuffers.emplace_back(
                    D3D11_BIND_CONSTANT_BUFFER
                );
        }

        mDynamicBuffers[index].resize(data.mSize);

        if (data.mSize > 0) {
            auto target = mDynamicBuffers[index].mapData();
            std::memcpy(target.mData, data.mData, data.mSize);
        }
    }
}
}