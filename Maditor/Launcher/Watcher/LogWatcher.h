#pragma once


			class LogWatcher : public Ogre::LogListener {
			public:
				LogWatcher(const std::string &name, const std::string &root);
				
				void logMessage(const std::string &msg, Ogre::LogMessageLevel level = Ogre::LML_TRIVIAL, const Engine::Util::TraceBack *traceback = 0, const std::string &fullTraceback = "");
			private:
				// Geerbt über LogListener
				virtual void messageLogged(const Ogre::String & message, Ogre::LogMessageLevel lml, bool maskDebug, const Ogre::String & logName, bool & skipThisMessage) override;

				std::string mSourcesRoot;
				std::string mName;


			};
