#include <gtest/gtest.h>

#include "Madgine/interfaceslib.h"

#include "Madgine/network/networkmanager.h"

#include <future>

TEST(NetworkManager, Connect)
{
	Engine::Network::NetworkManager server("testNetworkServer");

	bool done = false;

ASSERT_TRUE(server.startServer(1234));
	auto future = std::async(std::launch::async, [&]()
	{
		Engine::Serialize::StreamError result = server.acceptConnection(4000ms);
		while (!done)
		{
			server.sendMessages();
		}
		return result;
	});
	Engine::Network::NetworkManager client("testNetworkServer");

	EXPECT_EQ(client.connect("127.0.0.1", 1234, 2000ms ), Engine::Serialize::NO_ERROR);

	done = true;

EXPECT_EQ(future.get(), Engine::Serialize::NO_ERROR);
}

#ifdef _WIN32
TEST(asd, FakeTest)
{
	int i = 3;
}
#endif
