#include "maditorviewlib.h"

#include "mainwindow.h"
#include "mainwindow_ui.h"

#include "maditorview.h"



#include "editorsettingswidget.h"


#include "../model/addons/addon.h"

#include "../model/maditor.h"

#include "logs/logsview.h"
#include "project/projectview.h"
#include "application/applicationview.h"
#include "project/cmakeview.h"

#include "../model/project/modulelist.h"

namespace Maditor {
namespace View {

	MaditorView::MaditorView() :
		mDialogManager(std::make_unique<Dialogs::DialogManager>()),
		mSettings(QSettings::UserScope, "MadMan Studios", "Maditor-View"),
		mMainWindow(nullptr),
		mLogs(new LogsView),
		mProject(new ProjectView),
		mApplication(new ApplicationView),
		mCmake(new CmakeView){

		mRecentProjectsMenu = new QMenu("Recent Projects");
		mClearRecentProjectsAction = mRecentProjectsMenu->addAction("Clear List");
		mRecentProjectsMenu->addSeparator();
		mRecentProjectInitialActionCount = mRecentProjectsMenu->actions().count();

		connect(mRecentProjectsMenu, &QMenu::triggered, this, &MaditorView::recentProjectClicked);

	}

	MaditorView::~MaditorView()
	{

		mSettings.beginGroup("Window");
		mSettings.setValue("geometry", mMainWindow->saveGeometry());
		mSettings.setValue("state", mMainWindow->saveState(0));
		mSettings.endGroup();

		delete mCmake;
		delete mApplication;
		delete mLogs;
		delete mProject;

	}

	void MaditorView::setupUi(MainWindow * window)
	{
		ComponentView::setupUi(window);

		window->ui->menuFile->insertMenu(window->ui->actionSettings, mRecentProjectsMenu);

		if (model())
			model()->addons()->setupUi(window);


		addItemsToWindow(window);

		mApplication->setupUi(window);
		mLogs->setupUi(window);
		mProject->setupUi(window);
		mCmake->setupUi(window);

		connect(window->ui->actionSettings, &QAction::triggered, mDialogManager.get(), &Dialogs::DialogManager::showSettingsDialog);

	}

	void MaditorView::setModel(Model::Maditor * model)
	{
		ComponentView::setModel(model);

		createSettingsTab(mDialogManager.get(), new EditorSettingsWidget(model), "Projects");

		updateRecentProjects(model->recentProjects());

		connect(model, &Model::Maditor::recentProjectsChanged, this, &MaditorView::updateRecentProjects);
		connect(model, &Model::Maditor::projectOpened, this, &MaditorView::onProjectOpened);
		connect(model, &Model::Maditor::projectClosed, this, &MaditorView::onProjectClosed);

		if (model->project()) {
			onProjectOpened(model->project());
		}

		mLogs->setModel(model->logs());

		model->addons()->setup(this);

	}


	Dialogs::DialogManager * MaditorView::dialogs()
	{
		return mDialogManager.get();
	}

	bool MaditorView::closeEvent()
	{
		
		return true;
	}

	void MaditorView::createMainWindow() {
		mMainWindow = new MainWindow;
		setupUi(mMainWindow);

		setConnections({
			{ mClearRecentProjectsAction, &Model::Maditor::clearRecentProjects },
			{ mMainWindow->ui->actionNewProject, &Model::Maditor::newProject },
			{ mMainWindow->ui->actionLoadProject, &Model::Maditor::loadProject },
			{ mMainWindow->ui->actionCloseProject, &Model::Maditor::closeProject }
		});

		mMainWindow->show();

		mSettings.beginGroup("Window");
		mMainWindow->restoreGeometry(mSettings.value("geometry").toByteArray());
		mMainWindow->restoreState(mSettings.value("state").toByteArray(), 0);
		mSettings.endGroup(); 

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

	void MaditorView::onProjectOpened(Model::Project *project) {
		mApplication->setConfigModel(project->configList());
		mProject->setModel(project);
		mCmake->setModel(project->moduleList()->cmake());
		mMainWindow->ui->actionCloseProject->setEnabled(true);
	}

	void MaditorView::onProjectClosed(Model::Project* project)
	{
		mApplication->clearConfigModel();
		mProject->clearModel();
		mCmake->clearModel();
		mMainWindow->ui->actionCloseProject->setEnabled(false);
	}

} // namespace View
} // namespace Maditor
