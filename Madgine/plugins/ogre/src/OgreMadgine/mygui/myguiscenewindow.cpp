#include "../ogrelib.h"

#include "myguiscenewindow.h"

#include "myguisystem.h"
#include "../scene/ogrecamera.h"

namespace Engine
{
	namespace GUI
	{
		MyGUISceneWindow::MyGUISceneWindow(const std::string& name, MyGUIWindow* parent) :
			MyGUIWindow(name, parent),
			Window(name, parent),
			SceneWindow(name, parent)
		{
			mImageBox = parent->widget()->createWidget<MyGUI::ImageBox>("Imagebox", 0, 0, 0, 0, MyGUI::Align::Default, name);
			setup();
		}

		MyGUISceneWindow::MyGUISceneWindow(const std::string& name, MyGUISystem& system) :
		MyGUIWindow(name, system),
		Window(name, system),
		SceneWindow(name, system)
		{
			mImageBox = system.rootWidget()->createWidget<MyGUI::ImageBox>("Imagebox", 0, 0, 0, 0, MyGUI::Align::Default, name);
			setup();
		}

		MyGUISceneWindow::~MyGUISceneWindow()
		{
			Ogre::TextureManager::getSingleton().remove(mTexture);
			mTexture.reset();
		}

		Class MyGUISceneWindow::getClass()
		{
			return Class::SCENEWINDOW_CLASS;
		}

		void MyGUISceneWindow::setCamera(Scene::Camera* camera)
		{
			mCamera = dynamic_cast<Scene::OgreCamera *>(camera)->getCamera();

			mVp = mRenderTexture->addViewport(mCamera);
			mVp->setOverlaysEnabled(false);
			mVp->setClearEveryFrame(true);
			mVp->setBackgroundColour(Ogre::ColourValue::Black);
		}

		void MyGUISceneWindow::setup()
		{
			mTexture = Ogre::TextureManager::getSingleton().createManual(
				"RTT",
				Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
				Ogre::TEX_TYPE_2D,
				256,
				256,
				0,
				Ogre::PF_R8G8B8,
				Ogre::TU_RENDERTARGET);

			mRenderTexture = mTexture->getBuffer()->getRenderTarget();
		}

		MyGUI::Widget* MyGUISceneWindow::widget() const
		{
			return mImageBox;
		}

		void MyGUISceneWindow::setGameTextureSize(const Vector2& size)
		{
			if (size.x <= 0 || size.y <= 0)
				return;
			if (mTexture->getWidth() == size.x && mTexture->getHeight() == size.y)
				return;
			if (mCamera)
				mCamera->setAspectRatio(size.x / size.y);

			unsigned int width = static_cast<unsigned int>(size.x);
			unsigned int height = static_cast<unsigned int>(size.y);			

			Ogre::TextureManager::getSingleton().remove(mTexture);			

			mTexture = Ogre::TextureManager::getSingleton().createManual(
				"RTT",
				Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
				Ogre::TEX_TYPE_2D,
				width,
				height,
				0,
				Ogre::PF_R8G8B8,
				Ogre::TU_RENDERTARGET);

			//mRoot->getRenderSystem()->_cleanupDepthBuffers(false);

			mRenderTexture = mTexture->getBuffer()->getRenderTarget();

			if (mCamera) {
				mVp = mRenderTexture->addViewport(mCamera);
				mVp->setOverlaysEnabled(false);
				mVp->setClearEveryFrame(true);
				mVp->setBackgroundColour(Ogre::ColourValue::Black);
			}
			
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
