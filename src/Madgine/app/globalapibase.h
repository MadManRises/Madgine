#pragma once

#include "../core/madgineobject.h"
#include "Interfaces/scripting/types/scopebase.h"
#include "Interfaces/reflection/classname.h"


#include "Interfaces/uniquecomponent/uniquecomponent.h"

namespace Engine
{
	namespace App
	{
		class MADGINE_BASE_EXPORT GlobalAPIBase : public Core::MadgineObject, public Scripting::ScopeBase
		{
		public:
			GlobalAPIBase(App::Application &app);


			virtual void update();

			virtual const char* key() const = 0;

			template <class T>
			T &getGlobalAPIComponent(bool init = true)
			{
				return static_cast<T&>(getGlobalAPIComponent(component_index<T>(), init));
			}

			GlobalAPIBase &getGlobalAPIComponent(size_t i, bool = true);

			template <class T>
			T &getSceneComponent(bool init = true)
			{
				return static_cast<T&>(getSceneComponent(component_index<T>(), init));
			}

			Scene::SceneComponentBase &getSceneComponent(size_t i, bool = true);

			Scene::SceneManager &sceneMgr(bool = true);

			Scripting::GlobalScopeBase &globalScope();

			GlobalAPIBase &getSelf(bool = true);

			App::Application &app(bool = true);

			/*template <class T>
			T &getGuiHandler()
			{
				return static_cast<T&>(getGuiHandler(T::component_index()));
			}

			UI::GuiHandlerBase &getGuiHandler(size_t i);

			template <class T>
			T &getGameHandler()
			{
				return static_cast<T&>(getGameHandler(T::component_index()));
			}

			UI::GameHandlerBase &getGameHandler(size_t i);*/

		protected:
			bool init() override;
			void finalize() override;

		private:
			App::Application & mApp;
		};
	}
}

RegisterType(Engine::App::GlobalAPIBase);