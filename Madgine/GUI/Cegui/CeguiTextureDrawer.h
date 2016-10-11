#pragma once

#include "CeguiWindow.h"
#include "GUI/Windows/TextureDrawer.h"

namespace Engine {
	namespace GUI {
		namespace Cegui {
			class CeguiTextureDrawer : public TextureDrawer {
			public:
				CeguiTextureDrawer(Window *w, CEGUILauncher *cegui);



				// Inherited via TextureDrawer
				virtual void onTextureChanged(Ogre::TexturePtr &tex) override;

			private:
				CEGUILauncher *mCegui;
				CEGUI::BasicImage *mImage;
			};
		}
	}
}