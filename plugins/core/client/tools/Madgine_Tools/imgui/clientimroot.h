#pragma once

#include "Madgine/gui/widgets/toplevelwindow.h"
#include "Madgine/threading/framelistener.h"

#include "renderer/imroot.h"

#include "immanager.h"

namespace Engine {
namespace Tools {

    struct MADGINE_CLIENT_TOOLS_EXPORT ClientImRoot : public GUI::TopLevelWindowComponent<ClientImRoot>, public Threading::FrameListener {

        ClientImRoot(GUI::TopLevelWindow &window);

        const ImManager &manager() const;

		bool init() override;
        void finalize() override;

		bool frameStarted(std::chrono::microseconds timeSinceLastFrame) override;
        bool frameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Scene::ContextMask context) override;


		ImRoot mRoot;
    private:
        ImManagerSelector mManager;

    };

}
}