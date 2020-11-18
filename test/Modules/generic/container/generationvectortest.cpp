#include <gtest/gtest.h>

#include "Modules/moduleslib.h"

#include "Modules/generic/container/generationvector.h"

#include "../logContainer.h"

TEST(GenerationVector, Basic)
{
    Engine::GenerationVector<int> v;

    v.emplace(1);
    Engine::GenerationContainerIndex i2 = v.emplace(2).copyIndex();
    Engine::GenerationContainerIndex i3 = v.emplace(3).copyIndex();

    logContainer(v);

    v.erase(i2);

    logContainer(v);

    ASSERT_EQ(v[i3], 3);

    v.reset(i2);
    v.reset(i3);
}
