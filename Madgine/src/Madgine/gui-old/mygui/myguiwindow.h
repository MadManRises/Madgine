#pragma once

#include "../windows/windowcontainer.h"
#include "../guievents.h"

#ifdef _MSC_VER
#pragma warning (push, 0)
#endif
#include <MYGUI/MyGUI.h>
#ifdef _MSC_VER
#pragma warning (pop)
#endif

namespace Engine
{
	namespace GUI
	{
		namespace MyGui
		{
			class MyGUILauncher;

			class MADGINE_CLIENT_EXPORT MyGUIWindow :
				public WindowContainer
			{
			public:
				MyGUIWindow(MyGUI::Widget* widget, MyGUILauncher* gui, WindowContainer* parent);
				virtual ~MyGUIWindow();

				// Inherited via Window
				void showModal() override;

				// Inherited via Window
				void registerHandlerEvents(void* id, std::function<void(MouseEventArgs&)> mouseMove,
				                           std::function<void(MouseEventArgs&)> mouseDown,
				                           std::function<void(MouseEventArgs&)> mouseUp,
				                           std::function<void(MouseEventArgs&)> mouseScroll,
				                           std::function<bool(KeyEventArgs&)> keyPress) override;
				void unregisterCustomEvents(void* id) override;

				void registerEvent(void* id, EventType type, std::function<void()> event) override;


				// Inherited via Window
				bool isVisible() override;
				void releaseInput() override;
				void captureInput() override;
				void hideModal() override;
				void show() override;
				void hide() override;
				void setVisible(bool b) override;
				void activate() override;
				void moveToFront() override;
				void setEnabled(bool b) override;

				void setPixelSize(const Ogre::Vector2& size) override;
				Ogre::Vector2 getPixelSize() override;
				void setPixelPosition(const Ogre::Vector2& pos) override;
				Ogre::Vector2 getPixelPosition() override;


				// Inherited via WindowContainer
				WindowContainer* loadLayoutWindow(const std::string& name) override;

				MyGUI::Widget* window() const;


			protected:
				// Inherited via Window
				Window* createImpl(Class _class) override;

				void handleKeyEvent(MyGUI::Widget* w, MyGUI::KeyCode c);

				// Inherited via WindowContainer
				WindowContainer* createChildWindow(const std::string& name, Class _class, const std::string& customSkin = "")
				override;

				void buildChildren() override;

				void needMouse() const;

			private:

				void registerEvent(void* id, std::function<void(const std::string&)> f, MyGUI::EventHandle_WidgetString& event);
				void registerEvent(void* id, std::function<void()> f, MyGUI::EventHandle_WidgetVoid& event);
				void registerEvent(void* id, std::function<void(MouseEventArgs&)> f, MyGUI::EventHandle_WidgetIntIntButton& event);
				void registerEvent(void* id, std::function<void(MouseEventArgs&)> f, MyGUI::EventHandle_WidgetIntInt& event);
				void registerEvent(void* id, std::function<void(MouseEventArgs&)> f, MyGUI::EventHandle_WidgetInt& event);
				void registerEvent(void* id, std::function<void()> f, MyGUI::EventHandle_WidgetWidget& event);


			private:
				MyGUI::Widget* mWindow;
				MyGUILauncher* mGui;

				struct EventHandlerClass
				{
					virtual ~EventHandlerClass() = default;
				};

				template <int, class...>
				struct EventTypeWrapper
				{
				};

				template <class... _Ty>
				struct EventTypeWrapper<1, _Ty...>
				{
					using type = MyGUI::delegates::CMultiDelegate1<_Ty...>;
					using delegate = MyGUI::delegates::IDelegate1<_Ty...>;
				};

				template <class... _Ty>
				struct EventTypeWrapper<2, _Ty...>
				{
					using type = MyGUI::delegates::CMultiDelegate2<_Ty...>;
					using delegate = MyGUI::delegates::IDelegate2<_Ty...>;
				};

				template <class... _Ty>
				struct EventTypeWrapper<3, _Ty...>
				{
					using type = MyGUI::delegates::CMultiDelegate3<_Ty...>;
					using delegate = MyGUI::delegates::IDelegate3<_Ty...>;
				};

				template <class... _Ty>
				struct EventTypeWrapper<4, _Ty...>
				{
					using type = MyGUI::delegates::CMultiDelegate4<_Ty...>;
					using delegate = MyGUI::delegates::IDelegate4<_Ty...>;
				};


				template <class... _Ty>
				struct EventHandler : public EventHandlerClass
				{
					using F = std::function<void(_Ty ...)>;
					using Delegate = typename EventTypeWrapper<sizeof...(_Ty), _Ty...>::delegate;
					using Event = typename EventTypeWrapper<sizeof...(_Ty), _Ty...>::type;

					EventHandler(const F& f, Event& event) :
						mEvent(event),
						mFunc(f)
					{
						mDelegate = MyGUI::newDelegate(this, &EventHandler::fire);
						event += mDelegate;
					}

					void fire(_Ty ... args)
					{
						mFunc(args...);
					}

					virtual ~EventHandler()
					{
						mEvent -= mDelegate;
					}

					Event& mEvent;
					F mFunc;
					Delegate* mDelegate;
				};

				template <class... _Ty>
				static EventHandler<_Ty...>* wrapEvent(const std::function<void(_Ty ...)>& f,
				                                       typename EventTypeWrapper<sizeof...(_Ty), _Ty...>::type& event)
				{
					return new EventHandler<_Ty...>(f, event);
				}

				std::map<void*, std::list<std::unique_ptr<EventHandlerClass>>> mEventHandlers;
				std::map<void*, std::list<std::function<bool(KeyEventArgs&)>>> mKeyHandlers;
			};
		}
	}
}
