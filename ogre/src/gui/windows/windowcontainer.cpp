#include "ogrelib.h"

#include "windowcontainer.h"

#include "gui/guisystem.h"

#include "window.h"

#include "../windownames.h"

namespace Engine
{
	namespace GUI
	{
		WindowContainer::WindowContainer(GUISystem* gui, WindowContainer* parent, const std::string& name) :
			mGui(gui),
			mParent(parent),
			mName(name),
			mClass(Class::WINDOW_CLASS)
		{
			gui->addWindow(name, this);
		}

		WindowContainer::~WindowContainer()
		{
			mGui->removeWindow(mName);
			assert(mChildren.empty());
		}

		void WindowContainer::unregisterAllEvents(void* id)
		{
			unregisterCustomEvents(id);
			mResizeListeners.erase(id);
			for (WindowContainer* child : mChildren)
			{
				child->unregisterAllEvents(id);
			}
		}

		void WindowContainer::registerEvent(void* id, EventType type, std::function<void()> event)
		{
			switch (type)
			{
			case EventType::WindowResized:
				mResizeListeners[id].push_back(event);
				break;
			default:
				throw 0;
			}
		}

		void WindowContainer::setSize(const WindowSizeRelVector& size)
		{
			mSize = size;
			mGui->setDirtyWindowSizes();
		}

		const WindowSizeRelVector& WindowContainer::getSize() const
		{
			return mSize;
		}

		void WindowContainer::setPos(const WindowSizeRelVector& pos)
		{
			mPos = pos;
			mGui->setDirtyWindowSizes();
		}

		const WindowSizeRelVector& WindowContainer::getPos() const
		{
			return mPos;
		}

		void WindowContainer::updateSize(const Ogre::Vector2& size)
		{
			Ogre::Vector2 newSize = mSize * size;
			Ogre::Vector2 newPos = mPos * size;
			setPixelSize(newSize);
			setPixelPosition(newPos);

			Ogre::Vector2 pixelSize = getPixelSize();
			for (WindowContainer* w : mChildren)
				w->updateSize(pixelSize);

			for (const std::pair<void * const, std::list<std::function<void()>>>& callbacks : mResizeListeners)
			{
				for (const std::function<void()>& callback : callbacks.second)
				{
					callback();
				}
			}
		}

		Window* WindowContainer::as(Class _class)
		{
			if (_class == Class::WINDOW_CLASS)
			{
				if (!mDefaultImpl)
					mDefaultImpl = std::unique_ptr<Window>(createImpl(_class));
				return mDefaultImpl.get();
			}

			if (mClass != Class::WINDOW_CLASS)
			{
				assert(mClass == _class);
			}
			else
			{
				mImpl = std::unique_ptr<Window>(createImpl(_class));
				mClass = _class;
			}
			return mImpl.get();
		}

		bool WindowContainer::hasClass(Class _class) const
		{
			if (_class == Class::WINDOW_CLASS)
				return true;
			return mClass == _class;
		}

		WindowContainer* WindowContainer::getChildRecursive(const std::string& name)
		{
			if (name == getName() || name == WindowNames::thisWindow) return this;
			for (WindowContainer* w : mChildren)
				if (WindowContainer* f = w->getChildRecursive(name))
					return f;
			return nullptr;
		}

		WindowContainer* WindowContainer::createChild(const std::string& name, Class _class, const std::string& customSkin)
		{
			WindowContainer* child = createChildWindow(name, _class, customSkin);
			child->updateSize(getPixelSize());
			mChildren.push_back(child);
			return child;
		}

		WindowContainer* WindowContainer::loadLayout(const std::string& name)
		{
			WindowContainer* child = loadLayoutWindow(name);
			if (child)
			{
				child->updateSize(getPixelSize());
				mChildren.push_back(child);
			}
			return child;
		}

		const std::list<WindowContainer*>& WindowContainer::getChildren() const
		{
			return mChildren;
		}

		void WindowContainer::destroy()
		{
			assert(mParent);
			mParent->destroyChild(this);
		}

		void WindowContainer::destroyChild(WindowContainer* w)
		{
			assert(std::find(mChildren.begin(), mChildren.end(), w) != mChildren.end());
			mChildren.remove(w);
			w->finalize();
			delete w;
		}

		std::string WindowContainer::getName() const
		{
			return mName;
		}

		void WindowContainer::buildHierarchy()
		{
			assert(mChildren.empty());
			buildChildren();
			for (WindowContainer* w : mChildren)
			{
				w->buildHierarchy();
			}
		}

		void WindowContainer::finalize()
		{
			if (mDefaultImpl)
				mDefaultImpl->invalidate();
			if (mImpl)
				mImpl->invalidate();
			while (!mChildren.empty())
				destroyChild(mChildren.front());
		}

		void WindowContainer::addChild(WindowContainer* child)
		{
			mChildren.push_back(child);
		}
	}
}
