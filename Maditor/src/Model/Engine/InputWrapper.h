#pragma once


namespace Maditor {
	namespace Model {
		namespace Watcher {

			class InputWrapper : public Engine::Input::InputHandler {

			public:
				InputWrapper();

				void setSystem(Engine::GUI::GUISystem *system);
				void clearSystem();
				virtual void update() override;


				virtual void keyPressEvent(QKeyEvent * ev);
				virtual void keyReleaseEvent(QKeyEvent * ev);
				virtual void mouseMoveEvent(QMouseEvent* e);
				virtual void wheelEvent(QWheelEvent* e);
				virtual void mousePressEvent(QMouseEvent* e);
				virtual void mouseReleaseEvent(QMouseEvent* e);

			private:
				void resetAccumulativeMouseMove();

				Engine::GUI::MouseButton::MouseButton convertMouseButton(Qt::MouseButton button);

			private:
				std::mutex mInputMutex;
				enum EventType {
					PRESS,
					RELEASE
				};
				std::queue<std::pair<EventType, Engine::GUI::MouseEventArgs>> mMouseClicks;
				std::queue<std::pair<EventType, Engine::GUI::KeyEventArgs>> mKeyPresses;
				bool mMove;
				Engine::GUI::MouseEventArgs mAccumulativeMouseMove;

				Engine::GUI::GUISystem *mSystem;
			};

		}
	}
}