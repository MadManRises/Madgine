#include "maditormodellib.h"
#include "inspector.h"

namespace Maditor {
	namespace Model {

		Inspector::Inspector() :
			mPending(false),
			mTimer(0)
		{
			reset();
			
		}

		void Inspector::start()
		{
			mTimer = startTimer(10);
		}

		void Inspector::reset()
		{
			mWrappers.clear();
			mWrappers.try_emplace(nullptr, this, nullptr).first->second.setType("Application");
			mIt = mWrappers.end();
			emit modelReset();
			if (mTimer) {
				killTimer(mTimer);
				mTimer = 0;
				mPending = false;
			}
		}

		void Inspector::timerEvent(QTimerEvent * e)
		{
			if (mPending)
				return;
			if (!mWrappers.empty()) {
				if (mIt == mWrappers.end())
					mIt = mWrappers.begin();
				mPending = true;
				mRequestUpdate(mIt->first, {});
				++mIt;				
			}
		}

		void Inspector::sendUpdateImpl(Engine::InvScopePtr ptr, bool exists, const std::string &key, const Engine::Serialize::SerializableMap<std::string, std::tuple<Engine::ValueType, std::string, Engine::KeyValueValueFlags>> &attributes) {
			mPending = false;
			auto it = mWrappers.find(ptr);
			if (it != mWrappers.end()) {
				if (exists) {
					it->second.update(key, attributes.data());
				}
				else {
					mWrappers.erase(it);
				}
			}
		}

		void Inspector::setField(Engine::InvScopePtr ptr, const std::string &name, const Engine::ValueType &value)
		{
			mSetField(ptr, name, value, {});
		}

		ScopeWrapperItem* Inspector::registerScope(Engine::InvScopePtr ptr)
		{
			return getScope(ptr);
		}

		void Inspector::unregisterScope(Engine::InvScopePtr ptr)
		{
			auto it = mWrappers.find(ptr);
			if (it == mIt) ++mIt;
			mWrappers.erase(it);
		}

		ScopeWrapperItem* Inspector::getScope(Engine::InvScopePtr ptr)
		{
			return &mWrappers.try_emplace(ptr, this, ptr).first->second;
		}

		ScopeWrapperItem* Inspector::root()
		{
			return &mWrappers.at(nullptr);
		}
	}
}
