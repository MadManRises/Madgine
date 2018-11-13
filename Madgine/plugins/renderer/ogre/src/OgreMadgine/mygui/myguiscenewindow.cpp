#include "../ogrelib.h"

#include "myguiscenewindow.h"

#include "myguitoplevelwindow.h"
#include "../scene/ogrecamera.h"

namespace Engine
{
	namespace GUI
	{
		MyGUISceneWindow::MyGUISceneWindow(const std::string& name, MyGUIWidget* parent) :
			MyGUIWidget(name, parent),
			Widget(name, parent),
			SceneWindow(name, parent)
		{
			mImageBox = parent->widget()->createWidget<MyGUI::ImageBox>("ImageBox", 0, 0, 0, 0, MyGUI::Align::Default, name);
			MyGUIWidget::setup();
		}

		MyGUISceneWindow::MyGUISceneWindow(const std::string& name, MyGUITopLevelWindow& window) :
		MyGUIWidget(name, window),
			Widget(name, window),
		SceneWindow(name, window)
		{
			mImageBox = window.rootWidget()->createWidget<MyGUI::ImageBox>("ImageBox", 0, 0, 0, 0, MyGUI::Align::Default, name);
			MyGUIWidget::setup();
		}

		MyGUISceneWindow::~MyGUISceneWindow()
		{
			if (mTexture) {
				Ogre::TextureManager::getSingleton().remove(mTexture);
				mTexture.reset();
			}
		}

		Class MyGUISceneWindow::getClass()
		{
			return Class::SCENEWINDOW_CLASS;
		}

		void MyGUISceneWindow::setCamera(Scene::Camera* camera)
		{
			mCamera = dynamic_cast<Scene::OgreCamera *>(camera)->getCamera();

			setup();

		}

		void MyGUISceneWindow::setup()
		{
			if (mCamera) {
				if (!mTexture) {
					mTexture = Ogre::TextureManager::getSingleton().createManual(
						"RTT",
						Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
						Ogre::TEX_TYPE_2D,
						getAbsoluteSize().x,
						getAbsoluteSize().y,
						0,
						Ogre::PF_R8G8B8,
						Ogre::TU_RENDERTARGET);

					MyGUI::OgreRenderManager* mgr = &MyGUI::OgreRenderManager::getInstance();
					MyGUI::OgreTexture* myguitex = static_cast<MyGUI::OgreTexture*>(mgr->getTexture(mTexture->getName()));
					if (myguitex)
					{
						if (myguitex->getOgreTexture() != mTexture)
							myguitex->setOgreTexture(mTexture);
					}

					mRenderTexture = mTexture->getBuffer()->getRenderTarget();

					mImageBox->setImageTexture("RTT");

					mVp = mRenderTexture->addViewport(mCamera);
					mVp->setOverlaysEnabled(false);
					mVp->setClearEveryFrame(true);
					mVp->setBackgroundColour(Ogre::ColourValue::Black);
				}
			}
		}

		MyGUI::Widget* MyGUISceneWindow::widget() const
		{
			return mImageBox;
		}

		void MyGUISceneWindow::setGameTextureSize()
		{
			Vector2 size = getAbsoluteSize().xy();
			if (mTexture && (mTexture->getWidth() != size.x || mTexture->getHeight() != size.y)) {
				Ogre::TextureManager::getSingleton().remove(mTexture);
				mTexture.reset();
			}
			if (size.x > 0 && size.y > 0) {	
				setup();				
			}
		}


		Ogre::Ray MyGUISceneWindow::mousePointToRay(const Vector2& mousePos) const
		{
			return mCamera->getCameraToViewportRay(mousePos.x, mousePos.y);
		}

		/*
		Ogre::Image OgreSceneRenderer::getScreenshot() const
		{
			Ogre::Image image;
			mTexture->convertToImage(image);
			return image;
		}
		*/

	}
}
