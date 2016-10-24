
#include "scenetexturecomponent.h"

#include "scenemanager.h"

#include "Math\bounds.h"

namespace Engine {
namespace Scene {



	BaseSceneTextureComponent::BaseSceneTextureComponent(const std::string & textureName, Ogre::PixelFormat pixelformat, int textureUsage) :
		mTextureName(textureName),
		mPixelFormat(pixelformat),
		mTextureUsage(textureUsage)
	{

	}

	size_t BaseSceneTextureComponent::width() const {
		return mWidth;
	}

	size_t BaseSceneTextureComponent::height() const {
		return mHeight;
	}

	void BaseSceneTextureComponent::updateTexture(SceneManager * sceneMgr) {
		const Engine::Math::Bounds &rasterizedBounds = sceneMgr->getRasterizedSceneBounds();
		mWidth = rasterizedBounds.width() + 1;
		mHeight = rasterizedBounds.height() + 1;

		clear();

		set(Ogre::TextureManager::getSingleton().createManual(
			mTextureName, // Name of texture
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, // Name of resource group in which the texture should be created
			Ogre::TEX_TYPE_2D, // Texture type
			mWidth, // Width
			mHeight, // Height
			0, // Number of mipmaps
			mPixelFormat, // Pixel format
			mTextureUsage // usage
		));

		const Ogre::PixelBox &pb = lock();

		memset(pb.data, 0, Ogre::PixelUtil::getMemorySize(mWidth, mHeight, 1, mPixelFormat));

		unlock();
	}

	float BaseSceneTextureComponent::aspectRatio() const {
		return float(mWidth) / mHeight;
	}

	size_t BaseSceneTextureComponent::clamp(size_t x, size_t min, size_t max) {
		return std::min(max, std::max(min, x));
	}

	size_t BaseSceneTextureComponent::index(size_t column, size_t line)
	{
		return clamp(line, 0, mHeight - 1) * mWidth + clamp(column, 0, mWidth - 1);
	}

} // namespace Scene
} // namespace Core

