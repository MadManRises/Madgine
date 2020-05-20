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

    LOG(Engine::Network::NetworkManagerResult::SUCCESS);
    LOG(Engine::Network::NetworkManagerResult { Engine::Network::NetworkManagerResult::SUCCESS });
    LOG(Engine::Network::NetworkManagerResult::NO_SERVER);
    LOG(Engine::Network::NetworkManagerResult { Engine::Network::NetworkManagerResult::NO_SERVER });
    LOG(Engine::Network::NetworkManagerResult::CONNECTION_REFUSED);
    LOG(Engine::Network::NetworkManagerResult { Engine::Network::NetworkManagerResult::CONNECTION_REFUSED });

    EXPECT_EQ(server.startServer(1234), Engine::Network::NetworkManagerResult::SUCCESS);
    auto future = wg.spawnTaskThread([&]() {
        Engine::Network::NetworkManagerResult result = server.acceptConnection(4000ms);
        while (!done) {
            server.sendMessages();
            Engine::Threading::DefaultTaskQueue::getSingleton().update();
        }
        return result;
    });
    Engine::Network::NetworkManager client("testNetworkServer");

    EXPECT_EQ(client.connect("127.0.0.1", 1234, 2000ms), Engine::StreamResult::SUCCESS);

    Engine::Threading::DefaultTaskQueue::getSingleton().update();

    done = true;

    EXPECT_EQ(future.get(), Engine::StreamResult::SUCCESS);
}
