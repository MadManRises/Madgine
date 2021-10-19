#pragma once

#include "toolscollector.h"
#include "toolbase.h"

#include "Madgine/controls/axiseventlistener.h"

namespace Engine {
namespace Tools {

	struct MADGINE_CONTROLS_TOOLS_EXPORT ControlsView : Tool<ControlsView>, Controls::AxisEventListener{
    
		ControlsView(ImRoot &root);

		virtual std::string_view key() const override;

		virtual bool init() override;

		virtual void render() override;

		virtual bool onAxisEvent(const Input::AxisEventArgs &arg) override;

		float mLeftStickX, mLeftStickY;

	};

}
}

RegisterType(Engine::Tools::ControlsView)