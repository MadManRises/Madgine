#include "maditorlib.h"

#include "LogWatcher.h"

#include "LogInfo.h"

#include "Shared.h"

namespace Maditor {
	namespace Model {
		namespace Watcher {


			OgreLogWatcher::OgreLogWatcher(LogType type, const std::string &name) :
				mType(type),
				mName(name)
			{
				if (mType == GuiLog) {
					mModel = std::unique_ptr<LogTableModel>(new LogTableModel);
					qRegisterMetaType<MessageType>();
					qRegisterMetaType<Traceback>();
					connect(this, &OgreLogWatcher::ogreMessageReceived, mModel.get(), &Model::Watcher::LogTableModel::addMessage);
				}
			}

			OgreLogWatcher::~OgreLogWatcher()
			{
			}

			OgreLogWatcher::LogType OgreLogWatcher::type()
			{
				return mType;
			}

			std::string OgreLogWatcher::getName()
			{
				return mName;
			}

			LogTableModel * OgreLogWatcher::model()
			{
				return mModel.get();
			}


			void OgreLogWatcher::logMessage(const QString & msg, MessageType level, const Traceback &traceback, const QString &fullTraceback)
			{
				emit ogreMessageReceived(msg, level, traceback, fullTraceback);
				emit messageReceived(msg);
			}


		}
	}
}