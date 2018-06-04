#include "maditorviewlib.h"

#include "mainwindow.h"
#include "mainwindow_ui.h"

#include "maditorview.h"



#include "editorsettingswidget.h"


#include "model/addons/addon.h"

#include "model/maditor.h"

namespace Maditor {
namespace View {

	MaditorView::MaditorView() :
		mDialogManager(new Dialogs::DialogManager),

		mSettings(nullptr),
		mMainWindow(nullptr){

		mRecentProjectsMenu = new QMenu("Recent Projects");
		mClearRecentProjectsAction = mRecentProjectsMenu->addAction("Clear List");
		mRecentProjectsMenu->addSeparator();
		mRecentProjectInitialActionCount = mRecentProjectsMenu->actions().count();

		connect(mRecentProjectsMenu, &QMenu::triggered, this, &MaditorView::recentProjectClicked);

		setConnections({
			{ mClearRecentProjectsAction, &Model::Maditor::clearRecentProjects }
		});

	}

	MaditorView::~MaditorView()
	{

		mSettings->beginGroup("Window");
		mSettings->setValue("geometry", mMainWindow->saveGeometry());
		mSettings->setValue("state", mMainWindow->saveState(0));
		mSettings->endGroup();

		delete mDialogManager;


	}

	void MaditorView::setupUi(MainWindow * window)
	{

		window->ui->menuFile->insertMenu(window->ui->actionSettings, mRecentProjectsMenu);

		if (model())
			model()->addons()->setupUi(window);


		addItemsToWindow(window);

		connect(window->ui->actionSettings, &QAction::triggered, mDialogManager, &Dialogs::DialogManager::showSettingsDialog);

	}

	void MaditorView::setModel(Model::Maditor * model)
	{
		ComponentView::setModel(model);

		mSettings = &model->settings();

		

		model->setDialogManager(mDialogManager);
		createSettingsTab(mDialogManager, new EditorSettingsWidget(model), "Projects");

		updateRecentProjects(model->recentProjects());

		connect(model, &Model::Maditor::recentProjectsChanged, this, &MaditorView::updateRecentProjects);

		model->addons()->setup(this);

		if (mMainWindow)
			mMainWindow->setModel(model);
	}


	Dialogs::DialogManager * MaditorView::dialogs()
	{
		return mDialogManager;
	}

	bool MaditorView::closeEvent()
	{
		
		return true;
	}

	void MaditorView::createMainWindow() {
		mMainWindow = new MainWindow;
		setupUi(mMainWindow);

		mMainWindow->show();

		mSettings->beginGroup("Window");
		mMainWindow->restoreGeometry(mSettings->value("geometry").toByteArray());
		mMainWindow->restoreState(mSettings->value("state").toByteArray(), 0);
		mSettings->endGroup(); 

		if (model())
			mMainWindow->setModel(model());
	}



	void MaditorView::updateRecentProjects(const QStringList & list)
	{
		QMenu *menu = mRecentProjectsMenu;
		for (QAction *action : menu->actions().mid(mRecentProjectInitialActionCount)) {
			menu->removeAction(action);
		}

		for (const QString &project : list) {
			menu->addAction(project);
		}
	}



	void MaditorView::recentProjectClicked(QAction * action)
	{
		if (mRecentProjectsMenu->actions().indexOf(action) >= mRecentProjectInitialActionCount)
			model()->loadProject(action->text());
	}

} // namespace View
} // namespace Maditor
