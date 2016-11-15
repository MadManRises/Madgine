#include "madgineinclude.h"

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


namespace Maditor {
	namespace View {

		MainWindow::MainWindow(Model::Editor *editor, QWidget *parent) :
			QMainWindow(parent),
			ui(new Ui::MainWindow),
			mEditor(editor)
		{
			ui->setupUi(this);

			
			updateRecentProjects(mEditor->recentProjects());
			

			mTarget = new OgreWindow(editor->application(), editor->watcher());
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

			setupConnections();

			
		}

		MainWindow::~MainWindow()
		{
			delete ui;
		}

		void MainWindow::startup()
		{
			QSettings &settings = mEditor->settings();

			restoreGeometry(settings.value("geometry").toByteArray());
			restoreState(settings.value("state").toByteArray(), 0);

			

			mEditor->onStartup();
		}


		void MainWindow::closeEvent(QCloseEvent *event)
		{

			QSettings &settings = mEditor->settings();
			settings.setValue("geometry", saveGeometry());
			settings.setValue("state", saveState(0));

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
			connect(this, &MainWindow::settingsRequest, &mDialogManager, &Dialogs::DialogManager::showSettingsDialog);

			connect(&mDialogManager, &Dialogs::DialogManager::newProjectDialogAccepted, mEditor, &Model::Editor::newProject);
			connect(&mDialogManager, &Dialogs::DialogManager::loadProjectDialogAccepted, mEditor, &Model::Editor::loadProject);
			Model::Generator::ClassGeneratorFactory *factory = mEditor->classGeneratorFactory();
			connect(&mDialogManager, &Dialogs::DialogManager::newClassDialogAccepted, factory, &Model::Generator::ClassGeneratorFactory::createClass);

			connect(factory, &Model::Generator::ClassGeneratorFactory::newEntityComponentRequest, &mDialogManager, &Dialogs::DialogManager::showNewEntityComponentDialog);
			connect(factory, &Model::Generator::ClassGeneratorFactory::newGameHandlerRequest, &mDialogManager, &Dialogs::DialogManager::showNewGameHandlerDialog);
			connect(factory, &Model::Generator::ClassGeneratorFactory::newGlobalAPIRequest, &mDialogManager, &Dialogs::DialogManager::showNewGlobalAPIDialog);
			connect(factory, &Model::Generator::ClassGeneratorFactory::newGuiHandlerRequest, &mDialogManager, &Dialogs::DialogManager::showNewGuiHandlerDialog);
			connect(factory, &Model::Generator::ClassGeneratorFactory::newOtherClassRequest, &mDialogManager, &Dialogs::DialogManager::showNewOtherClassDialog);
			connect(factory, &Model::Generator::ClassGeneratorFactory::newSceneComponentRequest, &mDialogManager, &Dialogs::DialogManager::showNewSceneComponentDialog);
			connect(&mDialogManager, &Dialogs::DialogManager::newEntityComponentDialogAccepted, factory, &Model::Generator::ClassGeneratorFactory::createEntityComponent);
			connect(&mDialogManager, &Dialogs::DialogManager::newGameHandlerDialogAccepted, factory, &Model::Generator::ClassGeneratorFactory::createGameHandler);
			connect(&mDialogManager, &Dialogs::DialogManager::newGlobalAPIDialogAccepted, factory, &Model::Generator::ClassGeneratorFactory::createGlobalAPI);
			connect(&mDialogManager, &Dialogs::DialogManager::newGuiHandlerDialogAccepted, factory, &Model::Generator::ClassGeneratorFactory::createGuiHandler);
			connect(&mDialogManager, &Dialogs::DialogManager::newOtherClassDialogAccepted, factory, &Model::Generator::ClassGeneratorFactory::createOtherClass);
			connect(&mDialogManager, &Dialogs::DialogManager::newSceneComponentDialogAccepted, factory, &Model::Generator::ClassGeneratorFactory::createSceneComponent);
								
		}


		void MainWindow::openSettings()
		{
			emit settingsRequest(mEditor);
		}

		void MainWindow::initApp()
		{
			ui->actionInit->setEnabled(false);
			mEditor->application()->load(mEditor->project(), mTarget);			
		}

		void MainWindow::startApp()
		{
			ui->actionStart->setEnabled(false);
			mEditor->application()->go();
		}

		void MainWindow::stopApp()
		{
			mEditor->application()->shutdown();
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
			mEditor->application()->cleanup();
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
			menu->clear();

			for (const QString &project : list) {
				menu->addAction(project);
			}
		}

		void MainWindow::recentProjectClicked(QAction * action)
		{
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

