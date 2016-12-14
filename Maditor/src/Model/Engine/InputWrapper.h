#pragma once

#define BOOST_DATE_TIME_NO_LIB
#include <boost\interprocess\ipc\message_queue.hpp>


#include "InputInfo.h"

namespace Maditor {
	namespace Model {
		namespace Watcher {



			class InputWrapper {

			public:
				InputWrapper();

				void setEnabled(bool b);

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


				bool mEnabled;

				InputShared &mShared;

			};

		}
	}
}