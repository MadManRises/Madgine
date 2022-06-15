#pragma once

#include "toolbase.h"
#include "toolscollector.h"

#include "DirectX12/util/directx12texture.h"

namespace Engine {
namespace Tools {

    struct DirectX12ToolConfig : public Tool<DirectX12ToolConfig> {

        SERIALIZABLEUNIT(DirectX12ToolConfig);

        DirectX12ToolConfig(ImRoot &root);
        DirectX12ToolConfig(const SceneEditor &) = delete;

        virtual Threading::Task<bool> init() override;
        virtual Threading::Task<void> finalize() override;

        virtual void renderMenu() override;

        std::string_view key() const override;

    private:
        Render::DirectX12Texture mImageTexture;
    };

}
}

REGISTER_TYPE(Engine::Tools::DirectX12ToolConfig)