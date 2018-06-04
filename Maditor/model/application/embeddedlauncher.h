#pragma once

#include "applicationlauncher.h"

#include "shared/ipcmanager/boostipcmanager.h"

#ifdef _WIN32
#define pid_t DWORD
#endif

namespace Maditor
{
	namespace Model
	{
	
		class MADITOR_MODEL_EXPORT EmbeddedLauncher : public ApplicationLauncher
		{
			Q_OBJECT
		public:
			EmbeddedLauncher(ApplicationConfig *config, const QString &uniqueName);
			~EmbeddedLauncher();

			bool isRemote() override;

			void setupNoDebug();
			

			bool isLaunched();

			std::string runtimeDir();

			OgreWindow *window();

			pid_t pid();

			void sendCommand(const QString &cmd);

		protected:
			void onDisconnected() override;
			void checkProcess();

			// Inherited via AppControl
			void shutdownImpl() override;

			void timerEvent(QTimerEvent *te) override;

			bool setupImpl() override;
			bool setupImpl(bool debug);

			Engine::Serialize::SerializeManager* network() override;

		protected slots:
		    void resizeWindow();
			void killImpl(Shared::ErrorCode cause) override;


		signals:
			void processStarted(pid_t, const Shared::ApplicationInfo &);
			void processError(int);

		
		private:
			pid_t mPID;
			HANDLE mHandle;
			HANDLE mChildOutRead, mChildOutWrite, mChildInRead, mChildInWrite;


			Shared::SharedMemory mMemory;
			Shared::BoostIPCManager mNetwork;
			std::unique_ptr<InputWrapper> mInput;
			OgreWindow *mWindow;

		};

	}
}
