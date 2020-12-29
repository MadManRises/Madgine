#include <gtest/gtest.h>

#include "serialize/network/networklib.h"

#include "serialize/network/networkmanager.h"

#include "Modules/threading/workgroup.h"

#include "Modules/threading/taskqueue.h"

#include <future>

TEST(NetworkManager, Connect)
{
    Engine::Threading::WorkGroup wg;

    Engine::Threading::TaskQueue taskQueue { "default" };

    Engine::Network::NetworkManager server("testNetworkServer");

    bool done = false;

    ASSERT_EQ(server.startServer(1234), Engine::Network::NetworkManagerResult::SUCCESS) << "SocketAPI: " << server.getSocketAPIError();

    #if !EMSCRIPTEN
    auto future = wg.spawnTaskThread([&]() {
        Engine::Network::NetworkManagerResult result = server.acceptConnection(400000ms);
        while (!done) {
            server.sendMessages();
            taskQueue.update(Engine::Threading::TaskMask::DEFAULT, &wg.hasInterrupt());
        }
        return result;
    });
    Engine::Network::NetworkManager client("testNetworkClient");

    EXPECT_EQ(client.connect("127.0.0.1", 1234, 200000ms), Engine::Network::NetworkManagerResult::SUCCESS) << "Stream-Error: " << server.getStreamError();

    taskQueue.update(Engine::Threading::TaskMask::DEFAULT, &wg.hasInterrupt());

    done = true;

    EXPECT_EQ(future.get(), Engine::Network::NetworkManagerResult::SUCCESS);
    #endif
}
