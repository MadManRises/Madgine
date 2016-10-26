#include "maditorinclude.h"

#include "Editor.h"
#include "Logsmodel.h"
#include "Engine/Watcher/ApplicationWatcher.h"
#include "Engine/ApplicationWrapper.h"
#include "Editors\VSLink.h"
#include "Project\Generator\ClassGeneratorFactory.h"
#include "Editors\ScriptEditorModel.h"
#include "Project\Generator\CommandLine.h"
#include "Project\ProjectLog.h"
#include "Engine\ModuleLoader.h"
#include "Engine\Watcher\ResourceWatcher.h"
#include "Engine/Watcher/LogWatcher.h"

namespace Maditor {
	namespace Model {

		Editor::Editor() :
			mApplicationWrapper(0),
			mApplicationWatcher(0),
			mVS(0),
			mScriptEditor(0),
			mOgreTarget(0),
			mSettings("MadMan Studios", "Maditor") {

			mRecentProjects = mSettings.value("recentProjects").toStringList();
			mReloadProject = mSettings.value("reloadProject").toBool();

			mLoader = new ModuleLoader;
			mApplicationWatcher = new Watcher::ApplicationWatcher(mLoader);
			mApplicationWrapper = new ApplicationWrapper(mApplicationWatcher, mLoader);

		}

		Editor::~Editor()
		{
			mSettings.setValue("recentProjects", mRecentProjects);
			mSettings.setValue("reloadProject", mReloadProject);

			delete mApplicationWrapper;
			delete mApplicationWatcher;
			delete mLoader;			
			delete mLogs;
		}

		void Editor::init(QWindow * target)
		{
			mOgreTarget = target;

			mVS = new Editors::VSLink;
			mScriptEditor = new Editors::ScriptEditorModel;
			mClassGeneratorFactory = new Generator::ClassGeneratorFactory;
			mLogs = new LogsModel;

			mApplicationWatcher->init();

			connect(mApplicationWatcher, &Watcher::ApplicationWatcher::logCreated, mLogs, &LogsModel::addLog);
			connect(mApplicationWatcher, &Watcher::ApplicationWatcher::logRemoved, mLogs, &LogsModel::removeLog);
			connect(mLogs, &LogsModel::openScriptFile, mScriptEditor, &Editors::ScriptEditorModel::openScriptFile);
			connect(mApplicationWatcher->resourceWatcher(), &Watcher::ResourceWatcher::openScriptFile, mScriptEditor, &Editors::ScriptEditorModel::openScriptFile);


			ProjectLog *defaultLog = new ProjectLog;

			mLogs->addLog(defaultLog);
			Generator::CommandLine::setLog(defaultLog);



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

		Editors::ScriptEditorModel *Editor::scriptEditor()
		{
			return mScriptEditor;
		}

		Editors::VSLink *Editor::vs()
		{
			return mVS;
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

			

			//mApplicationWrapper.go();
		}

		
	}
}