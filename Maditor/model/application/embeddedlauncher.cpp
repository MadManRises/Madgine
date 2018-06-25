#include "maditormodellib.h"

#include "embeddedlauncher.h"

#include "inputwrapper.h"

#include "project/applicationconfig.h"

#include "project/project.h"

#include "ogrewindow.h"

#include "../shared/ipcmanager/boostipcserver.h"

namespace Maditor
{
	namespace Model
	{
		EmbeddedLauncher::EmbeddedLauncher(ApplicationConfig* config, const QString& uniqueName) :
			ApplicationLauncher(config, uniqueName),
			mNetwork(&mMemory, uniqueName.toStdString()),
			mInput(std::make_unique<InputWrapper>(mMemory.data().mInput)),
			mWindow(config->launcher() == ApplicationConfig::MADITOR_LAUNCHER ? new OgreWindow(mInput.get()) : nullptr)
		{
			mNetwork.connectConnectionSlot(mOnSetupResult);

			mNetwork.addTopLevelItem(this);

			if (mWindow)
				connect(mWindow, &OgreWindow::resized, this, &EmbeddedLauncher::resizeWindow);

			connect(&mProcess, &Platform::ProcessLauncher::processError, this, &EmbeddedLauncher::processError);
			connect(&mProcess, &Platform::ProcessLauncher::processTerminated, this, &EmbeddedLauncher::onDisconnected);
			connect(&mProcess, &Platform::ProcessLauncher::outputReceived, [this](const std::string &s) {emit outputReceived(QString::fromStdString(s)); });

			std::experimental::filesystem::create_directory(runtimeDir());
		}

		EmbeddedLauncher::~EmbeddedLauncher()
		{
			killImpl(Shared::KILL_CLEANUP);

		}


		void EmbeddedLauncher::setupNoDebug()
		{
			emit applicationSettingup();
			setupImpl(false);
		}

		bool EmbeddedLauncher::setupImpl(bool debug)
		{
			mAppInfo.mDebugged = debug;
			mAppInfo.mAppName = getName().toStdString();

			std::string cmd;


			if (isLauncher())
			{
				mConfig->generateInfo(mAppInfo, mWindow);

				cmd = isClient() ? EMBEDDED_CLIENT_LAUNCHER_BINARY : EMBEDDED_SERVER_LAUNCHER_BINARY;

				cmd += " " + std::to_string(mMemory.id());
			}
			else
			{
				cmd = mConfig->customExecutableCmd().toStdString();
			}

			mProcess.launch(cmd, runtimeDir());

			emit processStarted(mProcess.pid(), mAppInfo);
			mUtil->stats()->setProcess(mProcess.handle());

			if (isLauncher())
			{
				mNetwork.connect_async(10000);
			}
			else
			{
				onApplicationConnected();
			}

			return true;
			
		}


		bool EmbeddedLauncher::isRemote()
		{
			return false;
		}

		std::string EmbeddedLauncher::runtimeDir()
		{
			return mConfig->project()->path().filePath(QString("debug/runtime/") + getName() + "/").toStdString();
		}

		
		OgreWindow* EmbeddedLauncher::window()
		{
			return mWindow;
		}

		
		pid_t EmbeddedLauncher::pid()
		{
			return mProcess.pid();
		}

		bool EmbeddedLauncher::isLaunched()
		{
			return mProcess.isLaunched();
		}

		void EmbeddedLauncher::resizeWindow()
		{
			AppControl::resizeWindow({});
		}

		void EmbeddedLauncher::killImpl(Shared::ErrorCode cause)
		{
			if (mProcess.kill(cause))
			{
				std::string msg = getName().toStdString() + ": Process killed! (" + Shared::to_string(cause) + ")";
				LOG_ERROR(msg);
			}
		}


		void EmbeddedLauncher::shutdownImpl()
		{
		}


		bool EmbeddedLauncher::setupImpl()
		{
			return setupImpl(true);			
		}


		Engine::Serialize::SerializeManager* EmbeddedLauncher::network()
		{
			return &mNetwork;
		}

		void EmbeddedLauncher::sendCommand(const QString& cmd)
		{
			mProcess.sendInput(cmd.toStdString());
		}

		void EmbeddedLauncher::onDisconnected()
		{
			ApplicationLauncher::onDisconnected();

			mMemory.mgr()->destroy<Shared::BoostIPCServer>("Server");

			mNetwork.close();

		}


	}
}
