#pragma once

#include "Madgine/pipelineloader/pipeline.h"

#include "../vulkanshaderloader.h"

#include "Madgine/render/vertexformat.h"

namespace Engine {
namespace Render {

    struct MADGINE_VULKAN_EXPORT VulkanPipeline : Pipeline {

        bool link(typename VulkanShaderLoader::Handle vertexShader, typename VulkanShaderLoader::Handle geometryShader, typename VulkanShaderLoader::Handle pixelShader);

        VkPipeline get(VertexFormat format, size_t groupSize, size_t samples, size_t instanceDataSize, VkRenderPass renderpass);

        std::array<std::array<VulkanPtr<VkPipeline, &vkDestroyPipeline>, 3>, 3> *ptr();
        
        void reset();

    private:
        std::array < std::array<std::array<VulkanPtr<VkPipeline, &vkDestroyPipeline>, 3>, 3>, 256> mPipelines;

        typename VulkanShaderLoader::Handle mVertexShader;
        typename VulkanShaderLoader::Handle mGeometryShader;
        typename VulkanShaderLoader::Handle mPixelShader;
    };

}
}