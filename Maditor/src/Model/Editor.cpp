#include "madgineinclude.h"

#include "Editor.h"
#include "Logsmodel.h"
#include "Engine/Watcher/ApplicationWatcher.h"
#include "Engine/ApplicationWrapper.h"
#include "Editors\EditorManager.h"
#include "Project\Generator\ClassGeneratorFactory.h"
#include "Project\Generator\CommandLine.h"
#include "Engine\ModuleLoader.h"
#include "Engine\Watcher\ResourceWatcher.h"
#include "Engine/Watcher/LogWatcher.h"
#include "Editors\ScriptEditorModel.h"
#include "Addons\Addon.h"

namespace Maditor {
	namespace Model {

		Editor::Editor(Addons::AddonCollector *collector) :
			mApplicationWrapper(0),
			mApplicationWatcher(0),
			mEditorManager(0),
			mSettings("MadMan Studios", "Maditor"),
			mLogs(0),
			mAddonCollector(collector)
		{

			mLog = new Watcher::OgreLogWatcher(Watcher::OgreLogWatcher::GuiLog, "Madgine.log");

			mRecentProjects = mSettings.value("recentProjects").toStringList();
			mReloadProject = mSettings.value("reloadProject").toBool();

			mLoader = new ModuleLoader;
			mApplicationWatcher = new Watcher::ApplicationWatcher(mLoader, mLog);
			mApplicationWrapper = new ApplicationWrapper(mApplicationWatcher, mLoader);

			mEditorManager = new Editors::EditorManager(mAddonCollector);
			mClassGeneratorFactory = new Generator::ClassGeneratorFactory;
			mLogs = new LogsModel;

			mApplicationWatcher->init();

			connect(mApplicationWatcher, &Watcher::ApplicationWatcher::logCreated, mLogs, &LogsModel::addLog);
			connect(mApplicationWatcher, &Watcher::ApplicationWatcher::logRemoved, mLogs, &LogsModel::removeLog);
			connect(mEditorManager->scriptEditor(), &Editors::ScriptEditorModel::documentSaved, mApplicationWatcher->resourceWatcher(), &Watcher::ResourceWatcher::reloadScriptFile);

			mLogs->addLog(mLog);
			Generator::CommandLine::setLog(mLog);
		}

		Editor::~Editor()
		{
			mSettings.setValue("recentProjects", mRecentProjects);
			mSettings.setValue("reloadProject", mReloadProject);

			delete mApplicationWrapper;
			delete mApplicationWatcher;
			delete mEditorManager;
			delete mLoader;		
			delete mLogs;

			delete mLog;
		}

		void Editor::onStartup()
		{

			if (mReloadProject && !mRecentProjects.isEmpty()) {
				loadProject(mRecentProjects.front());
			}
		}

		void Editor::newProject(const QString &path, const QString &name)
		{

			openProject(std::make_unique<Project>(path, name));

		}

		void Editor::loadProject(const QString & path)
		{

			openProject(std::unique_ptr<Project>(Project::load(path)));

		}



		ApplicationWrapper *Editor::application()
		{
			return mApplicationWrapper;
		}

		Watcher::ApplicationWatcher * Editor::watcher()
		{
			return mApplicationWatcher;
		}

		Generator::ClassGeneratorFactory *Editor::classGeneratorFactory()
		{
			return mClassGeneratorFactory;
		}

		Editors::EditorManager * Editor::editorManager()
		{
			return mEditorManager;
		}

		LogsModel * Editor::logsModel()
		{
			return mLogs;
		}

		Project *Editor::project()
		{
			return mProject.get();
		}


		QSettings & Editor::settings()
		{
			return mSettings;
		}

		const QStringList & Editor::recentProjects()
		{
			return mRecentProjects;
		}

		bool Editor::reloadProjectProperty()
		{
			return mReloadProject;
		}

		void Editor::setReloadProjectProperty(bool b)
		{
			mReloadProject = b;
		}

		void Editor::openProject(std::unique_ptr<Project> &&project)
		{
			if (!project->isValid()) return;

			mProject = std::forward<std::unique_ptr<Project>>(project);

			QString path = mProject->root();
			mRecentProjects.removeAll(path);
			mRecentProjects.push_front(path);

			emit recentProjectsChanged(mRecentProjects);


			emit projectOpened(mProject.get());

			mEditorManager->setCurrentRoot(mProject->root() + "src/");

			//mApplicationWrapper.go();
		}

		
	}
}