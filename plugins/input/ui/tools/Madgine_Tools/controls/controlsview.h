#pragma once

#include "toolbase.h"
#include "toolscollector.h"

#include "Madgine/controls/axiseventlistener.h"

namespace Engine {
namespace Tools {

    struct MADGINE_CONTROLS_TOOLS_EXPORT ControlsView : Tool<ControlsView>, Input::AxisEventListener {

        ControlsView(ImRoot &root);

        virtual std::string_view key() const override;

        virtual Threading::Task<bool> init() override;

        virtual void render() override;

        virtual bool onAxisEvent(const Input::AxisEventArgs &arg) override;

        float mLeftStickX, mLeftStickY;
    };

}
}

REGISTER_TYPE(Engine::Tools::ControlsView)