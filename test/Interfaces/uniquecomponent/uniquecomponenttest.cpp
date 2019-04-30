#include <gtest/gtest.h>

#include "Interfaces/interfaceslib.h"

#include "Interfaces/plugins/pluginmanager.h"

#include "Interfaces/threading/workgroup.h"

#include "Interfaces/uniquecomponent/uniquecomponentcollectormanager.h"

#include "uniquecomponentshared.h"

#include "libA.h"
#include "libB.h"

TEST(UniqueComponent, Registry)
{
	Engine::Threading::WorkGroup wg;

	Engine::Plugins::PluginManager pmgr{ "Test" };

	Engine::UniqueComponentCollectorManager cmgr{ pmgr };

	TestDriver driver;

	::Test::TestContainer<std::vector> v{ driver };

	ASSERT_EQ(v.size(), 0);

	ASSERT_TRUE(pmgr["Test"].loadPlugin("LibA"));

	ASSERT_EQ(v.size(), 1);

	ASSERT_TRUE(pmgr["Test"].loadPlugin("LibB"));

	ASSERT_EQ(v.size(), 2);

	size_t indexA = Engine::component_index<LibAComponent>();
	size_t indexB = Engine::component_index<LibBComponent>();

	ASSERT_NE(indexA, indexB);

	ASSERT_TRUE(pmgr["Test"].unloadPlugin("LibA"));

	ASSERT_EQ(v.size(), 1);
}
