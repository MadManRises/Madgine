#pragma once

#include "../applicationwrapper.h"

#include "Madgine/network/networkmanager.h"
#include "shared/projectfile.h"

#include "Madgine/serialize/streams/filebuffer.h"

namespace Maditor
{
	namespace Launcher
	{
		
		class StandaloneWrapper : public Engine::Serialize::SerializableUnit<StandaloneWrapper, ApplicationWrapper>
		{
		public:
			StandaloneWrapper();
			~StandaloneWrapper();

			int listen(int port);

		protected:
			int acceptConnection() override;
			Engine::Serialize::SerializeManager* mgr() override;

		
			void receiveFilelistImpl(const std::string &projectName, const Engine::Serialize::SerializableList<Shared::ProjectFile> &files);
			void receiveFileImpl(const std::string &path, const Engine::Serialize::FileBuffer &data);
			void requestFileImpl(const std::string &path);
			void sendReadyImpl();

		private:
			bool mRunRequested;

			Engine::Network::NetworkManager mNetwork;

			std::experimental::filesystem::path mRootPath;

			std::map<std::experimental::filesystem::path, std::experimental::filesystem::path> mMissingFiles;

			Engine::Serialize::Action<decltype(&StandaloneWrapper::receiveFilelistImpl), &StandaloneWrapper::receiveFilelistImpl, Engine::Serialize::ActionPolicy::request> receiveFileList;
			Engine::Serialize::Action<decltype(&StandaloneWrapper::receiveFileImpl), &StandaloneWrapper::receiveFileImpl, Engine::Serialize::ActionPolicy::request> receiveFile;
			Engine::Serialize::Action<decltype(&StandaloneWrapper::requestFileImpl), &StandaloneWrapper::requestFileImpl, Engine::Serialize::ActionPolicy::notification> requestFile;
			Engine::Serialize::Action<decltype(&StandaloneWrapper::sendReadyImpl), &StandaloneWrapper::sendReadyImpl, Engine::Serialize::ActionPolicy::notification> sendReady;

			Engine::Serialize::Serialized<std::string> mConfig;

#ifdef MADGINE_CLIENT_BUILD
		protected:
			Engine::Input::InputHandler* input() override;
#endif
		};

		
	}
}
