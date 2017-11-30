#pragma once

#include "gui/windows/texturedrawer.h"

namespace MyGUI
{
	class ImageBox;
}

namespace Engine
{
	namespace GUI
	{
		namespace MyGui
		{
			class MyGUIWindow;

			class MyGUITextureDrawer : public TextureDrawer
			{
			public:
				MyGUITextureDrawer(MyGUIWindow* w);

				// Inherited via TextureDrawer
				void setTexture(const Ogre::TexturePtr& tex) override;
				void setTexture(const std::string& name) override;

			private:
				MyGUI::ImageBox* mTextureDrawer;
			};
		}
	}
}
