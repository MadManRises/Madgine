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
				virtual void setTexture(const Ogre::TexturePtr &tex) override;
				virtual void setTexture(const std::string &name) override;			

			private:
				MyGUI::ImageBox *mTextureDrawer;
			};
		}
	}
}