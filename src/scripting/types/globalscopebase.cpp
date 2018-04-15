#include "../../baselib.h"
#include "globalscopebase.h"

#include "../types/luastate.h"


namespace Engine
{

	namespace Scripting
	{


		GlobalScopeBase::GlobalScopeBase(LuaState *state) :
			ScopeBase(state->createThread(this)),
			mState(state)
		{
			assert(table());

			mScopes = table().createTable();

			table().setMetatable("Interfaces.GlobalScope");
		}


		void GlobalScopeBase::executeString(const std::string& cmd)
		{
			mState->executeString(table().state(), cmd);
		}

		LuaTable GlobalScopeBase::createTable()
		{
			return mScopes.createTable();
		}

		GlobalAPIComponentBase & GlobalScopeBase::getGlobalAPIComponent(size_t)
		{
			throw 0;
		}

		Scene::SceneComponentBase& GlobalScopeBase::getSceneComponent(size_t i)
		{
			throw 0;
		}

		Scene::SceneManagerBase& GlobalScopeBase::sceneMgr()
		{
			throw 0;
		}

		GUI::GUISystem & GlobalScopeBase::gui()
		{
			throw 0;
		}

		UI::UIManager& GlobalScopeBase::ui()
		{
			throw 0;
		}

		UI::GameHandlerBase& GlobalScopeBase::getGameHandler(size_t i)
		{
			throw 0;
		}

		UI::GuiHandlerBase& GlobalScopeBase::getGuiHandler(size_t i)
		{
			throw 0;
		}

		lua_State* GlobalScopeBase::lua_state() const
		{
			return table().state();
		}

		
	}
}
