#include <gtest/gtest.h>

#include "interfaceslib.h"

#include "network/networkmanager.h"

#include <future>

TEST(NetworkManager, Connect) {
	Engine::Network::NetworkManager server;

	ASSERT_TRUE(server.startServer(1234));
	auto future = std::async(std::launch::async, [&]() {
		return server.acceptConnection(4000);
	});
	Engine::Network::NetworkManager client;

	EXPECT_EQ(client.connect("127.0.0.1", 1234, 2000), Engine::Serialize::NO_ERROR);

	EXPECT_EQ(future.get(), Engine::Serialize::NO_ERROR);

}

#ifdef _WIN32
TEST(asd, FakeTest) {
	int i = 3;
}
#endif
