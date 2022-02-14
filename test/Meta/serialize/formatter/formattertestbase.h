#pragma once

#include "../testunit.h"
#include "../testManager.h"

#include "Meta/serialize/streams/serializestream.h"

#include "Meta/serialize/container/noparent.h"

#include "Meta/serialize/streams/serializestreamdata.h"

#include "Meta/serialize/streams/operations.h"

template <typename Formatter>
void FormatterBaseTest(std::string_view expected = "")
{
    NoParentUnit<TestUnit> unit1;
    NoParentUnit<TestUnit> unit2;

    unit1.list1 = { 1, 2, 3 };
    unit1.list2 = { 4, 5 };
    unit1.set1 = { 6, 7, 8, 9 };
    unit1.set2 = { 10 };

    unit1.map1 = { { { 1, 3.0f }, 3 },
        { { 2, 4.0f }, 6 } };


    unit1.complexList1.emplace_back(2, 6.0f, "set");
    unit1.complexList1.emplace_back();

    const char *s = "Hello World!";
    unit1.bytes = { s, strlen(s) };

    std::unique_ptr<std::stringbuf> pBuffer = std::make_unique<std::stringbuf>();

    std::stringbuf *buffer = pBuffer.get();

    SerializeOutStream out { std::move(pBuffer), std::make_unique<SerializeStreamData>(std::make_unique<Formatter>()) };

    write(out, unit1, "unit1");

    std::string file = buffer->str();

    std::cout << file << std::endl;

    if (!out.format().mBinary)
        EXPECT_EQ(file, expected.data());

    std::cout << "Buffer-Size: " << file.size() << std::endl;

    SerializeInStream in { out.release(), std::make_unique<SerializeStreamData>(std::make_unique<Formatter>()) };

    HANDLE_STREAM_RESULT(read(in, unit2, nullptr));

    ASSERT_EQ(unit1, unit2);
}