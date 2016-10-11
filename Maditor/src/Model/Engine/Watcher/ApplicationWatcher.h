#pragma once

#include <qevent.h>
#include "LogWatcher.h"
#include <memory>
#include <OgreFrameListener.h>
#include <OgreRenderTarget.h>
#include <forward.h>
#include "ResourceWatcher.h"
#include "PerformanceWatcher.h"


namespace Maditor {

	namespace Model {

		class ModuleLoader;

		namespace Watcher {

			class LogsWatcher;

			class ApplicationWatcher : public QObject, public Ogre::FrameListener{
				Q_OBJECT

			public:
				ApplicationWatcher(ModuleLoader *loader);
				~ApplicationWatcher();

				void notifyApplicationCreated(const QString &root);
				void notifyApplicationShutdown();
				void notifyApplicationStarted();
				void notifyApplicationStopped();
				void notifyApplicationInitialized();

				ResourceWatcher *resourceWatcher();
				PerformanceWatcher *performanceWatcher();
				LogsWatcher *logsWatcher();

				void resizeWindow();

			signals:
				void renderStatsSetup(const QString &name, const Ogre::RenderTarget::FrameStats *stats);
				void applicationCreated();
				void applicationInitialized();
				void applicationStarted();
				void applicationStopped();

			protected:
				virtual bool frameRenderingQueued(const Ogre::FrameEvent &ev) override;

			private:

				void _resizeWindow();

			private:
				
				
				
				ModuleLoader *mModuleLoader;



				LogsWatcher *mLogsWatcher;
				ResourceWatcher *mResourceWatcher;				
				PerformanceWatcher *mPerformanceWatcher;

				Ogre::RenderTarget *mSceneRenderWindow;
				Ogre::RenderWindow *mGuiRenderWindow;
				const Ogre::RenderTarget::FrameStats *mSceneRenderStats, *mGuiRenderStats;


				bool mResizePending;

				QString mProjectRoot;
			};

		}
	}
}