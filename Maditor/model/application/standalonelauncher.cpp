#include "maditormodellib.h"

#include "standalonelauncher.h"

#include "project/applicationconfig.h"
#include <iostream>
#include "shared/projectfile.h"
#include "project/project.h"

namespace Maditor
{
	namespace Model
	{
		StandaloneLauncher::StandaloneLauncher(ApplicationConfig* config, const QString& uniqueName): SerializableUnit(
			config, uniqueName),
			mNetwork("Maditor-Standalone"),
			mConnectionSlot(this),
			mDisconnectedSlot(this)
		{
			mNetwork.addTopLevelItem(this);
			mNetwork.connectConnectionSlot(mConnectionSlot);
			mNetwork.connectSlaveStreamDisconnected(mDisconnectedSlot);
		}

		bool StandaloneLauncher::isRemote()
		{
			return true;
		}

		bool StandaloneLauncher::isConnected()
		{
			return mNetwork.isConnected();
		}

		void StandaloneLauncher::sendCommand(const QString&)
		{
		}

		void StandaloneLauncher::connect()
		{
			emit connecting();
			mNetwork.connect_async("127.0.0.1", 1234, 10000);
		}

		void StandaloneLauncher::shutdownImpl()
		{
		}

		Engine::Serialize::SerializeManager* StandaloneLauncher::network()
		{
			return &mNetwork;
		}

		bool StandaloneLauncher::setupImpl()
		{
			mAppInfo.mDebugged = false;
			mAppInfo.mAppName = getName().toStdString();

			mConfig->generateInfo(mAppInfo);

			std::experimental::filesystem::path dataRoot = "Media";
			std::experimental::filesystem::path mediaRoot = std::experimental::filesystem::path(mAppInfo.mDataDir) / "Media";

			if (!isClient())
			{
				dataRoot /= "scripts";
				mediaRoot /= "scripts";
			}

			std::experimental::filesystem::path root = mConfig->parentItem()->path().toStdString();

			std::list<Shared::ProjectFile> files;

			for (const auto& file : std::experimental::filesystem::recursive_directory_iterator(mediaRoot))
			{
				if (is_regular_file(file)){
					files.emplace_back(file.path(), dataRoot, root);
				}
			}

			std::experimental::filesystem::path binRoot = mAppInfo.mProjectDir;
			std::experimental::filesystem::path binTarget;
			binRoot /= "debug/bin";
			if (!mClientConfigString->empty())
			{
				std::cout << *mClientConfigString << std::endl;
			}
			for (const auto& file : std::experimental::filesystem::directory_iterator(binRoot))
			{
				if (is_regular_file(file) && file.path().extension() == ".dll"){
					files.emplace_back(file.path(), binTarget, root);
				}
			}



			sendFileList(mConfig->project()->getName().toStdString(), files, {});

			return true;
		}

		void StandaloneLauncher::killImpl(Shared::ErrorCode cause)
		{
			mNetwork.close();
		}

		void StandaloneLauncher::onConnectionResult(Engine::Serialize::StreamError result)
		{
			if (result == Engine::Serialize::NO_ERROR)
				connected();
			else
				disconnected();

		}

		void StandaloneLauncher::sendFilelistImpl(const std::string& projectName,
			const Engine::Serialize::SerializableList<Shared::ProjectFile>& files)
		{
		}

		void StandaloneLauncher::sendFileImpl(const std::string& path, const Engine::Serialize::FileBuffer& data)
		{
		}

		void StandaloneLauncher::fileRequestImpl(const std::string& path)
		{
			std::experimental::filesystem::path root = mConfig->parentItem()->path().toStdString();
			sendFile(path, Engine::Serialize::FileBuffer::readFile(root / path), {});
		}

		void StandaloneLauncher::startAppImpl()
		{
			LOG("Send Data");
			sendData();
		}
	}
}
