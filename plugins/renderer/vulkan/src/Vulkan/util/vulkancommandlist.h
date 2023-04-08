#pragma once

#include "Generic/nulledptr.h"

namespace Engine {
namespace Render {

    struct MADGINE_VULKAN_EXPORT VulkanCommandList {
        VulkanCommandList() = default;
        VulkanCommandList(VulkanRenderTarget *target, NulledPtr<std::remove_pointer_t<VkCommandBuffer>> buffer, bool temp);
        ~VulkanCommandList();

        VulkanCommandList &operator=(VulkanCommandList &&);

        void reset();

        operator VkCommandBuffer() const;

    private:
        VulkanRenderTarget *mTarget;
        NulledPtr<std::remove_pointer_t<VkCommandBuffer>> mBuffer;
        bool mTemp;
    };

}
}