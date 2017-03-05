#include "madginelib.h"

#include "OgreSceneManager.h"

#include "Entity/ogreentity.h"

#include "exceptionmessages.h"

#include "Math\bounds.h"

#include "Serialize\Streams\serializestream.h"

#include "Scene/scenecomponent.h"

#include "Util\Process.h"

#include "Util\Profiler.h"

#include "Scene/sceneexception.h"

#include "Serialize\serializablebase.h"

#include "Serialize\serializemanager.h"

#include "Serialize\ogreSerialize.h"

#include "Scripting\Parsing\scriptparser.h"


namespace Engine {

namespace Scene {


OgreSceneManager::OgreSceneManager(Ogre::Root *root) :
    mRoot(root),
    mSceneMgr(0),
    mTerrainGlobals(0),
    mTerrainGroup(0),
    mVp(0),
    mRenderTexture(0),
	mTerrainRayQuery(0),
	mEntities(this, &OgreSceneManager::createEntityData)	
{

    mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);

	createCamera();

	clear();

}

OgreSceneManager::~OgreSceneManager()
{
}

bool OgreSceneManager::init()
{


	//mShaderCollector.init(mSceneMgr);

	Ogre::MovableObject::setDefaultQueryFlags(Entity::Masks::DEFAULT_MASK);

	mTerrainRayQuery = mSceneMgr->createRayQuery(Ogre::Ray(), Entity::Masks::TERRAIN_MASK);
	mTerrainRayQuery->setSortByDistance(true);

	mGameTexture = mRoot->getTextureManager()->createManual(
		"RTT",
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Ogre::TEX_TYPE_2D,
		256,
		256,
		0,
		Ogre::PF_R8G8B8,
		Ogre::TU_RENDERTARGET);

	mRenderTexture = mGameTexture->getBuffer()->getRenderTarget();

	mVp = mRenderTexture->addViewport(mCamera);
	mVp->setOverlaysEnabled(false);
	mVp->setClearEveryFrame(true);
	mVp->setBackgroundColour(Ogre::ColourValue::Black);


//	mVp->setMaterialScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);

	return SceneManager::init();

}

void OgreSceneManager::finalize()
{
	SceneManager::finalize();

	if (mTerrainGlobals) OGRE_DELETE mTerrainGlobals;
	if (mTerrainGroup) OGRE_DELETE mTerrainGroup;

	if (mTerrainRayQuery) mSceneMgr->destroyQuery(mTerrainRayQuery);

}

void OgreSceneManager::readStaticScene(Serialize::SerializeInStream &in)
{

	clear();

	in.process().startSubProcess(1, "Creating Terrain...");

    readTerrain(in);

	in.process().step();
	in.process().endSubProcess();    
}

//---------------------------------------------------------------------------
void OgreSceneManager::createCamera(void)
{
    // Create the camera

    mCamera = mSceneMgr->createCamera("MadgineSceneCamera");
    mCamera->setPosition(1,80,0);
    mCamera->lookAt(0,0,0);


    mCamera->setFarClipDistance(5000);
    mCamera->setNearClipDistance(1);

}

void OgreSceneManager::readTerrain(Serialize::SerializeInStream &in)
{	
	in >> mHeightmap;

    if (isUsingHeightmap()) {

        mTerrainGlobals = OGRE_NEW Ogre::TerrainGlobalOptions();
        mTerrainGroup = OGRE_NEW Ogre::TerrainGroup(mSceneMgr, Ogre::Terrain::ALIGN_X_Z,
                        513, 120);
        mTerrainGroup->setFilenameConvention(Ogre::String("OgreRecastTerrain"),
                                             Ogre::String("ogt"));
        mTerrainGroup->setOrigin(Ogre::Vector3::ZERO);
        //configureTerrainDefaults(light);

        // Configure global
        mTerrainGlobals->setMaxPixelError(8);   // Precision of terrain
        // testing composite map
        mTerrainGlobals->setCompositeMapDistance(
            3000);     // Max distance of terrain to be rendered


        // Configure default import settings for if we use imported image
        Ogre::Terrain::ImportData &defaultimp =
            mTerrainGroup->getDefaultImportSettings();
        defaultimp.terrainSize = 513;
        defaultimp.worldSize = 120;
        defaultimp.inputScale =
            512.0; // was originally 600 due terrain.png is 8 bpp (with tile world size 12000)
        defaultimp.minBatchSize = 33;
        defaultimp.maxBatchSize = 65;

        // textures
        defaultimp.layerList.resize(3);
        defaultimp.layerList[0].worldSize = 100;
        defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_diffusespecular.dds");
        defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_normalheight.dds");
        defaultimp.layerList[1].worldSize = 30;
        defaultimp.layerList[1].textureNames.push_back("grass_green-01_diffusespecular.dds");
        defaultimp.layerList[1].textureNames.push_back("grass_green-01_normalheight.dds");
        defaultimp.layerList[2].worldSize = 200;
        defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_diffusespecular.dds");
        defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_normalheight.dds");

        Ogre::Image img;
        Ogre::LogManager::getSingletonPtr()->logMessage(
            std::string("Loading Heightmap from file:") + mHeightmap);
        img.load(mHeightmap, "Heightmaps");

        mTerrainGroup->defineTerrain(0, 0, &img);

        // sync load since we want everything in place when we start
        mTerrainGroup->loadAllTerrains(true);

        mTerrainGroup->freeTemporaryResources();
    }

	std::string obName;
	while (in.loopRead(obName)) {
		std::string mesh;
		in >> mesh;

		Ogre::Vector3 v;
		in >> v;
		
		Ogre::Quaternion q;
		in >> q;
		
		if (obName[0] == '_') {
			mInfoObjects[obName] = std::make_tuple(mesh, v, q);
		}
		else {
			Ogre::Entity *ent = mSceneMgr->createEntity(obName, mesh);
			ent->addQueryFlags(Entity::Masks::TERRAIN_MASK);

			Ogre::SceneNode *node = mTerrain->createChildSceneNode(obName, v, q);
			node->attachObject(ent);

			mTerrainEntities.push_back(node);
		}
	}

	Ogre::Vector3 lightPosition;
	while (in.loopRead(lightPosition)) {
		Ogre::Light* light = mSceneMgr->createLight();

		light->setDiffuseColour(1.0, 1.0, 1.0);
		light->setSpecularColour(1.0, 1.0, 1.0);

		float linear;
		in >> linear;

		Ogre::Light::LightTypes type;
		in >> (int&)type;

		float scale;
		in >> scale;

		light->setType(type);

		light->setPowerScale(scale);
		
		if (light->getType() != Ogre::Light::LT_POINT) {
			Ogre::Vector3 dir;
			in >> dir;
			light->setDirection(dir);
		}

		light->setPosition(lightPosition);
		light->setAttenuation(5000, 1.0, linear, 0.0);

		mStaticLights.push_back(light);
		
	}

	//if (mTerrainEntities.empty() && mHeightmap.empty()) throw std::invalid_argument("Empty Scene-Config");


}

std::list<Entity::Entity *> OgreSceneManager::entities()
{
    std::list<Entity::Entity*> result;
    for (Entity::Entity &e : mEntities){
        if (std::find(mEntityRemoveQueue.begin(), mEntityRemoveQueue.end(), &e) == mEntityRemoveQueue.end())
            result.push_back(&e);
    }
    return result;
}



Math::Bounds OgreSceneManager::getSceneBounds()
{
    mTerrain->_update(true, false);
    Ogre::AxisAlignedBox box = mTerrain->_getWorldAABB();
    return {box.getMinimum(), box.getMaximum()};
}



Math::Bounds OgreSceneManager::getRasterizedSceneBounds()
{
    Math::Bounds bounds = getSceneBounds();
    float left = floorf(bounds.left() / sSceneRasterSize);
    float bottom = floorf(bounds.bottom() / sSceneRasterSize);
    float right = ceilf(bounds.right() / sSceneRasterSize);
    float top = ceilf(bounds.top() / sSceneRasterSize);
    return Math::Bounds(left, bottom, right, top);
}

Ogre::Vector2 OgreSceneManager::rasterizePoint(const Ogre::Vector2 &v)
{
    Math::Bounds bounds = getRasterizedSceneBounds();
    float x = roundf(v.x / sSceneRasterSize) - bounds.left();
    float y = roundf(v.y / sSceneRasterSize) - bounds.bottom();
    return Ogre::Vector2(x, y);
}

Ogre::Vector3 OgreSceneManager::rasterizePoint(const Ogre::Vector3 &v)
{
    Math::Bounds bounds = getRasterizedSceneBounds();
    float x = roundf(v.x / sSceneRasterSize) - bounds.left();
    float z = roundf(v.z / sSceneRasterSize) - bounds.bottom();
    return Ogre::Vector3(x, v.y, z);
}

Ogre::Vector2 OgreSceneManager::relToScenePos(const Ogre::Vector2 &v)
{
    return getSceneBounds() * v;
}





const std::tuple<std::string, Ogre::Vector3, Ogre::Quaternion>& OgreSceneManager::getInfoObject(const std::string & name) const
{
	return mInfoObjects.at(name);
}


void OgreSceneManager::writeStaticScene(Serialize::SerializeOutStream & out) const
{
	out << mHeightmap;
	for (Ogre::SceneNode *n : mTerrainEntities) {
		Ogre::Entity * ent = (Ogre::Entity*)n->getAttachedObjectIterator().getNext();
		out << ent->getName() << ent->getMesh()->getName() << n->getPosition() << n->getOrientation();
	}
	for (const std::pair<const std::string, std::tuple<std::string, Ogre::Vector3, Ogre::Quaternion>> &t : mInfoObjects) {
		out << t.first << std::get<0>(t.second) << std::get<1>(t.second) << std::get<2>(t.second);
	}
	out << ValueType::EOL();

	for (Ogre::Light * light : mStaticLights) {

		out << light->getPosition() << light->getAttenuationLinear() << light->getType() << light->getPowerScale();

		if (light->getType() != Ogre::Light::LT_POINT) {
			out << light->getDirection();
		}

	}
	out << ValueType::EOL();
}

void OgreSceneManager::writeState(Serialize::SerializeOutStream &out) const
{
	writeStaticScene(out);

	saveComponentData(out);

	SerializableUnit::writeState(out);

    
}

void OgreSceneManager::readState(Serialize::SerializeInStream &in)
{
	readStaticScene(in);

	loadComponentData(in);
	
	SerializableUnit::readState(in);
		
}

void OgreSceneManager::readScene(Serialize::SerializeInStream & in, bool callInit)
{
	in.process().startSubProcess(1, "Loading Level...");

	readState(in);
	
	applySerializableMap(in.manager().slavesMap());

	if (callInit) {
		for (Entity::Entity &e : mEntities) {
			e.init();
		}
	}

	in.process().endSubProcess();
}

void OgreSceneManager::writeScene(Serialize::SerializeOutStream & out) const
{
	writeState(out);
}

void OgreSceneManager::makeLocalCopy(Entity::OgreEntity & e)
{
	mLocalEntities.emplace_back(e);
}

void OgreSceneManager::makeLocalCopy(Entity::OgreEntity && e)
{
	mLocalEntities.emplace_back(std::forward<Entity::OgreEntity>(e));
}

void OgreSceneManager::setEntitiesCallback(std::function<void(const Serialize::SerializableList<Entity::OgreEntity, const Scripting::Parsing::EntityNode*, Ogre::SceneNode*, Ogre::Entity*>::iterator&, int)> f)
{
	mEntities.setCallback(f);
}


Ogre::TerrainGroup *OgreSceneManager::terrainGroup() const
{
    return mTerrainGroup;
}



bool OgreSceneManager::isUsingHeightmap() const
{
    return !mHeightmap.empty();
}

const std::vector<Ogre::SceneNode*> &OgreSceneManager::terrainEntities()
{
	return mTerrainEntities;
}

std::tuple<const Scripting::Parsing::EntityNode *, Ogre::SceneNode *, Ogre::Entity*> OgreSceneManager::createEntityData(const std::string & behaviour, const std::string & name, const std::string & meshName)
{

	std::string actualName = name.empty() ? generateUniqueName() : name;

	Ogre::SceneNode *node = mEntitiesNode->createChildSceneNode(actualName);

	Ogre::Entity *mesh = 0;
	if (!meshName.empty()) {
		mesh = mSceneMgr->createEntity(actualName, meshName);

		node->attachObject(mesh);
	}

	const Scripting::Parsing::EntityNode *behaviourNode = 0;
	if (!behaviour.empty()) {
		behaviourNode = &Scripting::Parsing::ScriptParser::getSingleton().getEntityDescription(behaviour);
	}

	return std::make_tuple(behaviourNode, node, mesh);
	
}

Entity::Entity *OgreSceneManager::createEntity(const std::string &behaviour, const std::string &name, const std::string &meshName, const Scripting::ArgumentList &args)
{
	Entity::Entity &e = *mEntities.emplace_tuple_back(createEntityData(behaviour, name, meshName));
	e.init(args);
	return &e;
}

Ogre::Camera *OgreSceneManager::camera()
{
    return mCamera;
}

Ogre::SceneManager *OgreSceneManager::getSceneManager()
{
    return mSceneMgr;
}

void OgreSceneManager::setGameTextureSize(const Ogre::Vector2 & size)
{
	if (size.x <= 0 || size.y <= 0)
		return;
	if (mGameTexture->getWidth() == size.x && mGameTexture->getHeight() == size.y)
		return;
    mCamera->setAspectRatio(size.x / size.y);

    unsigned int width = (unsigned int)size.x;
    unsigned int height = (unsigned int)size.y;

	{
		Ogre::TexturePtr tmp = mGameTexture;

		mGameTexture = Ogre::TexturePtr();

		mRoot->getTextureManager()->remove(tmp);
	}	

	mGameTexture = mRoot->getTextureManager()->createManual(
		"RTT",
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Ogre::TEX_TYPE_2D,
		width,
		height,
		0,
		Ogre::PF_R8G8B8,
		Ogre::TU_RENDERTARGET);	
    	
	//mRoot->getRenderSystem()->_cleanupDepthBuffers(false);

	mRenderTexture = mGameTexture->getBuffer()->getRenderTarget();	

	mVp = mRenderTexture->addViewport(mCamera);
	mVp->setOverlaysEnabled(false);
	mVp->setClearEveryFrame(true);
	mVp->setBackgroundColour(Ogre::ColourValue::Black);


//	mVp->setMaterialScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
    
}


Ogre::Image OgreSceneManager::getScreenshot()
{
	Ogre::Image image;
	mGameTexture->convertToImage(image);
	return image;
}

const Ogre::SceneNode *OgreSceneManager::terrain()
{
    return mTerrain;
}

Ogre::Ray OgreSceneManager::mousePointToRay(const Ogre::Vector2 &mousePos)
{
    return mCamera->getCameraToViewportRay(mousePos.x, mousePos.y);
}

bool OgreSceneManager::mousePointToTerrainPoint(const Ogre::Vector2 &mousePos,
        Ogre::Vector3 &result, Ogre::uint32 mask)
{
    return rayToTerrainPoint(mousePointToRay(mousePos), result);
}

bool OgreSceneManager::rayToTerrainPoint(const Ogre::Ray &ray,
                                       Ogre::Vector3 &result, Ogre::uint32 mask)
{

    if (mTerrainGroup && (mask & Entity::Masks::TERRAIN_MASK) ) {
        // Perform the scene query
        Ogre::TerrainGroup::RayResult r = mTerrainGroup->rayIntersects(ray);
        if (r.hit) {
            result = r.position;
            return true;
        } else {
			if (mask == Entity::Masks::TERRAIN_MASK)
				return false;
        }
    }
    
	return RaycastFromPoint(ray, result, mask);
    
}


void OgreSceneManager::removeQueuedEntities()
{
    std::list<Entity::Entity *>::iterator it = mEntityRemoveQueue.begin();

	auto find = [&](const Entity::Entity &ent) {return &ent == *it; };

    while (it != mEntityRemoveQueue.end()) {

		auto ent = std::find_if(mEntities.begin(), mEntities.end(), find);
        if (ent == mEntities.end()) throw 0;

		mEntities.erase(ent);

        it = mEntityRemoveQueue.erase(it);
    }
}

void OgreSceneManager::clear()
{

	for (Entity::Entity &e : mEntities) {
		e.clear();
	}
    mEntities.clear();
    mEntityRemoveQueue.clear();

	mStaticLights.clear();
    mTerrainEntities.clear();
	mInfoObjects.clear();

    mSceneMgr->clearScene();

	mEntitiesNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("Entities");
	mTerrain = mSceneMgr->getRootSceneNode()->createChildSceneNode("Terrain");

	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.8f, 0.8f, 0.8f));
}




