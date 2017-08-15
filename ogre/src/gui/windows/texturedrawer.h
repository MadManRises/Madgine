#pragma once

#include "window.h"

#include "signalslot/slot.h"

#include "resources/ogretextureptr.h"

namespace Engine {
	namespace GUI {

		class OGREMADGINE_EXPORT TextureDrawer : public Window
		{
		public:
			TextureDrawer(WindowContainer *w) :
				Window(w),
				mTextureSlot(this) {}

			virtual ~TextureDrawer() = default;
			

			virtual void setTexture(const Ogre::TexturePtr &tex) = 0;
			virtual void setTexture(const std::string &name) = 0;
			inline void clearTexture() {
				setTexture(Ogre::TexturePtr());
			}
			inline void setTexture(Resources::OgreTexturePtr &tex) {
				mTextureSlot.disconnectAll();
				setTexture(static_cast<const Ogre::TexturePtr&>(tex));
				tex.connect(mTextureSlot);
			}

		protected:
			void updateTexture(const Ogre::TexturePtr &tex) {
				setTexture(tex);
			}

		private:
			SignalSlot::Slot<decltype(&TextureDrawer::updateTexture), &TextureDrawer::updateTexture> mTextureSlot;
		};

	}
}