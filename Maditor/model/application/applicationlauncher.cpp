#include "maditormodellib.h"

#include "applicationlauncher.h"


#include "shared/sharedmemory.h"




#include "project/applicationconfig.h"

#include "project/project.h"

#include "Madgine/serialize/toplevelids.h"


namespace Maditor
{
	namespace Model
	{
		ApplicationLauncher::ApplicationLauncher(ApplicationConfig* config, const QString& uniqueName) :
			Document(uniqueName),
			mLoader(config),

			mLog(config->project()->logs(), std::list<std::string>{"Ogre.log"}),
			mWaitingForLoader(false),

			mConfig(config),
			mAboutToBeDestroyed(false),
			mRunning(false),
			mPong(false),
			mOnSetupResult(this),
			mSetup(false)
		{
			startTimer(10);

			mPingTimer.setSingleShot(true);

			connect(&mPingTimer, &QTimer::timeout, this, &ApplicationLauncher::timeout);

			setStaticSlaveId(Engine::Serialize::MADITOR);
		}

		ApplicationLauncher::~ApplicationLauncher()
		{
			//kill(Shared::KILL_CLEANUP);			
		}

		void ApplicationLauncher::destroy()
		{
			if (!isSetup())
			{
				Document::destroy();
			}
			else
			{
				if (isLauncher())
				{
					mAboutToBeDestroyed = true;
					//connect(&mPingTimer, &QTimer::timeout, this, &ApplicationLauncher::destroy);
					shutdown();
					//mPingTimer.start(3000);
				}
				else
				{
					killImpl(Shared::KILL_USER_REQUEST);
					Document::destroy();
				}
			}
		}

		void ApplicationLauncher::setup()
		{
			emit applicationSettingup();
			setupImpl();
		}


		void ApplicationLauncher::onSetupResult(bool b)
		{
			if (!b)
			{
				killImpl(Shared::KILL_FAILED_CONNECTION);
			}
			else
			{
				sendData();
			}
		}


		void ApplicationLauncher::start()
		{
			AppControl::start({});
		}

		void ApplicationLauncher::pause()
		{
			AppControl::pause({});
		}

		void ApplicationLauncher::stop()
		{
			AppControl::stop({});
		}

		void ApplicationLauncher::startImpl()
		{
			mRunning = true;
			emit applicationStarted();
		}

		void ApplicationLauncher::stopImpl()
		{
			mRunning = false;
			emit applicationStopped();
		}

		void ApplicationLauncher::pauseImpl()
		{
		}

		void ApplicationLauncher::sendLua(const QString& cmd)
		{
			execLua(cmd.toStdString(), {});
		}


		ModuleLoader* ApplicationLauncher::moduleLoader()
		{
			return mLoader.ptr();
		}

		UtilModel* ApplicationLauncher::util()
		{
			return mUtil.ptr();
		}

		Inspector* ApplicationLauncher::inspector()
		{
			return mInspector.ptr();
		}


		bool ApplicationLauncher::isRunning()
		{
			return mRunning;
		}

		bool ApplicationLauncher::isSetup()
		{
			return mSetup;
		}


		bool ApplicationLauncher::isClient()
		{
			return isLauncher() && mConfig->launcherType() == Shared::CLIENT_LAUNCHER;
		}

		bool ApplicationLauncher::isLauncher()
		{
			return mConfig->launcher() == Model::ApplicationConfig::MADITOR_LAUNCHER;
		}


		void ApplicationLauncher::timerEvent(QTimerEvent* te)
		{
			network()->sendAndReceiveMessages();
			if (mWaitingForLoader && mLoader->done())
			{
				mLoader->setup2();
				mWaitingForLoader = false;
			}
		}

		void ApplicationLauncher::kill()
		{
			killImpl(Shared::KILL_USER_REQUEST);
		}

		void ApplicationLauncher::shutdown()
		{
			AppControl::stop({});
			AppControl::shutdown({});
		}

		void ApplicationLauncher::onDisconnected()
		{
			
			//receive pending messages
			while (!network()->isMaster() && network()->isMessageAvailable())
				network()->receiveMessages();

			mInspector->reset();
			mLoader->reset();
			mWaitingForLoader = false;
			mUtil->reset();

			mPingTimer.stop();

			mSetup = false;
			emit applicationShutdown();

			if (mAboutToBeDestroyed)
				destroy();
		}

		

		void ApplicationLauncher::onApplicationConnected()
		{
			if (isLauncher() && !mAppInfo.mDebugged)
			{
				ping({});
				mPingTimer.start(3000);
			}
			mInspector->start();
			mSetup = true;
			emit applicationSetup();
		}

		void ApplicationLauncher::pingImpl()
		{
			mPong = true;
		}

		void ApplicationLauncher::luaResponseImpl(const std::string& cmd, const std::string& response, bool failure)
		{
			if (!failure)
			{
				QMessageBox::information(nullptr, QString::fromStdString(cmd), QString::fromStdString(response));
			}
			else
			{
				QMessageBox::critical(nullptr, QString::fromStdString(cmd), QString::fromStdString(response));
			}
		}

		void ApplicationLauncher::sendData()
		{
			configure(mAppInfo, {});
			mLoader->setup();
			mWaitingForLoader = true;
		}

		void ApplicationLauncher::timeout()
		{
			if (mPong)
			{
				mPong = false;
				ping({});
				mPingTimer.start(3000);
			}
			else
			{
				killImpl(Shared::KILL_PING_TIMEOUT);
			}
		}
	}
}
