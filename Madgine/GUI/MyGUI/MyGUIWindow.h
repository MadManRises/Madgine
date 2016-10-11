#pragma once

#include "GUI\Windows\WindowContainer.h"
#include "GUI\GUIEvents.h"

#pragma warning (push, 0)
#include <MYGUI\MyGUI.h>
#pragma warning (pop)

namespace Engine {
	namespace GUI {
		namespace MyGui {
			class MyGUILauncher;

			class MyGUIWindow :
				public WindowContainer
			{
			public:
				MyGUIWindow(MyGUI::Widget *widget, MyGUILauncher *gui, WindowContainer *parent);
				virtual ~MyGUIWindow();

				// Inherited via Window
				virtual void showModal() override;

				// Inherited via Window
				virtual void registerHandlerEvents(void *id, std::function<void(GUI::MouseEventArgs&)> mouseMove, std::function<void(GUI::MouseEventArgs&)> mouseDown, std::function<void(GUI::MouseEventArgs&)> mouseUp, std::function<void(GUI::MouseEventArgs&)> mouseScroll, std::function<bool(GUI::KeyEventArgs&)> keyPress) override;
				virtual void unregisterAllEvents(void *id) override;

				virtual void registerEvent(void *id, EventType type, std::function<void()> event) override;


				// Inherited via Window
				virtual bool isVisible() override;
				virtual void releaseInput() override;
				virtual void captureInput() override;
				virtual void hideModal() override;
				virtual void show() override;
				virtual void hide() override;
				virtual void activate() override;
				virtual void moveToFront() override;
				virtual void setEnabled(bool b) override;

				virtual void setPixelSize(const Ogre::Vector2 & size) override;
				virtual void setPixelPosition(const Ogre::Vector2 & pos) override;
				virtual Ogre::Vector2 getPixelSize() override;



				// Inherited via WindowContainer
				virtual WindowContainer * loadLayoutWindow(const std::string & name) override;

				MyGUI::Widget *window();
				

			protected:
				// Inherited via Window
				virtual Window *createImpl(Class _class) override;

				void handleKeyEvent(MyGUI::Widget *w, MyGUI::KeyCode c);

				// Inherited via WindowContainer
				virtual WindowContainer * createChildWindow(const std::string & name, Class _class, const std::string &customSkin = "") override;

				virtual std::list<WindowContainer*> buildChildren() override;
			
			private:

				void registerEvent(void *id, std::function<void(const std::string&)> f, MyGUI::EventHandle_WidgetString &event);
				void registerEvent(void *id, std::function<void()> f, MyGUI::EventHandle_WidgetVoid &event);
				void registerEvent(void *id, std::function<void(MouseEventArgs&)> f, MyGUI::EventHandle_WidgetIntIntButton &event);
				void registerEvent(void *id, std::function<void(GUI::MouseEventArgs&)> f, MyGUI::EventHandle_WidgetIntInt &event);
				void registerEvent(void *id, std::function<void(GUI::MouseEventArgs&)> f, MyGUI::EventHandle_WidgetInt &event);
				void registerEvent(void *id, std::function<void()> f, MyGUI::EventHandle_WidgetWidget &event);


			private:
				MyGUI::Widget *mWindow;
				MyGUILauncher *mGui;

				struct EventHandlerClass {
					virtual ~EventHandlerClass() = default;
				};

				template <int, class...>
				struct EventTypeWrapper {};

				template <class... _Ty>
				struct EventTypeWrapper<1, _Ty...> {
					using type = MyGUI::delegates::CMultiDelegate1<_Ty...>;
					using delegate = MyGUI::delegates::IDelegate1<_Ty...>;
				};

				template <class... _Ty>
				struct EventTypeWrapper<2, _Ty...> {
					using type = MyGUI::delegates::CMultiDelegate2<_Ty...>;
					using delegate = MyGUI::delegates::IDelegate2<_Ty...>;
				};

				template <class... _Ty>
				struct EventTypeWrapper<3, _Ty...> {
					using type = MyGUI::delegates::CMultiDelegate3<_Ty...>;
					using delegate = MyGUI::delegates::IDelegate3<_Ty...>;
				};

				template <class... _Ty>
				struct EventTypeWrapper<4, _Ty...> {
					using type = MyGUI::delegates::CMultiDelegate4<_Ty...>;
					using delegate = MyGUI::delegates::IDelegate4<_Ty...>;
				};


				template <class... _Ty>
				struct EventHandler : public EventHandlerClass {
					using F = std::function<void(_Ty...)>;
					using Delegate = typename EventTypeWrapper<sizeof...(_Ty), _Ty...>::delegate;
					using Event = typename EventTypeWrapper<sizeof...(_Ty), _Ty...>::type;

					EventHandler(F f, Event &event) :
						mEvent(event),
						mFunc(f) {
						mDelegate = MyGUI::newDelegate(this, &EventHandler::fire);
						event += mDelegate;
					}
					void fire(_Ty... args) {
						mFunc(args...);
					}
					virtual ~EventHandler() {
						mEvent -= mDelegate;
					}
					F mFunc;
					Delegate *mDelegate;
					Event &mEvent;
				};

				template <class... _Ty>
				EventHandler<_Ty...> *wrapEvent(std::function<void(_Ty...)> f, typename EventTypeWrapper<sizeof...(_Ty), _Ty...>::type &event) {
					return new EventHandler<_Ty...>(f, event);
				}

				std::map<void*, std::list<Ogre::unique_ptr<EventHandlerClass>>> mEventHandlers;
				std::map<void*, std::list<std::function<bool(GUI::KeyEventArgs&)>>> mKeyHandlers;


};

		}
	}
}