// raycast from a point in to the scene.
// returns success or failure.
// on success the point is returned in the result.
bool OgreSceneManager::RaycastFromPoint(const Ogre::Ray &ray,
                                      Ogre::Vector3 &result, Ogre::uint32 mask) {

    // check we are initialised
    if (mTerrainRayQuery != NULL) {
        // create a query object
        mTerrainRayQuery->setRay(ray);
		mTerrainRayQuery->setQueryMask(mask);

        // execute the query, returns a vector of hits
        if (mTerrainRayQuery->execute().size() <= 0) {
            // raycast did not hit an objects bounding box
            return false;
        }
    } else {
        return false;
    }

    // at this point we have raycast to a series of different objects bounding boxes.
    // we need to test these different objects to see which is the first polygon hit.
    // there are some minor optimizations (distance based) that mean we wont have to
    // check all of the objects most of the time, but the worst case scenario is that
    // we need to test every triangle of every object.
    Ogre::Real closest_distance = -1.0f;
    Ogre::Vector3 closest_result;
    Ogre::RaySceneQueryResult &query_result = mTerrainRayQuery->getLastResults();
    for (size_t qr_idx = 0; qr_idx < query_result.size(); qr_idx++) {
        // stop checking if we have found a raycast hit that is closer
        // than all remaining entities
        if ((closest_distance >= 0.0f) &&
                (closest_distance < query_result[qr_idx].distance)) {
            break;
        }

        // only check this result if its a hit against an entity
        if ((query_result[qr_idx].movable != NULL) &&
                (query_result[qr_idx].movable->getMovableType().compare("Entity") == 0)) {
            // get the entity to check
            Ogre::Entity *pentity = static_cast<Ogre::Entity *>
                                    (query_result[qr_idx].movable);

            //assert(pentity->getParentNode()-> == mTerrain);

            // mesh data to retrieve
            size_t vertex_count;
            size_t index_count;
            Ogre::Vector3 *vertices;
            unsigned long *indices;

            // get the mesh information
            GetMeshInformation(pentity->getMesh(), vertex_count, vertices, index_count,
                               indices,
                               pentity->getParentNode()->_getDerivedPosition(),
                               pentity->getParentNode()->_getDerivedOrientation(),
                               pentity->getParentNode()->_getDerivedScale());

            // test for hitting individual triangles on the mesh
            bool new_closest_found = false;
            for (int i = 0; i < static_cast<int>(index_count); i += 3) {
                // check for a hit against this triangle

				//std::cout << ray.getDirection() << ", " << ray.getDirection().crossProduct(vertices[indices[i]] - ray.getOrigin()).squaredLength() << std::endl;


				if (ray.getDirection().crossProduct(vertices[indices[i]] - ray.getOrigin()).squaredLength() > std::max(vertices[indices[i]].squaredDistance(vertices[indices[i+1]]), vertices[indices[i]].squaredDistance(vertices[indices[i+2]]))) continue;

				

                std::pair<bool, Ogre::Real> hit = Ogre::Math::intersects(ray,
                                                  vertices[indices[i]],
                                                  vertices[indices[i+1]], vertices[indices[i+2]], true, false);

                // if it was a hit check if its the closest
                if (hit.first) {
                    if ((closest_distance < 0.0f) ||
                            (hit.second < closest_distance)) {
                        // this is the closest so far, save it off
                        closest_distance = hit.second;
                        new_closest_found = true;
                    }
                }
            }

            // free the verticies and indicies memory
            delete[] vertices;
            delete[] indices;

            // if we found a new closest raycast for this object, update the
            // closest_result before moving on to the next object.
            if (new_closest_found) {
                closest_result = ray.getPoint(closest_distance);
            }
        }
    }

    // return the result
    if (closest_distance >= 0.0f) {
        // raycast success
        result = closest_result;
        return (true);
    } else {
        // raycast failed
        return (false);
    }
}



