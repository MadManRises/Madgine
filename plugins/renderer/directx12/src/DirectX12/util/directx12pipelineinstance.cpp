#include "../directx12lib.h"

#include "directx12pipelineinstance.h"

#include "../directx12rendercontext.h"

namespace Engine {
namespace Render {

    DirectX12PipelineInstance::DirectX12PipelineInstance(const PipelineConfiguration &config, DirectX12PipelineLoader::HandleType pipeline)
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
}
}