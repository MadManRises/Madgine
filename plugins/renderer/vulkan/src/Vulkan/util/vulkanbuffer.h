#pragma once

#include "Generic/offsetptr.h"

namespace Engine {
namespace Render {

    struct MADGINE_VULKAN_EXPORT VulkanBuffer {

        VulkanBuffer() = default;
        VulkanBuffer(const ByteBuffer &data);
        VulkanBuffer(const VulkanBuffer &) = delete;
        VulkanBuffer(VulkanBuffer &&);
        ~VulkanBuffer();

        VulkanBuffer &operator=(const VulkanBuffer &) = delete;
        VulkanBuffer &operator=(VulkanBuffer &&);

        explicit operator bool() const;

        void bindVertex(VkCommandBuffer commandList, size_t index = 0) const;
        void bindIndex(VkCommandBuffer commandList) const;

        void reset(size_t size = 0);
        void setData(const ByteBuffer &data);

        WritableByteBuffer mapData(size_t size);
        WritableByteBuffer mapData();

        OffsetPtr address() const;
        VkBuffer buffer() const;

        size_t size() const;

    private:
        size_t mSize = 0;
        bool mIsPersistent;
        OffsetPtr mAddress;
    };

}
}