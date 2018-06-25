#include "maditormodellib.h"

#include "maditor.h"

#include "dialogmanager.h"

#include "addons/addon.h"

#include "Madgine/util/utilmethods.h"
#include "project/xmlexception.h"

namespace Maditor {
	namespace Model {

		DialogManager *DialogManager::sSingleton = 0;

		Maditor::Maditor() :
			mSettings(QSettings::IniFormat, QSettings::UserScope, "MadMan Studios", "Maditor"),
			mAddons(std::make_unique<Addons::AddonCollector>(this)),
			mLog(&mLogs)
		{

			mSettings.beginGroup("Editor");
			mRecentProjects = mSettings.value("recentProjects").toStringList();
			mReloadProject = mSettings.value("reloadProject").toBool();
			mSettings.endGroup();

			connect(this, &Maditor::projectOpened, mAddons.get(), &Addons::AddonCollector::onProjectOpened);

			Engine::Util::UtilMethods::setup(&mLog);

		}



		Maditor::~Maditor()
		{
			mSettings.beginGroup("Editor");
			mSettings.setValue("recentProjects", mRecentProjects);
			mSettings.setValue("reloadProject", mReloadProject);
			mSettings.endGroup();

		}

		void Maditor::init()
		{
			if (mReloadProject && !mRecentProjects.isEmpty()) {
				loadProject(mRecentProjects.front());
			}

			startTimer(100);
		}

		void Maditor::newProject(const QDir &path, const QString &name)
		{

			closeProject();
			path.mkdir(name);
			openProject(std::make_unique<Project>(&mLogs, path.filePath(name), name));

		}

		void Maditor::loadProject(const QDir & path)
		{

			closeProject();
			try {
				openProject(std::unique_ptr<Project>(Project::load(&mLogs, path)));
			} catch(const XmlException &e)
			{
				DialogManager::showErrorStatic("Xml Error", e.what());
			}

		}

		void Maditor::openProject(std::unique_ptr<Project> &&project)
		{
			if (!project->isValid()) return;

			mProject = std::forward<std::unique_ptr<Project>>(project);

			QString path = mProject->path().path();
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
			return mAddons.get();
		}

		void Maditor::newProject()
		{
			QString path, name;
			if (DialogManager::showNewProjectDialogStatic(path, name)) {
				newProject(path, name);
			}
		}

		void Maditor::closeProject()
		{
			if (mProject) {
				emit projectClosed(mProject.get());
				mProject.reset();
			}

		}

		void Maditor::loadProject()
		{
			QString path;
			if (DialogManager::showLoadProjectDialogStatic(path)) {
				loadProject(path);
			}
		}

		void Maditor::timerEvent(QTimerEvent * e)
		{
			mConnMgr.update();
		}

	}
}

