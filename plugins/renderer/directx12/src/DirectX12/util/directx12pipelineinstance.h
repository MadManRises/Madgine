#pragma once

#include "Madgine/pipelineloader/pipelineinstance.h"

#include "../directx12pipelineloader.h"

#include "directx12buffer.h"

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX12_EXPORT DirectX12PipelineInstance : PipelineInstance {

        DirectX12PipelineInstance(const PipelineConfiguration &config, const DirectX12Pipeline *pipeline);

        bool bind(ID3D12GraphicsCommandList *commandList, VertexFormat format, size_t groupSize, size_t samples) const;

        virtual WritableByteBuffer mapParameters(size_t index) override;

        virtual void setDynamicParameters(size_t index, const ByteBuffer &data) override;

        virtual void renderMesh(RenderTarget *target, const GPUMeshData *mesh) const override;
        virtual void renderMeshInstanced(RenderTarget *target, size_t count, const GPUMeshData *mesh, const ByteBuffer &instanceData) const override;

        virtual void bindTextures(RenderTarget *target, const std::vector<TextureDescriptor> &tex, size_t offset = 0) const override;        

    private:
        const DirectX12Pipeline *mPipeline;

        std::vector<DirectX12Buffer> mConstantBuffers;
        std::vector<DirectX12Buffer> mDynamicBuffers;

        DirectX12PipelineLoader::Handle mPipelineHandle;
    };

    struct MADGINE_DIRECTX12_EXPORT DirectX12PipelineInstanceHandle : DirectX12PipelineInstance {

        DirectX12PipelineInstanceHandle(const PipelineConfiguration &config, DirectX12PipelineLoader::Handle pipeline);

    private:
        DirectX12PipelineLoader::Handle mPipelineHandle;
    };

    struct MADGINE_DIRECTX12_EXPORT DirectX12PipelineInstancePtr : DirectX12PipelineInstance {

        DirectX12PipelineInstancePtr(const PipelineConfiguration &config, DirectX12PipelineLoader::Ptr pipeline);

    private:
        DirectX12PipelineLoader::Ptr mPipelinePtr;
    };

}
}