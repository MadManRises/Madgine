#pragma once

#include "../guievents.h"
#include "windowclass.h"
#include "windowsizerelvector.h"
#include "classid.h"

namespace Engine
{
	namespace GUI
	{
		class MADGINE_CLIENT_EXPORT WindowContainer
		{
		public:

			WindowContainer(GUISystem* gui, WindowContainer* parent, const std::string& name);
			virtual ~WindowContainer();


			virtual void releaseInput() = 0;
			virtual void captureInput() = 0;
			virtual void activate() = 0;
			virtual void moveToFront() = 0;


			virtual bool isVisible() = 0;
			virtual void showModal() = 0;
			virtual void hideModal() = 0;
			virtual void show() = 0;
			virtual void hide() = 0;
			virtual void setVisible(bool b) = 0;

			virtual void setEnabled(bool b) = 0;

			virtual void registerHandlerEvents(void* id,
			                                   std::function<void(MouseEventArgs&)> mouseMove,
			                                   std::function<void(MouseEventArgs&)> mouseDown,
			                                   std::function<void(MouseEventArgs&)> mouseUp,
			                                   std::function<void(MouseEventArgs&)> mouseScroll,
			                                   std::function<bool(KeyEventArgs&)> keyPress) = 0;
			void unregisterAllEvents(void* id);


			virtual void registerEvent(void* id,
			                           EventType type, std::function<void()> event);


			virtual void setPixelSize(const Ogre::Vector2& size) = 0;
			virtual Ogre::Vector2 getPixelSize() = 0;
			virtual void setPixelPosition(const Ogre::Vector2& size) = 0;
			virtual Ogre::Vector2 getPixelPosition() = 0;

			void setSize(const WindowSizeRelVector& size);
			const WindowSizeRelVector& getSize() const;
			void setPos(const WindowSizeRelVector& pos);
			const WindowSizeRelVector& getPos() const;

			void updateSize(const Ogre::Vector2& size);

			template <class T, class _ = std::enable_if<std::is_base_of<Window, T>::value>>
			T* as()
			{
				return static_cast<T*>(as(ClassId<T>::id));
			}

			Window* as(Class _class = Class::WINDOW_CLASS);

			bool hasClass(Class _class) const;

			WindowContainer* getChildRecursive(const std::string& name);
			WindowContainer* createChild(const std::string& name, Class _class, const std::string& customSkin = "");
			WindowContainer* loadLayout(const std::string& name);

			const std::list<WindowContainer *>& getChildren() const;

			void destroy();
			void destroyChild(WindowContainer* w);

			std::string getName() const;

			void buildHierarchy();

			void finalize();

		protected:

			virtual void unregisterCustomEvents(void* id) = 0;

			virtual Window* createImpl(Class _class) = 0;

			virtual WindowContainer* createChildWindow(const std::string& name, Class _class, const std::string& customSkin = "")
			= 0;
			virtual WindowContainer* loadLayoutWindow(const std::string& name) = 0;

			virtual void buildChildren() = 0;
			void addChild(WindowContainer* child);

		private:


			WindowSizeRelVector mSize;
			WindowSizeRelVector mPos;

			GUISystem* mGui;
			WindowContainer* mParent;

			std::list<WindowContainer *> mChildren;

			std::string mName;

			Class mClass;

			std::unique_ptr<Window> mImpl;
			std::unique_ptr<Window> mDefaultImpl;

			std::map<void*, std::list<std::function<void()>>> mResizeListeners;
		};
	}
}
