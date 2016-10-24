#include "ApplicationWatcher.h"
#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <libinclude.h>
#include <Scene/scenemanager.h>
#include <UI\UIManager.h>
#include <GUI/GUISystem.h>
#include <View/OgreWidgets/OgreWindow.h>
#include <OgreWindowEventUtilities.h>
#include "Model\Engine\ModuleLoader.h"
#include <qapplication.h>
#include "OgreSceneWatcher.h"
#include "LogWatcher.h"
#include "ResourceWatcher.h"
#include "PerformanceWatcher.h"

namespace Maditor {
	namespace Model {
		namespace Watcher {

			ApplicationWatcher::ApplicationWatcher(ModuleLoader *loader) :
				mGuiRenderWindow(0),
				mSceneRenderWindow(0),
				mModuleLoader(loader),
				mResizePending(false){



			}

			ApplicationWatcher::~ApplicationWatcher()
			{
				delete mResourceWatcher;
				delete mPerformanceWatcher;
				delete mOgreSceneWatcher;
			}

			void ApplicationWatcher::notifyApplicationCreated(const QString &root)
			{
				mProjectRoot = root;

				Ogre::Root::getSingleton().addFrameListener(this);
				
				mMadgineLog = new OgreLogWatcher(Ogre::LogManager::getSingleton().getLog("Madgine.log"), OgreLogWatcher::GuiLog, root);
				emit logCreated(mMadgineLog);
				mOgreLog = new OgreLogWatcher(Ogre::LogManager::getSingleton().getLog("Ogre.log"), OgreLogWatcher::TextLog, root);
				emit logCreated(mOgreLog);

				mGuiRenderWindow = Engine::UI::UIManager::getSingleton().renderWindow();
				mGuiRenderStats = &mGuiRenderWindow->getStatistics();
				emit renderStatsSetup("Gui", mGuiRenderStats);

				//mResourceWatcher.setup();

				emit applicationCreated();

				//_resizeWindow();
			}

			void ApplicationWatcher::notifyApplicationShutdown()
			{
				emit logRemoved(mMadgineLog);
				delete mMadgineLog;
				emit logRemoved(mOgreLog);
				delete mOgreLog;
				Ogre::Root::getSingleton().removeFrameListener(this);				
			}

			void ApplicationWatcher::notifyApplicationStarted()
			{
				emit applicationStarted();
			}

			void ApplicationWatcher::notifyApplicationStopped()
			{
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

			void ApplicationWatcher::resizeWindow()
			{
				mResizePending = true;
			}

			void ApplicationWatcher::init()
			{
				mResourceWatcher = new ResourceWatcher;
				mPerformanceWatcher = new PerformanceWatcher;
				mOgreSceneWatcher = new OgreSceneWatcher;
			}

			bool ApplicationWatcher::frameRenderingQueued(const Ogre::FrameEvent & ev)
			{

				mPerformanceWatcher->update();
				mModuleLoader->update();
				mOgreSceneWatcher->update();

				if (mResizePending) {
					mResizePending = false;
					_resizeWindow();
				}

				return true;
			}


			void ApplicationWatcher::_resizeWindow()
			{
				if (mGuiRenderWindow) {
					mGuiRenderWindow->windowMovedOrResized();
					Ogre::WindowEventUtilities::WindowEventListeners::iterator index,
						start = Ogre::WindowEventUtilities::_msListeners.lower_bound(mGuiRenderWindow),
						end = Ogre::WindowEventUtilities::_msListeners.upper_bound(mGuiRenderWindow);
					for (index = start; index != end; ++index)
						(index->second)->windowResized(mGuiRenderWindow);
				}
			}

		}
	}
}


