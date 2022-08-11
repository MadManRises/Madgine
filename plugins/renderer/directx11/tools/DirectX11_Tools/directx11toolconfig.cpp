#include "directx11toolslib.h"

#include "directx11toolconfig.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Madgine_Tools/inspector/inspector.h"

#include "imgui/imgui.h"

#include "Madgine/imageloaderlib.h"
#include "Madgine/imageloader/imageloader.h"

#include "Madgine/fontloader/fontloader.h"

#include "Madgine/imageloader/imagedata.h"

namespace Engine {
namespace Tools {

    DirectX11ToolConfig::DirectX11ToolConfig(ImRoot &root)
        : Tool<DirectX11ToolConfig>(root)
    {
    }

    Threading::Task<bool> DirectX11ToolConfig::init()
    {
        mImageTexture = { Render::TextureType_2D, Render::FORMAT_RGBA8, D3D11_BIND_SHADER_RESOURCE, 100, 100 };

        getTool<Inspector>().addPreviewDefinition<Render::FontLoader::Resource>([](Render::FontLoader::Resource *font) {
            Render::FontLoader::Handle handle = font->loadData();
            handle.info()->setPersistent(true);
            ImGui::Image((void *)handle->mTexture->mTextureHandle, { 100, 100 });
        });

        getTool<Inspector>().addPreviewDefinition<Resources::ImageLoader::Resource>([this](Resources::ImageLoader::Resource *image) {
            Resources::ImageLoader::Handle data = image->loadData();
            data.info()->setPersistent(true);

            if (data.available()) {
                mImageTexture.setData(data->mSize, data->mBuffer);
                ImGui::Image((void *)mImageTexture.mTextureHandle, data->mSize);
            }
        });

        co_return co_await ToolBase::init();
    }

    Threading::Task<void> DirectX11ToolConfig::finalize()
    {
        mImageTexture.reset();

        co_await ToolBase::finalize();
    }

    void DirectX11ToolConfig::renderMenu()
    {
    }

    std::string_view DirectX11ToolConfig::key() const
    {
        return "DirectX11ToolConfig";
    }

}
}

UNIQUECOMPONENT(Engine::Tools::DirectX11ToolConfig);

METATABLE_BEGIN_BASE(Engine::Tools::DirectX11ToolConfig, Engine::Tools::ToolBase)
METATABLE_END(Engine::Tools::DirectX11ToolConfig)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Tools::DirectX11ToolConfig, Engine::Tools::ToolBase)
SERIALIZETABLE_END(Engine::Tools::DirectX11ToolConfig)
