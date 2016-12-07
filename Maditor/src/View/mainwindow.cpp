#include "maditorlib.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"


#include "Model/Editor.h"

#include "OgreWidgets\OgreWindow.h"

#include "Model\Engine\ApplicationWrapper.h"

#include "Model\Engine\Watcher\ApplicationWatcher.h"
#include "Model\Engine\Watcher\ResourceWatcher.h"
#include "Model\Engine\Watcher\PerformanceWatcher.h"
#include "Model\Engine\Watcher\OgreSceneWatcher.h"
#include "Model\Engine\Watcher\ObjectsWatcher.h"

#include "Model\Editors\EditorManager.h"
#include "Model\Editors\ScriptEditor.h"

#include "editorsettingswidget.h"
#include "Dialogs\settingsdialog.h"

namespace Maditor {
	namespace View {

		MainWindow::MainWindow(Model::Editor *editor, QWidget *parent) :
			QMainWindow(parent),
			ui(new Ui::MainWindow),
			mEditor(editor),
			mEditorSettingsWidget(new EditorSettingsWidget(editor))
		{
			ui->setupUi(this);

			mRecentProjectInitialActionCount = ui->menuRecentProjects->actions().count();
			
			updateRecentProjects(mEditor->recentProjects());

			mDialogManager.settingsDialog()->addSettingsTab(mEditorSettingsWidget, "Projects");
			

			mTarget = new OgreWindow(editor->application());
			QWidget *ogre = QWidget::createWindowContainer(mTarget);

			ui->game->addWidget(ogre);

			Model::Watcher::ApplicationWatcher *watcher = editor->watcher();
			ui->LogsWidget->setModel(editor->logsModel());
			//ui->guiRenderStats->connectWatchers("Gui", watcher);
			//ui->sceneRenderStats->connectWatchers("Scene", watcher);
			ui->ResourcesWidget->setModel(watcher->resourceWatcher());
			ui->scriptEditorTabWidget->setModel(editor->editorManager()->scriptEditor());
			ui->PerformanceWidget->setModel(watcher->performanceWatcher());
			ui->OgreSceneWidget->setModel(watcher->ogreSceneWatcher());
			ui->ObjectsWidget->setModel(watcher->objectsWatcher());
			ui->NetworkWidget->setModel(watcher->client());

			setupConnections();

			
		}

		MainWindow::~MainWindow()
		{
			delete mEditorSettingsWidget;
			delete ui;
		}

		void MainWindow::startup()
		{
			QSettings &settings = mEditor->settings();
			settings.beginGroup("Window");
			restoreGeometry(settings.value("geometry").toByteArray());
			restoreState(settings.value("state").toByteArray(), 0);
			settings.endGroup();			

			mEditor->onStartup();
		}

		Dialogs::DialogManager * MainWindow::dialogs()
		{
			return &mDialogManager;
		}		

		void MainWindow::closeEvent(QCloseEvent *event)
		{

			QSettings &settings = mEditor->settings();
			settings.beginGroup("Window");
			settings.setValue("geometry", saveGeometry());
			settings.setValue("state", saveState(0));
			settings.endGroup();

			event->accept();
		}

