#include "../../../clientlib.h"

#include "ogremesh.h"

#include "ogreskeletonvisualizer.h"

#include "../../ogreSceneManager.h"

#include <OgreTagPoint.h>

namespace Engine
{
	API_IMPL(Scene::Entity::OgreSkeletonVisualizer);

	namespace Scene
	{
		namespace Entity
		{
			ENTITYCOMPONENT_IMPL(SkeletonVisualizer, OgreSkeletonVisualizer);

			OgreSkeletonVisualizer::OgreSkeletonVisualizer(Entity& entity, const Scripting::LuaTable& table) :
				EntityComponent(entity, table),
				mMesh(nullptr), mBoneSize(0), mShowBones(false)
			{
			}


			OgreSkeletonVisualizer::~OgreSkeletonVisualizer()
			{
			}

			void OgreSkeletonVisualizer::init()
			{
				mMesh = getEntity().getComponent<OgreMesh>();

				Ogre::Entity* entity = mMesh->getMesh();
				Ogre::SceneManager* sceneMgr = static_cast<OgreSceneManager&>(getEntity().sceneMgr()).getSceneManager();

				mBoneSize = 0.1f;

				createBoneMaterial();
				createBoneMesh();
				mShowBones = true;


				int numBones = entity->getSkeleton()->getNumBones();

				for (unsigned short int iBone = 0; iBone < numBones; ++iBone)
				{
					Ogre::Bone* pBone = entity->getSkeleton()->getBone(iBone);
					if (!pBone)
					{
						assert(false);
						continue;
					}

					pBone->scale(Ogre::Vector3(0.1f, 0.1f, 0.1f));

					Ogre::Entity* ent;
					Ogre::TagPoint* tp;

					// Absolutely HAVE to create bone representations first. Otherwise we would get the wrong child count
					// because an attached object counts as a child
					// Would be nice to have a function that only gets the children that are bones...
					unsigned short numChildren = pBone->numChildren();
					if (numChildren == 0)
					{
						// There are no children, but we should still represent the bone
						// Creates a bone of length 1 for leaf bones (bones without children)
						ent = sceneMgr->createEntity("SkeletonDebug/BoneMesh");
						tp = entity->attachObjectToBone(pBone->getName(), static_cast<Ogre::MovableObject*>(ent));
						mBoneEntities.push_back(ent);
					}
					else
					{
						for (int i = 0; i < numChildren; ++i)
						{
							Ogre::Vector3 v = pBone->getChild(i)->getPosition();
							// If the length is zero, no point in creating the bone representation
							float length = v.length();
							if (length < 0.00001f)
								continue;

							ent = sceneMgr->createEntity("SkeletonDebug/BoneMesh");
							tp = entity->attachObjectToBone(pBone->getName(), static_cast<Ogre::MovableObject*>(ent));
							mBoneEntities.push_back(ent);

							tp->setScale(length, length, length);
						}
					}

					/*ent = sceneMgr->createEntity("SkeletonDebug/AxesMesh");
					tp = entity->attachObjectToBone(pBone->getName(), (Ogre::MovableObject*)ent);
					// Make sure we don't wind up with tiny/giant axes and that one axis doesnt get squashed
					tp->setScale((mScaleAxes / entity->getParentSceneNode()->getScale().x), (mScaleAxes / entity->getParentSceneNode()->getScale().y), (mScaleAxes / entity->getParentSceneNode()->getScale().z));
					mAxisEntities.push_back(ent);*/

					Ogre::String name = entity->getName() + "SkeletonDebug/BoneText/Bone_";
					name += iBone;
				}

				//showBones(false);
			}

			void OgreSkeletonVisualizer::finalize()
			{
			}


			void OgreSkeletonVisualizer::showBones(bool show)
			{
				// Don't change anything if we are already in the proper state
				if (mShowBones == show)
					return;

				mShowBones = show;

				for (std::vector<Ogre::Entity*>::iterator it = mBoneEntities.begin(); it < mBoneEntities.end(); ++it)
				{
					static_cast<Ogre::Entity*>(*it)->setVisible(show);
				}
			}


