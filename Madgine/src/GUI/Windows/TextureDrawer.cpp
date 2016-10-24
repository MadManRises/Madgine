#include "libinclude.h"

#include "TextureDrawer.h"

#include "Scene\TextureComponent.h"

namespace Engine {
	namespace GUI {
		void Engine::GUI::TextureDrawer::setTexture(Scene::TextureComponent & tex)
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

