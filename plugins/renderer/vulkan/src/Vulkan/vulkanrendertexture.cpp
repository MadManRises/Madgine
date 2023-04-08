#include "vulkanlib.h"

#include "vulkanrendertexture.h"

#include "Madgine/render/rendertextureconfig.h"

namespace Engine {
namespace Render {

    VulkanRenderTexture::VulkanRenderTexture(VulkanRenderContext *context, const Vector2i &size, const RenderTextureConfig &config)
        : VulkanRenderTarget(context, false, config.mName)
        , mTexture(TextureType_2D, true, FORMAT_RGBA8)
        , mSize { 0, 0 }
    {
        //context->waitForGPU();

        resize(size, config);

        mTexture.setName(config.mName.empty() ? "RenderTexture" : config.mName);
    }

    VulkanRenderTexture::~VulkanRenderTexture()
    {
        shutdown();
    }

    bool VulkanRenderTexture::resize(const Vector2i &size, const RenderTextureConfig &config)
    {
        if (mSize == size)
            return false;

        mSize = size;

        //mTexture.resize(size);
        mTexture.setData(size, {});

        VkFormat vFormat;
        switch (mTexture.format()) {
        case FORMAT_RGBA8:
            vFormat = VK_FORMAT_R8G8B8A8_UNORM;
            break;
        case FORMAT_RGBA16F:
            vFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
            break;
        default:
            std::terminate();
        }

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

        setup(size, vFormat, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 2, dependencies, config.mCreateDepthBufferView);

        VkImageView views[2] { mTexture.view(), mDepthView };
        VkFramebufferCreateInfo framebufferInfo {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = mRenderPass;
        framebufferInfo.attachmentCount = 2;
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

    bool VulkanRenderTexture::resizeImpl(const Vector2i &size)
    {
        return resize(size, {});
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
        return mTexture.descriptor();
    }

    size_t VulkanRenderTexture::textureCount() const
    {
        return 1;
    }

    const VulkanTexture &VulkanRenderTexture::texture() const
    {
        return mTexture;
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

}
}
