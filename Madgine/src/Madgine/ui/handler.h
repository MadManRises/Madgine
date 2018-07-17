#pragma once

#include "../gui/guievents.h"
#include "../gui/windows/classid.h"
#include "../madgineobject.h"
#include "../scripting/types/scopebase.h"
#include "../scripting/types/globalapicomponentbase.h"

namespace Engine
{
	namespace UI
	{
		struct MADGINE_CLIENT_EXPORT WindowDescriber
		{
			WindowDescriber(const std::string& windowName,
			                std::function<bool(GUI::Window*)> init) :
				mWindowName(windowName),
				mInit(init)
			{
			}

			std::string mWindowName;
			std::function<bool(GUI::Window*)> mInit;
			;
		};


		class MADGINE_CLIENT_EXPORT Handler : public MadgineObject, public Scripting::ScopeBase
		{
		public:
			Handler(UIManager &ui, const std::string& windowName);
			virtual ~Handler() = default;

			bool init() override;
			void finalize() override;
			virtual bool preInit();

			virtual void onMouseVisibilityChanged(bool b);

			GUI::Window* window() const;

			virtual void sizeChanged();

			virtual const char* key() const = 0;
			
			App::Application &app();
			UIManager &ui();

			template <class T>
			T &getSceneComponent()
			{
				return static_cast<T&>(getSceneComponent(T::component_index()));
			}

			Scene::SceneComponentBase &getSceneComponent(size_t i);

			Scene::SceneManager &sceneMgr();

			template <class T>
			T &getGlobalAPIComponent()
			{
				return static_cast<T&>(getGlobalAPIComponent(T::component_index()));
			}

			Scripting::GlobalAPIComponentBase &getGlobalAPIComponent(size_t i);

			template <class T>
			T &getGuiHandler()
			{
				return static_cast<T&>(getGuiHandler(T::component_index()));
			}

			GuiHandlerBase &getGuiHandler(size_t i);

			template <class T>
			T &getGameHandler()
			{
				return static_cast<T&>(getGameHandler(T::component_index()));
			}

			GameHandlerBase &getGameHandler(size_t i);

			void registerWindow(const std::string& name, std::function<bool(GUI::Window*)> init);

		private:
			bool installToWindow(GUI::Window* w);

		protected:
			bool init(GUI::Window* w);


			virtual void onMouseMove(GUI::MouseEventArgs& me);

			virtual void onMouseDown(GUI::MouseEventArgs& me);

			virtual void onMouseUp(GUI::MouseEventArgs& me);

			virtual bool onKeyPress(const GUI::KeyEventArgs& evt);

			

		private:
			
			void injectMouseMove(GUI::MouseEventArgs& evt);
			void injectMouseDown(GUI::MouseEventArgs& evt);
			void injectMouseUp(GUI::MouseEventArgs& evt);
			bool injectKeyPress(const GUI::KeyEventArgs& evt);

		protected:
			GUI::Window* mWindow;

			UIManager &mUI;

			const std::string mWindowName;

		private:
			std::list<WindowDescriber> mWindows;

		};
	}
}
