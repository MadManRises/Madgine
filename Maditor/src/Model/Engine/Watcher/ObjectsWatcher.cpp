#include "madgineinclude.h"

#include "ObjectsWatcher.h"

#include "Util\MadgineObjectCollector.h"

#include "Common/Shared.h"

namespace Maditor {
	namespace Model {
		namespace Watcher {

			ObjectsWatcher::ObjectsWatcher() :
				mShared(SharedMemory::getSingleton().mObjects)
			{
				startTimer(3000);
			}


			void ObjectsWatcher::timerEvent(QTimerEvent * event)
			{
				boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(mShared.mMutex);
				if (mShared.mDataChanged) {
					beginResetModel();
					for (const ObjectInfo &o : mShared.mObjects) {
						auto it = std::find_if(mObjects.begin(), mObjects.end(), [=](const Object &ob) {return ob.mName == o.mName.c_str(); });
						if (it == mObjects.end())
							mObjects.push_back({ o.mName.c_str(), o.mState, o.mPtr });
						else {
							it->mState = o.mState;
							it->mPtr = o.mPtr;
						}
					}
					mShared.mDataChanged = false;
					endResetModel();
				}
			}

			Q_INVOKABLE int ObjectsWatcher::rowCount(const QModelIndex & parent) const
			{

				return mObjects.size();
			}
			Q_INVOKABLE int ObjectsWatcher::columnCount(const QModelIndex & parent) const
			{
				return 3;
			}
			Q_INVOKABLE QVariant ObjectsWatcher::data(const QModelIndex & index, int role) const
			{
				if (!index.isValid())
					return QVariant();

				auto it = mObjects.begin();
				std::advance(it, index.row());
				const Object &o = *it;

				if (role == Qt::BackgroundColorRole) {
					if (index.column() != 1)
						return QVariant();
					else {
						switch(o.mState) {
						case (Engine::Util::ObjectState)-1:
							return QColor(Qt::red);
						case Engine::Util::ObjectState::CONSTRUCTED:
							return QColor(Qt::yellow);
						case Engine::Util::ObjectState::INITIALIZED:
							return QColor(Qt::green);
						case Engine::Util::ObjectState::IN_SCENE:
							return QColor(Qt::magenta);
						default:
							return QVariant();
						}
					}
				}
				
				if (role == Qt::DisplayRole) {

					switch (index.column()) {
					case 0:
						return QString::fromStdString(o.mName);
					case 1:
						switch (o.mState) {
						case (Engine::Util::ObjectState) - 1:
							return "Deleted";
						case Engine::Util::ObjectState::CONSTRUCTED:
							return "Constructed";
						case Engine::Util::ObjectState::INITIALIZED:
							return "Initialized";
						case Engine::Util::ObjectState::IN_SCENE:
							return "In Scene";
						default:
							return QVariant();
						}
					case 2:
						return QString::number((uint64_t)o.mPtr, 16);
					default:
						return QVariant();
					}
				}

				return QVariant();

			}
		}
	}
}