		void MainWindow::setupConnections()
		{
			
			//Project-related
			connect(mEditor, &Model::Editor::projectOpened, this, &MainWindow::onProjectOpened);
			connect(mEditor, &Model::Editor::projectOpened, &mDialogManager, &Dialogs::DialogManager::onProjectOpened);

			connect(mEditor, &Model::Editor::recentProjectsChanged, this, &MainWindow::updateRecentProjects);
			connect(ui->menuRecentProjects, &QMenu::triggered, this, &MainWindow::recentProjectClicked);

			


			//Editor-related
			connect(mEditor->editorManager()->scriptEditor(), &Model::Editors::ScriptEditorModel::showDoc, this, &MainWindow::ensureVisible);


			//Watcher-related
			connect(mEditor->watcher(), &Model::Watcher::ApplicationWatcher::applicationCreated, this, &MainWindow::onAppCreated, Qt::QueuedConnection);
			connect(mEditor->watcher(), &Model::Watcher::ApplicationWatcher::applicationInitialized, this, &MainWindow::onAppInitialized, Qt::QueuedConnection);
			connect(mEditor->watcher(), &Model::Watcher::ApplicationWatcher::applicationStarted, this, &MainWindow::onAppStarted, Qt::QueuedConnection);
			connect(mEditor->watcher(), &Model::Watcher::ApplicationWatcher::applicationStopped, this, &MainWindow::onAppStopped, Qt::QueuedConnection);
			connect(mEditor->watcher(), &Model::Watcher::ApplicationWatcher::applicationShutdown, this, &MainWindow::onAppShutdown, Qt::QueuedConnection);

			connect(mEditor->watcher(), &Model::Watcher::ApplicationWatcher::applicationInitialized, mTarget, &OgreWindow::setInput);

			//Dialogs-related
			connect(ui->actionNewProject, &QAction::triggered, &mDialogManager, &Dialogs::DialogManager::showNewProjectDialog);
			connect(ui->actionLoadProject, &QAction::triggered, &mDialogManager, &Dialogs::DialogManager::showLoadProjectDialog);
			connect(ui->actionSettings, &QAction::triggered, &mDialogManager, &Dialogs::DialogManager::showSettingsDialog);

			connect(&mDialogManager, &Dialogs::DialogManager::newProjectDialogAccepted, mEditor, &Model::Editor::newProject);
			connect(&mDialogManager, &Dialogs::DialogManager::loadProjectDialogAccepted, mEditor, &Model::Editor::loadProject);
			Model::Generators::ClassGeneratorFactory *factory = mEditor->classGeneratorFactory();
			connect(&mDialogManager, &Dialogs::DialogManager::newClassDialogAccepted, factory, &Model::Generators::ClassGeneratorFactory::createClass);

			connect(factory, &Model::Generators::ClassGeneratorFactory::newEntityComponentRequest, &mDialogManager, &Dialogs::DialogManager::showNewEntityComponentDialog);
			connect(factory, &Model::Generators::ClassGeneratorFactory::newGameHandlerRequest, &mDialogManager, &Dialogs::DialogManager::showNewGameHandlerDialog);
			connect(factory, &Model::Generators::ClassGeneratorFactory::newGlobalAPIRequest, &mDialogManager, &Dialogs::DialogManager::showNewGlobalAPIDialog);
			connect(factory, &Model::Generators::ClassGeneratorFactory::newGuiHandlerRequest, &mDialogManager, &Dialogs::DialogManager::showNewGuiHandlerDialog);
			connect(factory, &Model::Generators::ClassGeneratorFactory::newOtherClassRequest, &mDialogManager, &Dialogs::DialogManager::showNewOtherClassDialog);
			connect(factory, &Model::Generators::ClassGeneratorFactory::newSceneComponentRequest, &mDialogManager, &Dialogs::DialogManager::showNewSceneComponentDialog);
			connect(&mDialogManager, &Dialogs::DialogManager::newEntityComponentDialogAccepted, factory, &Model::Generators::ClassGeneratorFactory::createEntityComponent);
			connect(&mDialogManager, &Dialogs::DialogManager::newGameHandlerDialogAccepted, factory, &Model::Generators::ClassGeneratorFactory::createGameHandler);
			connect(&mDialogManager, &Dialogs::DialogManager::newGlobalAPIDialogAccepted, factory, &Model::Generators::ClassGeneratorFactory::createGlobalAPI);
			connect(&mDialogManager, &Dialogs::DialogManager::newGuiHandlerDialogAccepted, factory, &Model::Generators::ClassGeneratorFactory::createGuiHandler);
			connect(&mDialogManager, &Dialogs::DialogManager::newOtherClassDialogAccepted, factory, &Model::Generators::ClassGeneratorFactory::createOtherClass);
			connect(&mDialogManager, &Dialogs::DialogManager::newSceneComponentDialogAccepted, factory, &Model::Generators::ClassGeneratorFactory::createSceneComponent);
								
		}


		void MainWindow::initApp()
		{
			ui->actionInit->setEnabled(false);
			mEditor->application()->load(mEditor->project(), mTarget);			
		}

