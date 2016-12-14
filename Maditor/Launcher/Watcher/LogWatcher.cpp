#include "madgineinclude.h"

#include "OgreLogManager.h"

#include "LogWatcher.h"

#include "LogInfo.h"

#include "Shared.h"

			LogWatcher::LogWatcher(const std::string &name, const std::string & root) :
				mName(name),
				mSourcesRoot(root + "src/")
			{
				Ogre::LogManager::getSingleton().getLog(name)->addListener(this);
			}
			
			void LogWatcher::logMessage(const std::string & msg, Ogre::LogMessageLevel level, const Engine::Util::TraceBack *traceback, const std::string &fullTraceback)
			{
				/*LogMessage &message = mShared.mMadgineLog.create();
				message.mFullTraceback = fullTraceback.c_str();
				switch (level) {
				case Ogre::LML_TRIVIAL:
					message.mType = LOG_TYPE;
					break;
				case Ogre::LML_NORMAL:
					message.mType = WARNING_TYPE;
					break;
				case Ogre::LML_CRITICAL:
					message.mType = ERROR_TYPE;
					break;
				}
				message.mMsg = msg.c_str();
				if (traceback) {
					strcpy_s(message.mTraceback.mFile, traceback->mFile.c_str());
					strcpy_s(message.mTraceback.mFunction, traceback->mFunction.c_str());
					message.mTraceback.mLineNr = traceback->mLineNr;
				}
				else {
					message.mTraceback.mFile[0] = '\0';
					message.mTraceback.mFunction[0] = '\0';
					message.mTraceback.mLineNr = -1;
				}
				mShared.mMadgineLog.send();*/
			}

			void LogWatcher::messageLogged(const Ogre::String & message, Ogre::LogMessageLevel lml, bool maskDebug, const Ogre::String & logName, bool & skipThisMessage)
			{
				std::stringstream fullTraceback;
				const Engine::Util::TraceBack *last = 0;
				for (const Engine::Util::TraceBack &t : Engine::Util::UtilMethods::traceBack()) {
					if (last)
						fullTraceback << std::endl;
					last = &t;
					fullTraceback << /*relative*/t.mFile << "(" << t.mLineNr << "): " << t.mFunction;
				}
				logMessage(message, lml, last, fullTraceback.str());				
			}


