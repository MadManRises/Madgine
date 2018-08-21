#pragma once

#include "Madgine/gui/widgets/scenewindow.h"
#include "myguiwidget.h"

namespace Engine
{
	namespace GUI
	{

		class OGREMADGINE_EXPORT MyGUISceneWindow : public SceneWindow, public MyGUIWidget
		{
		public:
			MyGUISceneWindow(const std::string &name, MyGUIWidget *parent);
			MyGUISceneWindow(const std::string &name, MyGUITopLevelWindow &system);
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