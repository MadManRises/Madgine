#pragma once

#include "toolscollector.h"
#include "toolbase.h"

#include "DirectX12/util/directx12texture.h"

namespace Engine {
namespace Tools {

    struct DirectX12ToolConfig : public Tool<DirectX12ToolConfig> {

        SERIALIZABLEUNIT(DirectX12ToolConfig);

        DirectX12ToolConfig(ImRoot &root);
        DirectX12ToolConfig(const SceneEditor &) = delete;

        virtual bool init() override;
        virtual void finalize() override;

        std::string_view key() const override;

    private:
        Render::DirectX12Texture mImageTexture;
    };

}
}

RegisterType(Engine::Tools::DirectX12ToolConfig);