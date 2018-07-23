#pragma once

#include "../gui/guievents.h"
#include "../gui/windows/classid.h"
#include "../madgineobject.h"
#include "../scripting/types/scopebase.h"
#include "../scripting/types/globalapicomponentbase.h"
#include "../signalslot/slot.h"

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

			virtual void onMouseVisibilityChanged(bool b);

			GUI::Window* window() const;

			virtual void sizeChanged();

			virtual const char* key() const = 0;
			
			App::Application &app(bool = true);
			UIManager &ui(bool = true);

			template <class T>
			T &getSceneComponent(bool init = true)
			{
				return static_cast<T&>(getSceneComponent(T::component_index(), init));
			}

			Scene::SceneComponentBase &getSceneComponent(size_t i, bool = true);

			Scene::SceneManager &sceneMgr(bool = true);

			template <class T>
			T &getGlobalAPIComponent(bool init = true)
			{
				return static_cast<T&>(getGlobalAPIComponent(T::component_index(), init));
			}

			Scripting::GlobalAPIComponentBase &getGlobalAPIComponent(size_t i, bool = true);

			template <class T>
			T &getGuiHandler(bool init = true)
			{
				return static_cast<T&>(getGuiHandler(T::component_index(), init));
			}

			GuiHandlerBase &getGuiHandler(size_t i, bool = true);

			template <class T>
			T &getGameHandler(bool init = true)
			{
				return static_cast<T&>(getGameHandler(T::component_index(), init));
			}

			GameHandlerBase &getGameHandler(size_t i, bool = true);

			void registerWindow(const std::string& name, std::function<bool(GUI::Window*)> init);

		private:
			bool installToWindow(GUI::Window* w);

		protected:

			bool init() override;
			void finalize() override;

			bool init(GUI::Window* w);


			virtual void onMouseMove(GUI::MouseEventArgs& me);

			virtual void onMouseDown(GUI::MouseEventArgs& me);

			virtual void onMouseUp(GUI::MouseEventArgs& me);

			virtual bool onKeyPress(const GUI::KeyEventArgs& evt);

			

		public:
			
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

			SignalSlot::Slot<&Handler::injectMouseMove> mMouseMoveSlot;
			SignalSlot::Slot<&Handler::injectMouseDown> mMouseDownSlot;
			SignalSlot::Slot<&Handler::injectMouseUp> mMouseUpSlot;


		};
	}
}
