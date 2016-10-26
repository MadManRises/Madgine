#pragma once

#include "UI\Process.h"
#include "ImageSets\imagesetmanager.h"
#include "Scripting\Parsing\scriptparser.h"

namespace Engine {
	namespace Resources {

		class MADGINE_EXPORT ResourceLoader : public UI::Process, public Ogre::ResourceGroupListener, public Ogre::Singleton<ResourceLoader>, public Ogre::GeneralAllocatedObject {
		public:
			ResourceLoader(const std::string &mediaPath);
			~ResourceLoader();

			void load();

			std::string getMediaPath(const std::string &filename, const std::string &folder = "");

			void resourceGroupScriptingStarted(const Ogre::String &groupName,
				size_t scriptCount);
			void scriptParseStarted(const Ogre::String &scriptName, bool &skipThisScript);
			void scriptParseEnded(const Ogre::String &scriptName, bool skipped);
			void resourceGroupScriptingEnded(const Ogre::String &groupName);
			void resourceGroupLoadStarted(const Ogre::String &groupName,
				size_t resourceCount);
			void resourceLoadStarted(const Ogre::ResourcePtr &resource);
			void resourceLoadEnded(void);
			void worldGeometryStageStarted(const Ogre::String &description);
			void worldGeometryStageEnded(void);
			void resourceGroupLoadEnded(const Ogre::String &groupName);

			Scripting::Parsing::ScriptParser *scriptParser();

		private:
			Ogre::ResourceGroupManager *mRgm;

			std::string mMediaPath;

			Ogre::unique_ptr<Scripting::Parsing::ScriptParser> mParser;
			Ogre::unique_ptr<ImageSets::ImageSetManager> mImageSetManager;
		};

	}
}