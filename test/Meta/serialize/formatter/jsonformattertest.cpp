#include <gtest/gtest.h>

#include "Meta/metalib.h"

#include "Meta/serialize/formatter/jsonformatter.h"

#include "formattertestbase.h"

using namespace Engine::Serialize;
using namespace std::chrono_literals;

TEST(Serialize_Formatter, JSON)
{
    FormatterBaseTest<JSONFormatter>(R"(
"Item" : {
    "__extended" : {
        "syncId" : 10
    },
    "list1" : [1,2,3],
    "list2" : [4,5],
    "set1" : [6,7,8,9],
    "set2" : [10],
    "map1" : [
        {
            "Key" : {
                "First" : 1,
                "Second" : 3
            },
            "Value" : 3
        },
        {
            "Key" : {
                "First" : 2,
                "Second" : 4
            },
            "Value" : 6
        }
    ],
    "complexList1" : [
        {
            "__extended" : {
                "first" : 2,
                "second" : 6,
                "third" : "set",
                "serId" : 1
            }
        },
        {
            "__extended" : {
                "first" : 2,
                "second" : 4,
                "third" : "default",
                "serId" : 2
            }
        }
    ],
    "complexList2" : [
    ],
    "complexList3" : [
    ],
    "pod" : [1,2]
})");
}
