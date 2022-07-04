#pragma once

#include "toolbase.h"
#include "toolscollector.h"

#include "textureloader.h"

namespace Engine {
namespace Tools {

    struct MADGINE_CLIENT_TOOLS_EXPORT GamepadViewer : Tool<GamepadViewer> {

        GamepadViewer(ImRoot &root);

        virtual Threading::Task<bool> init() override;

        virtual std::string_view key() const override;

        virtual void render() override;

    private:
        Render::TextureLoader::Handle mGamepadTexture;
    };

}
}

REGISTER_TYPE(Engine::Tools::GamepadViewer)