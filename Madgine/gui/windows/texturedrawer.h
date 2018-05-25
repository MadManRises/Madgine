#pragma once

#include "window.h"

#include "../../signalslot/slot.h"

#include "../../resources/ogretextureptr.h"

namespace Engine
{
	namespace GUI
	{
		class MADGINE_CLIENT_EXPORT TextureDrawer : public Window
		{
		public:
			TextureDrawer(WindowContainer* w) :
				Window(w),
				mTextureSlot(this)
			{
			}

			virtual ~TextureDrawer() = default;


			virtual void setTexture(const Ogre::TexturePtr& tex) = 0;
			virtual void setTexture(const std::string& name) = 0;

			void clearTexture()
			{
				setTexture(Ogre::TexturePtr());
			}

			void setTexture(Resources::OgreTexturePtr& tex)
			{
				mTextureSlot.disconnectAll();
				setTexture(static_cast<const Ogre::TexturePtr&>(tex));
				tex.connect(mTextureSlot);
			}

		protected:
			void updateTexture(const Ogre::TexturePtr& tex)
			{
				setTexture(tex);
			}

		private:
			SignalSlot::Slot<decltype(&TextureDrawer::updateTexture), &TextureDrawer::updateTexture> mTextureSlot;
		};
	}
}
