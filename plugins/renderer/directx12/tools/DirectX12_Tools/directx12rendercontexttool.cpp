#include "directx12toolslib.h"

#include "directx12rendercontexttool.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Madgine_Tools/inspector/inspector.h"

#include "imgui/imgui.h"

#include "Madgine/imageloaderlib.h"
#include "Madgine/imageloader/imageloader.h"

#include "Madgine/fontloader/fontloader.h"

#include "Madgine/imageloader/imagedata.h"

UNIQUECOMPONENT(Engine::Tools::DirectX12RenderContextTool);

METATABLE_BEGIN_BASE(Engine::Tools::DirectX12RenderContextTool, Engine::Tools::RenderContextTool)
METATABLE_END(Engine::Tools::DirectX12RenderContextTool)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Tools::DirectX12RenderContextTool, Engine::Tools::RenderContextTool)
SERIALIZETABLE_END(Engine::Tools::DirectX12RenderContextTool)

namespace Engine {
namespace Tools {

    DirectX12RenderContextTool::DirectX12RenderContextTool(ImRoot &root)
        : ToolVirtualImpl<DirectX12RenderContextTool, RenderContextTool>(root)
    {
    }

    Threading::Task<bool> DirectX12RenderContextTool::init()
    {
        mImageTexture = { Render::TextureType_2D, false, Render::FORMAT_RGBA8_SRGB, 100, 100 };

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

    Threading::Task<void> DirectX12RenderContextTool::finalize()
    {
        mImageTexture.reset();

        co_await RenderContextTool::finalize();
    }

    std::string_view DirectX12RenderContextTool::key() const
    {
        return "DirectX12RenderContextTool";
    }

}
}
