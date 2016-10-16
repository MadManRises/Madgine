#include "ApplicationWatcher.h"
#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <libinclude.h>
#include <OGRE/scenemanager.h>
#include <UI\UIManager.h>
#include <GUI/GUISystem.h>
#include <View/OgreWidgets/OgreWindow.h>
#include <OgreWindowEventUtilities.h>
#include "Model\Engine\ModuleLoader.h"
#include <qapplication.h>
#include "LogsWatcher.h"
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
				mResizePending(false),
				mLogsWatcher(0){

				mResourceWatcher = new ResourceWatcher;
				mPerformanceWatcher = new PerformanceWatcher;
				mLogsWatcher = new LogsWatcher;
				mOgreSceneWatcher = new OgreSceneWatcher;

			}

			ApplicationWatcher::~ApplicationWatcher()
			{
				delete mResourceWatcher;
				delete mPerformanceWatcher;
				delete mLogsWatcher;
				delete mOgreSceneWatcher;
			}

			void ApplicationWatcher::notifyApplicationCreated(const QString &root)
			{
				mProjectRoot = root;

				Ogre::Root::getSingleton().addFrameListener(this);
				
				mLogsWatcher->setup(root);

				mGuiRenderWindow = Engine::UI::UIManager::getSingleton().renderWindow();
				mGuiRenderStats = &mGuiRenderWindow->getStatistics();
				emit renderStatsSetup("Gui", mGuiRenderStats);

				mOgreSceneWatcher->setRoot(Engine::OGRE::SceneManager::getSingleton().getSceneManager()->getRootSceneNode());

				//mResourceWatcher.setup();

				emit applicationCreated();

				//_resizeWindow();
			}

			void ApplicationWatcher::notifyApplicationShutdown()
			{
				mLogsWatcher->cleanUp();
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
				//Ogre::RenderTarget *target = Engine::OGRE::SceneManager::getSingleton().getRenderTarget();
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

			LogsWatcher * ApplicationWatcher::logsWatcher()
			{
				return mLogsWatcher;
			}

			OgreSceneWatcher * ApplicationWatcher::ogreSceneWatcher()
			{
				return mOgreSceneWatcher;
			}

			void ApplicationWatcher::resizeWindow()
			{
				mResizePending = true;
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


