#include "maditorview.h"

#include "documenttabwidget.h"
#include "documenttabbar.h"

#include "mainwindow.h"

#include "centralwidget.h"

namespace Maditor {
	namespace View {

		DocumentTabWidget::DocumentTabWidget(CentralWidget *centralWidget, QWidget *parent) :
			QTabWidget(parent),
			mCentralWidget(centralWidget)
		{
			setTabBar(new DocumentTabBar);
			setTabsClosable(true);
			setAcceptDrops(true);
			centralWidget->addTabWidget(this);
		}

		void DocumentTabWidget::focus()
		{
			mCentralWidget->focus(this);
		}

		void DocumentTabWidget::tabInserted(int index) {
			QTabWidget::tabInserted(index);			
		}

		void DocumentTabWidget::dragMoveEvent(QDragMoveEvent * event)
		{
			QTabWidget::dragMoveEvent(event);

			if (count() > (event->source() == this ? 1 : 0)) {

				QPoint p = event->pos();

				if (p.x() >= width() / 5 && p.x() <= 4 * width() / 5 &&
					p.y() >= height() / 5 && p.y() <= 4 * height() / 5) {
				}
				else {
					int offset;
					int length;
					Qt::Orientation orientation;
					if (p.x() < width() / 5) {
						orientation = Qt::Horizontal;
						offset = 0;
						length = width();
					}
					else if (p.x() > 4 * width() / 5) {
						orientation = Qt::Horizontal;
						offset = 1;
						length = width();
					}
					else if (p.y() < height() / 5) {
						orientation = Qt::Vertical;
						offset = 0;
						length = height();
					}
					else if (p.y() > 4 * height() / 5) {
						orientation = Qt::Vertical;
						offset = 1;
						length = height();
					}

					QSplitter *splitter;
					QWidget *parent = parentWidget();
					if (CentralWidget *w = dynamic_cast<CentralWidget*>(parent)) {
						splitter = new QSplitter();
						splitter->setChildrenCollapsible(false);
						splitter->setOrientation(orientation);
						splitter->addWidget(this);
						w->setWidget(splitter);

						splitter->setSizes({ length });
						//splitter->setFixedSize(s);

					}
					else if (QSplitter *s = dynamic_cast<QSplitter*>(parent)) {
						/*if (s->orientation() == orientation) {
							splitter = s;
						}
						else {*/
							splitter = new QSplitter();
							splitter->setChildrenCollapsible(false);
							splitter->setOrientation(orientation);
							int i = s->indexOf(this);
							QList<int> sizes = s->sizes();
							splitter->addWidget(this);
							s->insertWidget(i, splitter);
							s->setSizes(sizes);
						//}
					}
					else {
						throw 0;
					}

					int index = splitter->indexOf(this);
					splitter->insertWidget(index + offset, new DocumentTabWidget(mCentralWidget, splitter));
					QList<int> sizes = splitter->sizes();
					sizes[index + offset] = length / 2;
					sizes[index + 1 - offset] = length / 2;
					splitter->setSizes(sizes);
				}
			}

		}

		void DocumentTabWidget::dragEnterEvent(QDragEnterEvent * event)
		{
			QTabWidget::dragEnterEvent(event);
			event->accept();			
		}

		void DocumentTabWidget::dragLeaveEvent(QDragLeaveEvent * event)
		{
			QTabWidget::dragLeaveEvent(event);

			if (count() == 0) {
				if (QSplitter *s = dynamic_cast<QSplitter*>(parentWidget())) {
					removeWidget(this);
				}
			}

		}

		void DocumentTabWidget::mousePressEvent(QMouseEvent * event)
		{
			QTabWidget::mousePressEvent(event);
			focus();
		}

		void DocumentTabWidget::removeWidget(QWidget *w) {
			QSplitter *s = dynamic_cast<QSplitter*>(w->parentWidget());
			assert(s);
			w->hide();
			w->deleteLater();
			if (s->count() == 2) {
				int index = s->indexOf(w);
				replaceSplitter(s, s->widget(1 - index));
			}
			else if (s->count() == 1) {
				removeWidget(s);
			}
		}

		void DocumentTabWidget::replaceSplitter(QSplitter * splitter, QWidget * w)
		{
			QWidget *parent = splitter->parentWidget();
			if (CentralWidget *m = dynamic_cast<CentralWidget*>(parent)) {		

				//QSize s = splitter->size();
				m->setWidget(w);
				//w->setFixedSize(s);
			}
			else if (QSplitter *s = dynamic_cast<QSplitter*>(parent)) {
				int index = s->indexOf(splitter);
				QList<int> list = s->sizes();
				list.insert(index + 1, 0);
				s->insertWidget(index, w);
				s->setSizes(list);
			}
			else {
				throw 0;
			}
			splitter->hide();			
			splitter->deleteLater();
		}

		void DocumentTabWidget::dropEvent(QDropEvent * event)
		{
			if (event->source() == this)
				return;
			if (event->mimeData()->hasFormat("application/x-document")) {
				DocumentTabWidget *tabWidget = static_cast<DocumentTabWidget*>(event->source());
				QWidget *w = tabWidget->widget(event->mimeData()->data("application/x-document").toInt());
				setCurrentIndex(addTab(w, w->windowTitle()));
			}
		}

		void DocumentTabWidget::tabRemoved(int index)
		{
			QTabWidget::tabRemoved(index);
			if (count() == 0) {
				if (QSplitter *s = dynamic_cast<QSplitter*>(parentWidget())) {
					removeWidget(this);
				}
			}
		}

	}
}
