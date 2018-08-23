#pragma once

#include "Madgine/serialize/container/action.h"
#include "Madgine/serialize/container/map.h"
#include "Madgine/scripting/types/globalapicomponent.h"

#include "Madgine/signalslot/slot.h"
#include "Madgine/scripting/datatypes/luatable.h"

#include "Madgine/generic/valuetype.h"

namespace Maditor {
	namespace Launcher {

		class InspectorThreadInstance : public Engine::Scripting::GlobalAPIComponent<InspectorThreadInstance> {
		public:
			InspectorThreadInstance(Engine::App::Application &app);

			void getUpdate(Engine::InvScopePtr ptr, Inspector *inspector);
			void setField(Engine::InvScopePtr ptr, const std::string &name, const Engine::ValueType &value, Inspector *inspector);

			lua_State *state();

			static InspectorThreadInstance *getInstance(lua_State *thread);			

		//protected:
			void update(Engine::InvScopePtr ptr, Inspector *inspector);
			void setFieldImpl(Engine::InvScopePtr ptr, const std::string &name, const Engine::ValueType &value, Inspector *inspector);

		protected:
			virtual bool init() override;
			virtual void finalize() override;

		private:
			Engine::SignalSlot::Slot<&InspectorThreadInstance::update> mUpdate;
			Engine::SignalSlot::Slot<&InspectorThreadInstance::setFieldImpl> mSetField;

			static std::mutex sMappingsMutex;
			static std::map<lua_State *, InspectorThreadInstance*> sMappings;

			lua_State *mState;
		};

		class Inspector : public Engine::Serialize::SerializableUnit<Inspector> {
		public:
			Inspector();

			void init(Engine::Scripting::GlobalScopeBase &global);
			void getUpdate(Engine::InvScopePtr ptr, InspectorThreadInstance *thread);
			void setField(Engine::InvScopePtr ptr, const std::string &name, const Engine::ValueType &value);
		
		protected:
			void requestUpdateImpl(Engine::InvScopePtr ptr);
			void setFieldImpl(Engine::InvScopePtr ptr, const std::string &name, const Engine::ValueType &value);
			void sendUpdateImpl(Engine::InvScopePtr ptr, bool exists, const std::string &key, const Engine::Serialize::SerializableMap<std::string, std::tuple<Engine::ValueType, std::string, Engine::KeyValueValueFlags>> &attributes) {}

			void itemRemoved(Engine::InvScopePtr ptr);
			void itemUpdate(Engine::InvScopePtr ptr, const std::string &key, const Engine::Serialize::SerializableMap<std::string, std::tuple<Engine::ValueType, std::string, Engine::KeyValueValueFlags>> &attributes);

		protected:
			Engine::Scripting::ScopeBase *validate(Engine::InvScopePtr ptr);
			bool isValid(Engine::InvScopePtr ptr);
			void markItem(Engine::Scripting::ScopeBase *ptr, InspectorThreadInstance *thread);

		private:
			std::mutex mItemsMutex;
			std::map<Engine::InvScopePtr, std::tuple<InspectorThreadInstance*, bool>> mItems;

			Engine::Serialize::Action<&Inspector::requestUpdateImpl, Engine::Serialize::ActionPolicy::request> mRequestUpdate;
			Engine::Serialize::Action<&Inspector::sendUpdateImpl, Engine::Serialize::ActionPolicy::notification> mSendUpdate;
			Engine::Serialize::Action<&Inspector::setFieldImpl, Engine::Serialize::ActionPolicy::request> mSetField;
			std::unique_ptr<Engine::SignalSlot::Slot<&Inspector::itemRemoved>> mItemRemoved;
			std::unique_ptr<Engine::SignalSlot::Slot<&Inspector::itemUpdate>> mItemUpdate;

			static const luaL_Reg sMarkMetafunctions[];
			static int lua_scopeGc(lua_State *);

			Engine::Scripting::GlobalScopeBase *mGlobal;
		};
	}
}
