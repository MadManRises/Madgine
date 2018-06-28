#include "ogrelib.h"

#include "ogremeshloader.h"

#include <assimp/Importer.hpp>

namespace Engine
{
	OgreMeshLoader::OgreMeshLoader(Resources::ResourceManager& mgr) :
		ResourceLoader(mgr, { ".mesh" })
	{
	}

	std::shared_ptr<Ogre::MeshPtr> OgreMeshLoader::load(Resource* res)
	{

		res->setPersistent(true);
		
		std::string extension = res->extension();

		Ogre::MeshPtr mesh;

		if (extension == ".mesh")
		{
			mesh = Ogre::MeshManager::getSingleton().createManual(res->name(), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

			Ogre::MeshSerializer meshSerializer;

			std::ifstream file(res->path(), std::ios::binary);			

			Ogre::DataStreamPtr streamPtr(new Ogre::FileStreamDataStream(&file, false));

			meshSerializer.importMesh(streamPtr, mesh.get());
		}

		Assimp::Importer importer;

		return std::make_shared<Ogre::MeshPtr>(mesh);
	}

}
