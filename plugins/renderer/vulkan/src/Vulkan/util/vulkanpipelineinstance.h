#pragma once

#include "Madgine/pipelineloader/pipelineinstance.h"

#include "../vulkanpipelineloader.h"

#include "vulkanbuffer.h"

namespace Engine {
namespace Render {

    struct MADGINE_VULKAN_EXPORT VulkanPipelineInstance : PipelineInstance {

        VulkanPipelineInstance(const PipelineConfiguration &config, const VulkanPipeline *pipeline);
        ~VulkanPipelineInstance();

        bool bind(VkCommandBuffer commandList, VertexFormat format, size_t groupSize, size_t samples, VkRenderPass renderpass) const;

        virtual WritableByteBuffer mapParameters(size_t index) override;

        virtual void renderMesh(RenderTarget *target, const GPUMeshData *mesh) const override;
        virtual void renderMeshInstanced(RenderTarget *target, size_t count, const GPUMeshData *mesh, const ByteBuffer &instanceData) const override;

        virtual void bindTextures(RenderTarget *target, const std::vector<TextureDescriptor> &tex, size_t offset = 0) const override;

    private:
        const VulkanPipeline *mPipeline;

        std::vector<VulkanBuffer> mConstantBuffers;

        VkDescriptorSet mUboDescriptorSet;
    };

    struct MADGINE_VULKAN_EXPORT VulkanPipelineInstanceHandle : VulkanPipelineInstance {

        VulkanPipelineInstanceHandle(const PipelineConfiguration &config, VulkanPipelineLoader::Handle pipeline);

    private:
        VulkanPipelineLoader::Handle mPipelineHandle;
    };

    struct MADGINE_VULKAN_EXPORT VulkanPipelineInstancePtr : VulkanPipelineInstance {

        VulkanPipelineInstancePtr(const PipelineConfiguration &config, VulkanPipelineLoader::Ptr pipeline);

    private:
        VulkanPipelineLoader::Ptr mPipelinePtr;
    };

}
}