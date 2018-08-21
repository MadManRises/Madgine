#pragma once

#include "Madgine/serialize/container/map.h"

#include "scopewrapper.h"

#include "Madgine/serialize/container/action.h"

namespace Maditor {
	namespace Model {
		class MADITOR_MODEL_EXPORT Inspector : public QObject, public Engine::Serialize::SerializableUnit<Inspector> {
			Q_OBJECT
		public:
			Inspector();

			void start();
			void reset();

			void sendUpdateImpl(Engine::InvScopePtr ptr, bool exists, const std::string &key, const Engine::Serialize::SerializableMap<std::string, std::tuple<Engine::ValueType, std::string, Engine::KeyValueValueFlags>> &attributes);
			void setField(Engine::InvScopePtr ptr, const std::string &name, const Engine::ValueType &value);

			ScopeWrapperItem *registerScope(Engine::InvScopePtr ptr);
			void unregisterScope(Engine::InvScopePtr ptr);			

			ScopeWrapperItem *getScope(Engine::InvScopePtr ptr);

			ScopeWrapperItem *root();

		signals:
			void modelReset();

		protected:
			void requestUpdateImpl(Engine::InvScopePtr ptr) {}
			void setFieldImpl(Engine::InvScopePtr ptr, const std::string &name, const Engine::ValueType &value) {}

			virtual void timerEvent(QTimerEvent *e) override;

		private:
			std::map<Engine::InvScopePtr, ScopeWrapperItem> mWrappers;
			std::map<Engine::InvScopePtr, ScopeWrapperItem>::iterator mIt;

			Engine::Serialize::Action<&Inspector::requestUpdateImpl, Engine::Serialize::ActionPolicy::request> mRequestUpdate;
			Engine::Serialize::Action<&Inspector::sendUpdateImpl, Engine::Serialize::ActionPolicy::notification> mSendUpdate;
			Engine::Serialize::Action<&Inspector::setFieldImpl, Engine::Serialize::ActionPolicy::request> mSetField;

			bool mPending;
			int mTimer;
		};
	}
}
