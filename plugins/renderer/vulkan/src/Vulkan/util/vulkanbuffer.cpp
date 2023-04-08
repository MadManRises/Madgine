#include "../vulkanlib.h"

#include "vulkanbuffer.h"

#include "../vulkanrendercontext.h"

#include "Generic/bytebuffer.h"

#include "Generic/align.h"

#include "vulkanconstantbufferheap.h"

namespace Engine {
namespace Render {

    VulkanBuffer::VulkanBuffer(VulkanBuffer &&other)
        : mAddress(std::exchange(other.mAddress, {}))
        , mSize(std::exchange(other.mSize, 0))
        , mIsPersistent(std::exchange(other.mIsPersistent, false))
    {
    }

    VulkanBuffer::VulkanBuffer(const ByteBuffer &data)
    {
        setData(data);
    }

    VulkanBuffer::~VulkanBuffer()
    {
        reset();
    }

    VulkanBuffer &VulkanBuffer::operator=(VulkanBuffer &&other)
    {
        std::swap(mAddress, other.mAddress);
        std::swap(mSize, other.mSize);
        std::swap(mIsPersistent, other.mIsPersistent);
        return *this;
    }

    VulkanBuffer::operator bool() const
    {
        return mSize > 0;
    }

    void VulkanBuffer::bindVertex(VkCommandBuffer commandList, size_t index) const
    {
        VkDeviceSize offset = mAddress.offset();
        VkBuffer b = buffer();
        vkCmdBindVertexBuffers(commandList, index, 1, &b, &offset);
    }

    void VulkanBuffer::bindIndex(VkCommandBuffer commandList) const
    {
        vkCmdBindIndexBuffer(commandList, buffer(), mAddress.offset(), VK_INDEX_TYPE_UINT32);
    }

    void VulkanBuffer::reset(size_t size)
    {
        if (mAddress && mIsPersistent) {
            VulkanConstantBufferHeap &heap = VulkanRenderContext::getSingleton().mConstantBufferHeap;

            heap.deallocatePersistent(mAddress, mSize);
        }

        mIsPersistent = false;
        mAddress.reset();
        mSize = size;
    }

    void VulkanBuffer::setData(const ByteBuffer &data)
    {
        size_t expectedSize = alignTo(data.mSize, 256);
        if (data.mData) {
            VulkanConstantBufferHeap &heap = VulkanRenderContext::getSingleton().mConstantBufferHeap;

            if (!mIsPersistent || mSize != expectedSize) {
                reset(expectedSize);

                mAddress = heap.allocatePersistent(expectedSize);
            }

            mIsPersistent = true;
            heap.setData(mAddress, data);
        } else {
            reset(expectedSize);
        }
    }

    WritableByteBuffer VulkanBuffer::mapData(size_t size)
    {
        VulkanConstantBufferHeap &heap = VulkanRenderContext::getSingleton().mConstantBufferHeap;

        reset(alignTo(size, 256));

        mAddress = heap.allocateTemp(mSize);

        return heap.map(mAddress, mSize);
    }

    WritableByteBuffer VulkanBuffer::mapData()
    {
        return mapData(mSize);
    }

    OffsetPtr VulkanBuffer::address() const
    {
        return mAddress;
    }

    VkBuffer VulkanBuffer::buffer() const
    {
        VulkanConstantBufferHeap &heap = VulkanRenderContext::getSingleton().mConstantBufferHeap;
        return mSize > 0 ? (mIsPersistent ? heap.resourcePersistent() : heap.resourceTemp()) : nullptr;
    }

    size_t VulkanBuffer::size() const
    {
        return mSize;
    }

}
}