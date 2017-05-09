#include "madginelib.h"
#include "ResourceLoader.h"

#include "Database\messages.h"


template<> Engine::Resources::ResourceLoader *Ogre::Singleton<Engine::Resources::ResourceLoader>::msSingleton = 0;

namespace Engine {
	namespace Resources {

		ResourceLoader::ResourceLoader(const std::string &mediaPath) :
			mRgm(&Ogre::ResourceGroupManager::getSingleton()),
			mMediaPath(mediaPath)
		{
			// Load resource paths from config file
			Ogre::ConfigFile cf;
			cf.load(mediaPath + "resources.cfg");


			Ogre::String secName, typeName, archName;
			for (const std::pair<const Ogre::String, Ogre::ConfigFile::SettingsMultiMap> &p : cf.getSettingsBySection()) {
				secName = p.first;
				for (const std::pair<const Ogre::String, Ogre::String> &p : p.second) {
					typeName = p.first;
					archName = mediaPath + p.second;

					mRgm->addResourceLocation(
						archName, typeName, secName);
				}
			}

			mParser = std::make_unique<Scripting::Parsing::OgreScriptParser>(); // Initialise the Script Parser

			mImageSetManager = std::make_unique<Resources::ImageSets::ImageSetManager>();
		}

		ResourceLoader::~ResourceLoader()
		{

			mRgm->shutdownAll();
		}


		void ResourceLoader::load()
		{

			startSubProcess(mRgm->getResourceGroups().size(), Database::Messages::loadingMessage("Resources"));

			mRgm->addResourceGroupListener(this);

			mRgm->initialiseAllResourceGroups();

			mRgm->removeResourceGroupListener(this);

			endSubProcess();
		}

		std::string ResourceLoader::getMediaPath(const std::string & filename, const std::string & folder)
		{
			return mMediaPath + "Media\\" + (folder.empty() ? "" : folder + "\\") + filename;
		}


		void ResourceLoader::resourceGroupScriptingStarted(
			const Ogre::String &groupName, size_t scriptCount)
		{
			startSubProcess(scriptCount, Database::Messages::loadingMessage(groupName));
		}

		void ResourceLoader::scriptParseStarted(const Ogre::String &scriptName,
			bool &skipThisScript)
		{
		}

		void ResourceLoader::scriptParseEnded(const Ogre::String &scriptName,
			bool skipped)
		{
			step();
		}

		void ResourceLoader::resourceGroupScriptingEnded(const Ogre::String
			&groupName)
		{
			endSubProcess();
		}

		void ResourceLoader::resourceGroupLoadStarted(const Ogre::String
			&groupName, size_t resourceCount)
		{
		}

		void ResourceLoader::resourceLoadStarted(const Ogre::ResourcePtr
			&resource)
		{
		}

		void ResourceLoader::resourceLoadEnded()
		{
		}

		void ResourceLoader::worldGeometryStageStarted(const Ogre::String
			&description)
		{
		}

		void ResourceLoader::worldGeometryStageEnded()
		{
		}

		void ResourceLoader::resourceGroupLoadEnded(const Ogre::String &groupName)
		{
		}

		Scripting::Parsing::ScriptParser * ResourceLoader::scriptParser()
		{
			return mParser.get();
		}



	}
}