

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "OGRE\scenecomponent.h"

#include "UI\UIManager.h"

#include "Util\Profiler.h"

#include "OGRE\scenemanager.h"

#include "OGRE\Entity\entity.h"

#include "qevent.h"

#include "OgreWidgets/OgreWindow.h"

#include "Model/Editor.h"

#include "Model/Engine\ApplicationWrapper.h"

#include "Model/Engine\Watcher/ApplicationWatcher.h"

#include "Model\Editors\ScriptEditorModel.h"

#include "Model\Editors\VSLink.h"

#include <qsettings.h>

#include "Model\Engine\Watcher\ResourceWatcher.h"
#include "Model\Engine\Watcher\PerformanceWatcher.h"
#include "Model\Engine\Watcher\OgreSceneWatcher.h"



namespace Maditor {
	namespace View {

		MainWindow::MainWindow(Model::Editor *editor, QWidget *parent) :
			QMainWindow(parent),
			ui(new Ui::MainWindow),
			mEditor(editor)
		{
			ui->setupUi(this);

			QSettings &settings = editor->settings();

			restoreGeometry(settings.value("geometry").toByteArray());
			restoreState(settings.value("state").toByteArray(), 0);

			updateRecentProjects(editor->recentProjects());

			mTarget = new OgreWindow(editor->application(), editor->watcher());
			QWidget *ogre = QWidget::createWindowContainer(mTarget);

			ui->game->addWidget(ogre);


			Model::Watcher::ApplicationWatcher *watcher = editor->watcher();
			ui->LogsWidget->setModel(watcher->logsWatcher());
			//ui->guiRenderStats->connectWatchers("Gui", watcher);
			//ui->sceneRenderStats->connectWatchers("Scene", watcher);
			ui->ResourcesWidget->setModel(watcher->resourceWatcher());
			ui->scriptEditorTabWidget->setModel(editor->scriptEditor());
			ui->PerformanceWidget->setModel(watcher->performanceWatcher());
			ui->OgreSceneWidget->setModel(watcher->ogreSceneWatcher());

			setupConnections();

			editor->init(mTarget);
		}

		MainWindow::~MainWindow()
		{
			delete ui;
		}


		void MainWindow::setupScene()
		{
			ui->sceneHierarchy->setHeaderLabels({ "Node", "Position" });

			mSceneRoot = new QTreeWidgetItem({ "root" });
			ui->sceneHierarchy->addTopLevelItem(mSceneRoot);

			Ogre::SceneNode *root = Engine::OGRE::SceneManager::getSingleton().getSceneManager()->getRootSceneNode();

			mEntitiesNode = new QTreeWidgetItem(mSceneRoot, { "Entities" });

			mTerrain = new QTreeWidgetItem(mSceneRoot, { "Terrain" });
			Ogre::SceneNode *terrain = static_cast<Ogre::SceneNode*>(root->getChild("Terrain"));
			assert(terrain);
			buildSceneHierarchy(terrain, mTerrain);


		}

		void MainWindow::refresh()
		{

			Engine::Util::Profiler &profiler = Engine::Util::Profiler::getSingleton();
			fillData(profiler, mMainLoop, "Frame", profiler.getStats("Frame")->averageDuration());

			for (const std::pair<QTreeWidgetItem* const, Engine::OGRE::BaseSceneComponent *> &comp : mComponents) {
				comp.first->setCheckState(0, (comp.second->isEnabled() ? Qt::Checked : Qt::Unchecked));
			}

			for (const std::pair<QTreeWidgetItem* const, Engine::OGRE::Entity::Entity*> &ent : mEntities) {
				ent.first->setData(1, Qt::DisplayRole, QVariant(QString::fromStdString(Ogre::StringConverter::toString(ent.second->getPosition()))));
			}
		}

		void MainWindow::addEntity(Engine::OGRE::Entity::Entity * e)
		{
			QTreeWidgetItem *item = new QTreeWidgetItem(mEntitiesNode, { e->getName().c_str() });
			buildSceneHierarchy(e->getNode(), item);

			mEntities[item] = e;
		}

