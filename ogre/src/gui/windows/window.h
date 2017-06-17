#pragma once

#include "windowid.h"
#include "windowsizerelvector.h"

#include "GUI\GUIEvents.h"

#include "windowclass.h"
#include "WindowContainer.h"

namespace Engine {
	namespace GUI {
		class OGREMADGINE_EXPORT Window {
		public:
			Window(WindowContainer *w);
			virtual ~Window() = default;

			void setSize(const WindowSizeRelVector &size);
			const WindowSizeRelVector &getSize();
			void setPos(const WindowSizeRelVector &pos);
			const WindowSizeRelVector &getPos();
			Ogre::Vector2 getPixelSize();

			void destroy();

			void releaseInput();
			void captureInput();
			void activate();
			void moveToFront();


			bool isVisible();
			void showModal();
			void hideModal();
			void show();
			void hide();
			void setVisible(bool b);

			void setEnabled(bool b);

			void registerHandlerEvents(void *id,
				std::function<void(GUI::MouseEventArgs&)> mouseMove,
				std::function<void(GUI::MouseEventArgs&)> mouseDown,
				std::function<void(GUI::MouseEventArgs&)> mouseUp,
				std::function<void(GUI::MouseEventArgs&)> mouseScroll,
				std::function<bool(GUI::KeyEventArgs&)> keyPress);
			void unregisterAllEvents(void *id);

			void registerEvent(void *id,
				EventType type, std::function<void()> event);


			std::string getName();

			WindowContainer *container();

			template <class T>
			T *loadLayout(const std::string &name) {
				WindowContainer *c = mContainer->loadLayout(name);
				return c->as<T>();
			}

			template <class T>
			T *createChild(const std::string &name, const std::string &customSkin = "") {
				WindowContainer *c = mContainer->createChild(name, ClassId<T>::id, customSkin);
				return c->as<T>();
			}
			Window *createChild(const std::string &name, Class _class = Class::WINDOW_CLASS);

			template <class T, class _ = std::enable_if<std::is_base_of<Window, T>::value>>
			T *getChildRecursive(const std::string &name) {
				return mContainer->getChildRecursive(name)->as<T>();
			}
			Window *getChildRecursive(const std::string &name, Class _class);
			
			void invalidate();

		protected:

			bool mIsValid;
			WindowContainer *mContainer;

		};
	}
}