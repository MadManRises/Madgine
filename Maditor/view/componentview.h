#pragma once

#include "dialogs/dialogmanager.h"
#include "dialogs/settingsdialog.h"

namespace Maditor {
namespace View {

template <class T>
class ComponentView
{
public:
	ComponentView(T *model = nullptr) :
	mModel(model){

	}

	virtual void setModel(T *model) {
		mModel = model;
		setupConnections();
	}

	virtual void clearModel() {
		mModel = nullptr;
		deleteConnections();
	}

protected:
	void setConnections(const std::list<std::pair<QAction*, void(T::*)()>> &connections) {
		mConnections = connections;
		if (mModel)
			setupConnections();
	}

private:
	void deleteConnections() {
		for (const QMetaObject::Connection &conn : mCurrentConnections) {
			QObject::disconnect(conn);
		}
		mCurrentConnections.clear();
	}

	void setupConnections() {
		deleteConnections();
		for (const std::pair<QAction*, void(T::*)()> &p : mConnections) {
			mCurrentConnections.emplace_back(QObject::connect(p.first, &QAction::triggered, mModel, p.second));
		}
	}

protected:
	QToolBar *createToolbar(const QString &name, const QList<QAction *> &actions) {
		QToolBar *toolbar = new QToolBar(name);
		toolbar->setObjectName(name);

		toolbar->addActions(actions);

		mToolBars.push_back(toolbar);

		return toolbar;
	}

	QMenu *createMenu(const QString &name, const QList<QAction *> &actions) {
		QMenu *menu = new QMenu(name);
		menu->setObjectName(name);
		menu->addActions(actions);
		mMenus.push_back(menu);

		return menu;
	}

	void addItemsToWindow(QMainWindow *window) {
		for (QToolBar *toolbar : mToolBars) {
			window->addToolBar(toolbar);
		}
		for (QMenu *menu : mMenus) {
			window->menuBar()->addMenu(menu);
		}
	}

	void createSettingsTab(Dialogs::DialogManager *dialogs, Dialogs::SettingsTab *tab, const QString &name) {
		dialogs->settingsDialog()->addSettingsTab(tab, name);
	}

	T *model() {
		return mModel;
	}

private:
	std::list<QMetaObject::Connection> mCurrentConnections;
	std::list<std::pair<QAction *, void (T::*)()>> mConnections;

	std::list<QToolBar*> mToolBars;
	std::list<QMenu*> mMenus;

	T *mModel;
};


} // namespace View
} // namespace Maditor
