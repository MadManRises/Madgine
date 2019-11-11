#pragma once

#include "Madgine_Tools/imgui/immanager.h"

namespace Engine {
namespace Tools {

    class OpenGLImManager : public ImManagerComponent<OpenGLImManager> {
    public:
        OpenGLImManager(GUI::TopLevelWindow &window);
        ~OpenGLImManager();

        virtual void newFrame(float timeSinceLastFrame) override;
    };

}
}