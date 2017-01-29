#include "madginelib.h"

#include "AppStats.h"



namespace Engine {
	namespace Util {

		AppStats::AppStats(Ogre::RenderWindow *window) :
			mAverageFPS(this),
			mOgreMemory(this),
			startTrack(this, &AppStats::startTrackImpl),
			stopTrack(this, &AppStats::stopTrackImpl),
			mWindow(window),
			mTracker(Ogre::MemoryTracker::get())
		{
		}

		void AppStats::update()
		{
			mAverageFPS = mWindow->getAverageFPS();
			mOgreMemory = mTracker.getTotalMemoryAllocated();
		}

		void AppStats::startTrackImpl()
		{
			mMemoryImage = mTracker.mAllocations;
		}

		void AppStats::stopTrackImpl()
		{
			std::list<Ogre::MemoryTracker::Alloc> newAllocations;

			for (const std::pair<void * const, Ogre::MemoryTracker::Alloc> &p : mTracker.mAllocations) {
				if (mMemoryImage.count(p.first) == 0) {
					newAllocations.push_back(p.second);
				}
			}


			for (const Ogre::MemoryTracker::Alloc &alloc : newAllocations) {
				if (!alloc.filename.empty())
					std::cout << alloc.filename;
				else
					std::cout << "(unknown source):";

				std::cout << "(" << alloc.line << ") : {" << alloc.bytes << " bytes}" << " function: " << alloc.function << std::endl;
			}

		}
		
	}
}