#include "../directx12lib.h"

#include "directx12pipelineinstance.h"

#include "../directx12rendercontext.h"

#include "../directx12meshdata.h"
#include "../directx12rendertarget.h"

namespace Engine {
namespace Render {

    static constexpr D3D12_PRIMITIVE_TOPOLOGY sModes[] {
        D3D_PRIMITIVE_TOPOLOGY_POINTLIST,
        D3D_PRIMITIVE_TOPOLOGY_LINELIST,
        D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
    };

    DirectX12PipelineInstance::DirectX12PipelineInstance(const PipelineConfiguration &config, DirectX12PipelineLoader::Handle pipeline)
        : PipelineInstance(config)
        , mPipelineHandle(std::move(pipeline))
    {
        mPipelines = mPipelineHandle->ptr();

        mConstantBuffers.reserve(config.bufferSizes.size());
        for (size_t i = 0; i < config.bufferSizes.size(); ++i) {
            mConstantBuffers.emplace_back(ByteBuffer { nullptr, config.bufferSizes[i] });
        }
    }

    bool DirectX12PipelineInstance::bind(ID3D12GraphicsCommandList *commandList, VertexFormat format, size_t groupSize) const
    {
        ID3D12PipelineState *pipeline = mPipelines[format][groupSize - 1];
        if (!pipeline) {
            pipeline = mPipelineHandle->get(format, groupSize, mInstanceDataSize);
            if (!pipeline)
                return false;
        }
        commandList->SetPipelineState(pipeline);

        assert(groupSize > 0 && groupSize <= 3);
        D3D12_PRIMITIVE_TOPOLOGY mode = sModes[groupSize - 1];
        commandList->IASetPrimitiveTopology(mode);

        for (size_t i = 0; i < std::min(size_t { 3 }, mConstantBuffers.size()); ++i) {
            if (mConstantBuffers[i])
                commandList->SetGraphicsRootConstantBufferView(i, mConstantBuffers[i].gpuAddress());
        }
        /* for (size_t i = 0; i < mDynamicBuffers.size(); ++i) {
            if (mDynamicBuffers[i])
                commandList->SetGraphicsRootConstantBufferView(i + 3, mDynamicBuffers[i].gpuAddress());
        }*/

        DX12_CHECK();

        return true;
    }

    WritableByteBuffer DirectX12PipelineInstance::mapParameters(size_t index)
    {
        return mConstantBuffers[index].mapData();
    }

    void DirectX12PipelineInstance::setDynamicParameters(size_t index, const ByteBuffer &data)
    {
        if (mDynamicBuffers.size() <= index)
            mDynamicBuffers.resize(index + 1);

        if (data.mSize > 0) {
            auto target = mDynamicBuffers[index].mapData(data.mSize);
            std::memcpy(target.mData, data.mData, data.mSize);
        }
    }

    void DirectX12PipelineInstance::renderMesh(RenderTarget *target, const GPUMeshData *m) const
    {
        ID3D12GraphicsCommandList *commandList = static_cast<DirectX12RenderTarget*>(target)->mCommandList;

        const DirectX12MeshData *mesh = static_cast<const DirectX12MeshData *>(m);

        if (!bind(commandList, mesh->mFormat, mesh->mGroupSize))
            return;

        mesh->mVertices.bindVertex(commandList, mesh->mVertexSize);

        DirectX12RenderContext::getSingleton().mConstantBuffer.bindVertex(commandList, 0, 2);

        if (mesh->mIndices) {
            mesh->mIndices.bindIndex(commandList);
            commandList->DrawIndexedInstanced(mesh->mElementCount, 1, 0, 0, 0);
        } else {
            commandList->DrawInstanced(mesh->mElementCount, 1, 0, 0);
        }
    }

    void DirectX12PipelineInstance::renderMeshInstanced(RenderTarget *target, size_t count, const GPUMeshData *m, const ByteBuffer &instanceData) const
    {
        assert(instanceData.mSize > 0);
        assert(mInstanceDataSize * count == instanceData.mSize);

        ID3D12GraphicsCommandList *commandList = static_cast<DirectX12RenderTarget *>(target)->mCommandList;

        const DirectX12MeshData *mesh = static_cast<const DirectX12MeshData *>(m);

        if (!bind(commandList, mesh->mFormat, mesh->mGroupSize))
            return;

        mesh->mVertices.bindVertex(commandList, mesh->mVertexSize);

        DirectX12RenderContext::getSingleton().mConstantBuffer.bindVertex(commandList, 0, 2);

        DirectX12Buffer instanceBuffer;
        {
            auto target = instanceBuffer.mapData(instanceData.mSize);
            std::memcpy(target.mData, instanceData.mData, instanceData.mSize);
        }
        instanceBuffer.bindVertex(commandList, mInstanceDataSize, 1);

        if (mesh->mIndices) {
            mesh->mIndices.bindIndex(commandList);
            commandList->DrawIndexedInstanced(mesh->mElementCount, count, 0, 0, 0);
        } else {
            commandList->DrawInstanced(mesh->mElementCount, count, 0, 0);
        }
    }

    void DirectX12PipelineInstance::bindTextures(RenderTarget *target, const std::vector<TextureDescriptor> &tex, size_t offset) const
    {
        for (size_t i = 0; i < tex.size(); ++i) {
            if (tex[i].mTextureHandle) {
                D3D12_GPU_DESCRIPTOR_HANDLE handle;
                handle.ptr = tex[i].mTextureHandle;
                static_cast<DirectX12RenderTarget *>(target)->mCommandList->SetGraphicsRootDescriptorTable(4 + offset + i, handle);
            }
        }
    }

}
}