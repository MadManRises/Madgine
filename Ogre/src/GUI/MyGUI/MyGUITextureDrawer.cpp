#include "madginelib.h"

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


			void MyGUITextureDrawer::setTexture(const Ogre::TexturePtr &tex)
			{
				if (tex.isNull()) {
					mTextureDrawer->setImageTexture("");
				}
				else {
					MyGUI::OgreRenderManager *mgr = &MyGUI::OgreRenderManager::getInstance();
					MyGUI::OgreTexture *myguitex = static_cast<MyGUI::OgreTexture*>(mgr->getTexture(tex->getName()));
					if (myguitex) {
						if (myguitex->getOgreTexture() != tex)
							myguitex->setOgreTexture(tex);
					}
					mTextureDrawer->setImageTexture(tex->getName());
				}
				
			}
			void MyGUITextureDrawer::setTexture(const std::string & name)
			{
				Ogre::TexturePtr tex = Ogre::TextureManager::getSingleton().getByName(name);
				if (tex.isNull())
					mTextureDrawer->setImageTexture(name);
				else
					setTexture(tex);
			}

		}
	}
}