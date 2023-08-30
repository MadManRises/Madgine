#include "../directx11lib.h"

#include "directx11pipelineinstance.h"

#include "Meta/math/matrix4.h"

#include "../directx11rendercontext.h"

#include "Generic/bytebuffer.h"

#include "../directx11meshdata.h"

namespace Engine {
namespace Render {

    static constexpr D3D11_PRIMITIVE_TOPOLOGY sModes[] {
        D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
        D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
    };

    DirectX11PipelineInstance::DirectX11PipelineInstance(const PipelineConfiguration &config, ID3D11VertexShader *vertexShader, ID3D10Blob *vertexBlob, ID3D11PixelShader *pixelShader, ID3D11GeometryShader *geometryShader)
        : PipelineInstance(config)
        , mVertexShader(vertexShader)
        , mVertexShaderBlob(vertexBlob)
        , mGeometryShader(geometryShader)
        , mPixelShader(pixelShader)
    {
        mConstantBuffers.reserve(config.bufferSizes.size());
        for (size_t i = 0; i < config.bufferSizes.size(); ++i) {
            mConstantBuffers.emplace_back(D3D11_BIND_CONSTANT_BUFFER, ByteBuffer { nullptr, config.bufferSizes[i] });
        }
    }

    bool DirectX11PipelineInstance::bind(VertexFormat format, size_t groupSize) const
    {
        sDeviceContext->VSSetShader(mVertexShader, nullptr, 0);
        sDeviceContext->PSSetShader(mPixelShader, nullptr, 0);
        sDeviceContext->GSSetShader(mGeometryShader, nullptr, 0);

        assert(groupSize > 0 && groupSize <= 3);
        D3D11_PRIMITIVE_TOPOLOGY mode = sModes[groupSize - 1];
        sDeviceContext->IASetPrimitiveTopology(mode);

        ID3D11Buffer *buffers[20];
        assert(mConstantBuffers.size() <= 20);
        std::ranges::transform(mConstantBuffers, buffers, [](const DirectX11Buffer &buf) { return buf.handle(); });

        sDeviceContext->VSSetConstantBuffers(0, mConstantBuffers.size(), buffers);
        sDeviceContext->PSSetConstantBuffers(0, mConstantBuffers.size(), buffers);
        sDeviceContext->GSSetConstantBuffers(0, mConstantBuffers.size(), buffers);

        DirectX11RenderContext::getSingleton().bindFormat(format, mInstanceDataSize, mVertexShaderBlob);

        return true;
    }

    WritableByteBuffer DirectX11PipelineInstance::mapParameters(size_t index)
    {
        return mConstantBuffers[index].mapData();
    }

    void DirectX11PipelineInstance::renderMesh(RenderTarget *target, const GPUMeshData *m) const
    {
        const DirectX11MeshData *mesh = static_cast<const DirectX11MeshData *>(m);

        if (!bind(mesh->mFormat, mesh->mGroupSize))
            return;

        mesh->mVertices.bindVertex(mesh->mVertexSize);

        if (mesh->mIndices) {
            mesh->mIndices.bindIndex();
            sDeviceContext->DrawIndexed(mesh->mElementCount, 0, 0);
        } else {
            sDeviceContext->Draw(mesh->mElementCount, 0);
        }
    }

    void DirectX11PipelineInstance::renderMeshInstanced(RenderTarget *target, size_t count, const GPUMeshData *m, const ByteBuffer &instanceData) const
    {
        assert(mInstanceDataSize * count == instanceData.mSize);
        assert(instanceData.mSize > 0);

        const DirectX11MeshData *mesh = static_cast<const DirectX11MeshData *>(m);

        if (!bind(mesh->mFormat, mesh->mGroupSize))
            return;

        mesh->mVertices.bindVertex(mesh->mVertexSize);

        DirectX11Buffer instanceBuffer { D3D11_BIND_VERTEX_BUFFER, instanceData };
        instanceBuffer.bindVertex(mInstanceDataSize, 1);

        if (mesh->mIndices) {
            mesh->mIndices.bindIndex();
            sDeviceContext->DrawIndexedInstanced(mesh->mElementCount, count, 0, 0, 0);
        } else {
            sDeviceContext->DrawInstanced(mesh->mElementCount, count, 0, 0);
        }
    }

    void DirectX11PipelineInstance::bindTextures(RenderTarget *target, const std::vector<TextureDescriptor> &tex, size_t offset) const
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

    DirectX11PipelineInstanceHandle::DirectX11PipelineInstanceHandle(const PipelineConfiguration &config, typename DirectX11VertexShaderLoader::Handle vertexShader, typename DirectX11PixelShaderLoader::Handle pixelShader, typename DirectX11GeometryShaderLoader::Handle geometryShader)
        : DirectX11PipelineInstance(config,
            vertexShader ? vertexShader->mShader.get() : nullptr,
            vertexShader ? vertexShader->mBlob.get() : nullptr,
            pixelShader ? pixelShader->get() : nullptr,
            geometryShader ? geometryShader->get() : nullptr)
        , mVertexShaderHandle(std::move(vertexShader))
        , mPixelShaderHandle(std::move(pixelShader))
        , mGeometryShaderHandle(std::move(geometryShader))
    {
    }

    DirectX11PipelineInstancePtr::DirectX11PipelineInstancePtr(const PipelineConfiguration &config, typename DirectX11VertexShaderLoader::Ptr vertexShader, typename DirectX11PixelShaderLoader::Ptr pixelShader, typename DirectX11GeometryShaderLoader::Ptr geometryShader)
        : DirectX11PipelineInstance(config,
            vertexShader ? vertexShader->mShader.get() : nullptr,
            vertexShader ? vertexShader->mBlob.get() : nullptr,
            pixelShader ? pixelShader->get() : nullptr,
            geometryShader ? geometryShader->get() : nullptr)
        , mVertexShaderHandle(std::move(vertexShader))
        , mPixelShaderHandle(std::move(pixelShader))
        , mGeometryShaderHandle(std::move(geometryShader))
    {
    }

}
}