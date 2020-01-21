#include "directx11toolslib.h"

#include "directx11toolconfig.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"
#include "Modules/serialize/serializetable_impl.h"

#include "inspector/inspector.h"

#include "DirectX11/directx11fontdata.h"
#include "DirectX11/directx11fontloader.h"

#include "imgui/imgui.h"

#include "imageloaderlib.h"
#include "imageloader.h"

#include "imagedata.h"

namespace Engine {
namespace Tools {

    DirectX11ToolConfig::DirectX11ToolConfig(ImRoot &root)
        : Tool<DirectX11ToolConfig>(root)
    {
    }

    bool DirectX11ToolConfig::init()
    {
        mImageTexture = { Render::Texture2D, Render::FORMAT_UNSIGNED_BYTE, D3D11_BIND_SHADER_RESOURCE, 100, 100 };

        getTool<Inspector>().addPreviewDefinition<Render::DirectX11FontLoader::ResourceType>([](Render::DirectX11FontLoader::ResourceType *font) {
            font->setPersistent(true);
            ImGui::Image((void *)(uintptr_t)font->loadData()->mTexture.mTextureHandle, { 100, 100 });
        });

        getTool<Inspector>().addPreviewDefinition<Resources::ImageLoader::ResourceType>([this](Resources::ImageLoader::ResourceType *image) {
            image->setPersistent(true);
            Resources::ImageLoader::HandleType data = image->loadData();

            mImageTexture.setData({ data->mWidth, data->mHeight }, data->mBuffer);
            ImGui::Image((void *)(uintptr_t)mImageTexture.handle(), { static_cast<float>(data->mWidth), static_cast<float>(data->mHeight) });
        });

        return ToolBase::init();
    }

    void DirectX11ToolConfig::finalize()
    {
        mImageTexture.reset();

		ToolBase::finalize();
    }

    const char *DirectX11ToolConfig::key() const
    {
        return "DirectX11ToolConfig";
    }

}
}

UNIQUECOMPONENT(Engine::Tools::DirectX11ToolConfig);

METATABLE_BEGIN(Engine::Tools::DirectX11ToolConfig)
METATABLE_END(Engine::Tools::DirectX11ToolConfig)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Tools::DirectX11ToolConfig, Engine::Tools::ToolBase)
SERIALIZETABLE_END(Engine::Tools::DirectX11ToolConfig)

RegisterType(Engine::Tools::DirectX11ToolConfig);