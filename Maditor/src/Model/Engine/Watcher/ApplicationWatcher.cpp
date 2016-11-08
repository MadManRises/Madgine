#include "madgineinclude.h"

#include "ApplicationWatcher.h"

#include "Model\Engine\ModuleLoader.h"
#include "OgreSceneWatcher.h"
#include "LogWatcher.h"
#include "ResourceWatcher.h"
#include "PerformanceWatcher.h"
#include "ObjectsWatcher.h"

namespace Maditor {
	namespace Model {
		namespace Watcher {

			ApplicationWatcher::ApplicationWatcher(ModuleLoader *loader, OgreLogWatcher *madgineLog) :
				mGuiRenderWindow(0),
				mSceneRenderWindow(0),
				mModuleLoader(loader),
				mResizePending(false),
				mMadgineLog(madgineLog),
				mResourceWatcher(0),
				mPerformanceWatcher(0),
				mOgreSceneWatcher(0),
				mObjectsWatcher(0)
			{



			}

			ApplicationWatcher::~ApplicationWatcher()
			{
				if (mResourceWatcher)
					delete mResourceWatcher;
				if (mPerformanceWatcher)
					delete mPerformanceWatcher;
				if (mOgreSceneWatcher)
					delete mOgreSceneWatcher;
				if (mObjectsWatcher)
					delete mObjectsWatcher;
			}

			void ApplicationWatcher::notifyApplicationCreated(const QString &root)
			{
				mProjectRoot = root;

				Ogre::Root::getSingleton().addFrameListener(this);
				
				mMadgineLog->listen(Ogre::LogManager::getSingleton().getLog("Madgine.log"), root);
				mOgreLog = new OgreLogWatcher(OgreLogWatcher::TextLog);
				mOgreLog->listen(Ogre::LogManager::getSingleton().getLog("Ogre.log"), root);
				emit logCreated(mOgreLog);

				mGuiRenderWindow = Engine::App::Application::getSingleton().renderWindow();
				mGuiRenderStats = &mGuiRenderWindow->getStatistics();
				emit renderStatsSetup("Gui", mGuiRenderStats);

				//mResourceWatcher.setup();

				emit applicationCreated();

				//_resizeWindow();
			}

			void ApplicationWatcher::notifyApplicationShutdown()
			{
				Ogre::Root::getSingleton().removeFrameListener(this);	

				mPerformanceWatcher->clear();
				mResourceWatcher->clear();

				emit applicationShutdown();
			}

			void ApplicationWatcher::afterApplicationShutdown()
			{
				mMadgineLog->stopListening(false);
				emit logRemoved(mOgreLog);
				mOgreLog->stopListening(false);
				delete mOgreLog;
			}

			void ApplicationWatcher::notifyApplicationStarted()
			{
				emit applicationStarted();
			}

			void ApplicationWatcher::notifyApplicationStopped()
			{
				mOgreSceneWatcher->clear();
				emit applicationStopped();
			}

			void ApplicationWatcher::notifyApplicationInitialized()
			{
				//Ogre::RenderTarget *target = Engine::Scene::SceneManager::getSingleton().getRenderTarget();
				mResourceWatcher->init();

				emit applicationInitialized();
			}

			ResourceWatcher *ApplicationWatcher::resourceWatcher()
			{
				return mResourceWatcher;
			}

			PerformanceWatcher *ApplicationWatcher::performanceWatcher()
			{
				return mPerformanceWatcher;
			}

			/*LogsWatcher * ApplicationWatcher::logsWatcher()
			{
				return mLogsWatcher;
			}*/

			OgreSceneWatcher * ApplicationWatcher::ogreSceneWatcher()
			{
				return mOgreSceneWatcher;
			}

			ObjectsWatcher * ApplicationWatcher::objectsWatcher()
			{
				return mObjectsWatcher;
			}

			void ApplicationWatcher::resizeWindow()
			{
				mResizePending = true;
			}

			void ApplicationWatcher::init()
			{
				mResourceWatcher = new ResourceWatcher;
				mPerformanceWatcher = new PerformanceWatcher;
				mOgreSceneWatcher = new OgreSceneWatcher;
				mObjectsWatcher = new ObjectsWatcher;
			}

			bool ApplicationWatcher::frameRenderingQueued(const Ogre::FrameEvent & ev)
			{

				mPerformanceWatcher->update();
				mModuleLoader->update();
				mOgreSceneWatcher->update();

				if (mResizePending) {
					mResizePending = false;
					Engine::App::Application::getSingleton().resizeWindow();
				}

				return true;
			}

		}
	}
}


