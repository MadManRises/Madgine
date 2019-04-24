#include <gtest/gtest.h>

#include "Interfaces/interfaceslib.h"

#include "Interfaces/serialize/container/list.h"
#include "Interfaces/serialize/serializableunit.h"

#include "Interfaces/threading/workgroup.h"

#include "Interfaces/serialize/serializemanager.h"

#include "Interfaces/serialize/toplevelserializableunit.h"

#include "Interfaces/serialize/container/noparent.h"

#include "Interfaces/serialize/streams/buffered_streambuf.h"

TEST(Serialize_Container, Test1)
{
	using namespace Engine::Serialize;

	Engine::Threading::WorkGroup wg;


	struct TestManager : SerializeManager
	{
		TestManager() :
			SerializeManager("Test-Manager")
		{
		}

		using SerializeManager::addMasterStream;
		using SerializeManager::setSlaveStream;

	};
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

	SerializableList<int> &list1 = unit1.list1;
	list1.push_back(3);
	list1.push_back(4);
	list1.push_back(5);

	ObservableList<int, ContainerPolicies::allowAll>& list2 = unit1.list2;

	list2.push_back(1);
	list2.push_back(2);

	mgr1.addTopLevelItem(&unit1);
	mgr2.addTopLevelItem(&unit2);

	struct Buffer {
		std::array<char, 2048> buffer;
		size_t mWrittenCount = 0;
	};
	Buffer buffer;

	struct TestBuf : buffered_streambuf
	{
		TestBuf(Buffer &buffer, TestManager &mgr, ParticipantId id) :
			buffered_streambuf(mgr, id),
			mBuffer(buffer)
		{

		}

		virtual StreamError getError() {
			return WOULD_BLOCK;
		}

		virtual int recv(char* buffer, size_t count)
		{
			size_t avail = mBuffer.mWrittenCount - mReadOffset;
			size_t readCount = std::min(count, avail);
			if (readCount == 0)
				return -1;
			std::memcpy(buffer, mBuffer.buffer.data() + mReadOffset, readCount);
			mReadOffset += readCount;
			return static_cast<int>(readCount);
		}

		virtual int send(char* buffer, size_t count)
		{
			assert(mBuffer.mWrittenCount + count <= mBuffer.buffer.size());
			std::memcpy(mBuffer.buffer.data() + mBuffer.mWrittenCount, buffer, count);
			mBuffer.mWrittenCount += count;
			return static_cast<int>(count);
		}

		Buffer &mBuffer;
		size_t mReadOffset = 0;
	};

	mgr1.addMasterStream(BufferedInOutStream{ std::make_unique<TestBuf>(buffer, mgr1, 1) });
	mgr1.sendMessages();
	mgr2.setSlaveStream(BufferedInOutStream{ std::make_unique<TestBuf>(buffer, mgr2, 0) });

	ASSERT_EQ(unit1.list1, unit2.list1);
	ASSERT_EQ(unit1.list2, unit2.list2);

}
