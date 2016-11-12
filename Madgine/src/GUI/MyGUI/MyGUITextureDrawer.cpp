#include "libinclude.h"

#include "MyGUITextureDrawer.h"
#include "MyGUIWindow.h"



#ifdef _MSC_VER
#pragma warning (push, 0)
#endif
#include <MYGUI\MyGUI.h>
#include <MYGUI\MyGUI_OgreRenderManager.h>
#include <MYGUI\MyGUI_OgreTexture.h>
#ifdef _MSC_VER
#pragma warning (pop)
#endif


namespace Engine {
	namespace GUI {
		namespace MyGui {


			MyGUITextureDrawer::MyGUITextureDrawer(MyGUIWindow *w) :
				TextureDrawer(w),
				mTextureDrawer(w->window()->castType<MyGUI::ImageBox>())
			{
				mTextureDrawer->detachFromLayer();
			}


			void MyGUITextureDrawer::onTextureChanged(Ogre::TexturePtr &tex)
			{

				if (tex.isNull()) {
					mTextureDrawer->setImageTexture("");
				}
				else {
					MyGUI::OgreRenderManager &rm = MyGUI::OgreRenderManager::getInstance();
					MyGUI::OgreTexture *myGUITex = static_cast<MyGUI::OgreTexture*>(rm.getTexture(tex->getName()));
					if (myGUITex) {
						if (myGUITex->getOgreTexture() != tex)
							rm.destroyTexture(myGUITex);
					}
					mTextureDrawer->setImageTexture(tex->getName());
				}
				
			}
		}
	}
}