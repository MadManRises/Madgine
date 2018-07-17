#include "maditorlauncherlib.h"

#include "embeddedwrapper.h"

#include "../shared/errorcodes.h"

#include "inputwrapper.h"


namespace Maditor
{
	namespace Launcher
	{

		EmbeddedWrapper::EmbeddedWrapper(size_t id) :
		ApplicationWrapper(),
		mMemory(id),
		mNetwork(&mMemory, "Maditor-Link")
		{
			mNetwork.addTopLevelItem(this);
		}

		EmbeddedWrapper::~EmbeddedWrapper()
		{
			if (Engine::SignalSlot::ConnectionManager::getSingletonPtr()){
				Engine::SignalSlot::ConnectionManager::getSingleton().update();
				mNetwork.sendMessages();
				Engine::SignalSlot::ConnectionStore::globalStore().clear();
			}
			using namespace std::chrono;
			std::this_thread::sleep_for(2000ms);
		}

		int EmbeddedWrapper::acceptConnection()
		{

			if (!mNetwork.startServer()) {
				return Shared::FAILED_START_SERVER;
			}
			if (!mNetwork.acceptConnection(2000)) {
				mNetwork.close();
				return Shared::MADITOR_CONNECTION_TIMEOUT;
			}

			return Shared::NO_ERROR_SHARED;

		}

		Engine::Serialize::SerializeManager* EmbeddedWrapper::mgr()
		{
			return &mNetwork;
		}

		Engine::Input::InputHandler* EmbeddedWrapper::input()
		{
			if (!mInput)
				mInput = std::make_unique<InputWrapper>(mMemory.data().mInput);
			return mInput.get();
		}

	}
}


