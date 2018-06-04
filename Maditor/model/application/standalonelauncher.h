#pragma once

#include "applicationlauncher.h"

#include "Madgine/network/networkmanager.h"
#include "shared/projectfile.h"

#include "Madgine/serialize/streams/filebuffer.h"

namespace Maditor
{
	namespace Model
	{
		
		class MADITOR_MODEL_EXPORT StandaloneLauncher : public Engine::Serialize::SerializableUnit<StandaloneLauncher, ApplicationLauncher>
		{
			Q_OBJECT
		public:
			StandaloneLauncher(ApplicationConfig* config, const QString& uniqueName);

			bool isRemote() override;
			bool isConnected();

			void sendCommand(const QString&) override;

			void connect();

		protected:
			void shutdownImpl() override;
			
		signals:
			void connecting();
			void connected();
			void disconnected();

		protected:
			Engine::Serialize::SerializeManager* network() override;
			bool setupImpl() override;
			void killImpl(Shared::ErrorCode cause) override;

			void onConnectionResult(Engine::Serialize::StreamError result);

			void sendFilelistImpl(const std::string &projectName, const Engine::Serialize::SerializableList<Shared::ProjectFile> &files);
			void sendFileImpl(const std::string &path, const Engine::Serialize::FileBuffer &data);
			void fileRequestImpl(const std::string &path);
			void startAppImpl();


		private:
			Engine::Network::NetworkManager mNetwork;
			Engine::SignalSlot::Slot<decltype(&StandaloneLauncher::onConnectionResult), &StandaloneLauncher::onConnectionResult> mConnectionSlot;
			Engine::SignalSlot::Slot<decltype(&StandaloneLauncher::disconnected), &StandaloneLauncher::disconnected> mDisconnectedSlot;

			Engine::Serialize::Action<decltype(&StandaloneLauncher::sendFilelistImpl), &StandaloneLauncher::sendFilelistImpl, Engine::Serialize::ActionPolicy::request> sendFileList;
			Engine::Serialize::Action<decltype(&StandaloneLauncher::sendFileImpl), &StandaloneLauncher::sendFileImpl, Engine::Serialize::ActionPolicy::request> sendFile;
			Engine::Serialize::Action<decltype(&StandaloneLauncher::fileRequestImpl), &StandaloneLauncher::fileRequestImpl, Engine::Serialize::ActionPolicy::notification> fileRequest;
			Engine::Serialize::Action<decltype(&StandaloneLauncher::startAppImpl), &StandaloneLauncher::startAppImpl, Engine::Serialize::ActionPolicy::notification> startApp;

			Engine::Serialize::Serialized<std::string> mClientConfigString;

		};

	}
}
