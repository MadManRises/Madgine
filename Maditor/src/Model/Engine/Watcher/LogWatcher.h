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

				OgreLogWatcher(Ogre::Log *log, LogType type, const QString &root);
				~OgreLogWatcher();

				LogType type();

				virtual std::string getName() override;

				LogTableModel *model();

			signals:


				void ogreMessageReceived(const QString &msg, Ogre::LogMessageLevel level = Ogre::LML_NORMAL, const QList<Engine::Util::TraceBack> &traceback = {});

			private:
				// Geerbt über LogListener
				virtual void messageLogged(const Ogre::String & message, Ogre::LogMessageLevel lml, bool maskDebug, const Ogre::String & logName, bool & skipThisMessage) override;

				std::unique_ptr<LogTableModel> mModel;
				Ogre::Log *mLog;
				LogType mType;
			};

		}
	}
}