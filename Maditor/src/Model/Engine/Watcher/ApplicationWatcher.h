#pragma once

#include "ApplicationInfo.h"

#include "StatsInfo.h"

#include "PerformanceWatcher.h"
#include "ObjectsWatcher.h"

#include "Model\Network\MadgineNetworkClient.h"

namespace Maditor {
	namespace Model {
		namespace Watcher {


			class ApplicationWatcher : public QObject{
				Q_OBJECT

			public:
				ApplicationWatcher(OgreLogWatcher *madgineLog);
				~ApplicationWatcher();

				void notifyApplicationCreated();
				void notifyApplicationShutdown();				
				void notifyApplicationStarted();
				void notifyApplicationStopped();
				void notifyApplicationInitialized();

				ResourceWatcher *resourceWatcher();
				PerformanceWatcher *performanceWatcher();
				OgreSceneWatcher *ogreSceneWatcher();
				ObjectsWatcher *objectsWatcher();

				void update();

				void init();

				Network::MadgineNetworkClient *client();

			signals:
				void logCreated(OgreLogWatcher *log);
				void logRemoved(OgreLogWatcher *log);
				void renderStatsSetup(const QString &name, const FrameStats &stats);
				void applicationCreated();
				void applicationInitialized();
				void applicationStarted();
				void applicationStopped();
				void applicationShutdown();


			private:				

				ResourceWatcher *mResourceWatcher;				
				PerformanceWatcher mPerformanceWatcher;
				OgreSceneWatcher *mOgreSceneWatcher;
				ObjectsWatcher mObjectsWatcher;


				OgreLogWatcher *mMadgineLog;

				Network::MadgineNetworkClient mClient;

			};

		}
	}
}