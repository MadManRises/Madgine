#include "vulkanlib.h"

#include "vulkanrendertarget.h"

#include "vulkanrendercontext.h"

#include "Meta/math/matrix4.h"
#include "Meta/math/rect2i.h"

namespace Engine {
namespace Render {

    extern uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    extern void transitionImageLayout(VkCommandBuffer commandList, VkImage image, VkFormat format, VkImageAspectFlags aspectMask, VkImageLayout oldLayout, VkImageLayout newLayout);

    VulkanRenderTarget::VulkanRenderTarget(VulkanRenderContext *context, bool global, std::string name)
        : RenderTarget(context, global, name)
    {

        mLastCompletedFenceValue = 5;

        VkSemaphoreTypeCreateInfo timelineCreateInfo;
        timelineCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
        timelineCreateInfo.pNext = NULL;
        timelineCreateInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
        timelineCreateInfo.initialValue = mLastCompletedFenceValue;

        VkSemaphoreCreateInfo semaphoreInfo;
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        semaphoreInfo.pNext = &timelineCreateInfo;
        semaphoreInfo.flags = 0;

        VkResult result = vkCreateSemaphore(GetDevice(), &semaphoreInfo, nullptr, &mSemaphore);
        VK_CHECK(result);

        mNextFenceValue = mLastCompletedFenceValue + 1;
    }

    VulkanRenderTarget::~VulkanRenderTarget()
    {
        for (auto &[fence, buffer] : mBufferPool) {
            vkFreeCommandBuffers(GetDevice(), context()->mCommandPool, 1, &buffer);
        }
    }

