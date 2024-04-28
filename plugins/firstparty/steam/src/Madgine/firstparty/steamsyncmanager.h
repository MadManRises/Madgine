#pragma once

#include "Madgine/firstparty/firstpartysyncmanager.h"

#include <steam/steam_api.h>

#include "Generic/execution/signal.h"


namespace Engine {
namespace FirstParty {

    struct SteamSyncManager : FirstPartySyncManager {
        using FirstPartySyncManager::FirstPartySyncManager;        

        void setup();

        void listen(size_t count, Serialize::Format format);

        void connectImpl(Execution::VirtualReceiverBase<Serialize::SyncManagerResult> &receiver, CSteamID target, Serialize::Format format, TimeOut timeout = {});
        ASYNC_STUB(connect, connectImpl, Execution::make_simple_virtual_sender<Serialize::SyncManagerResult>);

        void disconnect();

        Execution::SignalStub<> &playersConnected();

    private:
        Serialize::Format mFormat = nullptr;

        HSteamListenSocket mListenSocket = k_HSteamListenSocket_Invalid;

        size_t mRemainingPlayersToConnect;
        Execution::Signal<> mPlayersConnected;

        STEAM_CALLBACK(SteamSyncManager, onIncomingConnection, SteamNetConnectionStatusChangedCallback_t);
    };

}
}