#include "vulkanlib.h"

#include "vulkantextureloader.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "Modules/uniquecomponent/uniquecomponent.h"

#include "vulkanrendercontext.h"

UNIQUECOMPONENT(Engine::Render::VulkanTextureLoader);

METATABLE_BEGIN(Engine::Render::VulkanTextureLoader)
MEMBER(mResources)
METATABLE_END(Engine::Render::VulkanTextureLoader)

METATABLE_BEGIN_BASE(Engine::Render::VulkanTextureLoader::Resource, Engine::Render::TextureLoader::Resource)
METATABLE_END(Engine::Render::VulkanTextureLoader::Resource)



namespace Engine {
namespace Render {

    VulkanTextureLoader::VulkanTextureLoader()
    {
    }

    bool VulkanTextureLoader::loadImpl(VulkanTexture &tex, ResourceDataInfo &info)
    {
        throw 0;
    }

    void VulkanTextureLoader::unloadImpl(VulkanTexture &tex)
    {
        tex.reset();
    }

    bool VulkanTextureLoader::create(Texture &tex, TextureType type, TextureFormat format)
    {
        static_cast<VulkanTexture &>(tex) = VulkanTexture { type, false, format };

        return true;
    }

    void VulkanTextureLoader::setData(Texture &tex, Vector2i size, const ByteBuffer &data)
    {
        static_cast<VulkanTexture &>(tex).setData(size, data);
    }

    void VulkanTextureLoader::setSubData(Texture &tex, Vector2i offset, Vector2i size, const ByteBuffer &data)
    {
        static_cast<VulkanTexture &>(tex).setSubData(offset, size, data);
    }

    Threading::TaskQueue *VulkanTextureLoader::loadingTaskQueue() const
    {
        return VulkanRenderContext::renderQueue();
    }

}
}
