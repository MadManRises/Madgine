#include "../vulkanlib.h"

#include "vulkantexture.h"

#include "Generic/align.h"

#include "../vulkanrendercontext.h"

namespace Engine {
namespace Render {

    extern void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer *buffer, VkDeviceMemory *bufferMemory);
    extern uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    void transitionImageLayout(VkCommandBuffer commandList, VkImage image, VkFormat format, VkImageAspectFlags aspectMask, VkImageLayout oldLayout, VkImageLayout newLayout)
    {
        VkImageMemoryBarrier barrier {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = aspectMask;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        switch (oldLayout) {
        case VK_IMAGE_LAYOUT_UNDEFINED:
            barrier.srcAccessMask = 0;
            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            break;
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
            sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            break;
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            break;
        default:
            throw 0;
        }

        switch (newLayout) {
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            break;
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            break;
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
            break;
        default:
            throw 0;
        }
        vkCmdPipelineBarrier(
            commandList,
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier);
    }

    VulkanTexture::VulkanTexture(TextureType type, bool isRenderTarget, DataFormat format, size_t width, size_t height, const ByteBuffer &data)
        : Texture(type, format, { static_cast<int>(width), static_cast<int>(height) })
        , mIsRenderTarget(isRenderTarget)
    {
        VkFormat vFormat;
        VkImageViewType vType;
        size_t byteCount;
        switch (format) {
        case FORMAT_RGBA8:
            vFormat = VK_FORMAT_R8G8B8A8_UNORM;
            byteCount = 4;
            break;
        case FORMAT_RGBA16F:
            vFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
            byteCount = 8;
            break;
        default:
            std::terminate();
        }

        VkDeviceSize imageSize = width * height * byteCount;

        switch (type) {
        case TextureType_2D: {
            VkImageCreateInfo imageInfo {};
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.imageType = VK_IMAGE_TYPE_2D;
            imageInfo.extent.width = width;
            imageInfo.extent.height = height;
            imageInfo.extent.depth = 1;
            imageInfo.mipLevels = 1;
            imageInfo.arrayLayers = 1;
            imageInfo.format = vFormat;
            imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
            imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
            if (isRenderTarget)
                imageInfo.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
            imageInfo.flags = 0; // Optional

            VkResult result = vkCreateImage(GetDevice(), &imageInfo, nullptr, &mImage);
            VK_CHECK(result);

            VkMemoryRequirements memRequirements;
            vkGetImageMemoryRequirements(GetDevice(), mImage, &memRequirements);

            VkMemoryAllocateInfo allocInfo {};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memRequirements.size;
            allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

            result = vkAllocateMemory(GetDevice(), &allocInfo, nullptr, &mDeviceMemory);
            VK_CHECK(result);

            vkBindImageMemory(GetDevice(), mImage, mDeviceMemory, 0);

            VulkanRenderContext &context = VulkanRenderContext::getSingleton();

            VkCommandBufferAllocateInfo cmdAllocInfo {};
            cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            cmdAllocInfo.commandPool = context.mCommandPool;
            cmdAllocInfo.commandBufferCount = 1;

            VkCommandBuffer commandBuffer;
            vkAllocateCommandBuffers(GetDevice(), &cmdAllocInfo, &commandBuffer);

            VkCommandBufferBeginInfo beginInfo {};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

            vkBeginCommandBuffer(commandBuffer, &beginInfo);

            VulkanPtr<VkDeviceMemory, &vkFreeMemory> stagingBufferMemory;
            VulkanPtr<VkBuffer, &vkDestroyBuffer> stagingBuffer;
            if (data.mData) {

                createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

                void *ptr;
                vkMapMemory(GetDevice(), stagingBufferMemory, 0, imageSize, 0, &ptr);
                memcpy(ptr, data.mData, static_cast<size_t>(imageSize));
                vkUnmapMemory(GetDevice(), stagingBufferMemory);

                transitionImageLayout(commandBuffer, mImage, vFormat, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

                VkBufferImageCopy region {};
                region.bufferOffset = 0;
                region.bufferRowLength = 0;
                region.bufferImageHeight = 0;

                region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                region.imageSubresource.mipLevel = 0;
                region.imageSubresource.baseArrayLayer = 0;
                region.imageSubresource.layerCount = 1;

                region.imageOffset = { 0, 0, 0 };
                region.imageExtent = {
                    static_cast<uint32_t>(width),
                    static_cast<uint32_t>(height),
                    1
                };
                vkCmdCopyBufferToImage(
                    commandBuffer,
                    stagingBuffer,
                    mImage,
                    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                    1,
                    &region);

                transitionImageLayout(commandBuffer, mImage, vFormat, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

            } else {
                transitionImageLayout(commandBuffer, mImage, vFormat, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            }
            vkEndCommandBuffer(commandBuffer);

            VkSubmitInfo submitInfo {};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &commandBuffer;

            vkQueueSubmit(context.mGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
            vkQueueWaitIdle(context.mGraphicsQueue);

            vkFreeCommandBuffers(GetDevice(), context.mCommandPool, 1, &commandBuffer);

            vType = VK_IMAGE_VIEW_TYPE_2D;

            break;
        }
        default:
            std::terminate();
        }

        VkImageViewCreateInfo viewInfo {};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = mImage;
        viewInfo.viewType = vType;
        viewInfo.format = vFormat;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        VkResult result = vkCreateImageView(GetDevice(), &viewInfo, nullptr, reinterpret_cast<VkImageView *>(&mTextureHandle));
        VK_CHECK(result);
    }

    VulkanTexture::VulkanTexture(TextureType type, bool isRenderTarget, DataFormat format)
        : Texture(type, format)
        , mIsRenderTarget(isRenderTarget)
    {
    }

    VulkanTexture::VulkanTexture(VulkanTexture &&other)
        : Texture(std::move(other))
        , mIsRenderTarget(std::exchange(other.mIsRenderTarget, false))
    {
    }

    VulkanTexture::~VulkanTexture()
    {
        reset();
    }

    VulkanTexture &VulkanTexture::operator=(VulkanTexture &&other)
    {
        Texture::operator=(std::move(other));
        std::swap(mDeviceMemory, other.mDeviceMemory);
        std::swap(mImage, other.mImage);
        std::swap(mIsRenderTarget, other.mIsRenderTarget);
        return *this;
    }

    void VulkanTexture::reset()
    {
        if (mTextureHandle) {
            vkDestroyImageView(GetDevice(), reinterpret_cast<VkImageView>(mTextureHandle), nullptr);
            mTextureHandle = 0;
        }
        mDeviceMemory.reset();
        mImage.reset();
    }

    void VulkanTexture::setData(Vector2i size, const ByteBuffer &data)
    {
        *this = VulkanTexture { mType, mIsRenderTarget, mFormat, static_cast<size_t>(size.x), static_cast<size_t>(size.y), data };
    }

    void VulkanTexture::setSubData(Vector2i offset, Vector2i size, const ByteBuffer &data)
    {
        VkFormat vFormat;
        VkImageViewType vType;
        size_t byteCount;
        switch (mFormat) {
        case FORMAT_RGBA8:
            vFormat = VK_FORMAT_R8G8B8A8_UNORM;
            byteCount = 4;
            break;
        case FORMAT_RGBA16F:
            vFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
            byteCount = 8;
            break;
        default:
            std::terminate();
        }
        VkDeviceSize imageSize = size.x * size.y * byteCount;

        VulkanRenderContext &context = VulkanRenderContext::getSingleton();

        VkCommandBufferAllocateInfo cmdAllocInfo {};
        cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        cmdAllocInfo.commandPool = context.mCommandPool;
        cmdAllocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(GetDevice(), &cmdAllocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        VulkanPtr<VkDeviceMemory, &vkFreeMemory> stagingBufferMemory;
        VulkanPtr<VkBuffer, &vkDestroyBuffer> stagingBuffer;
        if (data.mData) {

            createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

            void *ptr;
            vkMapMemory(GetDevice(), stagingBufferMemory, 0, imageSize, 0, &ptr);
            memcpy(ptr, data.mData, static_cast<size_t>(imageSize));
            vkUnmapMemory(GetDevice(), stagingBufferMemory);

            transitionImageLayout(commandBuffer, mImage, vFormat, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

            VkBufferImageCopy region {};
            region.bufferOffset = 0;
            region.bufferRowLength = 0;
            region.bufferImageHeight = 0;

            region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            region.imageSubresource.mipLevel = 0;
            region.imageSubresource.baseArrayLayer = 0;
            region.imageSubresource.layerCount = 1;

            region.imageOffset = { offset.x, offset.y, 0 };
            region.imageExtent = {
                static_cast<uint32_t>(size.x),
                static_cast<uint32_t>(size.y),
                1
            };
            vkCmdCopyBufferToImage(
                commandBuffer,
                stagingBuffer,
                mImage,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1,
                &region);

            transitionImageLayout(commandBuffer, mImage, vFormat, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        } else {
            transitionImageLayout(commandBuffer, mImage, vFormat, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(context.mGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(context.mGraphicsQueue);

        vkFreeCommandBuffers(GetDevice(), context.mCommandPool, 1, &commandBuffer);
    }

    VkImageView VulkanTexture::view() const
    {
        return reinterpret_cast<VkImageView>(mTextureHandle);
    }

    void VulkanTexture::setName(std::string_view name)
    {
    }
}
}