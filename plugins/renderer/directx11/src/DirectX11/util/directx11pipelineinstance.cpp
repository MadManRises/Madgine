#include "../directx11lib.h"

#include "directx11pipelineinstance.h"

#include "Meta/math/matrix4.h"

#include "../directx11rendercontext.h"

#include "Generic/bytebuffer.h"

namespace Engine {
namespace Render {

    DirectX11PipelineInstance::DirectX11PipelineInstance(const PipelineConfiguration &config, typename DirectX11VertexShaderLoader::HandleType vertexShader, typename DirectX11PixelShaderLoader::HandleType pixelShader, typename DirectX11GeometryShaderLoader::HandleType geometryShader, bool dynamic)
        : PipelineInstance(config)
        , mVertexShaderHandle(std::move(vertexShader))
        , mPixelShaderHandle(std::move(pixelShader))
        , mGeometryShaderHandle(std::move(geometryShader))
        , mIsDynamic(dynamic)
    {
        if (dynamic)
            mVertexShaders = mVertexShaderHandle->ptr();
        else
            mVertexShader = mVertexShaderHandle->get(mFormat, mInstanceDataSize);

        mPixelShader = mPixelShaderHandle ? mPixelShaderHandle->get() : nullptr;
        mGeometryShader = mGeometryShaderHandle ? mGeometryShaderHandle->get() : nullptr;

        mConstantBuffers.reserve(config.bufferSizes.size());
        for (size_t i = 0; i < config.bufferSizes.size(); ++i) {
            mConstantBuffers.emplace_back(D3D11_BIND_CONSTANT_BUFFER, ByteBuffer { nullptr, config.bufferSizes[i] });
        }
    }

    void DirectX11PipelineInstance::bind(VertexFormat format) const
    {
        if (mIsDynamic) {
            if (mVertexShaders[format])
                sDeviceContext->VSSetShader(mVertexShaders[format], nullptr, 0);
            else
                sDeviceContext->VSSetShader(mVertexShaderHandle->get(format, mInstanceDataSize), nullptr, 0);
        } else {
            sDeviceContext->VSSetShader(mVertexShader, nullptr, 0);
        }
        sDeviceContext->PSSetShader(mPixelShader, nullptr, 0);
        sDeviceContext->GSSetShader(mGeometryShader, nullptr, 0);

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

    WritableByteBuffer DirectX11PipelineInstance::mapParameters(size_t index)
    {
        return mConstantBuffers[index].mapData();
    }

    void DirectX11PipelineInstance::setInstanceData(const ByteBuffer &data)
    {
        mInstanceBuffer.setData(data);
    }

    void DirectX11PipelineInstance::setDynamicParameters(size_t index, const ByteBuffer &data)
    {
        if (mDynamicBuffers.size() <= index) {
            mDynamicBuffers.reserve(index + 1);
            while (mDynamicBuffers.size() <= index)
                mDynamicBuffers.emplace_back(
                    D3D11_BIND_CONSTANT_BUFFER);
        }

        mDynamicBuffers[index].resize(data.mSize);

        if (data.mSize > 0) {
            auto target = mDynamicBuffers[index].mapData();
            std::memcpy(target.mData, data.mData, data.mSize);
        }
    }
}
}