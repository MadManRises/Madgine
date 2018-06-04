#pragma once

#include "Madgine/serialize/container/map.h"

#include "scopewrapper.h"

#include "Madgine/serialize/container/action.h"

namespace Maditor {
	namespace Model {
		class MADITOR_MODEL_EXPORT Inspector : public TreeUnit<Inspector> {
			Q_OBJECT
		public:
			Inspector();

			void start();
			void reset();

			virtual TreeItem *child(int i) override;
			virtual int childCount() const override;

			void sendUpdateImpl(Engine::InvScopePtr ptr, bool exists, const Engine::Serialize::SerializableMap<std::string, std::tuple<Engine::ValueType, Engine::KeyValueValueFlags>> &attributes);


			QModelIndex registerIndex(QObject *object, Engine::InvScopePtr ptr);
			void unregisterIndex(QObject *object);

			QModelIndex updateIndex(QObject *object, Engine::InvScopePtr ptr);

			virtual Qt::ItemFlags flags(const QModelIndex &index) const override;


		protected:
			void requestUpdateImpl(Engine::InvScopePtr ptr) {}

			virtual void timerEvent(QTimerEvent *e) override;

		private:
			std::map<Engine::InvScopePtr, ScopeWrapperItem> mWrappers;
			std::map<QObject *, Engine::InvScopePtr> mIndices;
			std::map<QObject *, Engine::InvScopePtr>::iterator mIt;

			Engine::Serialize::Action<decltype(&Inspector::requestUpdateImpl), &Inspector::requestUpdateImpl, Engine::Serialize::ActionPolicy::request> mRequestUpdate;
			Engine::Serialize::Action<decltype(&Inspector::sendUpdateImpl), &Inspector::sendUpdateImpl, Engine::Serialize::ActionPolicy::notification> mSendUpdate;

			bool mPending;
			int mTimer;
		};
	}
}
