#pragma once

#include "Madgine/window/mainwindowcomponent.h"
#include "Madgine/window/mainwindowcomponentcollector.h"


namespace Engine {
namespace Input {

    struct MADGINE_CLIENTCONTROLS_EXPORT ClientControlsManager : Window::MainWindowComponent<ClientControlsManager> {

        ClientControlsManager(Window::MainWindow &window);

        virtual std::string_view key() const override;

        virtual Threading::Task<bool> init() override;

        virtual bool injectAxisEvent(const AxisEventArgs &arg) override;

    private:
        ControlsManager *mManager;
    };

}
}

RegisterType(Engine::Input::ClientControlsManager)