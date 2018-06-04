
#include "maditorsharedlib.h"

#include "sharedmemory.h"

namespace Maditor {
	namespace Shared {

		size_t SharedMemory::sRunningId = 0;

		SharedMemory::SharedMemory() :
			mId(++sRunningId),
			mMemory(
				(
					boost::interprocess::shared_memory_object::remove((std::string("Maditor_Memory_")+std::to_string(mId)).c_str()),
					boost::interprocess::create_only
				), (std::string("Maditor_Memory_") + std::to_string(mId)).c_str(), 1000000),
			mCreate(true)
		{
			mData = mMemory.construct<Shared>("SharedData")(mMemory.get_segment_manager());
		}

		SharedMemory::SharedMemory(size_t id) :
			mId(id),
			mMemory(boost::interprocess::open_only, (std::string("Maditor_Memory_") + std::to_string(mId)).c_str()),
			mCreate(false)
		{
			mData = mMemory.find<Shared>("SharedData").first;
		}
		SharedMemory::~SharedMemory() {
			if (mCreate)
				boost::interprocess::shared_memory_object::remove((std::string("Maditor_Memory_") + std::to_string(mId)).c_str());
		}
		boost::interprocess::managed_shared_memory::segment_manager * SharedMemory::mgr() {
			return mMemory.get_segment_manager();
		}
		boost::interprocess::managed_shared_memory & SharedMemory::memory()
		{
			return mMemory;
		}
		Shared & SharedMemory::data() {
			return *mData;
		}
		size_t SharedMemory::id()
		{
			return mId;
		}
		std::string SharedMemory::uniqueName()
		{
			return "Mem_" + std::to_string(mId) + "_";
		}
		Shared::Shared(boost::interprocess::managed_shared_memory::segment_manager * mgr) :
			mInput(mgr)
		{}
	}
}
