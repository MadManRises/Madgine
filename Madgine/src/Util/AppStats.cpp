#include "madginelib.h"

#include "AppStats.h"

#include <iostream>


namespace Engine {
	namespace Util {

		AppStats::AppStats(Ogre::RenderWindow *window) :
			mAverageFPS(this),
#if OGRE_MEMORY_TRACKER
			mOgreMemory(this),
			startTrack(this, &AppStats::startTrackImpl),
			stopTrack(this, &AppStats::stopTrackImpl),
			mTracker(static_cast<TrackerAccessor&>(Ogre::MemoryTracker::get())),
#endif
			mWindow(window)
			
		{
		}

		void AppStats::update()
		{
			mAverageFPS = mWindow->getAverageFPS();
#if OGRE_MEMORY_TRACKER
			mOgreMemory = mTracker.getTotalMemoryAllocated();
#endif
		}

		void AppStats::startTrackImpl()
		{
			mMemoryImage = mTracker.mAllocations;
		}

		void AppStats::stopTrackImpl()
		{
			std::list<TrackerAccessor::Alloc> newAllocations;

			for (const std::pair<void * const, TrackerAccessor::Alloc> &p : mTracker.mAllocations) {
				if (mMemoryImage.count(p.first) == 0) {
					newAllocations.push_back(p.second);
				}
			}


			for (const TrackerAccessor::Alloc &alloc : newAllocations) {
				if (!alloc.filename.empty())
					std::cout << alloc.filename;
				else
					std::cout << "(unknown source):";

				std::cout << "(" << alloc.line << ") : {" << alloc.bytes << " bytes}" << " function: " << alloc.function << std::endl;
			}

		}
		
	}
}