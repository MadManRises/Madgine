#pragma once

#include "SharedBase.h"

#include "Util/objectstate.h"



struct ObjectInfo {
	ObjectInfo(boost::interprocess::managed_shared_memory::segment_manager *mgr) :
		mName(SharedCharAllocator(mgr)) {}

	SharedString mName;
	Engine::Util::ObjectState mState;
	void *mPtr;
};


struct ObjectsShared {
	ObjectsShared(boost::interprocess::managed_shared_memory::segment_manager *mgr) :
		mObjects(mgr) {

	}

	bool mDataChanged;
	boost::interprocess::interprocess_mutex mMutex;
	SharedList<ObjectInfo> mObjects;

};
