#pragma once

#include "GUI\GUIEvents.h"
#include "GUI\Windows\classid.h"
#include "MadgineObject.h"

namespace Engine {
	namespace UI {

		struct OGREMADGINE_EXPORT WindowDescriber {
			template <class WindowClass>
			WindowDescriber(const std::string &windowName, WindowClass **var = 0,
				const std::list<std::pair<GUI::EventType, std::function<void()>>> &events = {}) :
				mWindowName(windowName),
				mVar((GUI::Window**)(var)),
				mClass(GUI::ClassId<WindowClass>::id),
				mEvents(events)
				
			{}

			WindowDescriber(const std::string &windowName,
				const std::list<std::pair<GUI::EventType, std::function<void()>>> &events = {}) :
				WindowDescriber(windowName, (GUI::Window**)0, events) {}

			std::string mWindowName;
			GUI::Window **mVar;
			GUI::Class mClass;
			std::list<std::pair<GUI::EventType, std::function<void()>>> mEvents;
		};


		class OGREMADGINE_EXPORT Handler : public BaseMadgineObject {
		public:
			Handler(const std::string &windowName);
			virtual ~Handler() = default;

			virtual bool init();
			virtual void finalize();

			virtual void onMouseVisibilityChanged(bool b);

			GUI::Window * window();


			virtual void sizeChanged();

		private:
			bool installToWindow(GUI::Window * w);

		protected:
			bool init(GUI::Window *w);

			void registerWindow(const WindowDescriber &des);

			virtual void onMouseMove(GUI::MouseEventArgs &me);

			virtual void onMouseDown(GUI::MouseEventArgs &me);

			virtual void onMouseUp(GUI::MouseEventArgs &me);

			virtual bool onKeyPress(const GUI::KeyEventArgs &evt);


			template <class T, class... _Ty>
			void registerButton(const std::string &name,
				void (T::*f)(_Ty...), _Ty... args)
			{
				registerWindow({ name,{
					{ GUI::EventType::ButtonClick, event(f, args...) }
				}
				});
			}

			template <class T, class... _Ty>
			void registerButton(const std::string &name, Engine::GUI::Button **var,
				void (T::*f)(_Ty...), std::remove_const_t<_Ty>... args)
			{
				registerWindow({ name, var,{
					{ GUI::EventType::ButtonClick, event(f, args...) }
				}
				});
			}

			template <class T, class... _Ty>
			void registerKeyBinding(GUI::Key key,
				void (T::*f)(_Ty...))
			{
				registerKeyBinding(key, event(f));
			}


			template <class T, class... _Ty>
			std::function<void()> event(void (T::*f)(_Ty...), std::remove_const_t<_Ty>... args) {
				return std::bind(f, static_cast<T*>(this), args...);
			}

		private:


			virtual void injectMouseMove(GUI::MouseEventArgs &evt);
			virtual void injectMouseDown(GUI::MouseEventArgs &evt);
			virtual void injectMouseUp(GUI::MouseEventArgs &evt);
			virtual bool injectKeyPress(const GUI::KeyEventArgs &evt);

			void registerKeyBinding(GUI::Key key,
				std::function<void()> f);

		protected:
			GUI::Window * mWindow;

			UI::UIManager *mUI;

			const std::string mWindowName;

		private:
			std::list<WindowDescriber> mWindows;

			std::map<GUI::Key, std::function<void()>>
				mKeyBindings;


		};

	}
}

