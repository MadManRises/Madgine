#include "../vulkanlib.h"

#include "vulkanpipelineinstance.h"

#include "../vulkanrendercontext.h"

#include "../vulkanmeshdata.h"
#include "../vulkanrendertarget.h"

namespace Engine {
namespace Render {

    VulkanPipelineInstance::VulkanPipelineInstance(const PipelineConfiguration &config, const VulkanPipeline *pipeline)
        : PipelineInstance(config)
        , mPipeline(pipeline)
    {
        mConstantBuffers.resize(config.bufferSizes.size());
        for (size_t i = 0; i < config.bufferSizes.size(); ++i) {
            if (config.bufferSizes[i] > 0)
                mConstantBuffers[i] = ByteBuffer { nullptr, config.bufferSizes[i] };
        }

        VkDescriptorSetAllocateInfo allocInfo {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = VulkanRenderContext::getSingleton().mDescriptorPool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &std::as_const(VulkanRenderContext::getSingleton().mDescriptorSetLayouts[0]);
        VkResult result = vkAllocateDescriptorSets(GetDevice(), &allocInfo, &mUboDescriptorSet);
        VK_CHECK(result);

        for (size_t i = 0; i < std::min(size_t { 4 }, mConstantBuffers.size()); ++i) {
            if (!mConstantBuffers[i])
                continue;
            VkDescriptorBufferInfo bufferInfo {};
            bufferInfo.buffer = mConstantBuffers[i].buffer();
            bufferInfo.offset = 0;
            bufferInfo.range = mConstantBuffers[i].size();

            VkWriteDescriptorSet descriptorWrite {};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = mUboDescriptorSet;
            descriptorWrite.dstBinding = i;
            descriptorWrite.dstArrayElement = 0;

            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
            descriptorWrite.descriptorCount = 1;

            descriptorWrite.pBufferInfo = &bufferInfo;
            descriptorWrite.pImageInfo = nullptr; // Optional
            descriptorWrite.pTexelBufferView = nullptr; // Optional

            vkUpdateDescriptorSets(GetDevice(), 1, &descriptorWrite, 0, nullptr);
        }
    }

    VulkanPipelineInstance::~VulkanPipelineInstance()
    {
        VkResult result = vkFreeDescriptorSets(GetDevice(), VulkanRenderContext::getSingleton().mDescriptorPool, 1, &mUboDescriptorSet);
        VK_CHECK(result);
    }

    bool VulkanPipelineInstance::bind(VkCommandBuffer commandList, VertexFormat format, size_t groupSize, size_t samples, VkRenderPass renderpass) const
    {
        size_t samplesBits = sqrt(samples);
        assert(samplesBits * samplesBits == samples);

        VkPipeline pipeline = mPipeline->ptr()[format][groupSize - 1][samplesBits - 1];
        if (!pipeline) {
            pipeline = mPipeline->get(format, groupSize, samples, mInstanceDataSize, renderpass);
            if (!pipeline)
                return false;
        }
        vkCmdBindPipeline(commandList, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

        uint32_t offsets[4] {};
        for (size_t i = 0; i < std::min(size_t { 4 }, mConstantBuffers.size()); ++i) {
            if (!mConstantBuffers[i])
                continue;
            offsets[i] = mConstantBuffers[i].address().offset();
        }
        vkCmdBindDescriptorSets(commandList, VK_PIPELINE_BIND_POINT_GRAPHICS, VulkanRenderContext::getSingleton().mPipelineLayout, 0, 1, &mUboDescriptorSet, 4, offsets);

        return true;
    }

    WritableByteBuffer VulkanPipelineInstance::mapParameters(size_t index)
    {
        return mConstantBuffers[index].mapData();
    }

    void VulkanPipelineInstance::renderMesh(RenderTarget *target, const GPUMeshData *m) const
    {
        VkCommandBuffer commandList = static_cast<VulkanRenderTarget *>(target)->mCommandList;
        VkRenderPass renderpass = static_cast<VulkanRenderTarget *>(target)->mRenderPass;

        const VulkanMeshData *mesh = static_cast<const VulkanMeshData *>(m);

        if (!bind(commandList, mesh->mFormat, mesh->mGroupSize, static_cast<VulkanRenderTarget *>(target)->samples(), renderpass))
            return;

        mesh->mVertices.bindVertex(commandList);

        VulkanRenderContext::getSingleton().mConstantBuffer.bindVertex(commandList, 2);

        if (mesh->mIndices) {
            mesh->mIndices.bindIndex(commandList);
            vkCmdDrawIndexed(commandList, mesh->mElementCount, 1, 0, 0, 0);
        } else {
            vkCmdDraw(commandList, mesh->mElementCount, 1, 0, 0);
        }
    }

    void VulkanPipelineInstance::renderMeshInstanced(RenderTarget *target, size_t count, const GPUMeshData *m, const ByteBuffer &instanceData) const
    {
        assert(instanceData.mSize > 0);
        assert(mInstanceDataSize * count == instanceData.mSize);

        VkCommandBuffer commandList = static_cast<VulkanRenderTarget *>(target)->mCommandList;
        VkRenderPass renderpass = static_cast<VulkanRenderTarget *>(target)->mRenderPass;

        const VulkanMeshData *mesh = static_cast<const VulkanMeshData *>(m);

        if (!bind(commandList, mesh->mFormat, mesh->mGroupSize, static_cast<VulkanRenderTarget *>(target)->samples(), renderpass))
            return;

        mesh->mVertices.bindVertex(commandList);

        VulkanRenderContext::getSingleton().mConstantBuffer.bindVertex(commandList, 2);

        VulkanBuffer instanceBuffer;
        {
            auto target = instanceBuffer.mapData(instanceData.mSize);
            std::memcpy(target.mData, instanceData.mData, instanceData.mSize);
        }
        instanceBuffer.bindVertex(commandList, 1);

        if (mesh->mIndices) {
            mesh->mIndices.bindIndex(commandList);
            vkCmdDrawIndexed(commandList, mesh->mElementCount, count, 0, 0, 0);
        } else {
            vkCmdDraw(commandList, mesh->mElementCount, count, 0, 0);
        }
    }

    void VulkanPipelineInstance::bindTextures(RenderTarget *target, const std::vector<TextureDescriptor> &tex, size_t offset) const
    {
        for (size_t i = 0; i < tex.size(); ++i) {
            if (tex[i].mTextureHandle) {
                VkDescriptorImageInfo imageInfo {};
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageInfo.imageView = reinterpret_cast<VkImageView>(tex[i].mTextureHandle);
                imageInfo.sampler = VK_NULL_HANDLE;

                VkWriteDescriptorSet descriptorWrite {};
                descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrite.dstSet = VK_NULL_HANDLE;
                descriptorWrite.dstBinding = offset + i;
                descriptorWrite.dstArrayElement = 0;

                descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                descriptorWrite.descriptorCount = 1;

                descriptorWrite.pBufferInfo = nullptr;
                descriptorWrite.pImageInfo = &imageInfo; // Optional
                descriptorWrite.pTexelBufferView = nullptr; // Optional

                vkCmdPushDescriptorSetKHR(static_cast<VulkanRenderTarget *>(target)->mCommandList, VK_PIPELINE_BIND_POINT_GRAPHICS, VulkanRenderContext::getSingleton().mPipelineLayout, 1, 1, &descriptorWrite);
            }
        }
    }

    VulkanPipelineInstanceHandle::VulkanPipelineInstanceHandle(const PipelineConfiguration &config, VulkanPipelineLoader::Handle pipeline)
        : VulkanPipelineInstance(config, &*pipeline)
        , mPipelineHandle(std::move(pipeline))
    {
    }

    VulkanPipelineInstancePtr::VulkanPipelineInstancePtr(const PipelineConfiguration &config, VulkanPipelineLoader::Ptr pipeline)
        : VulkanPipelineInstance(config, &*pipeline)
        , mPipelinePtr(std::move(pipeline))
    {
    }

}
}