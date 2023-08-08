#include "directx11toolslib.h"

#include "directx11rendercontexttool.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Madgine_Tools/inspector/inspector.h"

#include "imgui/imgui.h"

#include "Madgine/imageloaderlib.h"
#include "Madgine/imageloader/imageloader.h"

#include "Madgine/fontloader/fontloader.h"

#include "Madgine/imageloader/imagedata.h"

UNIQUECOMPONENT(Engine::Tools::DirectX11RenderContextTool);

METATABLE_BEGIN_BASE(Engine::Tools::DirectX11RenderContextTool, Engine::Tools::RenderContextTool)
METATABLE_END(Engine::Tools::DirectX11RenderContextTool)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Tools::DirectX11RenderContextTool, Engine::Tools::RenderContextTool)
SERIALIZETABLE_END(Engine::Tools::DirectX11RenderContextTool)

namespace Engine {
namespace Tools {

    DirectX11RenderContextTool::DirectX11RenderContextTool(ImRoot &root)
        : ToolVirtualImpl<DirectX11RenderContextTool, RenderContextTool>(root)
    {
    }

    Threading::Task<bool> DirectX11RenderContextTool::init()
    {
        mImageTexture = { Render::TextureType_2D, Render::FORMAT_RGBA8_SRGB, D3D11_BIND_SHADER_RESOURCE, 100, 100 };

        getTool<Inspector>().addPreviewDefinition<Resources::ImageLoader::Resource>([this](Resources::ImageLoader::Resource *image) {
            Resources::ImageLoader::Handle data = image->loadData();
            data.info()->setPersistent(true);

            if (data.available()) {
                mImageTexture.setData(data->mSize, data->mBuffer);
                ImGui::Image((void *)mImageTexture.handle(), data->mSize);
            }
        });

        co_return co_await RenderContextTool::init();
    }

    Threading::Task<void> DirectX11RenderContextTool::finalize()
    {
        mImageTexture.reset();

        co_await RenderContextTool::finalize();
    }

    std::string_view DirectX11RenderContextTool::key() const
    {
        return "DirectX11RenderContextTool";
    }

}
}
