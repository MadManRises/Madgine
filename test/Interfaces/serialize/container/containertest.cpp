#include <gtest/gtest.h>

#include "Modules/moduleslib.h"

#include "Modules/serialize/container/list.h"
#include "Modules/serialize/serializableunit.h"

#include "Modules/threading/workgroup.h"

#include "Modules/serialize/serializemanager.h"

#include "Modules/serialize/toplevelserializableunit.h"

#include "Modules/serialize/container/noparent.h"

#include "Modules/serialize/streams/buffered_streambuf.h"

struct Buffer {
	std::array<char, 2048> mBuffer[2];
	size_t mWrittenCount[2] = { 0,0 };
};

struct TestBuf : Engine::Serialize::buffered_streambuf
{
	TestBuf(Buffer &buffer, Engine::Serialize::SerializeManager &mgr, Engine::Serialize::ParticipantId id) :
		buffered_streambuf(mgr, id),
		mBuffer(buffer)
	{

	}

	virtual Engine::StreamError getError() {
		return Engine::WOULD_BLOCK;
	}

	virtual int recv(char* buffer, size_t count)
	{
		size_t index = isMaster() ? 0 : 1;

		size_t avail = mBuffer.mWrittenCount[index] - mReadOffset;
		size_t readCount = std::min(count, avail);
		if (readCount == 0)
			return -1;
		std::memcpy(buffer, mBuffer.mBuffer[index].data() + mReadOffset, readCount);
		mReadOffset += readCount;
		return static_cast<int>(readCount);
	}

	virtual int send(char* buffer, size_t count)
	{
		size_t index = isMaster() ? 1 : 0;

		assert(mBuffer.mWrittenCount[index] + count <= mBuffer.mBuffer[index].size());
		std::memcpy(mBuffer.mBuffer[index].data() + mBuffer.mWrittenCount[index], buffer, count);
		mBuffer.mWrittenCount[index] += count;
		return static_cast<int>(count);
	}

	Buffer &mBuffer;
	size_t mReadOffset = 0;
};



struct TestManager : Engine::Serialize::SerializeManager
{
	TestManager() :
		SerializeManager("Test-Manager")
	{
	}

	void setBuffer(Buffer &buffer, bool slave)
	{
		if (slave)
			setSlaveStream(Engine::Serialize::BufferedInOutStream{ std::make_unique<TestBuf>(buffer, *this, 0) }, true, std::chrono::milliseconds{ 1000 });
		else
			addMasterStream(Engine::Serialize::BufferedInOutStream{ std::make_unique<TestBuf>(buffer, *this, 1) });
	}

};




TEST(Serialize_Container, Test1)
{
	using namespace Engine::Serialize;
	using namespace std::chrono_literals;

	Engine::Threading::WorkGroup wg;

	TestManager mgr1;
	TestManager mgr2;

	struct TestUnit : TopLevelSerializableUnit<TestUnit>
	{
		TestUnit() :
			TopLevelSerializableUnit<TestUnit>(10)
		{}

		SerializableList<int> list1;
		ObservableList<int, ContainerPolicies::allowAll> list2;
	};
	NoParentUnit<TestUnit> unit1;
	NoParentUnit<TestUnit> unit2;

	unit1.list1.push_back(3);
	unit1.list1.push_back(4);
	unit1.list1.push_back(5);


	unit1.list2.push_back(1);
	unit1.list2.push_back(2);

	mgr1.addTopLevelItem(&unit1);
	mgr2.addTopLevelItem(&unit2);

	Buffer buffer;
	mgr1.setBuffer(buffer, false);
	mgr1.sendMessages();
	mgr2.setBuffer(buffer, true);

	ASSERT_EQ(unit1.list1, unit2.list1);
	ASSERT_EQ(unit1.list2, unit2.list2);

	unit1.list2.push_back(6);
	ASSERT_EQ(unit1.list2.back(), 6);

	mgr1.sendMessages();
	mgr2.receiveMessages(1, 1000ms);

	ASSERT_EQ(unit1.list2, unit2.list2);

	unit2.list2.push_back(7);

	ASSERT_EQ(unit1.list2, unit2.list2);

	mgr2.sendMessages();
	mgr1.receiveMessages(1, 1000ms);

	ASSERT_EQ(unit1.list2.back(), 7);

	mgr1.sendMessages();
	mgr2.receiveMessages(1, 1000ms);

	ASSERT_EQ(unit1.list2, unit2.list2);
}
