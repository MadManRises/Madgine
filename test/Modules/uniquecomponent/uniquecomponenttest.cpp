#include <gtest/gtest.h>

#include "Modules/moduleslib.h"

#include "Modules/plugins/pluginmanager.h"
#include "Modules/plugins/pluginsection.h"

#include "Modules/threading/workgroup.h"

#include "Modules/uniquecomponent/uniquecomponentcollectormanager.h"

#include "uniquecomponentshared.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"
#include "Modules/uniquecomponent/uniquecomponentcontainer.h"

#include "Modules/threading/taskqueue.h"

#include "libA.h"
#include "libB.h"

TEST(UniqueComponent, Registry)
{
    Engine::Threading::WorkGroup wg;

    Engine::Threading::TaskQueue taskQueue { "UniqueComponent" };

	//TODO test properly

#if ENABLE_PLUGINS
    Engine::Plugins::PluginManager pmgr;

    Engine::UniqueComponentCollectorManager cmgr { pmgr };

    TestDriver driver;

    ::Test::TestContainer<std::vector<Engine::Placeholder<0>>> v { driver };

    ASSERT_EQ(v.size(), 0);

    Engine::SharedFuture<bool> f = pmgr["Test"].loadPlugin("LibA");
    ASSERT_FALSE(f.is_ready());
    wg.enterCurrentBarrier(&taskQueue, 0, true);
    ASSERT_TRUE(f.is_ready());
    ASSERT_EQ(f.get(), true);

    ASSERT_EQ(v.size(), 1);

    f = pmgr["Test"].loadPlugin("LibB");
    ASSERT_FALSE(f.is_ready());
    wg.enterCurrentBarrier(&taskQueue, 0, true);
    ASSERT_TRUE(f.is_ready());
    ASSERT_EQ(f.get(), true);


    ASSERT_EQ(v.size(), 2);

    size_t indexA = Engine::component_index<LibAComponent>();
    size_t indexB = Engine::component_index<LibBComponent>();

    ASSERT_NE(indexA, indexB);


    f = pmgr["Test"].unloadPlugin("LibB");
    ASSERT_FALSE(f.is_ready());
    wg.enterCurrentBarrier(&taskQueue, 0, true);
    ASSERT_TRUE(f.is_ready());
    ASSERT_EQ(f.get(), false);

    ASSERT_EQ(v.size(), 1);
#endif
}
