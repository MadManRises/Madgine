#include "maditormodellib.h"

#include "inputwrapper.h"

#include "shared/inputinfo.h"

namespace Maditor {
	namespace Model {

			InputWrapper::InputWrapper(Shared::InputShared &shared) :
				mShared(shared)
			{
				resetAccumulativeMouseMove();
			}

			void InputWrapper::keyPressEvent(QKeyEvent * ev)
			{
				char text = ev->text().isEmpty() ? 0 : ev->text().at(0).toLatin1();
				mShared.mKeyQueue.emplace(Shared::PRESS, Shared::KeyEventArgs{ ev->nativeScanCode(), text });
			}

			void InputWrapper::keyReleaseEvent(QKeyEvent * ev)
			{
				char text = ev->text().isEmpty() ? 0 : ev->text().at(0).toLatin1();			
				mShared.mKeyQueue.emplace(Shared::RELEASE, Shared::KeyEventArgs{ ev->nativeScanCode(), text });
			}

			void InputWrapper::mouseMoveEvent(QMouseEvent * ev)
			{
				boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(mShared.mMutex, boost::interprocess::try_to_lock);
				if (lock.owns()) {
					mShared.mAccumulativeMouseMove.moveDelta[0] += ev->x() - mShared.mAccumulativeMouseMove.position[0];
					mShared.mAccumulativeMouseMove.moveDelta[1] += ev->y() - mShared.mAccumulativeMouseMove.position[1];
					mShared.mAccumulativeMouseMove.position[0] = ev->x();
					mShared.mAccumulativeMouseMove.position[1] = ev->y();
					mShared.mMove = true;
				}
			}

			void InputWrapper::wheelEvent(QWheelEvent * ev)
			{
				boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(mShared.mMutex, boost::interprocess::try_to_lock);
				if (lock.owns()) {
					mShared.mAccumulativeMouseMove.scrollWheel += ev->angleDelta().y() / 120.0f;
					mShared.mMove = true;
				}
			}

			void InputWrapper::mousePressEvent(QMouseEvent * ev)
			{
				mShared.mMouseQueue.emplace(Shared::PRESS, Shared::MouseEventArgs{ { (float)ev->x(), (float)ev->y() }, convertMouseButton(ev->button()) });
			}

			void InputWrapper::mouseReleaseEvent(QMouseEvent * ev)
			{
				mShared.mMouseQueue.emplace(Shared::RELEASE, Shared::MouseEventArgs{ { (float)ev->x(), (float)ev->y() }, convertMouseButton(ev->button()) });
			}

			void InputWrapper::resetAccumulativeMouseMove()
			{
				boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(mShared.mMutex, boost::interprocess::try_to_lock);
				if (lock.owns()) {
					mShared.mMove = false;
					mShared.mAccumulativeMouseMove.moveDelta = { 0,0 };
					mShared.mAccumulativeMouseMove.scrollWheel = 0;
				}
			}

			Shared::MouseButton InputWrapper::convertMouseButton(Qt::MouseButton id)
			{
				switch (id) {
				case Qt::LeftButton:
					return Shared::MouseButton::LEFT_BUTTON;
					break;
				case Qt::RightButton:
					return Shared::MouseButton::RIGHT_BUTTON;
					break;
				case Qt::MiddleButton:
					return Shared::MouseButton::MIDDLE_BUTTON;
					break;
				default:
					throw 0;
				}
			}

		
	}
}
