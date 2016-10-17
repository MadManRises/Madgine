#include "libinclude.h"

#include "MyGUITextureDrawer.h"
#include "MyGUIWindow.h"

#pragma warning (push, 0)
#include <MYGUI\MyGUI.h>
#include <MYGUI\MyGUI_OgreRenderManager.h>
#include <MYGUI\MyGUI_OgreTexture.h>
#pragma warning (pop)

namespace Engine {
	namespace GUI {
		namespace MyGui {


			MyGUITextureDrawer::MyGUITextureDrawer(MyGUIWindow *w) :
				TextureDrawer(w),
				mTextureDrawer(static_cast<MyGUI::ImageBox*>(w->window()))
			{
				assert(mTextureDrawer->isType<MyGUI::ImageBox>());
				mTextureDrawer->detachFromLayer();
			}


			void MyGUITextureDrawer::onTextureChanged(Ogre::TexturePtr &tex)
			{
				

				if (!mTextureDrawer->_getTextureName().empty()) {
					MyGUI::OgreRenderManager &rm = MyGUI::OgreRenderManager::getInstance();
					MyGUI::OgreTexture *myGUITex = static_cast<MyGUI::OgreTexture*>(rm.getTexture(mTextureDrawer->_getTextureName()));
					if (myGUITex->getOgreTexture() == tex) return;
					rm.destroyTexture(myGUITex);
				}
				mTextureDrawer->setImageTexture(tex.isNull() ? "" : tex->getName());
			}
		}
	}
}