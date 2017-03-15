#pragma once

#include "Serialize\serializableunit.h"
#include "Serialize\Container\observed.h"
#include "Serialize\Container\action.h"



namespace Engine {
	namespace Util {
#if OGRE_MEMORY_TRACKER
		class TrackerAccessor : public Ogre::MemoryTracker {
		public:
			using MemoryTracker::AllocationMap;
			using MemoryTracker::Alloc;

			using MemoryTracker::mAllocations;
		};
#endif

		class OGREMADGINE_EXPORT AppStats : public Serialize::SerializableUnit<AppStats> {
		public:
			AppStats(Ogre::RenderWindow *window);

			
			void update();

#if OGRE_MEMORY_TRACKER
		protected:
			void startTrackImpl();
			void stopTrackImpl();

		private:
			
			

			TrackerAccessor::AllocationMap mMemoryImage;
			TrackerAccessor &mTracker;
			Serialize::Observed<size_t> mOgreMemory;
			Serialize::Action<decltype(&AppStats::startTrackImpl), &AppStats::startTrackImpl, Engine::Serialize::ActionPolicy::request> startTrack;
			Serialize::Action<decltype(&AppStats::stopTrackImpl), &AppStats::stopTrackImpl, Engine::Serialize::ActionPolicy::request> stopTrack;
#endif		

		private:
			Serialize::Observed<float> mAverageFPS;
			Ogre::RenderWindow *mWindow;

		};

	}
}
