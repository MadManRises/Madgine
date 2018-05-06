#pragma once

#include "../util/process.h"
#include "imagesets/imagesetmanager.h"
#include "../scripting/parsing/ogrescriptparser.h"

namespace Engine
{
	namespace Resources
	{
		class MADGINE_CLIENT_EXPORT ResourceLoader : public Util::Process, public Ogre::ResourceGroupListener,
		                                          public Ogre::GeneralAllocatedObject
		{
		public:
			ResourceLoader(App::OgreApplication* app, const std::string& mediaPath);
			virtual ~ResourceLoader();

			bool load_async();
			bool load();
			void loadScripts();

			std::string getMediaPath(const std::string& filename, const std::string& folder = "") const;

			void resourceGroupScriptingStarted(const Ogre::String& groupName,
			                                   size_t scriptCount) override;
			void scriptParseStarted(const Ogre::String& scriptName, bool& skipThisScript) override;
			void scriptParseEnded(const Ogre::String& scriptName, bool skipped) override;
			void resourceGroupScriptingEnded(const Ogre::String& groupName) override;
			void resourceGroupLoadStarted(const Ogre::String& groupName,
			                              size_t resourceCount) override;
			void resourceLoadStarted(const Ogre::ResourcePtr& resource) override;
			void resourceLoadEnded() override;
			void worldGeometryStageStarted(const Ogre::String& description) override;
			void worldGeometryStageEnded() override;
			void resourceGroupLoadEnded(const Ogre::String& groupName) override;

			Scripting::Parsing::ScriptParser* scriptParser() const;

			static ResourceLoader &getSingleton();
			static ResourceLoader *getSingletonPtr();

		private:
			Ogre::ResourceGroupManager* mRgm;

			App::Application* mApp;

			std::string mMediaPath;

			std::unique_ptr<Scripting::Parsing::OgreScriptParser> mParser;
			std::unique_ptr<ImageSets::ImageSetManager> mImageSetManager;

			static ResourceLoader *sSingleton;
		};
	}
}