// Get the mesh information for the given mesh.
// Code found in Wiki: www.ogre3d.org/wiki/index.php/RetrieveVertexData
void OgreSceneManager::GetMeshInformation(const Ogre::MeshPtr mesh,
                                        size_t &vertex_count,
                                        Ogre::Vector3 *&vertices,
                                        size_t &index_count,
                                        unsigned long *&indices,
                                        const Ogre::Vector3 &position,
                                        const Ogre::Quaternion &orient,
                                        const Ogre::Vector3 &scale)
{
    bool added_shared = false;
    size_t current_offset = 0;
    size_t shared_offset = 0;
    size_t next_offset = 0;
    size_t index_offset = 0;

    vertex_count = index_count = 0;

    // Calculate how many vertices and indices we're going to need
    for (unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i) {
        Ogre::SubMesh *submesh = mesh->getSubMesh(i);

        // We only need to add the shared vertices once
        if (submesh->useSharedVertices) {
            if (!added_shared) {
                vertex_count += mesh->sharedVertexData->vertexCount;
                added_shared = true;
            }
        } else {
            vertex_count += submesh->vertexData->vertexCount;
        }

        // Add the indices
        index_count += submesh->indexData->indexCount;
    }


    // Allocate space for the vertices and indices
    vertices = new Ogre::Vector3[vertex_count];
    indices = new unsigned long[index_count];

    added_shared = false;

    // Run through the submeshes again, adding the data into the arrays
    for (unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i) {
        Ogre::SubMesh *submesh = mesh->getSubMesh(i);

        Ogre::VertexData *vertex_data = submesh->useSharedVertices ?
                                        mesh->sharedVertexData : submesh->vertexData;

        if ((!submesh->useSharedVertices)||(submesh->useSharedVertices
                                            && !added_shared)) {
            if (submesh->useSharedVertices) {
                added_shared = true;
                shared_offset = current_offset;
            }

            const Ogre::VertexElement *posElem =
                vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);

            Ogre::HardwareVertexBufferSharedPtr vbuf =
                vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());

            unsigned char *vertex =
                static_cast<unsigned char *>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

            // There is _no_ baseVertexPointerToElement() which takes an Ogre::Real or a double
            //  as second argument. So make it float, to avoid trouble when Ogre::Real will
            //  be comiled/typedefed as double:
            //      Ogre::Real* pReal;
            float *pReal;

            for (size_t j = 0; j < vertex_data->vertexCount;
                    ++j, vertex += vbuf->getVertexSize()) {
                posElem->baseVertexPointerToElement(vertex, &pReal);

                Ogre::Vector3 pt(pReal[0], pReal[1], pReal[2]);

                vertices[current_offset + j] = (orient * (pt * scale)) + position;
            }

            vbuf->unlock();
            next_offset += vertex_data->vertexCount;
        }


        Ogre::IndexData *index_data = submesh->indexData;
        size_t numTris = index_data->indexCount / 3;
        Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;
        if (ibuf.isNull())
            continue;  // need to check if index buffer is valid (which will be not if the mesh doesn't have triangles like a pointcloud)

        bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);

        unsigned long  *pLong = static_cast<unsigned long *>(ibuf->lock(
                                    Ogre::HardwareBuffer::HBL_READ_ONLY));
        unsigned short *pShort = reinterpret_cast<unsigned short *>(pLong);


        size_t offset = (submesh->useSharedVertices)? shared_offset : current_offset;
        size_t index_start = index_data->indexStart;
        size_t last_index = numTris*3 + index_start;

        if (use32bitindexes)
            for (size_t k = index_start; k < last_index; ++k) {
                indices[index_offset++] = pLong[k] + static_cast<unsigned long>(offset);
            }

        else
            for (size_t k = index_start; k < last_index; ++k) {
                indices[ index_offset++ ] = static_cast<unsigned long>(pShort[k]) +
                                            static_cast<unsigned long>(offset);
            }

        ibuf->unlock();
        current_offset = next_offset;
    }
}

