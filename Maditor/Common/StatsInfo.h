#pragma once

#include "SharedBase.h"

struct FrameStats {

};

struct ProcessStats;
typedef boost::interprocess::allocator<ProcessStats,
	boost::interprocess::managed_shared_memory::segment_manager> SharedProcessStatsAllocator;


struct ProcessStats {
	ProcessStats(boost::interprocess::managed_shared_memory::segment_manager *mgr) :
		mName(SharedCharAllocator(mgr)),
		mChildren(mgr) 
	{
	}

	long long mAverageDuration;
	SharedString mName;

	SharedList<ProcessStats> mChildren;
};

struct SharedStats {
	SharedStats(boost::interprocess::managed_shared_memory::segment_manager *mgr) :
		mRootProcess(mgr)
	{}

	boost::interprocess::interprocess_mutex mMutex;
	ProcessStats mRootProcess;
};