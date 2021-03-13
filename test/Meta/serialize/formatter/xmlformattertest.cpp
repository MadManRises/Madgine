#include <gtest/gtest.h>

#include "Meta/metalib.h"

#include "Meta/serialize/formatter/xmlformatter.h"

#include "formattertestbase.h"

using namespace Engine::Serialize;
using namespace std::chrono_literals;

TEST(Serialize_Formatter, XML)
{
    FormatterBaseTest<XMLFormatter>(R"(<Item syncId=10>
    <list1>
        <Item>1</Item>
        <Item>2</Item>
        <Item>3</Item>
    </list1>
    <list2>
        <Item>4</Item>
        <Item>5</Item>
    </list2>
    <set1>
        <Item>6</Item>
        <Item>7</Item>
        <Item>8</Item>
        <Item>9</Item>
    </set1>
    <set2>
        <Item>10</Item>
    </set2>
    <map1>
        <Item>
            <Key>
                <First>1</First>
                <Second>3</Second>
            </Key>
            <Value>3</Value>
        </Item>
        <Item>
            <Key>
                <First>2</First>
                <Second>4</Second>
            </Key>
            <Value>6</Value>
        </Item>
    </map1>
    <complexList1>
        <Item first=2 second=6 third="set" fourth=1 serId=1>
        </Item>
        <Item first=2 second=4 third="default" fourth=1 serId=2>
        </Item>
    </complexList1>
    <complexList2>
    </complexList2>
    <complexList3>
    </complexList3>
    <pod>
        <Element>1</Element>
        <Element>2</Element>
    </pod>
</Item>
)");
}

TEST(Serialize_Formatter, XML_InvalidParse)
{
    std::unique_ptr<std::stringbuf> file = std::make_unique<std::stringbuf>(
        R"(<Item syncId=10>
    <list1>
        <Item>1</Item>
        <Item></Item> )" /* <--- Error here */ R"(
        <Item>3</Item>
    </list1>
    <list2>
        <Item>4</Item>
        <Item>5</Item>
    </list2>
    <set1>
        <Item>6</Item>
        <Item>7</Item>
        <Item>8</Item>
        <Item>9</Item>
    </set1>
    <set2>
        <Item>10</Item>
    </set2>
    <map1>
        <Item>
            <Key>
                <First>1</First>
                <Second>3</Second>
            </Key>
            <Value>3</Value>
        </Item>
        <Item>
            <Key>
                <First>2</First>
                <Second>4</Second>
            </Key>
            <Value>6</Value>
        </Item>
    </map1>
    <complexList1>
        <Item first=2 second=6 third="set" serId=1>
        </Item>
        <Item first=2 second=4 third="default" serId=2>
        </Item>
    </complexList1>
    <complexList2>
    </complexList2>
    <complexList3>
    </complexList3>
</Item>")");
    
    NoParentUnit<TestUnit> unit;        

    SerializeInStream in { std::move(file), std::make_unique<SerializeStreamData>(std::make_unique<XMLFormatter>()) };

    StreamResult result = read(in, unit, nullptr);        
    ASSERT_EQ(result.mState, StreamState::PARSE_ERROR);
    ASSERT_EQ(result.mError->mMsg, "ERROR: (4, 15): Expected: <int>"); 
}


TEST(Serialize_Formatter, XML_ExtendedOrder)
{
    std::unique_ptr<std::stringbuf> file = std::make_unique<std::stringbuf>(
        R"(<Item syncId=10>
    <list1>
        <Item>1</Item>
        <Item>2</Item>
        <Item>3</Item>
    </list1>
    <list2>
        <Item>4</Item>
        <Item>5</Item>
    </list2>
    <set1>
        <Item>6</Item>
        <Item>7</Item>
        <Item>8</Item>
        <Item>9</Item>
    </set1>
    <set2>
        <Item>10</Item>
    </set2>
    <map1>
        <Item>
            <Key>
                <First>1</First>
                <Second>3</Second>
            </Key>
            <Value>3</Value>
        </Item>
        <Item>
            <Key>
                <First>2</First>
                <Second>4</Second>
            </Key>
            <Value>6</Value>
        </Item>
    </map1>
    <complexList1>
        <Item second=6 first=2 third="set" serId=1 fourth=1>
        </Item>
        <Item first=2 second=4 third="default" serId=2 fourth=1>
        </Item>
    </complexList1>
    <complexList2>
    </complexList2>
    <complexList3>
    </complexList3>
    <pod>
        <Element>1</Element>
        <Element>2</Element>
    </pod>
</Item>)");

    NoParentUnit<TestUnit> unit;

    SerializeInStream in { std::move(file), std::make_unique<SerializeStreamData>(std::make_unique<XMLFormatter>()) };

    StreamResult result = read(in, unit, nullptr);
    ASSERT_EQ(result.mState, StreamState::OK) << *result.mError;
    ASSERT_EQ(unit.complexList1.front().i, 2);
    ASSERT_EQ(unit.complexList1.front().f, 6.0f);
    ASSERT_EQ(unit.complexList1.front().s, "set");
    ASSERT_EQ(unit.complexList1.front().b, true);
}

