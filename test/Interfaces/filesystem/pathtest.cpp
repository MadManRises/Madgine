#include <gtest/gtest.h>

#include "Interfaces/interfaceslib.h"

#include "Interfaces/filesystem/path.h"

TEST(Path, Basic)
{
    Engine::Filesystem::Path p1 { "/test" };
    Engine::Filesystem::Path p2 { "\\test2/mixed\\" };

    ASSERT_EQ(p2.str(), "/test2/mixed");
    ASSERT_EQ(p1.str(), "/test");
    ASSERT_EQ(p1.parentPath().str(), "/");
}
