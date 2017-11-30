#include "ogrelib.h"
#include "resourceloader.h"

#include "database/messages.h"

#include "app/ogreapplication.h"


template <>
Engine::Resources::ResourceLoader* Ogre::Singleton<Engine::Resources::ResourceLoader>::msSingleton = nullptr;

namespace Engine
{
	namespace Resources
	{
		ResourceLoader::ResourceLoader(App::OgreApplication* app, const std::string& mediaPath) :
			mRgm(&Ogre::ResourceGroupManager::getSingleton()),
			mApp(app),
			mMediaPath(mediaPath)
		{
			// Load resource paths from config file
			Ogre::ConfigFile cf;
			cf.load(mediaPath + "resources.cfg");


			Ogre::String secName, typeName, archName;
			for (const std::pair<const Ogre::String, Ogre::ConfigFile::SettingsMultiMap>& p : cf.getSettingsBySection())
			{
				secName = p.first;
				for (const std::pair<const Ogre::String, Ogre::String>& p2 : p.second)
				{
					typeName = p2.first;
					archName = mediaPath + p2.second;

					mRgm->addResourceLocation(
						archName, typeName, secName);
				}
			}

			mParser = std::make_unique<Scripting::Parsing::OgreScriptParser>(app); // Initialise the Script Parser

			mImageSetManager = std::make_unique<ImageSets::ImageSetManager>();
		}

		ResourceLoader::~ResourceLoader()
		{
			mRgm->shutdownAll();
		}

		bool ResourceLoader::load_async()
		{
			bool success = true;
			auto future = std::async([this]()
			{
				return load();
			});
			using namespace std::chrono_literals;
			while (future.wait_for(0s) == std::future_status::timeout && success)
			{
				if (!mApp->singleFrame(0))
					success = false;
			}
			return success && future.get();
		}

		bool ResourceLoader::load()
		{
			int mod = mRgm->isResourceGroupInitialised("Scripting") ? -1 : 0;

			startSubProcess(mRgm->getResourceGroups().size() + mod, Database::Messages::loadingMessage("Resources"));

			mRgm->addResourceGroupListener(this);

			mRgm->initialiseAllResourceGroups();

			mRgm->removeResourceGroupListener(this);

			endSubProcess();

			return true;
		}

		void ResourceLoader::loadScripts()
		{
			mRgm->initialiseResourceGroup("Scripting");
			mParser->setFinalized();
		}

		std::string ResourceLoader::getMediaPath(const std::string& filename, const std::string& folder) const
		{
			return mMediaPath + "Media\\" + (folder.empty() ? "" : folder + "\\") + filename;
		}


		void ResourceLoader::resourceGroupScriptingStarted(
			const Ogre::String& groupName, size_t scriptCount)
		{
			startSubProcess(scriptCount, Database::Messages::loadingMessage(groupName));
		}

		void ResourceLoader::scriptParseStarted(const Ogre::String& scriptName,
		                                        bool& skipThisScript)
		{
		}

		void ResourceLoader::scriptParseEnded(const Ogre::String& scriptName,
		                                      bool skipped)
		{
			step();
		}

		void ResourceLoader::resourceGroupScriptingEnded(const Ogre::String
			& groupName)
		{
			endSubProcess();
		}

		void ResourceLoader::resourceGroupLoadStarted(const Ogre::String
		                                              & groupName, size_t resourceCount)
		{
		}

		void ResourceLoader::resourceLoadStarted(const Ogre::ResourcePtr
			& resource)
		{
		}

		void ResourceLoader::resourceLoadEnded()
		{
		}

		void ResourceLoader::worldGeometryStageStarted(const Ogre::String
			& description)
		{
		}

		void ResourceLoader::worldGeometryStageEnded()
		{
		}

		void ResourceLoader::resourceGroupLoadEnded(const Ogre::String& groupName)
		{
		}

		Scripting::Parsing::ScriptParser* ResourceLoader::scriptParser() const
		{
			return mParser.get();
		}
	}
}
