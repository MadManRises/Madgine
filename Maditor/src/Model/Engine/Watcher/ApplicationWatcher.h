#pragma once



namespace Maditor {
	namespace Model {
		namespace Watcher {


			class ApplicationWatcher : public QObject, public Ogre::FrameListener{
				Q_OBJECT

			public:
				ApplicationWatcher(ModuleLoader *loader, OgreLogWatcher *madgineLog);
				~ApplicationWatcher();

				void notifyApplicationCreated(const QString &root);
				void notifyApplicationShutdown();
				void afterApplicationShutdown();
				void notifyApplicationStarted();
				void notifyApplicationStopped();
				void notifyApplicationInitialized();

				ResourceWatcher *resourceWatcher();
				PerformanceWatcher *performanceWatcher();
				OgreSceneWatcher *ogreSceneWatcher();
				ObjectsWatcher *objectsWatcher();

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
				void applicationShutdown();

			protected:
				virtual bool frameRenderingQueued(const Ogre::FrameEvent &ev) override;

			private:				
				
				ModuleLoader *mModuleLoader;

				ResourceWatcher *mResourceWatcher;				
				PerformanceWatcher *mPerformanceWatcher;
				OgreSceneWatcher *mOgreSceneWatcher;
				ObjectsWatcher *mObjectsWatcher;

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