void OgreSceneManager::getTerrainMeshInformation(size_t &vertex_count, Ogre::Vector3* &vertices,
	size_t &index_count, unsigned int* &indices)
{
	vertex_count = index_count = 0;


	size_t current_offset = vertex_count;
	size_t shared_offset = vertex_count;
	size_t next_offset = vertex_count;
	size_t index_offset = index_count;

	for (Ogre::SceneNode *node : mTerrainEntities) {
		const Ogre::MeshPtr mesh = ((Ogre::Entity*)node->getAttachedObjectIterator().getNext())->getMesh();

		bool added_shared = false;

		// Calculate how many vertices and indices we're going to need
		for (int i = 0; i < mesh->getNumSubMeshes(); i++)
		{
			Ogre::SubMesh* submesh = mesh->getSubMesh(i);

			// We only need to add the shared vertices once
			if (submesh->useSharedVertices)
			{
				if (!added_shared)
				{
					Ogre::VertexData* vertex_data = mesh->sharedVertexData;
					vertex_count += vertex_data->vertexCount;
					added_shared = true;
				}
			}
			else
			{
				Ogre::VertexData* vertex_data = submesh->vertexData;
				vertex_count += vertex_data->vertexCount;
			}

			// Add the indices
			Ogre::IndexData* index_data = submesh->indexData;
			index_count += index_data->indexCount;
		}
	}

	// Allocate space for the vertices and indices
	vertices = new Ogre::Vector3[vertex_count];
	indices = new unsigned[index_count];

	for (Ogre::SceneNode *node : mTerrainEntities) {
		const Ogre::MeshPtr mesh = ((Ogre::Entity*)node->getAttachedObjectIterator().getNext())->getMesh();

		Ogre::Quaternion orient = node->getOrientation();
		Ogre::Vector3 scale = node->getScale();
		Ogre::Vector3 position = node->getPosition();

		bool added_shared = false;

		// Run through the submeshes again, adding the data into the arrays
		for (int i = 0; i < mesh->getNumSubMeshes(); i++)
		{
			Ogre::SubMesh* submesh = mesh->getSubMesh(i);

			Ogre::VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;
			if ((!submesh->useSharedVertices) || (submesh->useSharedVertices && !added_shared))
			{
				if (submesh->useSharedVertices)
				{
					added_shared = true;
					shared_offset = current_offset;
				}

				const Ogre::VertexElement* posElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);
				Ogre::HardwareVertexBufferSharedPtr vbuf = vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());
				unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
				Ogre::Real* pReal;

				for (size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
				{
					posElem->baseVertexPointerToElement(vertex, &pReal);

					Ogre::Vector3 pt;

					pt.x = (*pReal++);
					pt.y = (*pReal++);
					pt.z = (*pReal++);

					pt = (orient * (pt * scale)) + position;

					vertices[current_offset + j].x = pt.x;
					vertices[current_offset + j].y = pt.y;
					vertices[current_offset + j].z = pt.z;
				}
				vbuf->unlock();
				next_offset += vertex_data->vertexCount;
			}

			Ogre::IndexData* index_data = submesh->indexData;

			size_t numTris = index_data->indexCount / 3;
			unsigned short* pShort;
			unsigned int* pInt;
			Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;
			bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);
			if (use32bitindexes) pInt = static_cast<unsigned int*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
			else pShort = static_cast<unsigned short*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

			for (size_t k = 0; k < numTris; ++k)
			{
				size_t offset = (submesh->useSharedVertices) ? shared_offset : current_offset;

				unsigned int vindex = use32bitindexes ? *pInt++ : *pShort++;
				indices[index_offset + 0] = vindex + offset;
				vindex = use32bitindexes ? *pInt++ : *pShort++;
				indices[index_offset + 1] = vindex + offset;
				vindex = use32bitindexes ? *pInt++ : *pShort++;
				indices[index_offset + 2] = vindex + offset;

				index_offset += 3;
			}
			ibuf->unlock();
			current_offset = next_offset;
		}
	}
}

Resources::OgreTexturePtr &OgreSceneManager::getGameTexture()
{
	return mGameTexture;
}

Ogre::Viewport * OgreSceneManager::getViewport()
{
	return mVp;
}

Ogre::RenderTarget * OgreSceneManager::getRenderTarget()
{
	return mRenderTexture;
}

Entity::Entity * OgreSceneManager::findEntity(const std::string & name)
{
	auto it = std::find_if(mEntities.begin(), mEntities.end(), [&](const Entity::Entity &e) {
		return e.getName() == name;
	});
	if (it == mEntities.end()) {
		throw 0;
	}
	else
		return &*it;
}

}
}

