#pragma once

#include "Project\Project.h"

namespace Maditor {
	namespace Model {
		class MADITOR_EXPORT Editor : public QObject {
			Q_OBJECT
		public:
			Editor();
			~Editor();

			void init();
			void onStartup();

			void newProject(const QString &path, const QString &name);
			void loadProject(const QString &path);

			ApplicationWrapper *application();
			Watcher::ApplicationWatcher *watcher();
			Generator::ClassGeneratorFactory *classGeneratorFactory();
			Editors::EditorManager *editorManager();
			LogsModel *logsModel();
			
			Project *project();


		private:
			void openProject(std::unique_ptr<Project> &&project);

		signals:
			void projectOpened(Project *);
			void recentProjectsChanged(const QStringList &list);

		private:

			Watcher::OgreLogWatcher *mLog;

			ModuleLoader *mLoader;

			Watcher::ApplicationWatcher *mApplicationWatcher;
			ApplicationWrapper *mApplicationWrapper;
			

			std::unique_ptr<Project> mProject;

			Generator::ClassGeneratorFactory *mClassGeneratorFactory;
			Editors::EditorManager *mEditorManager;

			LogsModel *mLogs;

			//////////Properties

		public:
			QSettings &settings();

			const QStringList &recentProjects();
			bool reloadProjectProperty();
			void setReloadProjectProperty(bool b);

		private:

			QStringList mRecentProjects;
			bool mReloadProject;

			QSettings mSettings;

		};
	}
}