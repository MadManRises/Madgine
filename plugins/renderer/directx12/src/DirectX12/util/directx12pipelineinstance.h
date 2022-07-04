#pragma once

#include "pipelineinstance.h"

#include "../directx12pipelineloader.h"

#include "directx12buffer.h"

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX12_EXPORT DirectX12PipelineInstance : PipelineInstance {

        DirectX12PipelineInstance(const PipelineConfiguration &config, DirectX12PipelineLoader::Handle pipeline);

        bool bind(ID3D12GraphicsCommandList *commandList, VertexFormat format, size_t groupSize) const;

        virtual WritableByteBuffer mapParameters(size_t index) override;

        virtual void setInstanceData(const ByteBuffer &data) override;

        virtual void setDynamicParameters(size_t index, const ByteBuffer &data) override;

    private:
        std::array<ReleasePtr<ID3D12PipelineState>, 3> *mPipelines;

        std::vector<DirectX12Buffer> mConstantBuffers;
        std::vector<DirectX12Buffer> mDynamicBuffers;

        DirectX12Buffer mInstanceBuffer;

        DirectX12PipelineLoader::Handle mPipelineHandle;
    };

}
}