#pragma once


namespace Maditor {

	

	namespace Model {

			class InputWrapper {

			public:
				InputWrapper(Shared::InputShared &shared);

				
				virtual void keyPressEvent(QKeyEvent * ev);
				virtual void keyReleaseEvent(QKeyEvent * ev);
				virtual void mouseMoveEvent(QMouseEvent* e);
				virtual void wheelEvent(QWheelEvent* e);
				virtual void mousePressEvent(QMouseEvent* e);
				virtual void mouseReleaseEvent(QMouseEvent* e);

			private:
				void resetAccumulativeMouseMove();

				Shared::MouseButton convertMouseButton(Qt::MouseButton button);

			private:
				Shared::InputShared &mShared;

			};

	}
}