#pragma once

#include "toolbase.h"
#include "toolscollector.h"

#include "DirectX11/util/directx11texture.h"

namespace Engine {
namespace Tools {

    struct DirectX11ToolConfig : public Tool<DirectX11ToolConfig> {

        SERIALIZABLEUNIT(DirectX11ToolConfig);

        DirectX11ToolConfig(ImRoot &root);
        DirectX11ToolConfig(const SceneEditor &) = delete;

        virtual Threading::Task<bool> init() override;
        virtual Threading::Task<void> finalize() override;

        virtual void renderMenu() override;

        std::string_view key() const override;

    private:
        Render::DirectX11Texture mImageTexture;
    };

}
}

REGISTER_TYPE(Engine::Tools::DirectX11ToolConfig)