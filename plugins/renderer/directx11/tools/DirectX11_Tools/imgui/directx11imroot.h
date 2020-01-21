#pragma once

#include "Madgine_Tools/imgui/clientimroot.h"

namespace Engine {
namespace Tools {

    struct DirectX11ImRoot : VirtualUniqueComponentImpl<DirectX11ImRoot, ClientImRoot> {

        DirectX11ImRoot(GUI::TopLevelWindow &window);
        ~DirectX11ImRoot();

		virtual bool init() override;
        virtual void finalize() override;

        virtual void newFrame(float timeSinceLastFrame) override;        
    };

}
}