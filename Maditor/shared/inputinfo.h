#pragma once

#include "sharedbase.h"

namespace Maditor {
	namespace Shared {

		enum InputEventType {
			PRESS,
			RELEASE
		};

		enum class MouseButton {
			NO_BUTTON,
			LEFT_BUTTON,
			RIGHT_BUTTON,
			MIDDLE_BUTTON
		};

		struct MouseEventArgs {
			MouseEventArgs() {}

			MouseEventArgs(const std::array<float, 2> &pos, MouseButton button) :
				position(pos),
				button(button),
				scrollWheel(0.0f)
			{}

			MouseEventArgs(const std::array<float, 2> &pos, const std::array<float, 2> &move, float scroll, MouseButton button = MouseButton::NO_BUTTON) :
				position(pos),
				moveDelta(move),
				button(button),
				scrollWheel(scroll)
			{

			}

			std::array<float, 2> position, moveDelta;
			MouseButton button;
			float scrollWheel;
		};

		struct KeyEventArgs {
			unsigned int key;
			char text;
		};

		typedef boost::interprocess::allocator<std::pair<InputEventType, MouseEventArgs>,
			boost::interprocess::managed_shared_memory::segment_manager> SharedMouseEventAllocator;

		typedef boost::interprocess::deque<std::pair<InputEventType, MouseEventArgs>, SharedMouseEventAllocator> SharedMouseDeque;

		typedef std::queue<std::pair<InputEventType, MouseEventArgs>, SharedMouseDeque> SharedMouseQueue;

		typedef boost::interprocess::allocator<std::pair<InputEventType, KeyEventArgs>,
			boost::interprocess::managed_shared_memory::segment_manager> SharedKeyEventAllocator;

		typedef boost::interprocess::deque<std::pair<InputEventType, KeyEventArgs>, SharedKeyEventAllocator> SharedKeyDeque;

		typedef std::queue<std::pair<InputEventType, KeyEventArgs>, SharedKeyDeque> SharedKeyQueue;


		struct InputShared {
			InputShared(boost::interprocess::managed_shared_memory::segment_manager *mgr) :
				mMouseQueue(SharedMouseEventAllocator(mgr)),
				mKeyQueue(SharedKeyEventAllocator(mgr)) {}

			boost::interprocess::interprocess_mutex mMutex;
			bool mMove;
			MouseEventArgs mAccumulativeMouseMove;

			SharedMouseQueue mMouseQueue;
			SharedKeyQueue mKeyQueue;
		};



	}
}
