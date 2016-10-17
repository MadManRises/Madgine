#pragma once


#include "uniquecomponentcollector.h"
#include "shaderimplementation.h"

namespace Engine {
namespace Resources {
namespace Shading {

class ShaderCollector : public Ogre::MaterialManager::Listener, public Ogre::Singleton<ShaderCollector>, public Ogre::ResourceAllocatedObject
{
public:
    ShaderCollector(Ogre::SceneManager *sceneMgr);
    ~ShaderCollector();

	void init();
	void finalize();

    Ogre::Technique *handleSchemeNotFound(unsigned short schemeIndex, const Ogre::String &schemeName, Ogre::Material *originalMaterial, unsigned short lodIndex, const Ogre::Renderable *rend);


private:
    Ogre::RTShader::ShaderGenerator *mShaderGenerator;

    static const std::string sTempShaderFolder;

	bool mVerbose;

	Ogre::unique_ptr<UniqueComponentCollector<ShaderFactoryBase>> mCollector;

	Ogre::SceneManager *mSceneMgr;

};

} // namespace Shading
} // namespace Resources
} // namespace Core

