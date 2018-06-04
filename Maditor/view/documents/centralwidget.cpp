#include "maditorview.h"

#include "centralwidget.h"

#include "documenttabwidget.h"

namespace Maditor {
	namespace View {

		CentralWidget::CentralWidget(QWidget *parent) :
			QWidget(parent),
			mCurrentTabWidget(nullptr)
		{
		}

		void CentralWidget::setWidget(QWidget * w)
		{
			static_cast<QGridLayout*>(layout())->addWidget(w, 0, 0);
		}

		void CentralWidget::addTabWidget(DocumentTabWidget * w)
		{
			if (!mCurrentTabWidget)
				mCurrentTabWidget = w;

			connect(w, &DocumentTabWidget::currentChanged, this, &CentralWidget::onCurrentTabChanged);
			connect(w, &DocumentTabWidget::tabCloseRequested, this, &CentralWidget::onTabCloseRequested);
			connect(w, &DocumentTabWidget::destroyed, this, &CentralWidget::removeTabWidget);

			mTabWidgets.push_back(w);
		}

		void CentralWidget::addTab(QWidget *w) {
			mCurrentTabWidget->addTab(w, w->windowTitle());
		}

		void CentralWidget::setCurrentDocumentWidget(QWidget * w)
		{
			parentTabWidget(w)->setCurrentWidget(w);
		}

		const std::list<DocumentTabWidget*>& Maditor::View::CentralWidget::tabWidgets() const
		{
			return mTabWidgets;
		}

		void CentralWidget::focus(DocumentTabWidget * w)
		{
			mCurrentTabWidget = w;
			emit currentChanged(w->currentWidget());
		}

		DocumentTabWidget * CentralWidget::parentTabWidget(QWidget * w)
		{
			QWidget *p = w->parentWidget();
			DocumentTabWidget *t = dynamic_cast<DocumentTabWidget*>(w);
			return t ? t : parentTabWidget(p);
		}

		void CentralWidget::removeTabWidget(QObject *obj) {
			mTabWidgets.remove(static_cast<DocumentTabWidget*>(obj));
		}

		void CentralWidget::onCurrentTabChanged(int index) {
			QTabWidget *t = static_cast<QTabWidget*>(sender());
			//if (t->hasFocus()) {
				emit currentChanged(t->widget(index));
			//}
		}

		void CentralWidget::onTabCloseRequested(int index)
		{
			QTabWidget *t = static_cast<QTabWidget*>(sender());
			emit tabCloseRequested(t->widget(index));
		}

	}
}
