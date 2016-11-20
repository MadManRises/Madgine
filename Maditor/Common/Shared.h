#pragma once

#include "ApplicationInfo.h"
#include "InputInfo.h"
#include "LogInfo.h"
#include "ObjectsInfo.h"
#include "StatsInfo.h"

#include <boost\interprocess\shared_memory_object.hpp>
#include <boost\interprocess\managed_shared_memory.hpp>

struct Shared {
	Shared(boost::interprocess::managed_shared_memory::segment_manager *mgr) :
		mAppInfo(mgr),
		mInput(mgr),
		mLog(mgr),
		mObjects(mgr),
		mStats(mgr)
	{}

	ApplicationInfo mAppInfo;
	InputShared mInput;
	SharedLog mLog;
	ObjectsShared mObjects;
	SharedStats mStats;
};

struct create_t {};
static constexpr create_t create;
struct open_t {};
static constexpr open_t open;

class SharedMemory {
public:
	SharedMemory(create_t) :
		mMemory((boost::interprocess::shared_memory_object::remove("Maditor_Memory"), boost::interprocess::create_only), "Maditor_Memory", 1000000),
		mCreate(true)
		{

		msSingleton = this;

		mData = mMemory.construct<Shared>("SharedData")(mMemory.get_segment_manager());
	}
	SharedMemory(open_t) :
		mMemory(boost::interprocess::open_only, "Maditor_Memory"),
		mCreate(false)
	{
		msSingleton = this;

		mData = mMemory.find<Shared>("SharedData").first;
	}

	~SharedMemory() {
		if (mCreate)
			boost::interprocess::shared_memory_object::remove("Maditor_Memory");
	}

	static Shared &getSingleton() {
		return *msSingleton->mData;
	}

	static boost::interprocess::managed_shared_memory::segment_manager *mgr() {
		return msSingleton->mMemory.get_segment_manager();
	}

private:
	Shared *mData;
	static SharedMemory *msSingleton;

	boost::interprocess::managed_shared_memory mMemory;

	bool mCreate;
};

