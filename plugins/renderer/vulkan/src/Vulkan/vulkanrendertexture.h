#pragma once

#include "util/vulkantexture.h"
#include "vulkanrendertarget.h"

namespace Engine {
namespace Render {

    struct MADGINE_VULKAN_EXPORT VulkanRenderTexture : VulkanRenderTarget {

        VulkanRenderTexture(VulkanRenderContext *context, const Vector2i &size, const RenderTextureConfig &config);
        ~VulkanRenderTexture();

        bool resizeImpl(const Vector2i &size) override;
        Vector2i size() const override;

        virtual void beginFrame() override;
        virtual void endFrame() override;

        virtual void beginIteration(size_t iteration) const override;
        virtual void endIteration(size_t iteration) const override;

        virtual TextureDescriptor texture(size_t index, size_t iteration = std::numeric_limits<size_t>::max()) const override;
        virtual size_t textureCount() const override;

        virtual void blit(RenderTarget *input) override;

        const std::vector<VulkanTexture> &textures() const;

    protected:
        void createRenderPass();

    private:
        std::vector<VulkanTexture> mTextures;

        Vector2i mSize;

        VulkanPtr<VkFramebuffer, &vkDestroyFramebuffer> mTextureFramebuffer;

        bool mCreateDepthBufferView;
    };

}
}