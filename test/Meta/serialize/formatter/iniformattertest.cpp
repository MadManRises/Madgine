#include <gtest/gtest.h>

#include "Meta/metalib.h"

#include "Meta/serialize/formatter/iniformatter.h"

#include "formattertestbase.h"

using namespace Engine::Serialize;
using namespace std::chrono_literals;

TEST(Serialize_Formatter, Ini)
{
    FormatterBaseTest<IniFormatter>(R"(syncId=10
size=3
Item=1
Item=2
Item=3
size=2
Item=4
Item=5
size=4
Item=6
Item=7
Item=8
Item=9
size=1
Item=10
size=2
First=1
Second=3
Value=3
First=2
Second=4
Value=6
size=2
first=2
second=6
third=set
fourth=true
serId=1
first=2
second=4
third=default
fourth=true
serId=2
size=0
size=0
Element=1
Element=2
)");
}
