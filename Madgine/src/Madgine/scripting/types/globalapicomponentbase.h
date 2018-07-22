#pragma once

#include "../../madgineobject.h"
#include "scopebase.h"

namespace Engine
{
	namespace Scripting
	{
		class MADGINE_BASE_EXPORT GlobalAPIComponentBase : public MadgineObject, public ScopeBase
		{
		public:
			GlobalAPIComponentBase(App::Application &app);


			virtual void clear();
			virtual void update();

			virtual const char* key() const = 0;

			template <class T>
			T &getGlobalAPIComponent(bool init = true)
			{
				return static_cast<T&>(getGlobalAPIComponent(T::component_index(), init));
			}

			GlobalAPIComponentBase &getGlobalAPIComponent(size_t i, bool = true);

			template <class T>
			T &getSceneComponent(bool init = true)
			{
				return static_cast<T&>(getSceneComponent(T::component_index(), init));
			}

			Scene::SceneComponentBase &getSceneComponent(size_t i, bool = true);

			Scene::SceneManager &sceneMgr(bool = true);

			GlobalScopeBase &globalScope();

			GlobalAPIComponentBase &getSelf(bool = true);

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
