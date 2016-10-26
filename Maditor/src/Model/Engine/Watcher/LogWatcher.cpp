#include "maditorinclude.h"

#include "LogWatcher.h"


namespace Maditor {
	namespace Model {
		namespace Watcher {


			OgreLogWatcher::OgreLogWatcher(Ogre::Log * log, LogType type, const QString & root) :
				mLog(log),
				mType(type)
			{
				moveToThread(QApplication::instance()->thread());
				mLog->addListener(this);
				if (type == GuiLog) {
					mModel = std::unique_ptr<LogTableModel>(new LogTableModel(root));
					mModel->moveToThread(QApplication::instance()->thread());
					connect(this, &OgreLogWatcher::ogreMessageReceived, mModel.get(), &Model::Watcher::LogTableModel::addMessage, Qt::QueuedConnection);
					connect(mModel.get(), &LogTableModel::openScriptFile, this, &OgreLogWatcher::openScriptFile);
				}
			}

			OgreLogWatcher::~OgreLogWatcher()
			{
				mLog->removeListener(this);
			}

			OgreLogWatcher::LogType OgreLogWatcher::type()
			{
				return mType;
			}

			std::string OgreLogWatcher::getName()
			{
				return mLog->getName();
			}

			LogTableModel * OgreLogWatcher::model()
			{
				return mModel.get();
			}

			void OgreLogWatcher::messageLogged(const Ogre::String & message, Ogre::LogMessageLevel lml, bool maskDebug, const Ogre::String & logName, bool & skipThisMessage)
			{
				QList<Engine::Util::TraceBack> list;
				for (const Engine::Util::TraceBack &t : Engine::Util::UtilMethods::traceBack()) {
					list << t;
				}
				emit ogreMessageReceived(message.c_str(), lml, list);
				emit messageReceived(message.c_str());
			}

		}
	}
}