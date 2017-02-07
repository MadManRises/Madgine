#include "madginelib.h"

#include "scenetexturecomponent.h"

#include "ogrescenemanager.h"

#include "Math\bounds.h"

namespace Engine {
namespace Scene {



	BaseSceneTextureComponent::BaseSceneTextureComponent(const std::string & textureName, Ogre::PixelFormat pixelformat, int textureUsage)
	{
		mTexture = Ogre::TextureManager::getSingleton().createManual(
			textureName, // Name of texture
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, // Name of resource group in which the texture should be created
			Ogre::TEX_TYPE_2D, // Texture type
			256, // Width
			256, // Height
			0, // Number of mipmaps
			pixelformat, // Pixel format
			textureUsage // usage
		);
	}

	size_t BaseSceneTextureComponent::width() const {
		return mTexture->getWidth();
	}

	size_t BaseSceneTextureComponent::height() const {
		return mTexture->getHeight();
	}

	const Ogre::PixelBox & BaseSceneTextureComponent::lock()
	{
		mTexture->getBuffer()->lock(Ogre::HardwareBuffer::HBL_DISCARD);
		return mTexture->getBuffer()->getCurrentLock();
	}

	void BaseSceneTextureComponent::unlock()
	{
		mTexture->getBuffer()->unlock();
	}

	Ogre::TexturePtr BaseSceneTextureComponent::texture()
	{
		return mTexture;
	}

	void BaseSceneTextureComponent::updateTexture(OgreSceneManager * sceneMgr) {
		const Engine::Math::Bounds &rasterizedBounds = sceneMgr->getRasterizedSceneBounds();

		mTexture->freeInternalResources();
		mTexture->setWidth((size_t)rasterizedBounds.width() + 1);
		mTexture->setHeight((size_t)rasterizedBounds.height() + 1);
		mTexture->createInternalResources();

		mTexture->getBuffer()->lock(Ogre::HardwareBuffer::HBL_DISCARD);
		const Ogre::PixelBox &pb = mTexture->getBuffer()->getCurrentLock();
		
		memset(pb.data, 0, Ogre::PixelUtil::getMemorySize(mTexture->getWidth(), mTexture->getHeight(), mTexture->getDepth(), mTexture->getFormat()));

		mTexture->getBuffer()->unlock();		
	}

	float BaseSceneTextureComponent::aspectRatio() const {
		return float(width()) / height();
	}

	size_t BaseSceneTextureComponent::clamp(size_t x, size_t min, size_t max) {
		return std::min(max, std::max(min, x));
	}

	size_t BaseSceneTextureComponent::index(size_t column, size_t line)
	{
		return clamp(line, 0, height() - 1) * width() + clamp(column, 0, width() - 1);
	}

} // namespace Scene
} // namespace Core

