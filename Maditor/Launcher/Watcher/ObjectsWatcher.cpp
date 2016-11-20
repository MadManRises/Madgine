#include "madgineinclude.h"

#include "ObjectsWatcher.h"


#include "Shared.h"

			ObjectsWatcher::ObjectsWatcher() :
				mShared(SharedMemory::getSingleton().mObjects)
			{
			}


			void ObjectsWatcher::update()
			{
				boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(mShared.mMutex);

				for (ObjectInfo &o : mShared.mObjects) {
					o.mState = (Engine::Util::ObjectState)-1;
				}
				for (Engine::Util::BaseMadgineObject *o : Engine::Util::MadgineObjectCollector::getSingleton()) {
					auto it = std::find_if(mShared.mObjects.begin(), mShared.mObjects.end(), [=](const ObjectInfo &ob) {return ob.mName == o->getName(); });
					if (it == mShared.mObjects.end()) {
						ObjectInfo &i = mShared.mObjects.push();
						i.mName = o->getName();
						i.mState = o->getState();
						i.mPtr = o;
					}
					else {
						it->mState = o->getState();
						it->mPtr = o;
					}
				}
				mShared.mDataChanged = true;
			}
