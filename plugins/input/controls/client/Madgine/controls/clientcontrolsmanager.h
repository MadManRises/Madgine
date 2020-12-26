#pragma once

#include "Madgine/window/mainwindowcomponent.h"
#include "Madgine/window/mainwindowcomponentcollector.h"
#include "Modules/uniquecomponent/uniquecomponentcollector.h"

namespace Engine {
namespace Controls {

    struct MADGINE_CLIENTCONTROLS_EXPORT ClientControlsManager : Window::MainWindowComponent<ClientControlsManager> {

        ClientControlsManager(Window::MainWindow &window);

        virtual std::string_view key() const override;

    private:
        ControlsManager &mManager;
    };

}
}