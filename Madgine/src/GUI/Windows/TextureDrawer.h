#pragma once

#include "window.h"
#include "Resources\texturelistener.h"

namespace Engine {
	namespace GUI {

		class MADGINE_EXPORT TextureDrawer : public Window, public Resources::TextureListener
		{
		public:
			using Window::Window;
			virtual ~TextureDrawer() = default;

			void setTexture(Resources::TextureComponent &tex);
			void setTexture(Ogre::TexturePtr &tex);

			// Inherited via TextureListener
			virtual void onTextureChanged(Ogre::TexturePtr &tex) = 0;
		};

	}
}