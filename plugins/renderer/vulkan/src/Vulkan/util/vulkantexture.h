#pragma once

#include "Meta/math/vector2i.h"

#include "Madgine/render/texture.h"

#include "Generic/bytebuffer.h"

#include "Madgine/render/texturedescriptor.h"

namespace Engine {
namespace Render {

    struct MADGINE_VULKAN_EXPORT VulkanTexture : Texture {

        VulkanTexture(TextureType type, bool isRenderTarget, DataFormat format, size_t width, size_t height, const ByteBuffer &data = {});
        VulkanTexture(TextureType type = TextureType_2D, bool isRenderTarget = false, DataFormat format = FORMAT_RGBA8);
        VulkanTexture(const VulkanTexture &) = delete;
        VulkanTexture(VulkanTexture &&);
        ~VulkanTexture();

        VulkanTexture &operator=(VulkanTexture &&);

        void reset();

        void setData(Vector2i size, const ByteBuffer &data);
        void setSubData(Vector2i offset, Vector2i size, const ByteBuffer &data);

        VkImageView view() const;

        void setName(std::string_view name);

    private:
        VulkanPtr<VkImage, &vkDestroyImage> mImage;
        VulkanPtr<VkDeviceMemory, &vkFreeMemory> mDeviceMemory;
        bool mIsRenderTarget;
    };

}
}