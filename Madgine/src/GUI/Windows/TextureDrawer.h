#pragma once

#include "window.h"
#include "Resources\texturelistener.h"

namespace Engine {
	namespace GUI {

		class MADGINE_EXPORT TextureDrawer : public Window, public Resources::TextureListener
		{
		public:
			TextureDrawer(WindowContainer *win);
			virtual ~TextureDrawer();

			void setTexture(Resources::TextureComponent &tex);
			void setTexture(Ogre::TexturePtr &tex);
			void clearTexture();

			// Inherited via TextureListener
			virtual void onTextureChanged(Ogre::TexturePtr &tex) = 0;

		private:
			void clearTextureComponent();

		private:
			Resources::TextureComponent *mCurrentTextureComponent;
		};

	}
}