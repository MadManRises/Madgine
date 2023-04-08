#include "vulkantoolslib.h"

#include "vulkantoolconfig.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Madgine_Tools/inspector/inspector.h"

#include "imgui/imgui.h"

#include "Madgine/imageloaderlib.h"
#include "Madgine/imageloader/imageloader.h"

#include "Madgine/fontloader/fontloader.h"

#include "Madgine/imageloader/imagedata.h"

UNIQUECOMPONENT(Engine::Tools::VulkanToolConfig);

METATABLE_BEGIN_BASE(Engine::Tools::VulkanToolConfig, Engine::Tools::ToolBase)
METATABLE_END(Engine::Tools::VulkanToolConfig)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Tools::VulkanToolConfig, Engine::Tools::ToolBase)
SERIALIZETABLE_END(Engine::Tools::VulkanToolConfig)

namespace Engine {
namespace Tools {

    VulkanToolConfig::VulkanToolConfig(ImRoot &root)
        : Tool<VulkanToolConfig>(root)
    {
    }

    Threading::Task<bool> VulkanToolConfig::init()
    {
        mImageTexture = { Render::TextureType_2D, false, Render::FORMAT_RGBA8, 100, 100 };

        getTool<Inspector>().addPreviewDefinition<Render::FontLoader::Resource>([](Render::FontLoader::Resource *font) {
            Render::FontLoader::Handle handle = font->loadData();
            handle.info()->setPersistent(true);
            ImGui::Image((void *)handle->mTexture->handle(), handle->mTexture->size());
        });

        getTool<Inspector>().addPreviewDefinition<Resources::ImageLoader::Resource>([this](Resources::ImageLoader::Resource *image) {
            Resources::ImageLoader::Handle data = image->loadData();
            data.info()->setPersistent(true);

            mImageTexture.setData(data->mSize, data->mBuffer);
            ImGui::Image((void *)mImageTexture.handle(), data->mSize);
        });

        co_return co_await ToolBase::init();
    }

    Threading::Task<void> VulkanToolConfig::finalize()
    {
        mImageTexture.reset();

        co_await ToolBase::finalize();
    }

    void VulkanToolConfig::renderMenu()
    {
    }

    std::string_view VulkanToolConfig::key() const
    {
        return "VulkanToolConfig";
    }

}
}
