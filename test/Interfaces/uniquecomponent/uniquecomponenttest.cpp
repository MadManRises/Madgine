#include <gtest/gtest.h>

#include "Modules/moduleslib.h"

#include "Modules/plugins/pluginmanager.h"

#include "Modules/threading/workgroup.h"

#include "Modules/uniquecomponent/uniquecomponentcollectormanager.h"

#include "uniquecomponentshared.h"

#include "libA.h"
#include "libB.h"

TEST(UniqueComponent, Registry)
{
	Engine::Threading::WorkGroup wg;

#if ENABLE_PLUGINS
	Engine::Plugins::PluginManager pmgr;

	Engine::UniqueComponentCollectorManager cmgr{ pmgr };
#endif

	TestDriver driver;

	::Test::TestContainer<std::vector> v{ driver };

#if ENABLE_PLUGINS
	ASSERT_EQ(v.size(), 0);

	ASSERT_TRUE(pmgr["Test"].loadPlugin("LibA"));

	ASSERT_EQ(v.size(), 1);

	ASSERT_TRUE(pmgr["Test"].loadPlugin("LibB"));
#endif

	ASSERT_EQ(v.size(), 2);

	size_t indexA = Engine::component_index<LibAComponent>();
	size_t indexB = Engine::component_index<LibBComponent>();

	ASSERT_NE(indexA, indexB);

#if ENABLE_PLUGINS
	ASSERT_TRUE(pmgr["Test"].unloadPlugin("LibA"));

	ASSERT_EQ(v.size(), 1);
#endif
}
