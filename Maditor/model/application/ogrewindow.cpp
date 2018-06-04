#include "maditormodellib.h"

#include "ogrewindow.h"

#include "inputwrapper.h"

//#include "Model\Engine\ApplicationWrapper.h"


namespace Maditor {
	namespace Model {

		/*
		Note that we pass any supplied QWindow parent to the base QWindow class. This is necessary should we
		need to use our class within a container.
		*/
		OgreWindow::OgreWindow(InputWrapper *input)
			: QWindow(),
			mInput(input),
			mUpdateSize(false)
		{
			installEventFilter(this);
		}

		/*
		Upon destruction of the QWindow object we destroy the Ogre3D scene.
		*/
		OgreWindow::~OgreWindow()
		{
		}

		/*
		In case any drawing surface backing stores (QRasterWindow or QOpenGLWindow) of Qt are supplied to this
		class in any way we inform Qt that they will be unused.
		*/
		void OgreWindow::render(QPainter *painter)
		{
			QBrush brush;
			brush.setColor(Qt::black);
			painter->fillRect(geometry(), brush);
		}

		/*
		Called after the QWindow is reopened or when the QWindow is first opened.
		*/
		void OgreWindow::exposeEvent(QExposeEvent *event)
		{
			Q_UNUSED(event);
			if (mUpdateSize) {
				mUpdateSize = false;
				emit resized();
			}
		}


		/*
		Our event filter; handles the resizing of the QWindow. When the size of the QWindow changes note the
		call to the Ogre3D window and camera. This keeps the Ogre3D scene looking correct.
		*/
		bool OgreWindow::eventFilter(QObject *target, QEvent *event)
		{
			if (target == this)
			{
				if (event->type() == QEvent::Resize)
				{
					if (isExposed())
					{
						emit resized();
					}
					else {
						mUpdateSize = true;
					}
				}
			}

			return false;
		}

		/*
		How we handle keyboard and mouse events.
		*/
		void OgreWindow::keyPressEvent(QKeyEvent * ev)
		{
			mInput->keyPressEvent(ev);
		}

		void OgreWindow::keyReleaseEvent(QKeyEvent * ev)
		{
			mInput->keyReleaseEvent(ev);
		}

		void OgreWindow::mouseMoveEvent(QMouseEvent* ev)
		{
			mInput->mouseMoveEvent(ev);
		}

		void OgreWindow::wheelEvent(QWheelEvent *ev)
		{
			mInput->wheelEvent(ev);
		}

		void OgreWindow::mousePressEvent(QMouseEvent* ev)
		{
			mInput->mousePressEvent(ev);
			emit clicked();
		}

		void OgreWindow::mouseReleaseEvent(QMouseEvent* ev)
		{
			mInput->mouseReleaseEvent(ev);
		}


	}
}
