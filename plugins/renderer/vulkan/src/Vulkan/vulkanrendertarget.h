#pragma once

#include "Madgine/render/rendertarget.h"

#include "util/vulkancommandlist.h"

#include "util/vulkantexture.h"

namespace Engine {
namespace Render {

    struct MADGINE_VULKAN_EXPORT VulkanRenderTarget : RenderTarget {

        VulkanRenderTarget(VulkanRenderContext *context, bool global, std::string name, size_t samples = 1, RenderTarget *blitSource = nullptr);
        ~VulkanRenderTarget();

        void createRenderPass(size_t colorAttachmentCount, VkFormat format, VkImageLayout layout, bool createDepthBufferView, std::span<VkSubpassDependency> dependencies);
        void setup(const Vector2i &size, bool createDepthBufferView = false);
        void shutdown();

        virtual bool skipFrame() override;
        void beginFrame() override;
        void endFrame() override;

        VulkanCommandList fetchCommandList(std::string_view name, bool temp);
        void ExecuteCommandList(NulledPtr<std::remove_pointer_t<VkCommandBuffer>> buffer, bool temp);

        bool isFenceComplete(uint64_t fenceValue);

        virtual void beginIteration(size_t iteration) const override;
        virtual void endIteration(size_t iteration) const override;

        virtual void pushAnnotation(const char *tag) override;
        virtual void popAnnotation() override;

        virtual Matrix4 getClipSpaceMatrix() const override;

        size_t samples() const;

        virtual void setRenderSpace(const Rect2i &space) override;

        virtual TextureDescriptor depthTexture() const override;

        virtual void clearDepthBuffer() override;

        virtual void blit(RenderTarget *input) = 0;

        VulkanRenderContext *context() const;

        VulkanPtr<VkRenderPass, &vkDestroyRenderPass> mRenderPass;

        VulkanCommandList mCommandList;

        Vector2i mSize;

        VkFramebuffer mFramebuffer;

        std::vector<std::pair<uint64_t, VkCommandBuffer>> mBufferPool;

        VulkanPtr<VkSemaphore, &vkDestroySemaphore> mSemaphore;

        VulkanPtr<VkSemaphore, &vkDestroySemaphore> mImageSemaphore;
        VulkanPtr<VkSemaphore, &vkDestroySemaphore> mRenderSemaphore;

        VulkanPtr<VkImage, &vkDestroyImage> mDepthImage;
        VulkanPtr<VkDeviceMemory, &vkFreeMemory> mDepthBuffer;
        VulkanPtr<VkImageView, &vkDestroyImageView> mDepthView;

        uint64_t mLastCompletedFenceValue = 0;
        uint64_t mNextFenceValue;

        size_t mSamples;
    };

}
}