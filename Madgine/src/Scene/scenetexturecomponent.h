#pragma once

#include "scenecomponent.h"

#include "TextureComponent.h"

namespace Engine {
	namespace Scene {

		class MADGINE_EXPORT BaseSceneTextureComponent : public TextureComponent
		{
		public:
			BaseSceneTextureComponent(const std::string &textureName, Ogre::PixelFormat pixelformat, int textureUsage = Ogre::TU_DYNAMIC_WRITE_ONLY);
			
			size_t width() const;
			size_t height() const;

		protected:

			void updateTexture(SceneManager *sceneMgr);


			float aspectRatio() const;

			size_t clamp(size_t x, size_t min, size_t max);

			size_t index(size_t column, size_t line);

		private:

			const std::string mTextureName;
			const Ogre::PixelFormat mPixelFormat;
			const int mTextureUsage;

			Ogre::uint mWidth, mHeight;



		};

		template <class T>
		class SceneTextureComponent : public SceneComponent<T>, public BaseSceneTextureComponent {
		public:
			using BaseSceneTextureComponent::BaseSceneTextureComponent;

			virtual void onSceneLoad() override
			{
				updateTexture(this->sceneMgr());
			}
		};

	} // namespace Scene
} // namespace Core

