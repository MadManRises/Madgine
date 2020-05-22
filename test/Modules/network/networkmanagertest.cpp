#include <gtest/gtest.h>

#include "Modules/moduleslib.h"

#include "Modules/network/networkmanager.h"

#include "Modules/threading/workgroup.h"

#include "Modules/threading/defaulttaskqueue.h"

#include <future>

TEST(NetworkManager, Connect)
{
    Engine::Threading::WorkGroup wg;

    Engine::Network::NetworkManager server("testNetworkServer");

    bool done = false;

    ASSERT_EQ(server.startServer(1234), Engine::Network::NetworkManagerResult::SUCCESS) << "SocketAPI: " << server.getSocketAPIError();

    #if !EMSCRIPTEN
    auto future = wg.spawnTaskThread([&]() {
        Engine::Network::NetworkManagerResult result = server.acceptConnection(400000ms);
        while (!done) {
            server.sendMessages();
            Engine::Threading::DefaultTaskQueue::getSingleton().update();
        }
        return result;
    });
    Engine::Network::NetworkManager client("testNetworkClient");

    EXPECT_EQ(client.connect("127.0.0.1", 1234, 200000ms), Engine::Network::NetworkManagerResult::SUCCESS) << "Stream-Error: " << server.getStreamError();

    Engine::Threading::DefaultTaskQueue::getSingleton().update();

    done = true;

    EXPECT_EQ(future.get(), Engine::Network::NetworkManagerResult::SUCCESS);
    #endif
}
