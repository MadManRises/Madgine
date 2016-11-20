#include "maditorlib.h"

#include "LogWatcher.h"

#include "Common\LogInfo.h"

#include "Common\Shared.h"

namespace Maditor {
	namespace Model {
		namespace Watcher {


			OgreLogWatcher::OgreLogWatcher(LogType type, const std::string &name) :
				mType(type),
				mName(name),
				mShared(SharedMemory::getSingleton().mLog)
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

			void OgreLogWatcher::update()
			{
				while (!mShared.mMadgineLog.empty()) {
					const LogMessage &msg = mShared.mMadgineLog.front();
					logMessage(msg.mMsg.c_str(), msg.mType, msg.mTraceback, msg.mFullTraceback.c_str());
					mShared.mMadgineLog.pop();
				}
			}

		}
	}
}