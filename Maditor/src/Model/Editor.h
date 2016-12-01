#pragma once

#include "Project\Project.h"
#include "../Common\Shared.h"

namespace Maditor {
	namespace Model {
		class MADITOR_EXPORT Editor : public QObject {
			Q_OBJECT
		public:
			Editor(Addons::AddonCollector *collector = 0);
			~Editor();

			void onStartup();

			void newProject(const QString &path, const QString &name);
			void loadProject(const QString &path);

			ApplicationWrapper *application();
			Watcher::ApplicationWatcher *watcher();
			Generators::ClassGeneratorFactory *classGeneratorFactory();
			Editors::EditorManager *editorManager();
			LogsModel *logsModel();
			
			Project *project();

			void clearRecentProjects();

		private:
			void openProject(std::unique_ptr<Project> &&project);

		signals:
			void projectOpened(Project *);
			void recentProjectsChanged(const QStringList &list);

		private:

			SharedMemory mMemory;

			Watcher::OgreLogWatcher *mLog;

			ModuleLoader *mLoader;

			Watcher::ApplicationWatcher *mApplicationWatcher;
			ApplicationWrapper *mApplicationWrapper;
			
			Addons::AddonCollector *mAddonCollector;

			std::unique_ptr<Project> mProject;

			Generators::ClassGeneratorFactory *mClassGeneratorFactory;
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