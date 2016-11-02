#include "maditorinclude.h"

#include "LogWatcher.h"


namespace Maditor {
	namespace Model {
		namespace Watcher {


			OgreLogWatcher::OgreLogWatcher(LogType type, const std::string &name) :
				mLog(0),
				mType(type),
				mName(name)
			{
				if (QApplication::instance())
					moveToThread(QApplication::instance()->thread());

				if (mType == GuiLog) {
					mModel = std::unique_ptr<LogTableModel>(new LogTableModel);
					if (QApplication::instance())
						mModel->moveToThread(QApplication::instance()->thread());
					connect(this, &OgreLogWatcher::ogreMessageReceived, mModel.get(), &Model::Watcher::LogTableModel::addMessage, Qt::QueuedConnection);
				}
			}

			OgreLogWatcher::~OgreLogWatcher()
			{
				if (mLog)
					mLog->removeListener(this);
			}

			void OgreLogWatcher::listen(Ogre::Log * log, const QString & root)
			{
				mLog = log;
				mSourcesRoot = root + "src/";
				mLog->addListener(this);
				
			}

			void OgreLogWatcher::stopListening(bool unregister)
			{
				if (mLog) {
					if (unregister)
						mLog->removeListener(this);
					mLog = 0;
				}
			}

			OgreLogWatcher::LogType OgreLogWatcher::type()
			{
				return mType;
			}

			std::string OgreLogWatcher::getName()
			{
				return mLog ? mLog->getName() : mName;
			}

			LogTableModel * OgreLogWatcher::model()
			{
				return mModel.get();
			}

			void OgreLogWatcher::logMessage(const QString & msg, Ogre::LogMessageLevel level, const QList<Engine::Util::TraceBack> &traceback)
			{
				emit ogreMessageReceived(msg, level, traceback);
				emit messageReceived(msg);
			}

			void OgreLogWatcher::messageLogged(const Ogre::String & message, Ogre::LogMessageLevel lml, bool maskDebug, const Ogre::String & logName, bool & skipThisMessage)
			{
				QList<Engine::Util::TraceBack> list;
				for (Engine::Util::TraceBack t : Engine::Util::UtilMethods::traceBack()) {
					t.mFile = mSourcesRoot.relativeFilePath(QString::fromStdString(t.mFile)).toStdString();
					list << t;
				}
				logMessage(QString::fromStdString(message), lml, list);				
			}

		}
	}
}