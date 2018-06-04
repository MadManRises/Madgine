#include "maditormodellib.h"

#include "maditor.h"

#include "dialogmanager.h"

#include "addons/addon.h"

#include "Madgine/util/utilmethods.h"

namespace Maditor {
	namespace Model {

		DialogManager *DialogManager::sSingleton = 0;

		Maditor::Maditor() :
			mSettings("MadMan Studios", "Maditor"),
			mAddons(new Addons::AddonCollector(this)),
			mLog(&mLogs)
		{

			mSettings.beginGroup("Editor");
			mRecentProjects = mSettings.value("recentProjects").toStringList();
			mReloadProject = mSettings.value("reloadProject").toBool();
			mSettings.endGroup();

			connect(this, &Maditor::projectOpened, mAddons, &Addons::AddonCollector::onProjectOpened);

			Engine::Util::UtilMethods::setup(&mLog);

			if (mReloadProject && !mRecentProjects.isEmpty()) {
				loadProject(mRecentProjects.front());
			}

			startTimer(100);
		}



		Maditor::~Maditor()
		{
			delete mAddons;
			mSettings.beginGroup("Editor");
			mSettings.setValue("recentProjects", mRecentProjects);
			mSettings.setValue("reloadProject", mReloadProject);
			mSettings.endGroup();

		}

		void Maditor::newProject(const QString &path, const QString &name)
		{

			openProject(std::make_unique<Project>(&mLogs, path, name));

		}

		void Maditor::loadProject(const QString & path)
		{

			openProject(std::unique_ptr<Project>(Project::load(&mLogs, path)));

		}

		void Maditor::openProject(std::unique_ptr<Project> &&project)
		{
			if (!project->isValid()) return;

			mProject = std::forward<std::unique_ptr<Project>>(project);

			QString path = mProject->path();
			mRecentProjects.removeAll(path);
			mRecentProjects.push_front(path);

			emit recentProjectsChanged(mRecentProjects);


			emit projectOpened(mProject.get());

			//mEditorManager->setCurrentRoot(mProject->moduleList()->path());
		}

		void Maditor::clearRecentProjects()
		{
			mRecentProjects.clear();
			emit recentProjectsChanged(mRecentProjects);
		}


		QSettings & Maditor::settings()
		{
			return mSettings;
		}

		const QStringList & Maditor::recentProjects()
		{
			return mRecentProjects;
		}

		bool Maditor::reloadProjectProperty()
		{
			return mReloadProject;
		}

		void Maditor::setReloadProjectProperty(bool b)
		{
			mReloadProject = b;
		}

		Project * Maditor::project()
		{
			return mProject.get();
		}

		LogTableModel * Maditor::logs()
		{
			return &mLogs;
		}

		Addons::AddonCollector * Maditor::addons()
		{
			return mAddons;
		}

		void Maditor::setDialogManager(DialogManager *manager) {
			mDialogManager = manager;
		}

		void Maditor::newProject()
		{
			QString path, name;
			if (mDialogManager->showNewProjectDialog(path, name)) {
				newProject(path, name);
			}
		}

		void Maditor::loadProject()
		{
			QString path;
			if (mDialogManager->showLoadProjectDialog(path)) {
				loadProject(path);
			}
		}

		void Maditor::timerEvent(QTimerEvent * e)
		{
			mConnMgr.update();
		}

	}
}

