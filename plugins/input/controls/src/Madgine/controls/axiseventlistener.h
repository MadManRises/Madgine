#pragma once

namespace Engine {
namespace Controls {

	struct AxisEventListener {
        virtual bool onAxisEvent(const Input::AxisEventArgs &arg) = 0;
	};

}
}