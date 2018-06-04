#include "maditorlauncherlib.h"
#include "inspector.h"

#include "Madgine/scripting/types/globalscopebase.h"
#include "Madgine/app/application.h"

#include "Madgine/scripting/parsing/scriptparser.h"
#include "Madgine/signalslot/connectionstore.h"

#include "Madgine/scripting/types/luastate.h"

extern "C"
{
#include <lua/lua.h>                                /* Always include this when calling Lua */
#include <lua/lauxlib.h>                            /* Always include this when calling Lua */
#include <lua/lualib.h>                             /* Always include this when calling Lua */
}

API_IMPL(Maditor::Launcher::InspectorThreadInstance);


namespace Maditor
{
	namespace Launcher
	{
		std::mutex InspectorThreadInstance::sMappingsMutex;
		std::map<lua_State*, InspectorThreadInstance *> InspectorThreadInstance::sMappings;

		const luaL_Reg Inspector::sMarkMetafunctions[] =
		{
			{"__gc", &lua_scopeGc},
			//{ "__pairs", &lua_pairs },
			//{"__newindex", &lua_newindex}
			{NULL, NULL}
		};


		int Inspector::lua_scopeGc(lua_State* state)
		{
			bool** ptr = static_cast<bool**>(lua_touserdata(state, -1));
			**ptr = false;
			lua_pop(state, 1);
			return 0;
		}


		InspectorThreadInstance::InspectorThreadInstance(Engine::App::Application &app) :
			Engine::Scripting::GlobalAPIComponent<InspectorThreadInstance>(app),
			mUpdate(this),			
			mState(nullptr)
		{
		}

		lua_State* InspectorThreadInstance::state()
		{
			return mState;
		}

		InspectorThreadInstance* InspectorThreadInstance::getInstance(lua_State* thread)
		{
			InspectorThreadInstance* result = nullptr;
			sMappingsMutex.lock();
			auto it = sMappings.find(thread);
			if (it != sMappings.end())
			{
				result = it->second;
			}
			sMappingsMutex.unlock();
			return result;
		}

		bool InspectorThreadInstance::init()
		{
			if (Engine::Scripting::GlobalAPIComponent<InspectorThreadInstance>::init())
			{
				mState = globalScope().lua_state();
				sMappingsMutex.lock();
				sMappings[mState] = this;
				sMappingsMutex.unlock();
				return true;
			}
			else
			{
				return false;
			}
		}

		void InspectorThreadInstance::finalize()
		{
			sMappingsMutex.lock();
			sMappings.erase(mState);
			sMappingsMutex.unlock();
			Engine::Scripting::GlobalAPIComponent<InspectorThreadInstance>::finalize();
		}

		void InspectorThreadInstance::getUpdate(Engine::InvScopePtr ptr, Inspector* inspector)
		{
			mUpdate.queue(ptr, inspector);
		}

		void InspectorThreadInstance::update(Engine::InvScopePtr ptr, Inspector* inspector)
		{
			inspector->getUpdate(ptr, this);
		}

		Inspector::Inspector() :
		mGlobal(nullptr)
		{
		}

		void Inspector::getUpdate(Engine::InvScopePtr ptr, InspectorThreadInstance* thread)
		{
			Engine::Scripting::ScopeBase* scope;
			if (isValid(ptr))
			{
				scope = validate(ptr);
			}
			else
			{
				mItemRemoved->queue(ptr);
				return;
			}

			Engine::Serialize::SerializableMap<std::string, std::tuple<Engine::ValueType, Engine::KeyValueValueFlags>>
				attributes;
			for (std::unique_ptr<Engine::KeyValueIterator> it = scope->iterator(); !it->ended(); ++(*it))
			{
				Engine::ValueType value = it->value();
				if (value.is<Engine::Scripting::ScopeBase*>())
				{
					attributes.try_emplace(
						it->key(), Engine::ValueType(Engine::InvScopePtr(value.as<Engine::Scripting::ScopeBase*>())), it->flags());
					mItemsMutex.lock();
					auto it2 = mItems.find(value.as<Engine::Scripting::ScopeBase*>());
					if (it2 == mItems.end())
					{
						markItem(value.as<Engine::Scripting::ScopeBase*>(), thread);
					}
					mItemsMutex.unlock();
				}
				else if (value.is<Engine::Scripting::LuaThread>())
				{
					lua_State* luaThread = value.as<Engine::Scripting::LuaThread>();
					InspectorThreadInstance* otherThread = InspectorThreadInstance::getInstance(luaThread);
					if (otherThread)
					{
						Engine::Scripting::ScopeBase* global = &otherThread->globalScope();
						attributes.try_emplace(it->key(), Engine::ValueType(Engine::InvScopePtr(global)), it->flags());
						mItemsMutex.lock();
						auto it = mItems.find(global);
						if (it == mItems.end())
						{
							markItem(global, otherThread);
						}
						mItemsMutex.unlock();
					}
				}
				else if (value.is<Engine::Scripting::ApiMethod>())
				{
				}
				else
				{
					attributes.try_emplace(it->key(), value, it->flags());
				}
			}
			mItemUpdate->queue(ptr, attributes);
		}

