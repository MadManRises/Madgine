#include "vulkanlib.h"

#include "vulkanrenderwindow.h"

#include "Interfaces/window/windowapi.h"

#include "vulkanrendercontext.h"

#if ANDROID
#    include <vulkan/vulkan_android.h>
#endif

namespace Engine {
namespace Render {

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats)
    {
        for (const auto &availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, const Vector2i &size)
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        } else {

            VkExtent2D actualExtent = {
                static_cast<uint32_t>(size.x),
                static_cast<uint32_t>(size.y)
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes)
    {
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VulkanRenderWindow::VulkanRenderWindow(VulkanRenderContext *context, Window::OSWindow *w, size_t samples)
        : VulkanRenderTarget(context, true, w->title(), samples)
        , mWindow(w)
    {
        VkSemaphoreCreateInfo binarySemaphoreInfo {};
        binarySemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        VkResult result = vkCreateSemaphore(GetDevice(), &binarySemaphoreInfo, nullptr, &mImageSemaphore);
        VK_CHECK(result);

        result = vkCreateSemaphore(GetDevice(), &binarySemaphoreInfo, nullptr, &mRenderSemaphore);
        VK_CHECK(result);

#if WINDOWS
        VkWin32SurfaceCreateInfoKHR createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        createInfo.hwnd = (HWND)w->mHandle;
        createInfo.hinstance = GetModuleHandle(nullptr);

        result = vkCreateWin32SurfaceKHR(GetInstance(), &createInfo, nullptr, &mSurface);
        VK_CHECK(result);
#elif ANDROID
        VkAndroidSurfaceCreateInfoKHR createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
        createInfo.window = (ANativeWindow *)w->mHandle;

        result = vkCreateAndroidSurfaceKHR(GetInstance(), &createInfo, nullptr, &mSurface);
        VK_CHECK(result);
#else
#    error "Unsupported Platform!"
#endif

        SwapChainSupportDetails support = querySwapChainSupport(GetPhysicalDevice(), mSurface);
        assert(!support.formats.empty() && !support.presentModes.empty());

        mSurfaceCapabilities = support.capabilities;

        mFormat = chooseSwapSurfaceFormat(support.formats);

        VkSubpassDependency dependency {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        createRenderPass(1, mFormat.format, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, false, { &dependency, 1 });

        InterfacesVector size = w->renderSize();
        create({ size.x, size.y });
    }

    VulkanRenderWindow::~VulkanRenderWindow()
    {

        shutdown();
    }

    bool VulkanRenderWindow::skipFrame()
    {
        if (mSwapChainImages.empty() && isFenceComplete(mResizeFence)) {
            mFramebuffers.clear();
            mSwapChainImageViews.clear();

            mSwapChain.reset();

            create(mResizeTarget);
        }
        if (mSwapChainImages.empty())
            return true;
        return VulkanRenderTarget::skipFrame();
    }

    void VulkanRenderWindow::beginFrame()
    {
        VkResult result = vkAcquireNextImageKHR(GetDevice(), mSwapChain, UINT64_MAX, mImageSemaphore, VK_NULL_HANDLE, &mImageIndex);
        VK_CHECK(result);
        mFramebuffer = mFramebuffers[mImageIndex];
        VulkanRenderTarget::beginFrame();
    }

    void VulkanRenderWindow::endFrame()
    {
        VulkanRenderTarget::endFrame();

        VkPresentInfoKHR presentInfo {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &std::as_const(mRenderSemaphore);

        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &std::as_const(mSwapChain);
        presentInfo.pImageIndices = &mImageIndex;

        presentInfo.pResults = nullptr; // Optional

        VkResult result = vkQueuePresentKHR(context()->mPresentQueue, &presentInfo);
        VK_CHECK(result);
    }

    void VulkanRenderWindow::beginIteration(size_t iteration) const
    {
        VulkanRenderTarget::beginIteration(iteration);
    }

    void VulkanRenderWindow::endIteration(size_t iteration) const
    {
        VulkanRenderTarget::endIteration(iteration);
    }

    Vector2i VulkanRenderWindow::size() const
    {
        InterfacesVector size = mWindow->renderSize();
        return { size.x, size.y };
    }

    uint32_t VulkanRenderWindow::imageCount() const
    {
        return mSwapChainImages.size();
    }

    uint32_t VulkanRenderWindow::minImageCount() const
    {
        return mSurfaceCapabilities.minImageCount;
    }

    VkSurfaceKHR VulkanRenderWindow::surface() const
    {
        return mSurface;
    }

    void VulkanRenderWindow::create(const Vector2i &size)
    {
        VkExtent2D extent = chooseSwapExtent(mSurfaceCapabilities, size);

        uint32_t imageCount = mSurfaceCapabilities.minImageCount + 1;
        if (mSurfaceCapabilities.maxImageCount > 0 && imageCount > mSurfaceCapabilities.maxImageCount) {
            imageCount = mSurfaceCapabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo2 {};
        createInfo2.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo2.surface = mSurface;
        createInfo2.minImageCount = imageCount;
        createInfo2.imageFormat = mFormat.format;
        createInfo2.imageColorSpace = mFormat.colorSpace;
        createInfo2.imageExtent = extent;
        createInfo2.imageArrayLayers = 1;
        createInfo2.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = findQueueFamilies(GetPhysicalDevice());
        uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

        if (indices.graphicsFamily != indices.presentFamily) {
            createInfo2.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo2.queueFamilyIndexCount = 2;
            createInfo2.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            createInfo2.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo2.queueFamilyIndexCount = 0; // Optional
            createInfo2.pQueueFamilyIndices = nullptr; // Optional
        }

        createInfo2.preTransform = mSurfaceCapabilities.currentTransform;

        createInfo2.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

        createInfo2.presentMode = VK_PRESENT_MODE_FIFO_KHR;
        createInfo2.clipped = VK_TRUE;

        createInfo2.oldSwapchain = VK_NULL_HANDLE;

        VkResult result = vkCreateSwapchainKHR(GetDevice(), &createInfo2, nullptr, &mSwapChain);
        VK_CHECK(result);

        setup(size);

        result = vkGetSwapchainImagesKHR(GetDevice(), mSwapChain, &imageCount, nullptr);
        VK_CHECK(result);

        mSwapChainImages.resize(imageCount);

        result = vkGetSwapchainImagesKHR(GetDevice(), mSwapChain, &imageCount, mSwapChainImages.data());
        VK_CHECK(result);

        mSwapChainImageViews.resize(imageCount);
        mFramebuffers.resize(imageCount);
        for (size_t i = 0; i < imageCount; ++i) {
            VkImageViewCreateInfo createInfo {};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = mSwapChainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = mFormat.format;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            VkResult result = vkCreateImageView(GetDevice(), &createInfo, nullptr, &mSwapChainImageViews[i]);
            VK_CHECK(result);

            VkImageView attachments[2] { mDepthView, mSwapChainImageViews[i] };
            VkFramebufferCreateInfo framebufferInfo {};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = mRenderPass;
            framebufferInfo.attachmentCount = 2;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = mSize.x;
            framebufferInfo.height = mSize.y;
            framebufferInfo.layers = 1;

            result = vkCreateFramebuffer(GetDevice(), &framebufferInfo, nullptr, &mFramebuffers[i]);
            VK_CHECK(result);
        }
    }

    bool VulkanRenderWindow::resizeImpl(const Vector2i &size)
    {
        mSwapChainImages.clear();

        mResizeFence = mNextFenceValue - 1;
        mResizeTarget = size;

        return true;
    }

    void VulkanRenderWindow::blit(RenderTarget *source)
    {
        throw 0;
    }
}
}