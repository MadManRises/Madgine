#include "vulkanlib.h"

#include "vulkanrendertexture.h"

#include "Madgine/render/rendertextureconfig.h"

namespace Engine {
namespace Render {

    VulkanRenderTexture::VulkanRenderTexture(VulkanRenderContext *context, const Vector2i &size, const RenderTextureConfig &config)
        : VulkanRenderTarget(context, false, config.mName, config.mSamples, config.mBlitSource)
        , mSize { 0, 0 }
        , mCreateDepthBufferView(config.mCreateDepthBufferView)
    {
        size_t bufferCount = config.mIterations > 1 ? 2 : 1;

        for (size_t i = 0; i < config.mTextureCount * bufferCount; ++i) {
            mTextures.emplace_back(config.mType, true, config.mFormat, config.mSamples);
        }

        createRenderPass();

        resize(size);

        for (VulkanTexture &tex : mTextures) {
            tex.setName(config.mName.empty() ? "RenderTexture" : config.mName);
        }
    }

    VulkanRenderTexture::~VulkanRenderTexture()
    {
        shutdown();
    }

    bool VulkanRenderTexture::resizeImpl(const Vector2i &size)
    {
        if (mSize == size)
            return false;

        mSize = size;

        for (VulkanTexture &tex : mTextures) {
            tex.setData(size, {});
        }
        setup(size, mCreateDepthBufferView);

        VkImageView views[5] { mDepthView };
        for (size_t i = 0; i < mTextures.size(); ++i)
            views[i + 1] = mTextures[i].view();
        VkFramebufferCreateInfo framebufferInfo {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = mRenderPass;
        framebufferInfo.attachmentCount = 1 + mTextures.size();
        framebufferInfo.pAttachments = views;
        framebufferInfo.width = mSize.x;
        framebufferInfo.height = mSize.y;
        framebufferInfo.layers = 1;

        mTextureFramebuffer.reset();
        VkResult result = vkCreateFramebuffer(GetDevice(), &framebufferInfo, nullptr, &mTextureFramebuffer);
        VK_CHECK(result);

        mFramebuffer = mTextureFramebuffer;

        return true;
    }

    void VulkanRenderTexture::beginIteration(size_t iteration) const
    {

        VulkanRenderTarget::beginIteration(iteration);
    }

    void VulkanRenderTexture::endIteration(size_t iteration) const
    {
        VulkanRenderTarget::endIteration(iteration);
    }

    TextureDescriptor VulkanRenderTexture::texture(size_t index, size_t iteration) const
    {
        return mTextures[index].descriptor();
    }

    size_t VulkanRenderTexture::textureCount() const
    {
        return 1;
    }

    void VulkanRenderTexture::blit(RenderTarget *input)
    {
        VulkanRenderTexture *inputTex = dynamic_cast<VulkanRenderTexture *>(input);
        assert(inputTex);

        size_t count = std::min(mTextures.size(), inputTex->mTextures.size());

        VkImageResolve region {};
        region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.srcSubresource.layerCount = 1;
        region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.dstSubresource.layerCount = 1;
        region.extent.depth = 1;
        region.extent.width = mSize.x;
        region.extent.height = mSize.y;

        for (size_t i = 0; i < count; ++i) {
            inputTex->mTextures[i].transition(mCommandList, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
            mTextures[i].transition(mCommandList, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
            vkCmdResolveImage(mCommandList, inputTex->mTextures[i].image(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, mTextures[i].image(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
            inputTex->mTextures[i].transition(mCommandList, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            mTextures[i].transition(mCommandList, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }
    }

    const std::vector<VulkanTexture> &VulkanRenderTexture::textures() const
    {
        return mTextures;
    }

    Vector2i VulkanRenderTexture::size() const
    {
        return mSize;
    }

    void VulkanRenderTexture::beginFrame()
    {
        VulkanRenderTarget::beginFrame();
    }

    void VulkanRenderTexture::endFrame()
    {
        VulkanRenderTarget::endFrame();
    }

    void VulkanRenderTexture::createRenderPass()
    {

        VkFormat vFormat;
        if (!mTextures.empty())
            vFormat = mTextures.front().format();

        VkSubpassDependency dependencies[2] {};

        dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[0].dstSubpass = 0;
        dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        dependencies[1].srcSubpass = 0;
        dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        VulkanRenderTarget::createRenderPass(mTextures.size(), vFormat, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mCreateDepthBufferView, dependencies);

    }

}
}