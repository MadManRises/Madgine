#include "maditormodellib.h"
#include "inspector.h"

namespace Maditor {
	namespace Model {

		Inspector::Inspector() :
			TreeUnit(2),
			mPending(false),
			mTimer(0)
		{
			reset();
			
		}

		void Inspector::start()
		{
			mTimer = startTimer(100);
		}

		void Inspector::reset()
		{
			beginResetModel();
			mWrappers.clear();
			mWrappers.try_emplace(nullptr, this, nullptr);
			mIt = mIndices.end();
			if (mTimer) {
				killTimer(mTimer);
				mTimer = 0;
				mPending = false;
			}
			endResetModel();
		}

		TreeItem * Inspector::child(int i)
		{
			auto it = mWrappers.begin();
			std::advance(it, i);
			return &it->second;
		}

		int Inspector::childCount() const
		{
			return mWrappers.size();
		}

		void Inspector::timerEvent(QTimerEvent * e)
		{
			if (mPending)
				return;
			if (!mIndices.empty()) {
				if (mIt == mIndices.end())
					mIt = mIndices.begin();
				mPending = true;
				mRequestUpdate(mIt->second, {});
				++mIt;				
			}
		}

		void Inspector::sendUpdateImpl(Engine::InvScopePtr ptr, bool exists, const Engine::Serialize::SerializableMap<std::string, std::tuple<Engine::ValueType, Engine::KeyValueValueFlags>> &attributes) {
			mPending = false;
			auto it = mWrappers.find(ptr);
			assert(it != mWrappers.end());
			if (exists) {
				it->second.update(attributes.data());
			}
			else {
				mWrappers.erase(it);
			}
		}

		QModelIndex Inspector::updateIndex(QObject *object, Engine::InvScopePtr ptr) {
			auto it = mIndices.find(object);
			if (it == mIndices.end())
				throw 0;
			it->second = ptr;
			auto result = mWrappers.try_emplace(ptr, this, ptr);
			int i = std::distance(mWrappers.begin(), result.first);
			if (result.second) {
				beginInsertRows(QModelIndex(), i, i);
				endInsertRows();
			}			
			return index(i, 0);
		}

		QModelIndex Inspector::registerIndex(QObject * object, Engine::InvScopePtr ptr)
		{
			auto it = mIndices.find(object);
			if (it != mIndices.end())
				throw 0;
			mIndices[object] = ptr;
			auto it2 = mWrappers.find(ptr);
			return index(std::distance(mWrappers.begin(), it2), 0);
		}

		void Inspector::unregisterIndex(QObject * object)
		{
			auto it = mIndices.find(object);
			if (it == mIndices.end())
				throw 0;
			mIndices.erase(it);
		}

		Qt::ItemFlags Inspector::flags(const QModelIndex & index) const
		{
			Qt::ItemFlags flags = TreeUnit::flags(index);
			ValueItem *value = dynamic_cast<ValueItem*>(item(index));
			if (value && value->isEditable()) {
				flags |= Qt::ItemIsEditable;
			}
			return flags;
		}

	}
}
