#include "maditorinclude.h"

#include "InputWrapper.h"

namespace Maditor {
	namespace Model {
		namespace Watcher {
			InputWrapper::InputWrapper() :
				mSystem(0),
				mMove(false),
				mAccumulativeMouseMove({ 0, 0 }, { 0, 0 }, 0)
			{
			}

			void InputWrapper::setSystem(Engine::GUI::GUISystem * system)
			{
				mSystem = system;
			}

			void InputWrapper::clearSystem()
			{
				mSystem = 0;
			}

			void InputWrapper::update()
			{
				mInputMutex.lock();
				while (!mMouseClicks.empty()) {
					const std::pair<EventType, Engine::GUI::MouseEventArgs> &p = mMouseClicks.front();
					mInputMutex.unlock();
					if (p.first == PRESS) {
						mSystem->injectMousePress(p.second);
					}
					else {
						mSystem->injectMouseRelease(p.second);
					}
					mInputMutex.lock();
					mMouseClicks.pop();
				}
				while (!mKeyPresses.empty()) {
					const std::pair<EventType, Engine::GUI::KeyEventArgs> &p = mKeyPresses.front();
					mInputMutex.unlock();
					if (p.first == PRESS) {
						mSystem->injectKeyPress(p.second);
					}
					else {
						mSystem->injectKeyRelease(p.second);
					}
					mInputMutex.lock();
					mKeyPresses.pop();
				}
				if (mMove) {
					mSystem->injectMouseMove(mAccumulativeMouseMove);
					resetAccumulativeMouseMove();
				}
				mInputMutex.unlock();
			}

			void InputWrapper::keyPressEvent(QKeyEvent * ev)
			{
				if (!mSystem) return;
				char text = ev->text().isEmpty() ? 0 : ev->text().at(0).toLatin1();
				mInputMutex.lock();				
				mKeyPresses.emplace(PRESS, Engine::GUI::KeyEventArgs{ (Engine::GUI::Key)ev->nativeScanCode(), text } );
				mInputMutex.unlock();
			}

			void InputWrapper::keyReleaseEvent(QKeyEvent * ev)
			{
				if (!mSystem) return;
				char text = ev->text().isEmpty() ? 0 : ev->text().at(0).toLatin1();
				mInputMutex.lock();				
				mKeyPresses.emplace(RELEASE, Engine::GUI::KeyEventArgs{ (Engine::GUI::Key)ev->nativeScanCode(), text });
				mInputMutex.unlock();
			}

			void InputWrapper::mouseMoveEvent(QMouseEvent * ev)
			{
				if (!mSystem) return;
				mInputMutex.lock();
				mAccumulativeMouseMove.moveDelta.x += ev->x() - mAccumulativeMouseMove.position.x;
				mAccumulativeMouseMove.moveDelta.y += ev->y() - mAccumulativeMouseMove.position.y;
				mAccumulativeMouseMove.position.x = ev->x();
				mAccumulativeMouseMove.position.y = ev->y();
				mMove = true;
				mInputMutex.unlock();
			}

			void InputWrapper::wheelEvent(QWheelEvent * ev)
			{
				if (!mSystem) return;
				mInputMutex.lock();
				mAccumulativeMouseMove.scrollWheel += ev->angleDelta().y() / 120.0f;
				mMove = true;
				mInputMutex.unlock();
			}

			void InputWrapper::mousePressEvent(QMouseEvent * ev)
			{
				if (!mSystem) return;
				mInputMutex.lock();
				mMouseClicks.emplace(PRESS, Engine::GUI::MouseEventArgs{ {(float)ev->x(), (float)ev->y()}, convertMouseButton(ev->button()) });
				mInputMutex.unlock();
			}

			void InputWrapper::mouseReleaseEvent(QMouseEvent * ev)
			{
				if (!mSystem) return;
				mInputMutex.lock();
				mMouseClicks.emplace(RELEASE, Engine::GUI::MouseEventArgs{ { (float)ev->x(), (float)ev->y() }, convertMouseButton(ev->button()) });
				mInputMutex.unlock();
			}

			void InputWrapper::resetAccumulativeMouseMove()
			{
				mMove = false;
				mAccumulativeMouseMove.moveDelta = { 0,0 };
				mAccumulativeMouseMove.scrollWheel = 0;
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