#pragma once

#include "GUI/Windows/TextureDrawer.h"
#include "MyGUIWindow.h"

namespace Engine {
	namespace GUI {
		namespace MyGui {
			class MyGUITextureDrawer : public TextureDrawer {
			public:
				MyGUITextureDrawer(MyGUIWindow *w);
				
				// Inherited via TextureDrawer
				virtual void onTextureChanged(Ogre::TexturePtr &tex) override;

			private:
				MyGUI::ImageBox *mTextureDrawer;
			};
		}
	}
}