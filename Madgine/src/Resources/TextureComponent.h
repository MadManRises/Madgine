#pragma once

namespace Engine {
	namespace Resources {
		class MADGINE_EXPORT TextureComponent {
		public:
			TextureComponent();

			void addListener(TextureListener *listener);
			void removeListener(TextureListener *listener);

			const Ogre::PixelBox & lock();

			void unlock();

			void operator= (const Ogre::TexturePtr &tex);
			void set(const Ogre::TexturePtr &tex);
			operator const Ogre::TexturePtr &();

			Ogre::TexturePtr &texture();

			bool isNull();

			void clear();

		private:
			Ogre::TexturePtr mTexture;

			std::list<TextureListener *> mListeners;
		};
	}
}