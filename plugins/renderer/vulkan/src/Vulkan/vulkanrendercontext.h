#pragma once

#include "Madgine/render/rendercontext.h"
#include "Madgine/render/rendercontextcollector.h"

#include "Modules/threading/threadlocal.h"

#include "util/vulkancommandlist.h"

#include "util/vulkanbuffer.h"

#include "Madgine/render/vertexformat.h"

#include "util/vulkanconstantbufferheap.h"

namespace Engine {
namespace Render {

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete()
        {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    MADGINE_VULKAN_EXPORT VkDevice GetDevice();
    MADGINE_VULKAN_EXPORT VkInstance GetInstance();
    MADGINE_VULKAN_EXPORT VkPhysicalDevice GetPhysicalDevice();

    struct MADGINE_VULKAN_EXPORT VulkanRenderContext : public RenderContextComponent<VulkanRenderContext> {
        VulkanRenderContext(Threading::TaskQueue *queue);
        ~VulkanRenderContext();

        virtual std::unique_ptr<RenderTarget> createRenderWindow(Window::OSWindow *w, size_t samples) override;
        virtual std::unique_ptr<RenderTarget> createRenderTexture(const Vector2i &size = { 1, 1 }, const RenderTextureConfig &config = {}) override;

        virtual bool beginFrame() override;
        virtual void endFrame() override;

        virtual Threading::Task<void> unloadAllResources() override;

        virtual bool supportsMultisampling() const override;

        static VulkanRenderContext &getSingleton();

        void createPipelineLayout();

        static std::pair<std::vector<VkVertexInputBindingDescription>, std::vector<VkVertexInputAttributeDescription>> createVertexLayout(VertexFormat format, size_t instanceDataSize);

        VkQueue mGraphicsQueue;
        VkQueue mPresentQueue;

        QueueFamilyIndices mIndices;

        VulkanPtr<VkDescriptorPool, &vkDestroyDescriptorPool> mDescriptorPool;
        VulkanPtr<VkCommandPool, &vkDestroyCommandPool> mCommandPool;

        VulkanPtr<VkDescriptorSetLayout, &vkDestroyDescriptorSetLayout> mDescriptorSetLayouts[3];
        VulkanPtr<VkPipelineLayout, &vkDestroyPipelineLayout> mPipelineLayout;

        VulkanPtr<VkSampler, &vkDestroySampler> mSamplers[2];
        VkDescriptorSet mSamplerDescriptorSet;

        VulkanConstantBufferHeap mConstantBufferHeap;

        VulkanBuffer mConstantBuffer;
    };

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

}
}

REGISTER_TYPE(Engine::Render::VulkanRenderContext)