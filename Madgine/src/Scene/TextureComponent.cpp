#include "libinclude.h"

#include "TextureComponent.h"

#include "texturelistener.h"

namespace Engine {
	namespace Scene {
		TextureComponent::TextureComponent()
		{
		}

		void TextureComponent::addListener(TextureListener * listener)
		{
			mListeners.push_back(listener);
		}

		const Ogre::PixelBox &TextureComponent::lock() {
			mTexture->getBuffer()->lock(Ogre::HardwareBuffer::HBL_DISCARD);
			return mTexture->getBuffer()->getCurrentLock();
		}

		void TextureComponent::unlock() {
			mTexture->getBuffer()->unlock();
		}

		void TextureComponent::operator=(const Ogre::TexturePtr &tex) {
			set(tex);
		}

		void TextureComponent::set(const Ogre::TexturePtr & tex)
		{
			mTexture = tex;
			for (TextureListener *listener : mListeners) {
				listener->onTextureChanged(mTexture);
			}
		}

		TextureComponent::operator const Ogre::TexturePtr&()
		{
			return mTexture;
		}

		Ogre::TexturePtr & TextureComponent::texture()
		{
			return mTexture;
		}

		bool TextureComponent::isNull()
		{
			return mTexture.isNull();
		}

		void TextureComponent::clear()
		{
			mTexture.setNull();
			for (TextureListener *listener : mListeners) {
				listener->onTextureChanged(mTexture);
			}
		}



	}
}