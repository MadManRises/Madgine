#include <gtest/gtest.h>

#include "Interfaces/interfaceslib.h"

#include "Interfaces/filesystem/path.h"

TEST(Path, Basic)
{
    Engine::Filesystem::Path p1 { "foo/test" };
    Engine::Filesystem::Path p2 { "foo\\test2/mixed\\" };

    ASSERT_EQ(p2.str(), "foo/test2/mixed");
    ASSERT_EQ(p1.str(), "foo/test");
    ASSERT_EQ(p1.parentPath().str(), "foo");
}
