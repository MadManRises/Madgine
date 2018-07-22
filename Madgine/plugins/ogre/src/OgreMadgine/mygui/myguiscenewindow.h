#pragma once

#include "Madgine/gui/windows/scenewindow.h"
#include "myguiwindow.h"

namespace Engine
{
	namespace GUI
	{

		class OGREMADGINE_EXPORT MyGUISceneWindow : public SceneWindow, public MyGUIWindow
		{
		public:
			MyGUISceneWindow(const std::string &name, MyGUIWindow *parent);
			MyGUISceneWindow(const std::string &name, MyGUISystem &system);
			~MyGUISceneWindow();

			Class getClass() override;

			virtual void setCamera(Scene::Camera *camera) override;

			Ogre::Ray mousePointToRay(const Vector2& mousePos) const;

		protected:
			void setup();
			void setGameTextureSize();

			MyGUI::Widget* widget() const override;

		private:
			MyGUI::ImageBox *mImageBox;

			Ogre::TexturePtr mTexture;

			Ogre::Viewport* mVp;
			Ogre::RenderTexture* mRenderTexture;
			Ogre::Camera *mCamera;
		};

	}
}