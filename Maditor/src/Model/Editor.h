#pragma once

#include "Project\Project.h"

#include "editorForward.h"

#include <qsettings.h>

class QString;

namespace Maditor {
	namespace Model {
		class Editor : public QObject {
			Q_OBJECT
		public:
			Editor();
			~Editor();

			void newProject(const QString &path, const QString &name, QWindow *target);
			void loadProject(const QString &path, QWindow *target);

			ApplicationWrapper *application();
			Watcher::ApplicationWatcher *watcher();
			Generator::ClassGeneratorFactory *classGeneratorFactory();
			Editors::ScriptEditorModel *scriptEditor();
			Editors::VSLink *vs();
			Project *project();

			QSettings &settings();

			const QStringList &recentProjects();

		private:
			void openProject(std::unique_ptr<Project> &&project, QWindow *target);

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

			QSettings mSettings;

			QStringList mRecentProjects;

		};
	}
}