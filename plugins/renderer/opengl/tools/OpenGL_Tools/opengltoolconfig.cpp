#include "opengltoolslib.h"

#include "opengltoolconfig.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Generic/bytebuffer.h"

#include "Madgine_Tools/inspector/inspector.h"

#include "imgui/imgui.h"

#include "Madgine/imageloaderlib.h"
#include "Madgine/imageloader/imageloader.h"

#include "Madgine/fontloader/fontloader.h"

#include "Madgine/imageloader/imagedata.h"

namespace Engine {
namespace Tools {

    OpenGLToolConfig::OpenGLToolConfig(ImRoot &root)
        : Tool<OpenGLToolConfig>(root)
    {
    }

    Threading::Task<bool> OpenGLToolConfig::init()
    {
        mImageTexture = { Render::TextureType_2D, Render::FORMAT_RGBA8 };

        getTool<Inspector>().addPreviewDefinition<Render::FontLoader::Resource>([](Render::FontLoader::Resource *font) {
            Render::FontLoader::Handle handle = font->loadData();
            handle.info()->setPersistent(true);
            if (handle.available())
                ImGui::Image((void *)(uintptr_t)handle->mTexture->mTextureHandle, { 100, 100 });
        });

        getTool<Inspector>().addPreviewDefinition<Resources::ImageLoader::Resource>([this](Resources::ImageLoader::Resource *image) {
            Resources::ImageLoader::Handle data = image->loadData();
            data.info()->setPersistent(true);

            mImageTexture.setData({ data->mWidth, data->mHeight }, { data->mBuffer, static_cast<size_t>(data->mWidth * data->mHeight) });
            ImGui::Image((void *)(uintptr_t)mImageTexture.mTextureHandle, { static_cast<float>(data->mWidth), static_cast<float>(data->mHeight) });
        });

        co_return co_await ToolBase::init();
    }

    Threading::Task<void> OpenGLToolConfig::finalize()
    {
        mImageTexture.reset();

        co_await ToolBase::finalize();
    }

    void OpenGLToolConfig::renderMenu() {

    }

    std::string_view OpenGLToolConfig::key() const
    {
        return "OpenGLToolConfig";
    }

}
}

UNIQUECOMPONENT(Engine::Tools::OpenGLToolConfig);

METATABLE_BEGIN_BASE(Engine::Tools::OpenGLToolConfig, Engine::Tools::ToolBase)
METATABLE_END(Engine::Tools::OpenGLToolConfig)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Tools::OpenGLToolConfig, Engine::Tools::ToolBase)
SERIALIZETABLE_END(Engine::Tools::OpenGLToolConfig)
