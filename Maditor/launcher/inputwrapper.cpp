#include "maditorlauncherlib.h"

#include "inputwrapper.h"

#include "../shared/inputinfo.h"

#include "Madgine/gui/guisystem.h"

#include "Madgine/input/inputevents.h"

namespace Maditor {

	//

	namespace Launcher {

		InputWrapper::InputWrapper(Shared::InputShared &shared) :
			mShared(shared)
		{

		}

		void InputWrapper::update()
		{

			while (!mShared.mMouseQueue.empty()) {
				const std::pair<Shared::InputEventType, Shared::MouseEventArgs> &evt = mShared.mMouseQueue.front();
				if (evt.first == Shared::PRESS) {
					injectMousePress({ evt.second.position, convertButton(evt.second.button) });
				}
				else {
					injectMouseRelease({ evt.second.position, convertButton(evt.second.button) });
				}
				mShared.mMouseQueue.pop();
			}

			while (!mShared.mKeyQueue.empty()) {
				const std::pair<Shared::InputEventType, Shared::KeyEventArgs> &evt = mShared.mKeyQueue.front();
				if (evt.first == Shared::PRESS) {
					injectKeyPress({ (Engine::Input::Key)evt.second.key, evt.second.text });
				}
				else {
					injectKeyRelease({ (Engine::Input::Key)evt.second.key, evt.second.text});
				}
				mShared.mKeyQueue.pop();
			}


			boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(mShared.mMutex);
			if (mShared.mMove) {
				injectMouseMove({ mShared.mAccumulativeMouseMove.position, mShared.mAccumulativeMouseMove.moveDelta, mShared.mAccumulativeMouseMove.scrollWheel });
				mShared.mMove = false;
				mShared.mAccumulativeMouseMove.moveDelta = { 0,0 };
				mShared.mAccumulativeMouseMove.scrollWheel = 0;
			}
		}


		Engine::Input::MouseButton::MouseButton InputWrapper::convertButton(Shared::MouseButton button)
		{
			return Engine::Input::MouseButton::MouseButton(button);
		}

	}
}

