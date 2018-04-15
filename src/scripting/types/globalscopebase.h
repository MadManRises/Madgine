#pragma once

#include "scopebase.h"

namespace Engine
{
	namespace Scripting
	{
		class MADGINE_BASE_EXPORT GlobalScopeBase : public ScopeBase
		{
		public:
			GlobalScopeBase(LuaState *state);

			void executeString(const std::string& cmd);

			lua_State* lua_state() const;

			LuaTable createTable();

			using ScopeBase::table;

			template <class T>
			T &getGlobalAPIComponent()
			{
				return static_cast<T&>(getGlobalAPIComponent(T::component_index()));
			}

			virtual GlobalAPIComponentBase &getGlobalAPIComponent(size_t);

			template <class T>
			T &getSceneComponent()
			{
				return static_cast<T&>(getSceneComponent(T::component_index()));
			}

			virtual Scene::SceneComponentBase &getSceneComponent(size_t);
			virtual Scene::SceneManagerBase &sceneMgr();
			virtual GUI::GUISystem &gui();
			virtual UI::UIManager &ui();

			template <class T>
			T &getGameHandler()
			{
				return static_cast<T&>(getGameHandler(T::component_index()));
			}

			virtual UI::GameHandlerBase &getGameHandler(size_t);

			template <class T>
			T &getGuiHandler()
			{
				return static_cast<T&>(getGuiHandler(T::component_index()));
			}

			virtual UI::GuiHandlerBase &getGuiHandler(size_t);
			

		private:
			LuaState * mState;
			LuaTable mScopes;
		};
	}
}
