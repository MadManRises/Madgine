#include "maditorview.h"

#include "documenttabbar.h"
#include "documenttabwidget.h"

namespace Maditor {
	namespace View {


		DocumentTabBar::DocumentTabBar(QWidget *parent) :
		QTabBar(parent),
		mPressedIndex(-1){
			setAcceptDrops(true);
		}

		/*!\reimp
		*/
		void DocumentTabBar::mousePressEvent(QMouseEvent *event)
		{
			const QPoint pos = event->pos();

			if (event->button() != Qt::LeftButton) {
				event->ignore();
				return;
			}

			mPressedIndex = tabAt(event->pos());
			if (mPressedIndex != -1) {
				mDragStartPosition = pos;
			}

			QTabBar::mousePressEvent(event);

			static_cast<DocumentTabWidget*>(parentWidget())->focus();

		}


		/*!\reimp
		*/
		void DocumentTabBar::mouseReleaseEvent(QMouseEvent *event)
		{
			if (event->button() != Qt::LeftButton) {
				event->ignore();
				return;
			}

			if (mPressedIndex != -1) {
				mDragStartPosition = QPoint();
			}

			QTabBar::mouseReleaseEvent(event);

		}


		/*!\reimp
		*/
		void DocumentTabBar::mouseMoveEvent(QMouseEvent *event)
		{

			QTabBar::mouseMoveEvent(event);

			// Start drag
			if (mPressedIndex != -1) {
				if ((event->pos() - mDragStartPosition).manhattanLength() > QApplication::startDragDistance()) {

					QMimeData *mimeData = new QMimeData;
					mimeData->setData("application/x-document", QByteArray::number(mPressedIndex));
					
					QDrag *drag = new QDrag(parentWidget());
					drag->setPixmap(style()->standardIcon(QStyle::SP_ArrowBack).pixmap(20, 20));					
					drag->setMimeData(mimeData);
					

					if (drag->exec() == Qt::MoveAction) {

					}

					mouseReleaseEvent(new QMouseEvent(QMouseEvent::MouseButtonRelease, event->pos(), Qt::LeftButton, Qt::LeftButton, event->modifiers()));

					//d->setupMovableTab();
				}
			}		

			

		}

		void DocumentTabBar::dragMoveEvent(QDragMoveEvent *event)
		{
			if (event->mimeData()->hasFormat("application/x-document")) {
				if (event->source() == parentWidget()) {
					QTabBar::mouseMoveEvent(new QMouseEvent(QEvent::MouseMove, event->pos(), Qt::LeftButton, event->mouseButtons(), event->keyboardModifiers()));
					/*event->setDropAction(Qt::MoveAction);
					event->accept();*/
					event->acceptProposedAction();
				}
				else {
					event->acceptProposedAction();
				}
			}
			else {
				event->ignore();
			}
		}

		void DocumentTabBar::dragEnterEvent(QDragEnterEvent * event)
		{
			QTabBar::dragEnterEvent(event);

			event->accept();

			/*if (event->mimeData()->hasFormat("application/x-document")) {
				if (event->source() == this) {

				}
				else {
					int index = tabAt(event->pos());
					if (index == -1)
						index = count();
					insertTab(index, "Test");
				}
			}
			else {
				event->ignore();
			}*/

		}

		void DocumentTabBar::dragLeaveEvent(QDragLeaveEvent * event)
		{
			QTabBar::dragLeaveEvent(event);

			event->accept();

			if (mPressedIndex != -1) {
				
			}

		}


	}
}
