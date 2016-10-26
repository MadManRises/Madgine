#pragma once

#include "Project\Project.h"

namespace Maditor {
	namespace Model {
		class Editor : public QObject {
			Q_OBJECT
		public:
			Editor();
			~Editor();

			void init(QWindow *target);
			void onStartup();

			void newProject(const QString &path, const QString &name);
			void loadProject(const QString &path);

			ApplicationWrapper *application();
			Watcher::ApplicationWatcher *watcher();
			Generator::ClassGeneratorFactory *classGeneratorFactory();
			Editors::ScriptEditorModel *scriptEditor();
			Editors::VSLink *vs();
			LogsModel *logsModel();
			
			Project *project();


		private:
			void openProject(std::unique_ptr<Project> &&project);

		signals:
			void projectOpened(Project *);
			void recentProjectsChanged(const QStringList &list);

		private:

			ModuleLoader *mLoader;

			Watcher::ApplicationWatcher *mApplicationWatcher;
			ApplicationWrapper *mApplicationWrapper;
			

			std::unique_ptr<Project> mProject;

			Generator::ClassGeneratorFactory *mClassGeneratorFactory;
			Editors::ScriptEditorModel *mScriptEditor;
			Editors::VSLink *mVS;
			LogsModel *mLogs;

			QWindow *mOgreTarget;


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