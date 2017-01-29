#pragma once

#include "Serialize\serializableunit.h"
#include "Serialize\Container\observed.h"
#include "Serialize\Container\action.h"

#define protected public
#include "OgreMemoryTracker.h"
#undef protected

namespace Engine {
	namespace Util {

		class MADGINE_EXPORT AppStats : public Serialize::SerializableUnit {
		public:
			AppStats(Ogre::RenderWindow *window);

			
			void update();

		protected:
			void startTrackImpl();
			void stopTrackImpl();

		private:
			Engine::Serialize::Observed<float> mAverageFPS;
			Engine::Serialize::Observed<size_t> mOgreMemory;
			Engine::Serialize::Action<Engine::Serialize::ActionPolicy::standard> startTrack, stopTrack;

			Ogre::MemoryTracker::AllocationMap mMemoryImage;
			Ogre::MemoryTracker &mTracker;

			Ogre::RenderWindow *mWindow;

		};

	}
}
