#include "../../ogrelib.h"

#include "windowcontainer.h"

#include "window.h"

namespace Engine
{
	namespace GUI
	{
		Window::Window(WindowContainer* w) :
			mIsValid(true),
			mContainer(w)
		{
		}

		void Window::setSize(const WindowSizeRelVector& size)
		{
			mContainer->setSize(size);
		}

		const WindowSizeRelVector& Window::getSize()
		{
			return mContainer->getSize();
		}

		void Window::setPos(const WindowSizeRelVector& pos)
		{
			mContainer->setPos(pos);
		}

		const WindowSizeRelVector& Window::getPos() const
		{
			return mContainer->getPos();
		}

		Ogre::Vector2 Window::getPixelSize() const
		{
			return mContainer->getPixelSize();
		}

		void Window::destroy()
		{
			mContainer->destroy();
		}

		void Window::releaseInput()
		{
			mContainer->releaseInput();
		}

		void Window::captureInput()
		{
			mContainer->captureInput();
		}

		void Window::activate()
		{
			mContainer->activate();
		}

		void Window::moveToFront()
		{
			mContainer->moveToFront();
		}

		bool Window::isVisible() const
		{
			return mContainer->isVisible();
		}

		void Window::showModal()
		{
			mContainer->showModal();
		}

		void Window::hideModal()
		{
			mContainer->hideModal();
		}

		void Window::show()
		{
			mContainer->show();
		}

		void Window::hide()
		{
			mContainer->hide();
		}

		void Window::setVisible(bool b)
		{
			mContainer->setVisible(b);
		}

		void Window::setEnabled(bool b)
		{
			mContainer->setEnabled(b);
		}

		void Window::registerHandlerEvents(void* id, std::function<void(MouseEventArgs&)> mouseMove,
		                                   std::function<void(MouseEventArgs&)> mouseDown,
		                                   std::function<void(MouseEventArgs&)> mouseUp,
		                                   std::function<void(MouseEventArgs&)> mouseScroll,
		                                   std::function<bool(KeyEventArgs&)> keyPress)
		{
			mContainer->registerHandlerEvents(id, mouseMove, mouseDown, mouseUp, mouseScroll, keyPress);
		}

		void Window::unregisterAllEvents(void* id)
		{
			mContainer->unregisterAllEvents(id);
		}

		void Window::registerEvent(void* id, EventType type, std::function<void()> event)
		{
			mContainer->registerEvent(id, type, event);
		}

		std::string Window::getName() const
		{
			return mContainer->getName();
		}

		WindowContainer* Window::container() const
		{
			return mContainer;
		}

		Window* Window::createChild(const std::string& name, Class _class)
		{
			return mContainer->createChild(name, _class)->as(_class);
		}

		Window* Window::getChildRecursive(const std::string& name, Class _class) const
		{
			WindowContainer* child = mContainer->getChildRecursive(name);
			if (!child)
				return nullptr;
			return child->as(_class);
		}

		void Window::invalidate()
		{
			mIsValid = false;
		}
	}
}
