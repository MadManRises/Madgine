#include <gtest/gtest.h>

#include "Modules/moduleslib.h"

#include "Modules/plugins/pluginmanager.h"
#include "Modules/plugins/pluginsection.h"

#include "Modules/threading/workgroup.h"

#include "Modules/uniquecomponent/uniquecomponentcollectormanager.h"

#include "uniquecomponentshared.h"

#include "libA.h"
#include "libB.h"

TEST(UniqueComponent, Registry)
{
    Engine::Threading::WorkGroup wg;

	//TODO test properly

#if ENABLE_PLUGINS
    Engine::Plugins::PluginManager pmgr;

    Engine::UniqueComponentCollectorManager cmgr { pmgr };

    TestDriver driver;

    ::Test::TestContainer<std::vector<Engine::Placeholder<0>>> v { driver };

    ASSERT_EQ(v.size(), 0);

    ASSERT_EQ(pmgr["Test"].loadPlugin("LibA"), Engine::Plugins::Plugin::LOADED);

    ASSERT_EQ(v.size(), 1);

    ASSERT_EQ(pmgr["Test"].loadPlugin("LibB"), Engine::Plugins::Plugin::LOADED);


    ASSERT_EQ(v.size(), 2);

    size_t indexA = Engine::component_index<LibAComponent>();
    size_t indexB = Engine::component_index<LibBComponent>();

    ASSERT_NE(indexA, indexB);


    ASSERT_EQ(pmgr["Test"].unloadPlugin("LibB"), Engine::Plugins::Plugin::UNLOADED);

    ASSERT_EQ(v.size(), 1);
#endif
}
