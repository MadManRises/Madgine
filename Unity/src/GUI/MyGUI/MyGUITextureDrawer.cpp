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
				setTexture(tex.isNull() ? "" : tex->getName());
			}
			void MyGUITextureDrawer::setTexture(const std::string & name)
			{
				const std::string &oldName = mTextureDrawer->_getTextureName();
				if (!oldName.empty()) {
					MyGUI::OgreRenderManager *mgr = &MyGUI::OgreRenderManager::getInstance();
					mgr->destroyTexture(mgr->getTexture(oldName));
				}
				mTextureDrawer->setImageTexture(name);

			}

		}
	}
}