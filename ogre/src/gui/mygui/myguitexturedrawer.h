#pragma once

#include "GUI/Windows/TextureDrawer.h"

namespace MyGUI {
	class ImageBox;
}

namespace Engine {
	namespace GUI {
		namespace MyGui {

			class MyGUIWindow;

			class MyGUITextureDrawer : public TextureDrawer {
			public:
				MyGUITextureDrawer(MyGUIWindow *w);
				
				// Inherited via TextureDrawer
				virtual void setTexture(const Ogre::TexturePtr &tex) override;
				virtual void setTexture(const std::string &name) override;			

			private:
				MyGUI::ImageBox *mTextureDrawer;
			};
		}
	}
}