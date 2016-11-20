#include "maditorlib.h"

#include "InputWrapper.h"

#include "Common/Shared.h"

namespace Maditor {
	namespace Model {
		namespace Watcher {
			InputWrapper::InputWrapper() :
				mEnabled(false),
				mShared(SharedMemory::getSingleton().mInput)
			{
				resetAccumulativeMouseMove();
			}

			void InputWrapper::setEnabled(bool b) {
				mEnabled = b;
			}

			void InputWrapper::keyPressEvent(QKeyEvent * ev)
			{
				if (!mEnabled) return;
				char text = ev->text().isEmpty() ? 0 : ev->text().at(0).toLatin1();
				mShared.mKeyQueue.emplace(PRESS, Engine::GUI::KeyEventArgs{ (Engine::GUI::Key)ev->nativeScanCode(), text });
			}

			void InputWrapper::keyReleaseEvent(QKeyEvent * ev)
			{
				if (!mEnabled) return;
				char text = ev->text().isEmpty() ? 0 : ev->text().at(0).toLatin1();			
				mShared.mKeyQueue.emplace(RELEASE, Engine::GUI::KeyEventArgs{ (Engine::GUI::Key)ev->nativeScanCode(), text });
			}

			void InputWrapper::mouseMoveEvent(QMouseEvent * ev)
			{
				if (!mEnabled) return;
				boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(mShared.mMutex);
				mShared.mAccumulativeMouseMove.moveDelta.x += ev->x() - mShared.mAccumulativeMouseMove.position.x;
				mShared.mAccumulativeMouseMove.moveDelta.y += ev->y() - mShared.mAccumulativeMouseMove.position.y;
				mShared.mAccumulativeMouseMove.position.x = ev->x();
				mShared.mAccumulativeMouseMove.position.y = ev->y();
				mShared.mMove = true;
			}

			void InputWrapper::wheelEvent(QWheelEvent * ev)
			{
				if (!mEnabled) return;
				boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(mShared.mMutex);
				mShared.mAccumulativeMouseMove.scrollWheel += ev->angleDelta().y() / 120.0f;
				mShared.mMove = true;
			}

			void InputWrapper::mousePressEvent(QMouseEvent * ev)
			{
				if (!mEnabled) return;
				mShared.mMouseQueue.emplace(PRESS, Engine::GUI::MouseEventArgs{ { (float)ev->x(), (float)ev->y() }, convertMouseButton(ev->button()) });
			}

			void InputWrapper::mouseReleaseEvent(QMouseEvent * ev)
			{
				if (!mEnabled) return;
				mShared.mMouseQueue.emplace(RELEASE, Engine::GUI::MouseEventArgs{ { (float)ev->x(), (float)ev->y() }, convertMouseButton(ev->button()) });
			}

			void InputWrapper::resetAccumulativeMouseMove()
			{
				boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(mShared.mMutex);
				mShared.mMove = false;
				mShared.mAccumulativeMouseMove.moveDelta = { 0,0 };
				mShared.mAccumulativeMouseMove.scrollWheel = 0;
			}

			Engine::GUI::MouseButton::MouseButton InputWrapper::convertMouseButton(Qt::MouseButton id)
			{
				switch (id) {
				case Qt::LeftButton:
					return Engine::GUI::MouseButton::LEFT_BUTTON;
					break;
				case Qt::RightButton:
					return Engine::GUI::MouseButton::RIGHT_BUTTON;
					break;
				case Qt::MiddleButton:
					return Engine::GUI::MouseButton::MIDDLE_BUTTON;
					break;
				default:
					throw 0;
				}
			}

		}
	}
}