		void MainWindow::startApp()
		{
			ui->actionStart->setEnabled(false);
			mEditor->application()->start();
		}

		void MainWindow::stopApp()
		{
			mEditor->application()->stop();
		}

		void MainWindow::pauseApp()
		{
		}

		void MainWindow::finalizeApp()
		{
			ui->actionStart->setEnabled(false);
			ui->actionStop->setEnabled(false);
			ui->actionPause->setEnabled(false);
			ui->actionFinalize->setEnabled(false);
			mEditor->application()->shutdown();
		}

		void MainWindow::ensureVisible(QWidget * widget)
		{
			while (widget->parentWidget()) {
				QTabWidget *tab = qobject_cast<QTabWidget*>(widget->parentWidget());
				QStackedWidget *stack = qobject_cast<QStackedWidget*>(widget->parentWidget());
				if (stack) {
					tab = qobject_cast<QTabWidget*>(stack->parentWidget());
					if (!tab)
						stack->setCurrentWidget(widget);
				}
				if (tab) {
					tab->setCurrentWidget(widget);
					if (stack)
						widget = widget->parentWidget();
				}
					
				
				widget = widget->parentWidget();
			}
		}

		void MainWindow::releaseProject()
		{
			mEditor->project()->release();
		}

		void MainWindow::clearRecentProjects()
		{
			mEditor->clearRecentProjects();
		}

		void MainWindow::showGame()
		{
			ui->game->setCurrentIndex(2);
			ensureVisible(ui->game);
		}

		void MainWindow::hideGame()
		{
			ui->game->setCurrentIndex(1);
		}

		void MainWindow::updateRecentProjects(const QStringList & list)
		{
			QMenu *menu = ui->menuRecentProjects;			
			for (QAction *action : menu->actions().mid(mRecentProjectInitialActionCount)) {
				menu->removeAction(action);
			}

			for (const QString &project : list) {
				menu->addAction(project);
			}
		}

		void MainWindow::recentProjectClicked(QAction * action)
		{
			if (ui->menuRecentProjects->actions().indexOf(action) >= mRecentProjectInitialActionCount)
				mEditor->loadProject(action->text());
		}
		

		void MainWindow::onAppCreated() {
			showGame();
			
		}

		void MainWindow::onAppInitialized() {
			hideGame();

			ui->actionInit->setEnabled(false);
			ui->actionFinalize->setEnabled(true);
			ui->actionStart->setEnabled(true);
			ui->actionPause->setEnabled(false);
			ui->actionStop->setEnabled(false);
		}

		void MainWindow::onAppStopped()
		{
			hideGame();

			ui->actionInit->setEnabled(false);
			ui->actionFinalize->setEnabled(true);
			ui->actionStart->setEnabled(true);
			ui->actionPause->setEnabled(false);
			ui->actionStop->setEnabled(false);
		}

		void MainWindow::onAppShutdown()
		{
			ui->game->setCurrentIndex(0);

			ui->actionInit->setEnabled(true);
			ui->actionFinalize->setEnabled(false);
			ui->actionStart->setEnabled(false);
			ui->actionPause->setEnabled(false);
			ui->actionStop->setEnabled(false);
		}

		void MainWindow::onProjectOpened(Model::Project *project) {
			ui->ProjectWidget->setModel(project);
			ui->MediaWidget->setModel(project->getMedia());
			connect(ui->MediaWidget, &QTreeView::doubleClicked, project, &Model::Project::mediaDoubleClicked);

			ui->actionInit->setEnabled(true);
			ui->actionFinalize->setEnabled(false);
			ui->actionStart->setEnabled(false);
			ui->actionPause->setEnabled(false);
			ui->actionStop->setEnabled(false);

			connect(&mDialogManager, &Dialogs::DialogManager::deleteClassDialogAccepted, project, &Model::Project::deleteClass);
		}

		void MainWindow::onAppStarted() {
			showGame();

			ui->actionInit->setEnabled(false); 
			ui->actionFinalize->setEnabled(true);
			ui->actionStart->setEnabled(false);
			ui->actionPause->setEnabled(true);
			ui->actionStop->setEnabled(true);			
		}


		

	}

}

