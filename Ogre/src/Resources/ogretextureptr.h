#pragma once

#include "Util\Process.h"
#include "ImageSets\imagesetmanager.h"
#include "Scripting\Parsing\ogrescriptparser.h"
#include "SignalSlot\signal.h"

namespace Engine {
	namespace Resources {

		class OGREMADGINE_EXPORT OgreTexturePtr : public Ogre::TexturePtr {
		public:
		
			void operator=(const Ogre::TexturePtr &tex);

			template <class T>
			void connect(T &slot) {
				mSignal.connect(slot);
			}

		private:
			SignalSlot::Signal<const Ogre::TexturePtr &> mSignal;
		};

	}
}