		void Inspector::init(Engine::Scripting::GlobalScopeBase &global)
		{
			mGlobal = &global;

			lua_State *state = global.lua_state();

			luaL_newmetatable(state, "Maditor.InspectorMarkMetatable");

			luaL_setfuncs(state, sMarkMetafunctions, 0);

			lua_pop(state, 1);

			mItemRemoved = std::make_unique<Engine::SignalSlot::Slot<decltype(&Inspector::itemRemoved), &Inspector::itemRemoved>
			>(this);
			mItemUpdate = std::make_unique<Engine::SignalSlot::Slot<decltype(&Inspector::itemUpdate), &Inspector::itemUpdate>
			>(this);
		}

		void Inspector::itemRemoved(Engine::InvScopePtr ptr)
		{
			mItemsMutex.lock();
			mItems.erase(ptr);
			mItemsMutex.unlock();
			mSendUpdate(ptr, false, {}, {});
		}

		void Inspector::itemUpdate(Engine::InvScopePtr ptr,
		                           const Engine::Serialize::SerializableMap<
			                           std::string, std::tuple<Engine::ValueType, Engine::KeyValueValueFlags>>& attributes)
		{
			mSendUpdate(ptr, true, attributes, {});
		}

		void Inspector::requestUpdateImpl(Engine::InvScopePtr ptr)
		{
			if (ptr)
			{
				mItemsMutex.lock();
				auto it = mItems.find(ptr);
				if (it != mItems.end())
				{
					InspectorThreadInstance* thread = std::get<0>(it->second);
					mItemsMutex.unlock();
					if (thread) {
						thread->getUpdate(ptr, this);
					}else
					{
						getUpdate(ptr, nullptr);
					}
				}
				else
				{
					mItemsMutex.unlock();
					mSendUpdate(ptr, false, {}, {});
				}
			}
			else
			{
				getUpdate(ptr, nullptr);
			}
		}

		bool Inspector::isValid(Engine::InvScopePtr ptr)
		{
			if (!ptr)
			{
				return true;
			}
			else
			{
				mItemsMutex.lock();
				auto it = mItems.find(ptr);
				InspectorThreadInstance* thread = std::get<0>(it->second);
				mItemsMutex.unlock();
				lua_State* state = thread ? thread->state() : mGlobal->lua_state();
				lua_gc(state, LUA_GCCOLLECT, 0);
				mItemsMutex.lock();
				bool valid = std::get<1>(it->second);
				mItemsMutex.unlock();
				return valid;
			}
		}

		void Inspector::markItem(Engine::Scripting::ScopeBase* ptr, InspectorThreadInstance* thread)
		{
			std::tuple<InspectorThreadInstance*, bool>& p = mItems[ptr] = std::make_tuple(thread, true);

			ptr->push();

			lua_State* state = thread ? thread->state() : mGlobal->lua_state();

			bool** bPtr = static_cast<bool**>(lua_newuserdata(state, sizeof(bool*)));
			*bPtr = &std::get<1>(p);

			luaL_getmetatable(state, "Maditor.InspectorMarkMetatable");

			lua_setmetatable(state, -2);

			lua_setfield(state, -2, "__InspectorMark__");

			lua_pop(state, 1);
		}

		Engine::Scripting::ScopeBase* Inspector::validate(Engine::InvScopePtr ptr)
		{
			if (ptr)
				return ptr.validate();
			else
				return mGlobal;
		}

	}
}
