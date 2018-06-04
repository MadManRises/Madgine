#pragma once

#include "Madgine/serialize/container/action.h"
#include "Madgine/serialize/container/map.h"
#include "Madgine/scripting/types/globalapicomponent.h"

#include "Madgine/signalslot/slot.h"
#include "Madgine/scripting/datatypes/luatable.h"

namespace Maditor {
	namespace Launcher {

		class InspectorThreadInstance : public Engine::Scripting::GlobalAPIComponent<InspectorThreadInstance> {
		public:
			InspectorThreadInstance(Engine::App::Application &app);

			void getUpdate(Engine::InvScopePtr ptr, Inspector *inspector);

			lua_State *state();

			virtual bool init() override;
			virtual void finalize() override;

			static InspectorThreadInstance *getInstance(lua_State *thread);			

		protected:
			void update(Engine::InvScopePtr ptr, Inspector *inspector);

		private:
			Engine::SignalSlot::Slot<decltype(&InspectorThreadInstance::update), &InspectorThreadInstance::update> mUpdate;

			static std::mutex sMappingsMutex;
			static std::map<lua_State *, InspectorThreadInstance*> sMappings;

			lua_State *mState;
		};

		class Inspector : public Engine::Serialize::SerializableUnit<Inspector> {
		public:
			Inspector();

			void init(Engine::Scripting::GlobalScopeBase &global);
			void getUpdate(Engine::InvScopePtr ptr, InspectorThreadInstance *thread);
		
		protected:
			void requestUpdateImpl(Engine::InvScopePtr ptr);
			void sendUpdateImpl(Engine::InvScopePtr ptr, bool exists, const Engine::Serialize::SerializableMap<std::string, std::tuple<Engine::ValueType, Engine::KeyValueValueFlags>> &attributes) {}

			void itemRemoved(Engine::InvScopePtr ptr);
			void itemUpdate(Engine::InvScopePtr ptr, const Engine::Serialize::SerializableMap<std::string, std::tuple<Engine::ValueType, Engine::KeyValueValueFlags>> &attributes);

		protected:
			Engine::Scripting::ScopeBase *validate(Engine::InvScopePtr ptr);
			bool isValid(Engine::InvScopePtr ptr);
			void markItem(Engine::Scripting::ScopeBase *ptr, InspectorThreadInstance *thread);

		private:
			std::mutex mItemsMutex;
			std::map<Engine::InvScopePtr, std::tuple<InspectorThreadInstance*, bool>> mItems;

			Engine::Serialize::Action<decltype(&Inspector::requestUpdateImpl), &Inspector::requestUpdateImpl, Engine::Serialize::ActionPolicy::request> mRequestUpdate;
			Engine::Serialize::Action<decltype(&Inspector::sendUpdateImpl), &Inspector::sendUpdateImpl, Engine::Serialize::ActionPolicy::notification> mSendUpdate;
			std::unique_ptr<Engine::SignalSlot::Slot<decltype(&Inspector::itemRemoved), &Inspector::itemRemoved>> mItemRemoved;
			std::unique_ptr<Engine::SignalSlot::Slot<decltype(&Inspector::itemUpdate), &Inspector::itemUpdate>> mItemUpdate;

			static const luaL_Reg sMarkMetafunctions[];
			static int lua_scopeGc(lua_State *);

			Engine::Scripting::GlobalScopeBase *mGlobal;
		};
	}
}
