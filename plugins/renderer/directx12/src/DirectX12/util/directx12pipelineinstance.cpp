#include "../directx12lib.h"

#include "directx12pipelineinstance.h"

#include "../directx12rendercontext.h"

#include "../directx12meshdata.h"

#include "Madgine/render/material.h"

namespace Engine {
namespace Render {

    DirectX12PipelineInstance::DirectX12PipelineInstance(const PipelineConfiguration &config, DirectX12PipelineLoader::Handle pipeline)
        : PipelineInstance(config)
        , mPipelineHandle(std::move(pipeline))
    {
        mPipelines = mPipelineHandle->ptr();

        mConstantBuffers.reserve(config.bufferSizes.size());
        for (size_t i = 0; i < config.bufferSizes.size(); ++i) {
            mConstantBuffers.emplace_back(ByteBuffer{ nullptr, config.bufferSizes[i] });
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

        if (mInstanceBuffer)
            mInstanceBuffer.bindVertex(commandList, mInstanceDataSize, 1);

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

    void DirectX12PipelineInstance::setInstanceData(const ByteBuffer &data)
    {
        if (data.mSize > 0) {
            auto target = mInstanceBuffer.mapData(data.mSize);
            std::memcpy(target.mData, data.mData, data.mSize);
        }
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

    
    void DirectX12PipelineInstance::renderMesh(const GPUMeshData *m, const Material *material) const
    {
        renderMeshInstanced(1, m, material);
        /* ID3D12GraphicsCommandList *commandList = context()->mCommandList.mList;

        const DirectX12MeshData *mesh = static_cast<const DirectX12MeshData *>(m);
        const DirectX12PipelineInstance *pipeline = static_cast<const DirectX12PipelineInstance *>(p);

        mesh->mVertices.bindVertex(commandList, mesh->mVertexSize);

        pipeline->bind(commandList, mesh->mFormat, mesh->mGroupSize);

        if (material)
            bindTextures({ { material->mDiffuseTexture, TextureType_2D } });

        constexpr D3D12_PRIMITIVE_TOPOLOGY modes[] {
            D3D_PRIMITIVE_TOPOLOGY_POINTLIST,
            D3D_PRIMITIVE_TOPOLOGY_LINELIST,
            D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
        };

        assert(mesh->mGroupSize > 0 && mesh->mGroupSize <= 3);
        D3D12_PRIMITIVE_TOPOLOGY mode = modes[mesh->mGroupSize - 1];
        commandList->IASetPrimitiveTopology(mode);

        if (mesh->mIndices) {
            mesh->mIndices.bindIndex(commandList);
            commandList->DrawIndexedInstanced(mesh->mElementCount, 1, 0, 0, 0);
        } else {
            commandList->DrawInstanced(mesh->mElementCount, 1, 0, 0);
        }*/
    }

    void DirectX12PipelineInstance::renderMeshInstanced(size_t count, const GPUMeshData *m, const Material *material) const
    {
        ID3D12GraphicsCommandList *commandList = DirectX12RenderContext::getSingleton().mCommandList.mList;

        const DirectX12MeshData *mesh = static_cast<const DirectX12MeshData *>(m);

        if (!bind(commandList, m->mFormat, m->mGroupSize))
            return;

        mesh->mVertices.bindVertex(commandList, mesh->mVertexSize);

        if (material)
            bindTextures({ { material->mDiffuseTexture, TextureType_2D } });

        constexpr D3D12_PRIMITIVE_TOPOLOGY modes[] {
            D3D_PRIMITIVE_TOPOLOGY_POINTLIST,
            D3D_PRIMITIVE_TOPOLOGY_LINELIST,
            D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
        };

        assert(m->mGroupSize > 0 && m->mGroupSize <= 3);
        D3D12_PRIMITIVE_TOPOLOGY mode = modes[m->mGroupSize - 1];
        commandList->IASetPrimitiveTopology(mode);

        if (mesh->mIndices) {
            mesh->mIndices.bindIndex(commandList);
            commandList->DrawIndexedInstanced(m->mElementCount, count, 0, 0, 0);
        } else {
            commandList->DrawInstanced(m->mElementCount, count, 0, 0);
        }
    }

    void DirectX12PipelineInstance::bindTextures(const std::vector<TextureDescriptor> &tex, size_t offset) const
    {
        for (size_t i = 0; i < tex.size(); ++i) {
            if (tex[i].mTextureHandle) {
                D3D12_GPU_DESCRIPTOR_HANDLE handle;
                handle.ptr = tex[i].mTextureHandle;
                DirectX12RenderContext::getSingleton().mCommandList.mList->SetGraphicsRootDescriptorTable(3 + offset + i, handle);
            }
        }
    }

}
}