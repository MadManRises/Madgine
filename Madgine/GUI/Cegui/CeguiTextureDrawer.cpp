#include "libinclude.h"
#include "CeguiTextureDrawer.h"
#include "ceguilauncher.h"

namespace Engine {
	namespace GUI {
		namespace Cegui {


			CeguiTextureDrawer::CeguiTextureDrawer(Window * w, CEGUILauncher *cegui) :
				TextureDrawer(w),
				mCegui(cegui)
			{
				mImage = (CEGUI::BasicImage *)(&CEGUI::ImageManager::getSingleton().create("BasicImage",
					name()));
				mImage->setAutoScaled(CEGUI::ASM_Both);
				getIntern<CEGUI::Window>()->setProperty("Image", name());
			}


			void CeguiTextureDrawer::onTextureChanged(Ogre::TexturePtr &tex)
			{
				CEGUI::Texture *texture = mCegui->createTexture(name(), tex, false);

				const CEGUI::Rectf rect(CEGUI::Vector2f(0.0f, 0.0f),
					texture->getOriginalDataSize());

				mImage->setTexture(texture);
				mImage->setArea(rect);
			}
		}
	}
}