		void MainWindow::fillData(Engine::Util::Profiler &profiler, QTreeWidgetItem * item, const std::string & name, long long frameDuration, const std::string & parentName)
		{
			/*long long duration = profiler.averageDuration(name);

			item->setData(1, Qt::DisplayRole, QVariant(duration));
			if (!parentName.empty())
				item->setData(2, Qt::DisplayRole, QVariant(((10000 * duration) / profiler.averageDuration(parentName)) / 100.0f));
			item->setData(3, Qt::DisplayRole, QVariant(((10000 * duration) / frameDuration) / 100.0f));

			int index = 0;
			for (const std::string &child : Engine::Util::Profiler::getSingleton().children(name)) {

				if (item->childCount() > index && item->child(index)->text(0) == QString::fromStdString(child)) {
					fillData(profiler, item->child(index), child, frameDuration, name);
					++index;
				}
				else
				{
					QTreeWidgetItem *item2 = new QTreeWidgetItem(item, { QString::fromStdString(child) });
					fillData(profiler, item2, child, frameDuration, name);

					if (name == "SceneComponents") {
						const auto &components = Engine::OGRE::SceneManager::getSingleton().getComponents();
						auto it = std::find_if(components.begin(), components.end(), [&](Engine::OGRE::BaseSceneComponent *comp) {return child == comp->componentName(); });
						mComponents[item2] = *it;
						item2->setCheckState(0, ((*it)->isEnabled() ? Qt::Checked : Qt::Unchecked));
					}
				}
			}*/
		}

		void MainWindow::handleChanges(QTreeWidgetItem *item, int column)
		{
			if (column == 0) {
				if (item->parent()->text(0) == "SceneComponents") {
					auto it = mComponents.find(item);
					it->second->setEnabled(item->checkState(0) == Qt::Checked);
				}
			}
		}

		void MainWindow::buildSceneHierarchy(Ogre::SceneNode * node, QTreeWidgetItem * item)
		{
			for (const std::pair<Ogre::String, Ogre::Node*> &p : node->getChildIterator()) {
				Ogre::SceneNode *child = dynamic_cast<Ogre::SceneNode*>(p.second);
				assert(child);
				buildSceneHierarchy(child, new QTreeWidgetItem(item, { p.first.c_str() }));
			}
			for (const std::pair<Ogre::String, Ogre::MovableObject*> &p : node->getAttachedObjectIterator()) {
				new QTreeWidgetItem(item, { (p.first + "(Object)").c_str() });
				Engine::OGRE::Entity::Entity *e = Engine::OGRE::Entity::Entity::entityFromMovable(p.second);
			}
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
			connect(mEditor->scriptEditor(), &Model::Editors::ScriptEditorModel::showDoc, this, &MainWindow::ensureVisible);

			connect(ui->actionOpenVS, &QAction::triggered, mEditor->vs(), &Model::Editors::VSLink::openVS);

			//Watcher-related
			connect(mEditor->watcher(), &Model::Watcher::ApplicationWatcher::applicationCreated, this, &MainWindow::showGame, Qt::QueuedConnection);
			connect(mEditor->watcher(), &Model::Watcher::ApplicationWatcher::applicationInitialized, this, &MainWindow::hideGame, Qt::QueuedConnection);
			connect(mEditor->watcher(), &Model::Watcher::ApplicationWatcher::applicationStarted, this, &MainWindow::onAppStarted, Qt::QueuedConnection);
			connect(mEditor->watcher(), &Model::Watcher::ApplicationWatcher::applicationStopped, this, &MainWindow::onAppStopped, Qt::QueuedConnection);

			connect(mEditor->watcher(), &Model::Watcher::ApplicationWatcher::applicationInitialized, mTarget, &OgreWindow::setInput);

			connect(ui->ResourcesWidget, &QTreeView::doubleClicked, mEditor->watcher()->resourceWatcher(), &Model::Watcher::ResourceWatcher::itemDoubleClicked);

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

		void MainWindow::startApp()
		{
			mEditor->application()->go();
		}

		void MainWindow::stopApp()
		{
			mEditor->application()->shutdown();
		}

		void MainWindow::pauseApp()
		{
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



		void MainWindow::onAppStopped()
		{
			hideGame();

			ui->actionStart->setEnabled(true);
			ui->actionPause->setEnabled(false);
			ui->actionStop->setEnabled(false);
		}

		void MainWindow::onProjectOpened(Model::Project *project) {
			ui->ProjectWidget->setProject(project);

			ui->actionStart->setEnabled(true);
		}

		void MainWindow::onAppStarted() {
			showGame();

			ui->actionStart->setEnabled(false);
			ui->actionPause->setEnabled(true);
			ui->actionStop->setEnabled(true);
		}

		

	}

}

