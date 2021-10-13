#include "directx12toolslib.h"

#include "directx12toolconfig.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "inspector/inspector.h"

#include "imgui/imgui.h"

#include "imageloaderlib.h"
#include "imageloader.h"

#include "fontloader.h"

#include "imagedata.h"

namespace Engine {
namespace Tools {

    DirectX12ToolConfig::DirectX12ToolConfig(ImRoot &root)
        : Tool<DirectX12ToolConfig>(root)
    {
    }

    bool DirectX12ToolConfig::init()
    {
        mImageTexture = { Render::TextureType_2D, false, Render::FORMAT_RGBA8, 100, 100 };

        getTool<Inspector>().addPreviewDefinition<Render::FontLoader::ResourceType>([](Render::FontLoader::ResourceType *font) {
            Render::FontLoader::HandleType handle = font->loadData();
            handle.info()->setPersistent(true);
            ImGui::Image((void *)handle->mTexture->mTextureHandle, { 100, 100 });
        });

        getTool<Inspector>().addPreviewDefinition<Resources::ImageLoader::ResourceType>([this](Resources::ImageLoader::ResourceType *image) {
            Resources::ImageLoader::HandleType data = image->loadData();
            data.info()->setPersistent(true);

            mImageTexture.setData({ data->mWidth, data->mHeight }, { data->mBuffer, static_cast<size_t>(data->mWidth * data->mHeight) });
            ImGui::Image((void *)mImageTexture.mTextureHandle, { static_cast<float>(data->mWidth), static_cast<float>(data->mHeight) });
        });

        return ToolBase::init();
    }

    void DirectX12ToolConfig::finalize()
    {
        mImageTexture.reset();

		ToolBase::finalize();
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


