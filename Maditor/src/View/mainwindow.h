#pragma once

#include <QMainWindow>
#include <qtreewidget.h>

#include "libinclude.h"

#include "Dialogs\DialogManager.h"

namespace Ui {
class MainWindow;
}

namespace Maditor {
	namespace Model {
		class Editor;
	}
	namespace View {

		class OgreWindow;
		class LogWatcher;

		class MainWindow : public QMainWindow
		{
			Q_OBJECT

		public:
			explicit MainWindow(Model::Editor *editor, QWidget *parent = 0);
			~MainWindow();


			void setupPerformanceWidget(const std::list<std::unique_ptr<Engine::OGRE::BaseSceneComponent>> &components);
			void setupScene();

			void refresh();

			void addEntity(Engine::OGRE::Entity::Entity *e);

		private:
			void fillData(Engine::Util::Profiler &profiler, QTreeWidgetItem *item, const std::string &name, long long frameDuration, const std::string &parentName = "");

			void handleChanges(QTreeWidgetItem *item, int column);

			void buildSceneHierarchy(Ogre::SceneNode *node, QTreeWidgetItem *item);

			virtual void closeEvent(QCloseEvent * event) override;

			void setupConnections();

		public slots:
			void newProject(const QString &path, const QString &name);
			void loadProject(const QString &path);

			void startApp();
			void stopApp();
			void pauseApp();

			void ensureVisible(QWidget *widget);

		private slots:
			void onAppStarted();
			void onAppStopped();
			void onProjectOpened(Model::Project *project);
			void showGame();
			void hideGame();

		private:
			Ui::MainWindow *ui;

			std::map<QTreeWidgetItem*, Engine::OGRE::BaseSceneComponent*> mComponents;

			QTreeWidgetItem *mMainLoop;

			QTreeWidgetItem *mSceneRoot, *mEntitiesNode, *mTerrain;

			std::map<QTreeWidgetItem*, Engine::OGRE::Entity::Entity*> mEntities;

			Model::Editor *mEditor;

			OgreWindow *mTarget;

			Dialogs::DialogManager mDialogManager;


		};
	}
}