			void OgreSkeletonVisualizer::createBoneMaterial()
			{
				Ogre::String matName = "SkeletonDebug/BoneMat";

				mBoneMatPtr = Ogre::MaterialManager::getSingleton().getByName(matName);
				if (!mBoneMatPtr)
				{
					mBoneMatPtr = Ogre::MaterialManager::getSingleton().create(
						matName, Ogre::ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME);

					Ogre::Pass* p = mBoneMatPtr->getTechnique(0)->getPass(0);
					p->setLightingEnabled(false);
					p->setPolygonModeOverrideable(false);
					p->setVertexColourTracking(Ogre::TVC_AMBIENT);
					p->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
					p->setCullingMode(Ogre::CULL_ANTICLOCKWISE);
					p->setDepthWriteEnabled(false);
					p->setDepthCheckEnabled(false);
				}
			}

			void OgreSkeletonVisualizer::createBoneMesh()
			{
				Ogre::String meshName = "SkeletonDebug/BoneMesh";
				mBoneMeshPtr = Ogre::MeshManager::getSingleton().getByName(meshName);
				if (!mBoneMeshPtr)
				{
					Ogre::ManualObject mo("tmp");
					mo.begin(mBoneMatPtr->getName());

					Ogre::Vector3 basepos[6] =
					{
						Ogre::Vector3(0, 0, 0),
						Ogre::Vector3(mBoneSize, mBoneSize * 2, mBoneSize),
						Ogre::Vector3(-mBoneSize, mBoneSize * 2, mBoneSize),
						Ogre::Vector3(-mBoneSize, mBoneSize * 2, -mBoneSize),
						Ogre::Vector3(mBoneSize, mBoneSize * 2, -mBoneSize),
						Ogre::Vector3(0, 1, 0),
					};

					// Two colours so that we can distinguish the sides of the bones (we don't use any lighting on the material)
					Ogre::ColourValue col = Ogre::ColourValue(0.5, 0.5, 0.5, 1);
					Ogre::ColourValue col1 = Ogre::ColourValue(0.6, 0.6, 0.6, 1);

					mo.position(basepos[0]);
					mo.colour(col);
					mo.position(basepos[2]);
					mo.colour(col);
					mo.position(basepos[1]);
					mo.colour(col);

					mo.position(basepos[0]);
					mo.colour(col1);
					mo.position(basepos[3]);
					mo.colour(col1);
					mo.position(basepos[2]);
					mo.colour(col1);

					mo.position(basepos[0]);
					mo.colour(col);
					mo.position(basepos[4]);
					mo.colour(col);
					mo.position(basepos[3]);
					mo.colour(col);

					mo.position(basepos[0]);
					mo.colour(col1);
					mo.position(basepos[1]);
					mo.colour(col1);
					mo.position(basepos[4]);
					mo.colour(col1);

					mo.position(basepos[1]);
					mo.colour(col1);
					mo.position(basepos[2]);
					mo.colour(col1);
					mo.position(basepos[5]);
					mo.colour(col1);

					mo.position(basepos[2]);
					mo.colour(col);
					mo.position(basepos[3]);
					mo.colour(col);
					mo.position(basepos[5]);
					mo.colour(col);

					mo.position(basepos[3]);
					mo.colour(col1);
					mo.position(basepos[4]);
					mo.colour(col1);
					mo.position(basepos[5]);
					mo.colour(col1);

					mo.position(basepos[4]);
					mo.colour(col);
					mo.position(basepos[1]);
					mo.colour(col);
					mo.position(basepos[5]);
					mo.colour(col);

					// indices
					mo.triangle(0, 1, 2);
					mo.triangle(3, 4, 5);
					mo.triangle(6, 7, 8);
					mo.triangle(9, 10, 11);
					mo.triangle(12, 13, 14);
					mo.triangle(15, 16, 17);
					mo.triangle(18, 19, 20);
					mo.triangle(21, 22, 23);

					mo.end();

					mBoneMeshPtr = mo.convertToMesh(meshName, Ogre::ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME);
				}
			}
		}
	}
}
