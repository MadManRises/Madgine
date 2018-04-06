#pragma once

#include "windowsizerelvector.h"

#include "../guievents.h"

#include "windowclass.h"
#include "WindowContainer.h"

namespace Engine
{
	namespace GUI
	{
		class OGREMADGINE_EXPORT Window
		{
		public:
			Window(WindowContainer* w);
			virtual ~Window() = default;

			void setSize(const WindowSizeRelVector& size);
			const WindowSizeRelVector& getSize();
			void setPos(const WindowSizeRelVector& pos);
			const WindowSizeRelVector& getPos() const;
			Ogre::Vector2 getPixelSize() const;

			void destroy();

			void releaseInput();
			void captureInput();
			void activate();
			void moveToFront();


			bool isVisible() const;
			void showModal();
			void hideModal();
			void show();
			void hide();
			void setVisible(bool b);

			void setEnabled(bool b);

			void registerHandlerEvents(void* id,
			                           std::function<void(MouseEventArgs&)> mouseMove,
			                           std::function<void(MouseEventArgs&)> mouseDown,
			                           std::function<void(MouseEventArgs&)> mouseUp,
			                           std::function<void(MouseEventArgs&)> mouseScroll,
			                           std::function<bool(KeyEventArgs&)> keyPress);
			void unregisterAllEvents(void* id);

			void registerEvent(void* id,
			                   EventType type, std::function<void()> event);


			std::string getName() const;

			WindowContainer* container() const;

			template <class T>
			T* loadLayout(const std::string& name)
			{
				WindowContainer* c = mContainer->loadLayout(name);
				return c->as<T>();
			}

			template <class T>
			T* createChild(const std::string& name, const std::string& customSkin = "")
			{
				WindowContainer* c = mContainer->createChild(name, ClassId<T>::id, customSkin);
				return c->as<T>();
			}

			Window* createChild(const std::string& name, Class _class = Class::WINDOW_CLASS);

			template <class T, class _ = std::enable_if<std::is_base_of<Window, T>::value>>
			T* getChildRecursive(const std::string& name)
			{
				return mContainer->getChildRecursive(name)->as<T>();
			}

			Window* getChildRecursive(const std::string& name, Class _class) const;

			void invalidate();

		protected:

			bool mIsValid;
			WindowContainer* mContainer;
		};
	}
}
