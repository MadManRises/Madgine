#include "../vulkanlib.h"

#include "vulkancommandlist.h"
#include "../vulkanrendertarget.h"

namespace Engine {
namespace Render {

    VulkanCommandList::VulkanCommandList(VulkanRenderTarget *target, NulledPtr<std::remove_pointer_t<VkCommandBuffer>> buffer, bool temp)
        : mTarget(target)
        , mBuffer(std::move(buffer))
        , mTemp(temp)
    {
    }

    VulkanCommandList::~VulkanCommandList()
    {
        reset();
    }

    VulkanCommandList &VulkanCommandList::operator=(VulkanCommandList &&other)
    {
        reset();
        mTarget = std::exchange(other.mTarget, nullptr);
        mBuffer = std::move(other.mBuffer);
        mTemp = std::exchange(other.mTemp, false);
        return *this;
    }

    void VulkanCommandList::reset()
    {
        if (mBuffer) {
            mTarget->ExecuteCommandList(std::move(mBuffer), mTemp);
        }
    }

    VulkanCommandList::operator VkCommandBuffer() const
    {
        return mBuffer;
    }

}
}