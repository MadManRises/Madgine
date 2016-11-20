#include "madgineinclude.h"

#include "ApplicationWatcher.h"

#include "Model\Engine\ModuleLoader.h"
#include "OgreSceneWatcher.h"
#include "LogWatcher.h"
#include "ResourceWatcher.h"


namespace Maditor {
	namespace Model {
		namespace Watcher {

			ApplicationWatcher::ApplicationWatcher(OgreLogWatcher *madgineLog) :
				mMadgineLog(madgineLog),
				mResourceWatcher(0),
				mOgreSceneWatcher(0)
			{



			}

			ApplicationWatcher::~ApplicationWatcher()
			{
				if (mResourceWatcher)
					delete mResourceWatcher;
				if (mOgreSceneWatcher)
					delete mOgreSceneWatcher;
			}

			void ApplicationWatcher::notifyApplicationCreated()
			{
				

				/*Ogre::Root::getSingleton().addFrameListener(this);
				
				mMadgineLog->listen(Ogre::LogManager::getSingleton().getLog("Madgine.log"), root);
				mOgreLog = new OgreLogWatcher(OgreLogWatcher::TextLog);
				mOgreLog->listen(Ogre::LogManager::getSingleton().getLog("Ogre.log"), root);
				emit logCreated(mOgreLog);

				mGuiRenderWindow = Engine::App::Application::getSingleton().renderWindow();
				mGuiRenderStats = &mGuiRenderWindow->getStatistics();
				emit renderStatsSetup("Gui", mGuiRenderStats);
				*/
				//mResourceWatcher.setup();

				emit applicationCreated();

				//_resizeWindow();
			}

			void ApplicationWatcher::notifyApplicationShutdown()
			{

				mPerformanceWatcher.clear();
				mResourceWatcher->clear();

				emit applicationShutdown();
			}

			void ApplicationWatcher::afterApplicationShutdown()
			{
				//emit logRemoved(mOgreLog);
				//mOgreLog->stopListening(false);
				//delete mOgreLog;
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
				//mResourceWatcher->init();

				emit applicationInitialized();
			}

			ResourceWatcher *ApplicationWatcher::resourceWatcher()
			{
				return mResourceWatcher;
			}

			PerformanceWatcher *ApplicationWatcher::performanceWatcher()
			{
				return &mPerformanceWatcher;
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
				return &mObjectsWatcher;
			}


			void ApplicationWatcher::init()
			{
				mResourceWatcher = new ResourceWatcher;
				mOgreSceneWatcher = new OgreSceneWatcher;
			}

			void ApplicationWatcher::update() {
				mMadgineLog->update();
			}

		}
	}
}


