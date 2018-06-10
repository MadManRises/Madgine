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
			                GUI::Window** var,
			                GUI::Class _class,
			                const std::list<std::pair<GUI::EventType, std::function<void()>>>& events) :
				mWindowName(windowName),
				mVar(var),
				mClass(_class),
				mEvents(events)
			{
			}

			std::string mWindowName;
			GUI::Window** mVar;
			GUI::Class mClass;
			std::list<std::pair<GUI::EventType, std::function<void()>>> mEvents;
		};


		class MADGINE_CLIENT_EXPORT Handler : public MadgineObject, public Scripting::ScopeBase
		{
		public:
			Handler(UIManager &ui, const std::string& windowName);
			virtual ~Handler() = default;

			bool init() override;
			void finalize() override;

			virtual void onMouseVisibilityChanged(bool b);

			GUI::Window* window() const;

			virtual void sizeChanged();

			virtual const char* key() const = 0;
			
			App::Application &app();

			template <class T>
			T &getSceneComponent()
			{
				return static_cast<T&>(getSceneComponent(T::component_index()));
			}

			Scene::SceneComponentBase &getSceneComponent(size_t i);

			Scene::SceneManagerBase &sceneMgr();

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

		private:
			bool installToWindow(GUI::Window* w);

		protected:
			bool init(GUI::Window* w);


			virtual void onMouseMove(GUI::MouseEventArgs& me);

			virtual void onMouseDown(GUI::MouseEventArgs& me);

			virtual void onMouseUp(GUI::MouseEventArgs& me);

			virtual bool onKeyPress(const GUI::KeyEventArgs& evt);


			template <class WindowClass>
			void registerWindow(const std::string& name, WindowClass** var = nullptr,
			                    const std::list<std::pair<GUI::EventType, std::function<void()>>>& events = {})
			{
				mWindows.emplace_back(
					name,
					reinterpret_cast<GUI::Window**>(var),
					GUI::ClassId<WindowClass>::id,
					events
				);
			}


			void registerWindow(const std::string& name,
			                    const std::list<std::pair<GUI::EventType, std::function<void()>>>& events = {})
			{
				registerWindow<GUI::Window>(name, nullptr, events);
			}

			template <class T, class... _Ty>
			void registerButton(const std::string& name,
			                    void (T::*f)(_Ty ...), _Ty ... args)
			{
				registerWindow(name, {
					               {GUI::EventType::ButtonClick, event(f, args...)}
				               }
				);
			}

			template <class T, class... _Ty>
			void registerButton(const std::string& name, GUI::Button** var,
			                    void (T::*f)(_Ty ...), std::remove_const_t<_Ty> ... args)
			{
				registerWindow(name, var, {
					               {GUI::EventType::ButtonClick, event(f, args...)}
				               }
				);
			}

			template <class T, class... _Ty>
			void registerKeyBinding(GUI::Key key,
			                        void (T::*f)(_Ty ...))
			{
				registerKeyBinding(key, event(f));
			}


			template <class T, class... _Ty>
			std::function<void()> event(void (T::*f)(_Ty ...), std::remove_const_t<_Ty> ... args)
			{
				return std::bind(f, static_cast<T*>(this), args...);
			}

		private:


			void injectMouseMove(GUI::MouseEventArgs& evt);
			void injectMouseDown(GUI::MouseEventArgs& evt);
			void injectMouseUp(GUI::MouseEventArgs& evt);
			bool injectKeyPress(const GUI::KeyEventArgs& evt);

			void registerKeyBinding(GUI::Key key,
			                        std::function<void()> f);

		protected:
			GUI::Window* mWindow;

			UIManager &mUI;

			const std::string mWindowName;

		private:
			std::list<WindowDescriber> mWindows;

			std::map<GUI::Key, std::function<void()>>
			mKeyBindings;
		};
	}
}