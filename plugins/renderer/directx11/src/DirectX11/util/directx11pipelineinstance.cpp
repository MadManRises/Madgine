#include "../directx11lib.h"

#include "directx11pipelineinstance.h"

#include "Meta/math/matrix4.h"

#include "../directx11rendercontext.h"

#include "Generic/bytebuffer.h"

#include "../directx11meshdata.h"

#include "Madgine/render/material.h"

namespace Engine {
namespace Render {

    DirectX11PipelineInstance::DirectX11PipelineInstance(const PipelineConfiguration &config, typename DirectX11VertexShaderLoader::Handle vertexShader, typename DirectX11PixelShaderLoader::Handle pixelShader, typename DirectX11GeometryShaderLoader::Handle geometryShader, bool dynamic)
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

    void DirectX11PipelineInstance::renderMesh(const GPUMeshData *m, const Material *material) const
    {
        const DirectX11MeshData *mesh = static_cast<const DirectX11MeshData *>(m);

        mesh->mVertices.bindVertex(mesh->mVertexSize);

        bind(mesh->mFormat);

        DirectX11RenderContext::getSingleton().bindFormat(mesh->mFormat, mInstanceDataSize);

        if (material)
            bindTextures({ { material->mDiffuseTexture, TextureType_2D }, { material->mEmissiveTexture, TextureType_2D } });

        constexpr D3D11_PRIMITIVE_TOPOLOGY modes[] {
            D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
            D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
            D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
        };

        assert(mesh->mGroupSize > 0 && mesh->mGroupSize <= 3);
        D3D11_PRIMITIVE_TOPOLOGY mode = modes[mesh->mGroupSize - 1];
        sDeviceContext->IASetPrimitiveTopology(mode);

        if (mesh->mIndices) {
            mesh->mIndices.bindIndex();
            sDeviceContext->DrawIndexed(mesh->mElementCount, 0, 0);
        } else {
            sDeviceContext->Draw(mesh->mElementCount, 0);
        }
    }

    void DirectX11PipelineInstance::renderMeshInstanced(size_t count, const GPUMeshData *m, const Material *material) const
    {
        const DirectX11MeshData *mesh = static_cast<const DirectX11MeshData *>(m);

        mesh->mVertices.bindVertex(mesh->mVertexSize);

        bind(mesh->mFormat);

        DirectX11RenderContext::getSingleton().bindFormat(mesh->mFormat, mInstanceDataSize);

        if (material)
            bindTextures({ { material->mDiffuseTexture, TextureType_2D }, { material->mEmissiveTexture, TextureType_2D } });

        constexpr D3D11_PRIMITIVE_TOPOLOGY modes[] {
            D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
            D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
            D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
        };

        assert(mesh->mGroupSize > 0 && mesh->mGroupSize <= 3);
        D3D11_PRIMITIVE_TOPOLOGY mode = modes[mesh->mGroupSize - 1];
        sDeviceContext->IASetPrimitiveTopology(mode);

        if (mesh->mIndices) {
            mesh->mIndices.bindIndex();
            sDeviceContext->DrawIndexedInstanced(mesh->mElementCount, count, 0, 0, 0);
        } else {
            sDeviceContext->DrawInstanced(mesh->mElementCount, count, 0, 0);
        }
    }

    void DirectX11PipelineInstance::bindTextures(const std::vector<TextureDescriptor> &tex, size_t offset) const
    {
        bindTexturesImpl(tex, offset);
    }

    void DirectX11PipelineInstance::bindTexturesImpl(const std::vector<TextureDescriptor> &tex, size_t offset)
    {
        std::vector<ID3D11ShaderResourceView *> handles;
        std::ranges::transform(tex, std::back_inserter(handles), [](const TextureDescriptor &desc) {
            return reinterpret_cast<ID3D11ShaderResourceView *>(desc.mTextureHandle);
        });
        sDeviceContext->PSSetShaderResources(offset, tex.size(), handles.data());
    }

}
}