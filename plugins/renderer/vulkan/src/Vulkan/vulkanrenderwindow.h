#pragma once

#include "vulkanrendertarget.h"

namespace Engine {
namespace Render {

    struct MADGINE_VULKAN_EXPORT VulkanRenderWindow : VulkanRenderTarget {
        VulkanRenderWindow(VulkanRenderContext *context, Window::OSWindow *w, size_t samples);
        VulkanRenderWindow(const VulkanRenderWindow &) = delete;
        ~VulkanRenderWindow();

        virtual bool skipFrame() override;
        virtual void beginFrame() override;
        virtual void endFrame() override;

        virtual void beginIteration(size_t iteration) const override;
        virtual void endIteration(size_t iteration) const override;

        virtual bool resizeImpl(const Vector2i &size) override;
        virtual Vector2i size() const override;

        uint32_t imageCount() const;
        uint32_t minImageCount() const;

        VkSurfaceKHR surface() const;

        virtual void blit(RenderTarget *source) override;

    protected:
        void create(const Vector2i &size);

    private:
        Window::OSWindow *mWindow;

        uint64_t mResizeFence;
        Vector2i mResizeTarget;

        VkSurfaceCapabilitiesKHR mSurfaceCapabilities;
        VulkanPtr3<VkSurfaceKHR, &vkDestroySurfaceKHR> mSurface;
        VulkanPtr<VkSwapchainKHR, &vkDestroySwapchainKHR> mSwapChain;
        uint32_t mImageIndex;
        std::vector<VkImage> mSwapChainImages;
        std::vector<VulkanPtr<VkImageView, &vkDestroyImageView>> mSwapChainImageViews;
        std::vector<VulkanPtr<VkFramebuffer, &vkDestroyFramebuffer>> mFramebuffers;
        VkSurfaceFormatKHR mFormat;        
    };

}
}

REGISTER_TYPE(Engine::Render::VulkanRenderWindow)