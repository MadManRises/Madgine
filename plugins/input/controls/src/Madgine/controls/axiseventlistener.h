#pragma once

namespace Engine {
namespace Input {

	struct AxisEventListener {
        virtual bool onAxisEvent(const AxisEventArgs &arg) = 0;
	};

}
}