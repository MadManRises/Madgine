#include "libinclude.h"

#include "TextureDrawer.h"

#include "Resources\TextureComponent.h"

namespace Engine {
	namespace GUI {
		TextureDrawer::TextureDrawer(WindowContainer * win) :
			Window(win),
			mCurrentTextureComponent(0)
		{
		}
		TextureDrawer::~TextureDrawer()
		{
			clearTextureComponent();
		}
		void TextureDrawer::setTexture(Resources::TextureComponent & tex)
		{
			clearTextureComponent();
			tex.addListener(this);
			mCurrentTextureComponent = &tex;
			onTextureChanged(tex.texture());
		}

		void TextureDrawer::setTexture(Ogre::TexturePtr & tex)
		{
			clearTextureComponent();
			onTextureChanged(tex);
		}

		void TextureDrawer::clearTexture()
		{
			clearTextureComponent();
			setTexture(Ogre::TexturePtr());
		}

		void TextureDrawer::clearTextureComponent()
		{
			if (mCurrentTextureComponent) {
				mCurrentTextureComponent->removeListener(this);
				mCurrentTextureComponent = 0;				
			}
		}

	}
}

