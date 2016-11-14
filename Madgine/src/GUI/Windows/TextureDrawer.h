#pragma once

#include "window.h"

namespace Engine {
	namespace GUI {

		class MADGINE_EXPORT TextureDrawer : public Window
		{
		public:
			using Window::Window;
			virtual ~TextureDrawer() = default;
			
			virtual void setTexture(Ogre::TexturePtr &tex) = 0;
			inline void clearTexture() {
				setTexture(Ogre::TexturePtr());
			}
		};

	}
}