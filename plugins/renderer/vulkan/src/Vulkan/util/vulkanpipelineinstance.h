#pragma once

#include "Madgine/pipelineloader/pipelineinstance.h"

#include "../vulkanpipelineloader.h"

#include "vulkanbuffer.h"

namespace Engine {
namespace Render {

    struct MADGINE_VULKAN_EXPORT VulkanPipelineInstance : PipelineInstance {

        VulkanPipelineInstance(const PipelineConfiguration &config, VulkanPipelineLoader::Handle pipeline);
        ~VulkanPipelineInstance();

        bool bind(VkCommandBuffer commandList, VertexFormat format, size_t groupSize, size_t samples, VkRenderPass renderpass) const;

        virtual WritableByteBuffer mapParameters(size_t index) override;

        virtual void setDynamicParameters(size_t index, const ByteBuffer &data) override;

        virtual void renderMesh(RenderTarget *target, const GPUMeshData *mesh) const override;
        virtual void renderMeshInstanced(RenderTarget *target, size_t count, const GPUMeshData *mesh, const ByteBuffer &instanceData) const override;

        virtual void bindTextures(RenderTarget *target, const std::vector<TextureDescriptor> &tex, size_t offset = 0) const override;

    private:
        std::array<std::array<VulkanPtr<VkPipeline, &vkDestroyPipeline>, 3>, 3> *mPipelines;

        std::vector<VulkanBuffer> mConstantBuffers;
        std::vector<VulkanBuffer> mDynamicBuffers;

        VulkanPipelineLoader::Handle mPipelineHandle;

        VkDescriptorSet mUboDescriptorSet;
    };

}
}