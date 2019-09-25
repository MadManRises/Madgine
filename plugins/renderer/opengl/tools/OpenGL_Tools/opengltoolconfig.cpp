#include "opengltoolslib.h"

#include "opengltoolconfig.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"
#include "Modules/serialize/serializetable_impl.h"

#include "inspector/inspector.h"

#include "OpenGL/openglfontdata.h"
#include "OpenGL/openglfontloader.h"

#include "imgui/imgui.h"

#include "imageloaderlib.h"
#include "imageloader.h"

#include "imagedata.h"

namespace Engine {
namespace Tools {

    OpenGLToolConfig::OpenGLToolConfig(ImRoot &root)
        : Tool<OpenGLToolConfig>(root)
    {
    }

    bool OpenGLToolConfig::init()
    {
        mImageTexture = {};

        getTool<Inspector>().addPreviewDefinition<Render::OpenGLFontLoader::ResourceType>([](Render::OpenGLFontLoader::ResourceType *font) {
            font->setPersistent(true);
            ImGui::Image((void *)(uintptr_t)font->loadData()->mTexture.handle(), { 100, 100 });
        });

        getTool<Inspector>().addPreviewDefinition<Resources::ImageLoader::ResourceType>([this](Resources::ImageLoader::ResourceType *image) {
            image->setPersistent(true);
            std::shared_ptr<ImageData> data = image->loadData();

            mImageTexture.setData({ data->mWidth, data->mHeight }, data->mBuffer, GL_UNSIGNED_BYTE);
            ImGui::Image((void *)(uintptr_t)mImageTexture.handle(), { static_cast<float>(data->mWidth), static_cast<float>(data->mHeight) });
        });

        return ToolBase::init();
    }

    const char *OpenGLToolConfig::key() const
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

RegisterType(Engine::Tools::OpenGLToolConfig);