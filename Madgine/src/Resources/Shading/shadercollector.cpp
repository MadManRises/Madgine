#include "madginelib.h"
#include "shadercollector.h"
#include "Os/os.h"


namespace Engine {
namespace Resources {
namespace Shading {

const std::string ShaderCollector::sTempShaderFolder = "shaders\\";

ShaderCollector::ShaderCollector() :
	mVerbose(false)
{	
}

ShaderCollector::~ShaderCollector()
{	
}

void ShaderCollector::init(Ogre::SceneManager *sceneMgr)
{
	if (!Ogre::RTShader::ShaderGenerator::initialize())
		throw 0;

	mInitialized = true;

	mShaderGenerator = Ogre::RTShader::ShaderGenerator::getSingletonPtr();

	mShaderGenerator->setTargetLanguage("glsl");

	Ogre::MaterialManager::getSingleton().addListener(this);

	Os::createDir(sTempShaderFolder);

	// Set shader cache path.
	mShaderGenerator->setShaderCachePath(sTempShaderFolder);

	// Set the scene manager.
	mShaderGenerator->addSceneManager(sceneMgr);

	mCollector = OGRE_MAKE_UNIQUE(UniqueComponentCollector<ShaderFactoryBase>)();

	for (const Ogre::unique_ptr<ShaderFactoryBase> &fac : *mCollector) {
		fac->init();
	}
}

void ShaderCollector::finalize()
{
	if (mInitialized) {
		Ogre::RTShader::ShaderGenerator::getSingleton().removeAllShaderBasedTechniques();
		mCollector.reset();
		Ogre::RTShader::ShaderGenerator::finalize();
		mInitialized = false;
	}
}

Ogre::Technique *ShaderCollector::handleSchemeNotFound(unsigned short schemeIndex, const Ogre::String &schemeName, Ogre::Material *originalMaterial, unsigned short lodIndex, const Ogre::Renderable *rend)
{

    Ogre::Technique* generatedTech = NULL;

    // Case this is the default shader generator scheme.
    if (schemeName == Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME)
    {
        bool techniqueCreated;

        // Create shader generated technique for this material.
        techniqueCreated = mShaderGenerator->createShaderBasedTechnique(
            originalMaterial->getName(),
            Ogre::MaterialManager::DEFAULT_SCHEME_NAME,
            schemeName);

        // Case technique registration succeeded.
        if (techniqueCreated)
        {
            // Force creating the shaders for the generated technique.
            mShaderGenerator->validateMaterial(schemeName, originalMaterial->getName());

            // Grab the generated technique.
            Ogre::Material::TechniqueIterator itTech = originalMaterial->getTechniqueIterator();

            while (itTech.hasMoreElements())
            {
                Ogre::Technique* curTech = itTech.getNext();

                if (curTech->getSchemeName() == schemeName)
                {
                    generatedTech = curTech;
                    break;
                }
            }

			if (mVerbose)
				Ogre::LogManager::getSingleton().logMessage(std::string("Created RT-Material: ") + originalMaterial->getName());
        }
    }

    return generatedTech;

}


} // namespace Shading
} // namespace Resources
} // namespace Core

