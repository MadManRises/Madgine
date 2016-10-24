#pragma once

#include <qevent.h>
#include <memory>
#include <OgreFrameListener.h>
#include <OgreRenderTarget.h>
#include <forward.h>
#include "LogWatcher.h"


namespace Maditor {

	namespace Model {

		class ModuleLoader;

		namespace Watcher {

			class ResourceWatcher;
			class PerformanceWatcher;
			class OgreSceneWatcher;

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
				//LogsWatcher *logsWatcher();
				OgreSceneWatcher *ogreSceneWatcher();

				void resizeWindow();

				void init();

			signals:
				void logCreated(OgreLogWatcher *log);
				void logRemoved(OgreLogWatcher *log);
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

				ResourceWatcher *mResourceWatcher;				
				PerformanceWatcher *mPerformanceWatcher;
				OgreSceneWatcher *mOgreSceneWatcher;

				Ogre::RenderTarget *mSceneRenderWindow;
				Ogre::RenderWindow *mGuiRenderWindow;
				const Ogre::RenderTarget::FrameStats *mSceneRenderStats, *mGuiRenderStats;

				OgreLogWatcher *mMadgineLog;
				OgreLogWatcher *mOgreLog;

				bool mResizePending;

				QString mProjectRoot;
			};

		}
	}
}