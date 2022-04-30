#pragma once

#include "toolbase.h"
#include "toolscollector.h"

namespace Engine {
namespace Tools {

    struct MADGINE_CLIENT_TOOLS_EXPORT GamepadViewer : Tool<GamepadViewer> {

        GamepadViewer(ImRoot &root);

        virtual std::string_view key() const override;        

        virtual void render() override;
    };

}
}

RegisterType(Engine::Tools::GamepadViewer)