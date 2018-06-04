#pragma once

#include "applicationinfo.h"
#include "inputinfo.h"


namespace Maditor {
	namespace Shared {


		struct Shared {
			Shared(boost::interprocess::managed_shared_memory::segment_manager *mgr);

			InputShared mInput;
		};


class MADITOR_SHARED_EXPORT SharedMemory {
public:

	struct create_t {};
	static constexpr create_t create = create_t();
	struct open_t {};
	static constexpr open_t open = open_t();

	SharedMemory();//create
	SharedMemory(size_t id);//open

	~SharedMemory();

	boost::interprocess::managed_shared_memory::segment_manager *mgr();
	boost::interprocess::managed_shared_memory &memory();

	Shared &data();

	size_t id();

	std::string uniqueName();

private:
	static size_t sRunningId;

	size_t mId;
	
	Shared *mData;

	boost::interprocess::managed_shared_memory mMemory;

	bool mCreate;

	
};


	}
}
