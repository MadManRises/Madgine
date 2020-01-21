#pragma once

#include "toolscollector.h"

#include "DirectX11/util/directx11texture.h"

namespace Engine {
namespace Tools {

    struct DirectX11ToolConfig : public Tool<DirectX11ToolConfig> {

        SERIALIZABLEUNIT;

        DirectX11ToolConfig(ImRoot &root);
        DirectX11ToolConfig(const SceneEditor &) = delete;

        virtual bool init() override;
        virtual void finalize() override;

        const char *key() const override;

    private:
        Render::DirectX11Texture mImageTexture;
    };

}
}