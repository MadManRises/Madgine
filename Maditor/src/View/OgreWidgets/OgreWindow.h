#pragma once

#include <qwindow.h>

namespace Maditor {
	namespace Model {
		namespace Watcher {
			class InputWrapper;
			class ApplicationWatcher;
		}
		class ApplicationWrapper;
	}
	namespace View {
		class OgreWindow : public QWindow
		{
			/*
			A QWindow still inherits from QObject and can have signals/slots; we need to add the appropriate
			Q_OBJECT keyword so that Qt's intermediate compiler can do the necessary wireup between our class
			and the rest of Qt.
			*/
			Q_OBJECT

		public:
			explicit OgreWindow(Model::ApplicationWrapper *app, Model::Watcher::ApplicationWatcher *watcher, QWindow *parent = NULL);
			~OgreWindow();

			/*
			We declare these methods virtual to allow for further inheritance.
			*/
			virtual void render(QPainter *painter);

			/*
			We use an event filter to be able to capture keyboard/mouse events. More on this later.
			*/
			virtual bool eventFilter(QObject *target, QEvent *event);

		public slots:
			void setInput();

		protected:


			/*
			The below methods are what is actually fired when they keys on the keyboard are hit.
			Similar events are fired when the mouse is pressed or other events occur.
			*/
			virtual void keyPressEvent(QKeyEvent * ev);
			virtual void keyReleaseEvent(QKeyEvent * ev);
			virtual void mouseMoveEvent(QMouseEvent* e);
			virtual void wheelEvent(QWheelEvent* e);
			virtual void mousePressEvent(QMouseEvent* e);
			virtual void mouseReleaseEvent(QMouseEvent* e);

			virtual void exposeEvent(QExposeEvent *event);

		private:
			Model::Watcher::InputWrapper *mInput;
			Model::Watcher::ApplicationWatcher *mWatcher;
			Model::ApplicationWrapper *mApp;
			bool mUpdateSize;

		};
	}
}