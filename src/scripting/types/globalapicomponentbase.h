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

			bool init() override;
			void finalize() override;

			virtual void clear();
			virtual void update();

			virtual const char* key() const = 0;

			template <class T>
			T &getGlobalAPIComponent()
			{
				return static_cast<T&>(getGlobalAPIComponent(T::component_index()));
			}

			GlobalAPIComponentBase &getGlobalAPIComponent(size_t i);

			template <class T>
			T &getSceneComponent()
			{
				return static_cast<T&>(getSceneComponent(T::component_index()));
			}

			Scene::SceneComponentBase &getSceneComponent(size_t i);

			Scene::SceneManagerBase &sceneMgr();

			GlobalScopeBase &globalScope();

			template <class T>
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

			UI::GameHandlerBase &getGameHandler(size_t i);

		private:
			App::Application & mApp;
		};
	}
}
