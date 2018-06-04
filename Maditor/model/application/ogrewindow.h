#pragma once


namespace Maditor {
	namespace Model {
		class MADITOR_MODEL_EXPORT OgreWindow : public QWindow
		{
			Q_OBJECT

		public:
			explicit OgreWindow(InputWrapper *input);
			~OgreWindow();

			/*
			We declare these methods virtual to allow for further inheritance.
			*/
			virtual void render(QPainter *painter);

			/*
			We use an event filter to be able to capture keyboard/mouse events. More on this later.
			*/
			virtual bool eventFilter(QObject *target, QEvent *event);

		signals:
			void resized();
			void clicked();

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
			InputWrapper *mInput;
			bool mUpdateSize;

		};
	}
}