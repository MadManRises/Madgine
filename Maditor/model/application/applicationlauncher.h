#pragma once

#include "shared/appcontrol.h"
#include "moduleloader.h"
#include "Madgine/serialize/container/serialized.h"
#include "utilmodel.h"
#include "logreceiver.h"
#include "model/documents/document.h"
#include "shared/errorcodes.h"
#include "inspector/inspector.h"
#include "shared/sharedmemory.h"


namespace Maditor {

	namespace Model {

		class MADITOR_MODEL_EXPORT ApplicationLauncher : public Document, public Engine::Serialize::SerializableUnit<ApplicationLauncher, Shared::AppControl> {
			Q_OBJECT

		public:
			ApplicationLauncher(ApplicationConfig *config, const QString &uniqueName);
			virtual ~ApplicationLauncher();

			void destroy() override;

			void setup();
			
			void shutdown();
			void kill();

			void start();
			void pause();
			void stop();
			
			
			ModuleLoader *moduleLoader();
			UtilModel *util();
			Inspector *inspector();
			

			bool isRunning();
			
			bool isSetup();
			virtual bool isRemote() = 0;

			bool isClient();
			bool isLauncher();


			void sendLua(const QString &cmd);

			virtual void sendCommand(const QString &) = 0;

		protected:
			void timerEvent(QTimerEvent * te) override;			

			void onSetupResult(bool b);
			virtual void onDisconnected();

			// Inherited via AppControl
			void startImpl() override;

			void stopImpl() override;

			void pauseImpl() override;

			// Inherited via AppControl
			void onApplicationConnected() override;

			void pingImpl() override;

			void luaResponseImpl(const std::string&, const std::string&, bool) override;

			Engine::SignalSlot::Slot<decltype(&ApplicationLauncher::onSetupResult), &ApplicationLauncher::onSetupResult> mOnSetupResult;

			virtual Engine::Serialize::SerializeManager *network() = 0;

			virtual bool setupImpl() = 0;
			
			void sendData();

		protected slots:
			void timeout();
			virtual void killImpl(Shared::ErrorCode cause) = 0;

		signals:
			void applicationSettingup();
			void applicationSetup();
			void applicationStarted();
			void applicationStopped();
			void applicationShutdown();

			

			void destroyApplication(ApplicationLauncher *launcher);

			void outputReceived(const QString &);

		protected:
			Shared::ApplicationInfo mAppInfo;
			ApplicationConfig *mConfig;

			Engine::Serialize::Serialized<UtilModel> mUtil;

		private:

			Engine::Serialize::Serialized<LogReceiver> mLog;
			Engine::Serialize::Serialized<ModuleLoader> mLoader;
			Engine::Serialize::Serialized<Inspector> mInspector;
			
			bool mWaitingForLoader;
			
			bool mAboutToBeDestroyed;

			bool mRunning;

			bool mSetup;

			QTimer mPingTimer;
			bool mPong;

		};

	}
}
