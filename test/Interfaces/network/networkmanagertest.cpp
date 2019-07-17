#include <gtest/gtest.h>

#include "Modules/moduleslib.h"

#include "Modules/network/networkmanager.h"

#include "Modules/threading/workgroup.h"

#include <future>

TEST(NetworkManager, Connect)
{
	Engine::Threading::WorkGroup wg;

	Engine::Network::NetworkManager server("testNetworkServer");

	bool done = false;

	ASSERT_TRUE(server.startServer(1234));
	auto future = std::async(std::launch::async, [&]()
	{
		Engine::StreamError result = server.acceptConnection(4000ms);
		while (!done)
		{
			server.sendMessages();
			wg.taskQueue().update();
		}
		return result;
	});
	Engine::Network::NetworkManager client("testNetworkServer");

	EXPECT_EQ(client.connect("127.0.0.1", 1234, 2000ms ), Engine::NO_ERROR);

	done = true;

	EXPECT_EQ(future.get(), Engine::NO_ERROR);
}

