#pragma once

#include "LogTableModel.h"
#include "Model\log.h"


namespace Maditor {
	namespace Model {
		namespace Watcher {


			class OgreLogWatcher : public Log, public Ogre::LogListener {
				Q_OBJECT
			public:
				enum LogType {
					TextLog,
					GuiLog
				};

				OgreLogWatcher(LogType type, const std::string &name = "");
				~OgreLogWatcher();

				void listen(Ogre::Log *log, const QString &root);
				void stopListening(bool unregister = true);

				LogType type();

				virtual std::string getName() override;

				LogTableModel *model();

				void logMessage(const QString &msg, Ogre::LogMessageLevel level = Ogre::LML_TRIVIAL, const QList<Engine::Util::TraceBack> &traceback = {});

			signals:


				void ogreMessageReceived(const QString &msg, Ogre::LogMessageLevel level = Ogre::LML_TRIVIAL, const QList<Engine::Util::TraceBack> &traceback = {});

			private:
				// Geerbt über LogListener
				virtual void messageLogged(const Ogre::String & message, Ogre::LogMessageLevel lml, bool maskDebug, const Ogre::String & logName, bool & skipThisMessage) override;

				std::unique_ptr<LogTableModel> mModel;
				Ogre::Log *mLog;
				LogType mType;
				QDir mSourcesRoot;
				std::string mName;
			};

		}
	}
}