    void VulkanRenderTarget::setup(const Vector2i &size, VkFormat format, VkImageLayout layout, size_t dependencyCount, VkSubpassDependency *dependencies, bool createDepthView)
    {
        mSize = size;

        mDepthBuffer.reset();
        mDepthImage.reset();
        mDepthView.reset();

        VkImageCreateInfo imageInfo {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = size.x;
        imageInfo.extent.height = size.y;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = VK_FORMAT_D24_UNORM_S8_UINT;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        if (createDepthView)
            imageInfo.usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.flags = 0; // Optional

        VkResult result = vkCreateImage(GetDevice(), &imageInfo, nullptr, &mDepthImage);
        VK_CHECK(result);

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(GetDevice(), mDepthImage, &memRequirements);

        VkMemoryAllocateInfo allocInfo {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        result = vkAllocateMemory(GetDevice(), &allocInfo, nullptr, &mDepthBuffer);
        VK_CHECK(result);

        vkBindImageMemory(GetDevice(), mDepthImage, mDepthBuffer, 0);

        VkImageViewCreateInfo viewInfo {};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = mDepthImage;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = VK_FORMAT_D24_UNORM_S8_UINT;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        result = vkCreateImageView(GetDevice(), &viewInfo, nullptr, &mDepthView);
        VK_CHECK(result);

        VkAttachmentDescription attachments[2] {};
        attachments[0].format = format;
        attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;

        attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

        attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachments[0].finalLayout = layout;

        attachments[1].format = VK_FORMAT_D24_UNORM_S8_UINT;
        attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;

        attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

        attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

        attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference colorAttachmentRef {};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef {};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;

        VkRenderPassCreateInfo renderPassInfo {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 2;
        renderPassInfo.pAttachments = attachments;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = dependencyCount;
        renderPassInfo.pDependencies = dependencies;

        mRenderPass.reset();
        result = vkCreateRenderPass(GetDevice(), &renderPassInfo, nullptr, &mRenderPass);
        VK_CHECK(result);
    }

    void VulkanRenderTarget::shutdown()
    {
    }

    bool VulkanRenderTarget::skipFrame()
    {
        if (!isFenceComplete(mNextFenceValue - 1))
            return true;

        return false;
    }

    void VulkanRenderTarget::beginFrame()
    {
        mCommandList = fetchCommandList(name(), false);

        VkRenderPassBeginInfo renderPassInfo {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = mRenderPass;
        renderPassInfo.framebuffer = mFramebuffer;

        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = { static_cast<uint32_t>(mSize.x), static_cast<uint32_t>(mSize.y) };

        VkClearValue clearColors[2] { { { 0.2f, 0.3f, 0.3f, 1.0f } }, { { 1.0f, 1.0f, 1.0f, 1.0f } } };
        renderPassInfo.clearValueCount = 2;
        renderPassInfo.pClearValues = clearColors;

        vkCmdBeginRenderPass(mCommandList, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        setRenderSpace({ { 0, 0 }, mSize });

        RenderTarget::beginFrame();
    }

    void VulkanRenderTarget::endFrame()
    {
        RenderTarget::endFrame();

        vkCmdEndRenderPass(mCommandList);

        mCommandList.reset();
    }

    Matrix4 VulkanRenderTarget::getClipSpaceMatrix() const
    {
        return Matrix4 { 1, 0, 0, 0,
            0, -1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1 };
    }

    VulkanCommandList VulkanRenderTarget::fetchCommandList(std::string_view name, bool temp)
    {
        VkCommandBuffer buffer = nullptr;

        if (mBufferPool.size() > 1) {
            auto &[fenceValue, b] = mBufferPool.front();

            if (isFenceComplete(fenceValue)) {
                buffer = b;
                mBufferPool.erase(mBufferPool.begin());

                VkResult result = vkResetCommandBuffer(buffer, 0);
                VK_CHECK(result);
            }
        }

        if (!buffer) {
            VkCommandBufferAllocateInfo allocInfo {};
            allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocInfo.commandPool = context()->mCommandPool;
            allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocInfo.commandBufferCount = 1;

            VkResult result = vkAllocateCommandBuffers(GetDevice(), &allocInfo, &buffer);
            VK_CHECK(result);
        }

        VkCommandBufferBeginInfo beginInfo {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0; // Optional
        beginInfo.pInheritanceInfo = nullptr; // Optional

        VkResult result = vkBeginCommandBuffer(buffer, &beginInfo);
        VK_CHECK(result);

        vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, context()->mPipelineLayout, 2, 1, &context()->mSamplerDescriptorSet, 0, nullptr);

        return { this, buffer, temp };
    }

    void VulkanRenderTarget::ExecuteCommandList(NulledPtr<std::remove_pointer_t<VkCommandBuffer>> buffer, bool temp)
    {
        VkResult result = vkEndCommandBuffer(buffer);
        VK_CHECK(result);

        const uint64_t waitValues[] = { mNextFenceValue - 1, 0 };
        const uint64_t signalValues[] = { mNextFenceValue, 0 };

        VkTimelineSemaphoreSubmitInfo timelineInfo {};
        timelineInfo.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
        timelineInfo.pNext = NULL;
        timelineInfo.waitSemaphoreValueCount = 1 + (!temp && mImageSemaphore);
        timelineInfo.pWaitSemaphoreValues = waitValues;
        timelineInfo.signalSemaphoreValueCount = 1 + (!temp && mRenderSemaphore);
        timelineInfo.pSignalSemaphoreValues = signalValues;

        VkSubmitInfo submitInfo {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.pNext = &timelineInfo;

        VkSemaphore waitSemaphores[] = { mSemaphore, mImageSemaphore };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1 + (!temp && mImageSemaphore);
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        VkCommandBuffer dummy = buffer;
        submitInfo.pCommandBuffers = &dummy;

        VkSemaphore signalSemaphores[] = { mSemaphore, mRenderSemaphore };
        submitInfo.signalSemaphoreCount = 1 + (!temp && mRenderSemaphore);
        submitInfo.pSignalSemaphores = signalSemaphores;

        ++mNextFenceValue;

        result = vkQueueSubmit(context()->mGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        VK_CHECK(result);

        mBufferPool.emplace_back(mNextFenceValue, buffer);
    }

    bool VulkanRenderTarget::isFenceComplete(uint64_t fenceValue)
    {
        // if it's greater than last seen fence value
        // check fence for latest completed value
        if (fenceValue > mLastCompletedFenceValue)
            vkGetSemaphoreCounterValueKHR(GetDevice(), mSemaphore, &mLastCompletedFenceValue);

        return fenceValue <= mLastCompletedFenceValue;
    }

    void VulkanRenderTarget::beginIteration(size_t iteration) const
    {
        RenderTarget::beginIteration(iteration);
    }

    void VulkanRenderTarget::endIteration(size_t iteration) const
    {
        RenderTarget::endIteration(iteration);
    }

    void VulkanRenderTarget::setRenderSpace(const Rect2i &space)
    {
        VkViewport viewport {};
        viewport.width = static_cast<float>(space.mSize.x);
        viewport.height = static_cast<float>(space.mSize.y);
        viewport.x = static_cast<float>(space.mTopLeft.x);
        viewport.y = static_cast<float>(space.mTopLeft.y);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(mCommandList, 0, 1, &viewport);

        VkRect2D scissor {};
        scissor.offset = { space.mTopLeft.x, space.mTopLeft.y };
        scissor.extent = { static_cast<uint32_t>(space.mTopLeft.x + space.mSize.x), static_cast<uint32_t>(space.mTopLeft.y + space.mSize.y) };
        vkCmdSetScissor(mCommandList, 0, 1, &scissor);
    }

    void VulkanRenderTarget::pushAnnotation(const char *tag)
    {
        VkDebugUtilsLabelEXT labelInfo {};
        labelInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
        std::fill_n(labelInfo.color, 4, 1.0f);
        labelInfo.pLabelName = tag;
        vkCmdBeginDebugUtilsLabelEXT(mCommandList, &labelInfo);
    }

    void VulkanRenderTarget::popAnnotation()
    {
        vkCmdEndDebugUtilsLabelEXT(mCommandList);
    }

    void VulkanRenderTarget::clearDepthBuffer()
    {
    }

    TextureDescriptor VulkanRenderTarget::depthTexture() const
    {
        return { reinterpret_cast<TextureHandle>(mDepthView.get()), TextureType_2D };
    }

    VulkanRenderContext *VulkanRenderTarget::context() const
    {
        return static_cast<VulkanRenderContext *>(RenderTarget::context());
    }

}
}
