#pragma once


namespace Engine {
	namespace Resources {
		namespace ImageSets {

			class MADGINE_EXPORT ImageSetManager : public Ogre::Singleton<ImageSetManager>, public Ogre::GeneralAllocatedObject {

			public:
				Ogre::TexturePtr getImageTexture(const std::string &setName, const std::string &imageName) const;

			};

		}
	}
}