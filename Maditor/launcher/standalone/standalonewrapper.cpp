#include "../maditorlauncherlib.h"

#include "standalonewrapper.h"

#include "shared/errorcodes.h"
#include <iostream>

#ifdef MADGINE_CLIENT_BUILD
#include "../inputwrapper.h"
#endif

namespace Maditor
{
	namespace Launcher
	{

		StandaloneWrapper::StandaloneWrapper() :
		mNetwork("Standalone-Maditor-Launcher"),
		mConfig(CONFIG),
		mRunRequested(false)
		{
			mNetwork.addTopLevelItem(this);
		}

		StandaloneWrapper::~StandaloneWrapper()
		{
			if (Engine::SignalSlot::ConnectionManager::getSingletonPtr()){
				Engine::SignalSlot::ConnectionManager::getSingleton().update();
				mNetwork.sendMessages();
				Engine::SignalSlot::ConnectionStore::globalStore().clear();
			}
			using namespace std::chrono;
			std::this_thread::sleep_for(2000ms);
		}

		int StandaloneWrapper::listen(int port)
		{
			if (!mNetwork.startServer(port)) {
				return Shared::FAILED_START_SERVER;
			}
			LOG("Waiting for Connection...");
			while(1)
			{
				if (mNetwork.clientCount() == 0)
				{
					mNetwork.acceptConnections(1);
				}else
				{
					mNetwork.sendAndReceiveMessages();	
					if (mRunRequested)
					{
						LOG(std::to_string(run()));
						mRunRequested = false;
					}
				}				
			}
		}

		int StandaloneWrapper::acceptConnection()
		{

			
			
			/*
				mNetwork.close();
				return Shared::MADITOR_CONNECTION_TIMEOUT;
			}*/

			return Shared::NO_ERROR_SHARED;

		}

		Engine::Serialize::SerializeManager* StandaloneWrapper::mgr()
		{
			return &mNetwork;
		}

		void StandaloneWrapper::receiveFilelistImpl(const std::string& projectName,
			const Engine::Serialize::SerializableList<Shared::ProjectFile>& files)
		{

			mRootPath = std::experimental::filesystem::current_path() / projectName;
			mMissingFiles.clear();

			for (const Shared::ProjectFile &projectFile : files){
				//std::cout << projectFile.mPath << ", " << projectFile.mSize << ", " << projectFile.mSimpleHash << std::endl;
				//assert(projectFile.mPath.is_relative());
				if (!projectFile.compare(mRootPath))
				{
					mMissingFiles[projectFile.mSourcePath] = projectFile.mPath;
					requestFile(projectFile.mSourcePath.generic_string(), {});
				}
			}

			if (mMissingFiles.empty())
			{
				sendReady({});
			}

		}

		void StandaloneWrapper::receiveFileImpl(const std::string& path, const Engine::Serialize::FileBuffer& data)
		{
			auto it = mMissingFiles.find(path);
			LOG(Engine::message("Received '", "'.")(it->second));
			data.writeFile(mRootPath / it->second, true);
			mMissingFiles.erase(it);
			if (mMissingFiles.empty())
			{
				sendReady({});
			}
		}

		void StandaloneWrapper::requestFileImpl(const std::string& path)
		{
		}

		void StandaloneWrapper::sendReadyImpl()
		{
			mRunRequested = true;
		}

#ifdef MADGINE_CLIENT_BUILD
		Engine::Input::InputHandler* StandaloneWrapper::input()
		{
			return nullptr;
		}
#endif
	}
}


