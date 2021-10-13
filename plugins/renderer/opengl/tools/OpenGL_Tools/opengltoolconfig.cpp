#include "opengltoolslib.h"

#include "opengltoolconfig.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Generic/bytebuffer.h"

#include "inspector/inspector.h"

#include "imgui/imgui.h"

#include "imageloaderlib.h"
#include "imageloader.h"

#include "fontloader.h"

#include "imagedata.h"

namespace Engine {
namespace Tools {

    OpenGLToolConfig::OpenGLToolConfig(ImRoot &root)
        : Tool<OpenGLToolConfig>(root)
    {
    }

    bool OpenGLToolConfig::init()
    {
        mImageTexture = { Render::TextureType_2D, Render::FORMAT_RGBA8 };

        getTool<Inspector>().addPreviewDefinition<Render::FontLoader::ResourceType>([](Render::FontLoader::ResourceType *font) {
            Render::FontLoader::HandleType handle = font->loadData();
            handle.info()->setPersistent(true);
            ImGui::Image((void *)(uintptr_t)handle->mTexture->mTextureHandle, { 100, 100 });
        });

        getTool<Inspector>().addPreviewDefinition<Resources::ImageLoader::ResourceType>([this](Resources::ImageLoader::ResourceType *image) {
            Resources::ImageLoader::HandleType data = image->loadData();
            data.info()->setPersistent(true);

            mImageTexture.setData({ data->mWidth, data->mHeight }, { data->mBuffer, static_cast<size_t>(data->mWidth * data->mHeight) });
            ImGui::Image((void *)(uintptr_t)mImageTexture.mTextureHandle, { static_cast<float>(data->mWidth), static_cast<float>(data->mHeight) });
        });

        return ToolBase::init();
    }

    void OpenGLToolConfig::finalize()
    {
        mImageTexture.reset();

        ToolBase::finalize();
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
