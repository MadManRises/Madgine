#include <gtest/gtest.h>

#include "Interfaces/interfaceslib.h"

#include "Generic/bits.h"

#include "logContainer.h"

TEST(Bits, Basic)
{
    constexpr bool b1 = std::is_same_v<Engine::MinimalHoldingUIntType_t<1>, bool>;
    ASSERT_TRUE(b1);
    constexpr bool b2 = std::is_same_v<Engine::MinimalHoldingUIntType_t<5>, uint8_t>;
    ASSERT_TRUE(b2);
    constexpr bool b3 = std::is_same_v<Engine::MinimalHoldingUIntType_t<63>, uint64_t>;
    ASSERT_TRUE(b3);

    ASSERT_EQ(Engine::bitSize(43), 6);
    ASSERT_EQ(Engine::bitSize(11), 4);
    ASSERT_EQ(Engine::bitSize(16), 5);
}

TEST(Bits, BitArray)
{

    Engine::BitArray<10, 11> data;

    size_t i = 1;
    for (auto field : data) {
        field = i;
        i += 1;
    }

    logContainer(data);
    
    i = 1;
    for (auto field : data) {
        ASSERT_EQ(field, i);
        i += 1;
    }
}
