#pragma once

#include "Madgine_Tools/toolbase.h"
#include "Madgine_Tools/toolscollector.h"

#include "DirectX11/util/directx11texture.h"

#include "Madgine_Tools/render/rendercontexttool.h"

namespace Engine {
namespace Tools {

    struct DirectX11RenderContextTool : public ToolVirtualImpl<DirectX11RenderContextTool, RenderContextTool> {

        SERIALIZABLEUNIT(DirectX11RenderContextTool)

        DirectX11RenderContextTool(ImRoot &root);

        virtual Threading::Task<bool> init() override;
        virtual Threading::Task<void> finalize() override;

        std::string_view key() const override;

    private:
        Render::DirectX11Texture mImageTexture;
    };

}
}

REGISTER_TYPE(Engine::Tools::DirectX11RenderContextTool)