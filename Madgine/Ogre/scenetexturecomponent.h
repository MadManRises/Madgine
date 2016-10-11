#pragma once

#include "scenecomponent.h"

#include "Math/bounds.h"

#include "scenemanager.h"

#include "TextureComponent.h"

namespace Engine {
namespace OGRE {

template <class T>
class SceneTextureComponent : public SceneComponent<T>, public TextureComponent
{
public:
    SceneTextureComponent(const std::string &textureName, Ogre::PixelFormat pixelformat, int textureUsage = Ogre::TU_DYNAMIC_WRITE_ONLY) :
        mTextureName(textureName),
        mPixelFormat(pixelformat),
        mTextureUsage(textureUsage)
    {

    }

    virtual void onSceneLoad() override
    {
        updateTexture(); 
    }

    size_t width() const{
        return mWidth;
    }
    size_t height() const{
        return mHeight;
    }

protected:

    void updateTexture(){
        Engine::Math::Bounds rasterizedBounds = sceneMgr()->getRasterizedSceneBounds();
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


    float aspectRatio() const{
        return float(mWidth) / mHeight;
    }

	size_t clamp(size_t x, size_t min, size_t max) {
		return std::min(max, std::max(min, x));
	}

    size_t index(size_t column, size_t line)
    {
        return clamp(line, 0, mHeight-1) * mWidth + clamp(column, 0, mWidth-1);
    }

private:

    const std::string mTextureName;
    const Ogre::PixelFormat mPixelFormat;
    const int mTextureUsage;

    Ogre::uint mWidth, mHeight;



};

} // namespace OGRE
} // namespace Core

