#include "../clientlib.h"
#include "ogretextureptr.h"


namespace Engine
{
	namespace Resources
	{
		void OgreTexturePtr::operator=(const Ogre::TexturePtr& tex)
		{
			Ogre::TexturePtr::operator=(tex);
			mSignal.emit(*this);
		}
	}
}
