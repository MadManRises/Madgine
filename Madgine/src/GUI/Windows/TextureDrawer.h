#pragma once

#include "window.h"
#include "Scene\texturelistener.h"

namespace Engine {
	namespace GUI {

		class MADGINE_EXPORT TextureDrawer : public Window, public Scene::TextureListener
		{
		public:
			using Window::Window;
			virtual ~TextureDrawer() = default;

			void setTexture(Scene::TextureComponent &tex);
			void setTexture(Ogre::TexturePtr &tex);

			// Inherited via TextureListener
			virtual void onTextureChanged(Ogre::TexturePtr &tex) = 0;
		};

	}
}