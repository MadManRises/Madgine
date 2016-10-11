#include "libinclude.h"
#include "ResourceLoader.h"

#include "UI\loadingscreenhandler.h"

#include "Database\messages.h"


namespace Engine {
	namespace Resources {

		ResourceLoader::ResourceLoader() :
			mRgm(&Ogre::ResourceGroupManager::getSingleton())

		{
			mParser = OGRE_MAKE_UNIQUE(Scripting::Parsing::ScriptParser)(); // Initialise the Script Parser

			mImageSetManager = OGRE_MAKE_UNIQUE(Resources::ImageSets::ImageSetManager)();
		}

		ResourceLoader::~ResourceLoader()
		{

			mRgm->shutdownAll();
		}

		void ResourceLoader::setup(const std::string &mediaPath)
		{
			mMediaPath = mediaPath;

			// Load resource paths from config file
			Ogre::ConfigFile cf;
			cf.load(mediaPath + "resources.cfg");

			// Go through all sections & settings in the file
			Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

			Ogre::String secName, typeName, archName;
			while (seci.hasMoreElements()) {
				secName = seci.peekNextKey();
				for (const std::pair<Ogre::String, Ogre::String> &p : *seci.getNext()) {
					typeName = p.first;
					archName = mediaPath + p.second;

					mRgm->addResourceLocation(
						archName, typeName, secName);
				}
			}

		}

		void ResourceLoader::load()
		{

			UI::LoadingScreenHandler::getSingleton().setProcess(this);

			startSubProcess(mRgm->getResourceGroups().size(), Database::Messages::loadingMessage("Resources"));

			mRgm->addResourceGroupListener(this);

			mRgm->initialiseAllResourceGroups();

			mRgm->removeResourceGroupListener(this);

			endSubProcess();

			UI::LoadingScreenHandler::getSingleton().clearProcess();
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