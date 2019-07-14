#pragma once

#include "../core/madgineobject.h"
#include "Modules/scripting/types/scopeentity.h"

#include "Modules/uniquecomponent/uniquecomponent.h"

namespace Engine
{
	namespace App
	{
		class MADGINE_BASE_EXPORT GlobalAPIBase : public Core::MadgineObject, public ScopeBase
		{
		public:
			GlobalAPIBase(App::Application &app);


			virtual void update();

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
		
			virtual App::Application &app(bool = true) override;
			virtual const Core::MadgineObject *parent() const override;

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