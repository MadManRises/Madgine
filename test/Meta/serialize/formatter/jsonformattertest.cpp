#include <gtest/gtest.h>

#include "Meta/metalib.h"

#include "Meta/serialize/formatter/jsonformatter.h"

#include "formattertestbase.h"

using namespace Engine::Serialize;
using namespace std::chrono_literals;

TEST(Serialize_Formatter, JSON)
{
    FormatterBaseTest<JSONFormatter>(R"(
"unit1" : {
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
                "fourth" : true,
                "serId" : 1
            }
        },
        {
            "__extended" : {
                "first" : 2,
                "second" : 4,
                "third" : "default",
                "fourth" : true,
                "serId" : 2
            }
        }
    ],
    "complexList2" : [
    ],
    "complexList3" : [
    ],
    "pod" : [1,2],
    "bytes" : "SGVsbG8gV29ybGQh"
})");
}

TEST(Serialize_Formatter, JSON_InvalidParse)
{
    std::unique_ptr<std::stringbuf> file = std::make_unique<std::stringbuf>(
        R"(
"Item" : {
    "__extended" : {
        "syncId" : 10
    },
    "list1" : [1,2,3],
    "list2" : [4,5],
    "set1" : [6,,8,9],
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
                "fourth" : true,
                "serId" : 1
            }
        },
        {
            "__extended" : {
                "first" : 2,
                "second" : 4,
                "third" : "default",
                "fourth" : true,
                "serId" : 2
            }
        }
    ],
    "complexList2" : [
    ],
    "complexList3" : [
    ],
    "pod" : [1,2],
    "bytes" : "SGVsbG8gV29ybGQh"
})");

    NoParentUnit<TestUnit> unit;

    SerializeInStream in { std::move(file), std::make_unique<SerializeStreamData>(std::make_unique<JSONFormatter>()) };

    StreamResult result = read(in, unit, nullptr);
    ASSERT_EQ(result.mState, StreamState::PARSE_ERROR);
    ASSERT_EQ(result.mError->mMsg, "ERROR: (8, 16): Expected: <int>");
}

TEST(Serialize_Formatter, JSON_ExtendedOrder)
{
    std::unique_ptr<std::stringbuf> file = std::make_unique<std::stringbuf>(
        R"(
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
                "second" : 6,
                "first" : 2,
                "third" : "set",
                "serId" : 1,
                "fourth" : true
            }
        },
        {
            "__extended" : {
                "first" : 2,
                "second" : 4,
                "third" : "default",
                "serId" : 2,
                "fourth" : true
            }
        }
    ],
    "complexList2" : [
    ],
    "complexList3" : [
    ],
    "pod" : [1,2],
    "bytes" : "SGVsbG8gV29ybGQh"
})");

    NoParentUnit<TestUnit> unit;

    SerializeInStream in { std::move(file), std::make_unique<SerializeStreamData>(std::make_unique<JSONFormatter>()) };

    HANDLE_STREAM_RESULT(read(in, unit, nullptr));    
    ASSERT_EQ(unit.complexList1.front().i, 2);
    ASSERT_EQ(unit.complexList1.front().f, 6.0f);
    ASSERT_EQ(unit.complexList1.front().s, "set");
    ASSERT_EQ(unit.complexList1.front().b, true);
}
