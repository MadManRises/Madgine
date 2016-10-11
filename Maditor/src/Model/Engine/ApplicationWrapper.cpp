#include "ApplicationWrapper.h"

#include "Model\Editor.h"

#include <libinclude.h>
#include <App\Application.h>
#include <App\appsettings.h>
#include <qapplication.h>
#include <qDebug>
#include <chrono>
#include <qwindow>

#include "InputWrapper.h"

#include "Watcher\ApplicationWatcher.h"

namespace Maditor {
	namespace Model {
		ApplicationWrapper::ApplicationWrapper(Watcher::ApplicationWatcher *watcher, ModuleLoader *loader) :
			mWork(true),
			mWorker(&ApplicationWrapper::workerLoop, this),
			mApplication(0),
			mSettings(0),
			mWatcher(watcher),
			mLoader(loader),
			mInput(0)
		{
			std::stringstream ss;
			ss << mWorker.get_id();
			qDebug() << QString::fromStdString(ss.str());
		}
		ApplicationWrapper::~ApplicationWrapper()
		{
			cleanup();
		}

		void ApplicationWrapper::load(Project *project, QWindow *target)
		{
			mSettings = new Engine::App::AppSettings;
			mSettings->mWindowName = "QtOgre";
			mSettings->mRootDir = (project->root() + "Data/").toStdString();
			Ogre::NameValuePairList parameters;

			/*
			Flag within the parameters set so that Ogre3D initializes an OpenGL context on it's own.
			*/
			parameters["currentGLContext"] = Ogre::String("false");

			/*
			We need to supply the low level OS window handle to this QWindow so that Ogre3D knows where to draw
			the scene. Below is a cross-platform method on how to do this.
			If you set both options (externalWindowHandle and parentWindowHandle) this code will work with OpenGL
			and DirectX.
			*/
#if defined(Q_OS_MAC) || defined(Q_OS_WIN)
			parameters["externalWindowHandle"] = Ogre::StringConverter::toString((size_t)(target->winId()));
			parameters["parentWindowHandle"] = Ogre::StringConverter::toString((size_t)(target->winId()));
#else
			parameters["externalWindowHandle"] = Ogre::StringConverter::toString((unsigned long)(this->winId()));
			parameters["parentWindowHandle"] = Ogre::StringConverter::toString((unsigned long)(this->winId()));
#endif

#if defined(Q_OS_MAC)
			parameters["macAPI"] = "cocoa";
			parameters["macAPICocoaUseNSView"] = "true";
#endif


			doTask([=] () {
				mLoader->setup(project->root() + "bin/", project->root() + "runtime/", project);
				mApplication = new Engine::App::Application;
				mInput = new Watcher::InputWrapper;
				mApplication->setupExternal(mSettings->mRootDir + "plugins.cfg", "QtOgre", target->width(), target->height(), parameters, mInput);
				mInput->setSystem(&Engine::GUI::GUISystem::getSingleton());
				mWatcher->notifyApplicationCreated(project->root());
				mApplication->init(*mSettings);
				mLoader->update();
				mWatcher->notifyApplicationInitialized();
			});
		}


		void ApplicationWrapper::go()
		{
			doTask([=]() {
				mWatcher->notifyApplicationStarted();
				mApplication->go();
				mWatcher->notifyApplicationStopped();
			});
		}

		void ApplicationWrapper::cleanup()
		{
			if (mApplication) {
				doTask([&]() {
					mInput = 0;
					mWatcher->notifyApplicationShutdown();
					delete mApplication;
					mApplication = 0;
					delete mSettings;
					mSettings = 0;
				});
				shutdown();
			}
			mWork = false;
			mWorker.join();
		}

		void ApplicationWrapper::shutdown()
		{			
			mApplication->shutdown();
		}

		Watcher::InputWrapper * ApplicationWrapper::input()
		{
			return mInput;
		}

		void ApplicationWrapper::workerLoop()
		{
			using namespace std::chrono_literals;
			while (mWork) {
				while (mWorkQueue.size() > 0) {
					mMutex.lock();
					std::function<void()> f = mWorkQueue.front();
					mWorkQueue.pop();
					mMutex.unlock();
					f();
				}
				mLoader->update();
				std::this_thread::sleep_for(250ms);				
			}
		}
		void ApplicationWrapper::doTask(std::function<void()> task)
		{
			mMutex.lock();
			mWorkQueue.push(task);
			mMutex.unlock();
		}
	}
}