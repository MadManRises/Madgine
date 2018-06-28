#pragma once

#include "Madgine/resources/resourceloader.h"

namespace Engine
{

	class OgreMeshLoader : public Resources::ResourceLoader<OgreMeshLoader, Ogre::MeshPtr>
	{
	public:
		OgreMeshLoader(Resources::ResourceManager &mgr);

		virtual std::shared_ptr<Data> load(ResourceType* res) override;

	};

}
