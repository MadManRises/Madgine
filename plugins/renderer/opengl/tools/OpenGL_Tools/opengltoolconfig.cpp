#include "opengltoolslib.h"

#include "opengltoolconfig.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"

#include "Modules/generic/bytebuffer.h"

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
        mImageTexture = { GL_UNSIGNED_BYTE };

        getTool<Inspector>().addPreviewDefinition<Render::FontLoader::ResourceType>([](Render::FontLoader::ResourceType *font) {
            font->setPersistent(true);
            ImGui::Image((void *)(uintptr_t)font->loadData()->mTextureHandle, { 100, 100 });
        });

        getTool<Inspector>().addPreviewDefinition<Resources::ImageLoader::ResourceType>([this](Resources::ImageLoader::ResourceType *image) {
            image->setPersistent(true);
            Resources::ImageLoader::HandleType data = image->loadData();

            mImageTexture.setData({ data->mWidth, data->mHeight }, { data->mBuffer, static_cast<size_t>(data->mWidth * data->mHeight) });
            ImGui::Image((void *)(uintptr_t)mImageTexture.handle(), { static_cast<float>(data->mWidth), static_cast<float>(data->mHeight) });
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

METATABLE_BEGIN(Engine::Tools::OpenGLToolConfig)
METATABLE_END(Engine::Tools::OpenGLToolConfig)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Tools::OpenGLToolConfig, Engine::Tools::ToolBase)
SERIALIZETABLE_END(Engine::Tools::OpenGLToolConfig)

