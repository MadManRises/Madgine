#pragma once


#include "Dialogs\DialogManager.h"


namespace Maditor {
	namespace View {

		namespace Ui {
			class MainWindow;
		}

		class MADITOR_EXPORT MainWindow : public QMainWindow
		{
			Q_OBJECT

		public:
			explicit MainWindow(Model::Editor *editor, QWidget *parent = 0);
			~MainWindow();

			void startup();

			/*void setupScene();

			void refresh();

			void addEntity(Engine::Scene::Entity::Entity *e);*/

		private:
			//void fillData(Engine::Util::Profiler &profiler, QTreeWidgetItem *item, const std::string &name, long long frameDuration, const std::string &parentName = "");

			//void handleChanges(QTreeWidgetItem *item, int column);

			//void buildSceneHierarchy(Ogre::SceneNode *node, QTreeWidgetItem *item);

			virtual void closeEvent(QCloseEvent * event) override;

			void setupConnections();

		signals:
			void settingsRequest(Model::Editor *editor);

		public slots:

			void openSettings();

			void initApp();
			void startApp();
			void stopApp();
			void pauseApp();
			void finalizeApp();

			void ensureVisible(QWidget *widget);

			void releaseProject();

		private slots:
			void onAppCreated();
			void onAppInitialized();
			void onAppStarted();
			void onAppStopped();
			void onAppShutdown();
			void onProjectOpened(Model::Project *project);
			void showGame();
			void hideGame();

			void updateRecentProjects(const QStringList &list);
			void recentProjectClicked(QAction *action);

		private:
			Ui::MainWindow *ui;

			Model::Editor *mEditor;

			OgreWindow *mTarget;

			Dialogs::DialogManager mDialogManager;


		};
	}
}

