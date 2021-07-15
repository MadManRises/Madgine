#pragma once

#include "Madgine_Tools/imgui/clientimroot.h"

namespace Engine {
namespace Tools {

    struct OpenGLImRoot : VirtualScope<OpenGLImRoot, Window::MainWindowVirtualImpl<OpenGLImRoot, ClientImRoot>> {

        OpenGLImRoot(Window::MainWindow &window);
        ~OpenGLImRoot();

        virtual bool init() override;
        virtual void finalize() override;

        virtual void newFrame(float timeSinceLastFrame) override;

        virtual void renderMainDrawList() override;
    };

}
}

RegisterType(Engine::Tools::OpenGLImRoot);