#include "directx12toolslib.h"

#include "directx12toolconfig.h"

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

    DirectX12ToolConfig::DirectX12ToolConfig(ImRoot &root)
        : Tool<DirectX12ToolConfig>(root)
    {
    }

    Threading::Task<bool> DirectX12ToolConfig::init()
    {
        mImageTexture = { Render::TextureType_2D, false, Render::FORMAT_RGBA8, 100, 100 };

        getTool<Inspector>().addPreviewDefinition<Render::FontLoader::Resource>([](Render::FontLoader::Resource *font) {
            Render::FontLoader::Handle handle = font->loadData();
            handle.info()->setPersistent(true);
            ImGui::Image((void *)handle->mTexture->mTextureHandle, { 100, 100 });
        });

        getTool<Inspector>().addPreviewDefinition<Resources::ImageLoader::Resource>([this](Resources::ImageLoader::Resource *image) {
            Resources::ImageLoader::Handle data = image->loadData();
            data.info()->setPersistent(true);

            mImageTexture.setData({ data->mWidth, data->mHeight }, { data->mBuffer, static_cast<size_t>(data->mWidth * data->mHeight) });
            ImGui::Image((void *)mImageTexture.mTextureHandle, { static_cast<float>(data->mWidth), static_cast<float>(data->mHeight) });
        });

        co_return co_await ToolBase::init();
    }

    Threading::Task<void> DirectX12ToolConfig::finalize()
    {
        mImageTexture.reset();

        co_await ToolBase::finalize();
    }

    void DirectX12ToolConfig::renderMenu()
    {
    }

    std::string_view DirectX12ToolConfig::key() const
    {
        return "DirectX12ToolConfig";
    }

}
}

UNIQUECOMPONENT(Engine::Tools::DirectX12ToolConfig);

METATABLE_BEGIN_BASE(Engine::Tools::DirectX12ToolConfig, Engine::Tools::ToolBase)
METATABLE_END(Engine::Tools::DirectX12ToolConfig)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Tools::DirectX12ToolConfig, Engine::Tools::ToolBase)
SERIALIZETABLE_END(Engine::Tools::DirectX12ToolConfig)
