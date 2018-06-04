#pragma once

#include "mainwindow_ui.h"
#include "documentview.h"

namespace Maditor{
	namespace View{



template <class Model, class View>
class WindowSpawner
{
public:
	WindowSpawner() :
		mWidget(nullptr)
	{
	}

	void setupUi(const Ui::MainWindow * ui) {
		mWidget = ui->centralwidget;
		QObject::connect(mWidget, &CentralWidget::tabCloseRequested, std::bind(&WindowSpawner<Model, View>::tabCloseAction, this, std::placeholders::_1));
		QObject::connect(mWidget, &CentralWidget::currentChanged, std::bind(&WindowSpawner<Model, View>::currentTabChangedAction, this, std::placeholders::_1));

	}

	template <class... _Ty>
	View *spawn(Model *model, _Ty&&... args) {
		assert(mWidget);
		auto it = mViews.find(model);
		if (it == mViews.end()) {
			View *win = new View(model, std::forward<_Ty>(args)...);
			mViews[model] = win;
			win->updateTitle();
			mWidget->addTab(win);
			//QObject::connect(win, &View::windowTitleChanged, [=](const QString &title) {mTabWidget->setTabText(mTabWidget->indexOf(win), title); });
			return win;
		}
		else {
			mWidget->setCurrentDocumentWidget(it->second);
			return it->second;
		}
	}

protected:
	
	void remove(Model *model) {
		auto it = mViews.find(model);
		if (it != mViews.end()) {
			it->second->deleteLater();
			mViews.erase(it);
		}
	}

	void tabCloseAction(QWidget *widget) {
		if (View *w = dynamic_cast<View*>(widget)) {
			if (std::find_if(mViews.begin(), mViews.end(), [=](const std::pair<Model * const, View *> &p) {return p.second == w; }) != mViews.end())
				if (w->requestClose(false) != QMessageBox::Abort)
					w->document()->destroy();
		}
	}

	void currentTabChangedAction(QWidget *widget) {
		if (View *w = dynamic_cast<View*>(widget)) {
			if (std::find_if(mViews.begin(), mViews.end(), [=](const std::pair<Model * const, View *> &p) {return p.second == w; }) != mViews.end()) {
				currentTabSet(w);
				return;
			}
		}
		currentTabCleared(widget);
	}

	virtual void currentTabSet(View *win) {}
	virtual void currentTabCleared(QWidget *w) {}

private:
	CentralWidget *mWidget;
	
	std::map<Model*, View*> mViews;

};


} // namespace View
} // namespace Maditor
