#pragma once


namespace Engine
{
	namespace Resources
	{
		namespace ImageSets
		{
			class OGREMADGINE_EXPORT ImageSetManager : public Ogre::Singleton<ImageSetManager>,
			                                           public Ogre::GeneralAllocatedObject
			{
			public:
				Ogre::TexturePtr getImageTexture(const std::string& setName, const std::string& imageName);

			private:
				std::map<std::string, std::map<std::string, Ogre::TexturePtr>> mTextures;
			};
		}
	}
}
