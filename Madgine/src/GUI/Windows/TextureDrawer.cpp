#include "libinclude.h"

#include "TextureDrawer.h"

#include "OGRE\TextureComponent.h"

namespace Engine {
	namespace GUI {
		void Engine::GUI::TextureDrawer::setTexture(OGRE::TextureComponent & tex)
		{
			tex.addListener(this);
			if (!tex.isNull())
				onTextureChanged(tex.texture());
		}

		void TextureDrawer::setTexture(Ogre::TexturePtr & tex)
		{
			onTextureChanged(tex);
		}

	}
}

