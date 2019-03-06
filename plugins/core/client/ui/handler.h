#pragma once

#include "Madgine/core/madgineobject.h"
#include "Interfaces/scripting/types/scopebase.h"
#include "Interfaces/signalslot/slot.h"

namespace Engine
{
	namespace UI
	{
		struct MADGINE_CLIENT_EXPORT WindowDescriber
		{
			WindowDescriber(const std::string& widgetName,
			                std::function<bool(GUI::Widget*)> init) :
				mWidgetName(widgetName),
				mInit(init)
			{
			}

			std::string mWidgetName;
			std::function<bool(GUI::Widget*)> mInit;
			;
		};


		class MADGINE_CLIENT_EXPORT Handler : public Core::MadgineObject, public Scripting::ScopeBase
		{
		public:
			Handler(UIManager &ui, const std::string& windowName);
			virtual ~Handler() = default;

			virtual void onMouseVisibilityChanged(bool b);

			GUI::Widget* widget() const;

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

			App::GlobalAPIBase &getGlobalAPIComponent(size_t i, bool = true);

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

			void registerWidget(const std::string& name, std::function<bool(GUI::Widget*)> init);

		private:
			bool installToWidget(GUI::Widget* w);

		protected:

			bool init() override;
			void finalize() override;

			bool init(GUI::Widget* w);


			virtual void onMouseMove(const Input::MouseEventArgs& me);

			virtual void onMouseDown(const Input::MouseEventArgs& me);

			virtual void onMouseUp(const Input::MouseEventArgs& me);

			virtual bool onKeyPress(const Input::KeyEventArgs& evt);

			

		public:
			
			void injectMouseMove(const Input::MouseEventArgs& evt);
			void injectMouseDown(const Input::MouseEventArgs& evt);
			void injectMouseUp(const Input::MouseEventArgs& evt);
			bool injectKeyPress(const Input::KeyEventArgs& evt);

		protected:
			GUI::Widget* mWidget;

			UIManager &mUI;

			const std::string mWidgetName;

		private:
			std::list<WindowDescriber> mWidgets;

			SignalSlot::Slot<&Handler::injectMouseMove> mMouseMoveSlot;
			SignalSlot::Slot<&Handler::injectMouseDown> mMouseDownSlot;
			SignalSlot::Slot<&Handler::injectMouseUp> mMouseUpSlot;


		};
	}
}
