#pragma once

#include "project/project.h"
#include "maditorlog.h"
#include "logs/logtablemodel.h"
#include "Madgine/signalslot/connectionstore.h"

namespace Maditor {
	namespace Model {

		class MADITOR_MODEL_EXPORT Maditor : public QObject {
			Q_OBJECT

		public:
			Maditor();
			virtual ~Maditor();

			void init();

			void newProject(const QDir &path, const QString &name);
			void loadProject(const QDir &path);
			

			void clearRecentProjects();


			QSettings &settings();

			const QStringList &recentProjects();
			bool reloadProjectProperty();
			void setReloadProjectProperty(bool b);
			
			Project *project();
			LogTableModel *logs();

			Addons::AddonCollector *addons();

		protected:
			virtual void timerEvent(QTimerEvent *e) override;

		public slots:
			void loadProject();
			void newProject();
			void closeProject();


		signals:
			void projectOpened(Project *project);
			void projectClosed(Project *project);
			void recentProjectsChanged(const QStringList &list);


		private:
			void openProject(std::unique_ptr<Project>&& project);

		private:
			QStringList mRecentProjects;
			bool mReloadProject;

			QSettings mSettings;

			std::unique_ptr<Addons::AddonCollector> mAddons;

			
			MaditorLog mLog;
			LogTableModel mLogs;
			
			std::unique_ptr<Project> mProject;

			Engine::SignalSlot::ConnectionManager mConnMgr;

		};

	}
}
