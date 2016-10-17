#include "libinclude.h"
#include "handler.h"
#include "UI/UIManager.h"
#include "Gui\GUISystem.h"
#include "GUI\Windows\Window.h"
#include "Database\exceptionmessages.h"
#include "windownames.h"

namespace Engine {
namespace UI {

Handler::Handler(const std::string &windowName) :
    mWindow(0),
    mUI(&UI::UIManager::getSingleton()),
    mWindowName(windowName)
{

}

Handler::~Handler()
{
	if (mWindow)
		mWindow->unregisterAllEvents(this);
}


void Handler::installToWindow(GUI::Window * w)
{
    mWindow = w;

	w->registerHandlerEvents(this, 
		std::bind(&Handler::injectMouseMove, this, std::placeholders::_1),
		std::bind(&Handler::injectMouseDown, this, std::placeholders::_1),
		std::bind(&Handler::injectMouseUp, this, std::placeholders::_1),
		std::bind(&Handler::injectMouseMove, this, std::placeholders::_1),
		std::bind(&Handler::injectKeyPress, this, std::placeholders::_1));

	for (const WindowDescriber &des : mWindows) {
		GUI::Window * window = (des.mWindowName == WindowNames::thisWindow ? mWindow : w->getChildRecursive(des.mWindowName, des.mClass));
		if (des.mVar) {
			if (!window)
				throw Database::Exceptions::windowNotFound(des.mWindowName);
			*des.mVar = window;
		}

		if (window) {
			for (auto &event : des.mEvents) {
				//TODO
				window->registerEvent(this, event.first, event.second);
			}
		}
	}

	mWindows.clear();

}

void Handler::init()
{
	installToWindow(mUI->gui()->getWindowByName(mWindowName));
}

void Handler::init(GUI::Window *window)
{
    installToWindow(window);
}

void Handler::injectMouseMove(GUI::MouseEventArgs &evt)
{
    onMouseMove(evt);
}

void Handler::injectMouseDown(GUI::MouseEventArgs &evt)
{
    onMouseDown(evt);
}

void Handler::injectMouseUp(GUI::MouseEventArgs &evt)
{
    onMouseUp(evt);
}

bool Handler::injectKeyPress(const GUI::KeyEventArgs &evt)
{
    return onKeyPress(evt);
}

void Handler::onMouseMove(GUI::MouseEventArgs &me)
{

}

void Handler::onMouseDown(GUI::MouseEventArgs &me)
{

}

void Handler::onMouseUp(GUI::MouseEventArgs &me)
{

}

bool Handler::onKeyPress(const GUI::KeyEventArgs &evt)
{
	auto it = mKeyBindings.find(evt.scancode);
	if (it != mKeyBindings.end()) {
		it->second();
		return true;
	}

	return false;
}


void Handler::registerWindow(const WindowDescriber &des)
{
	mWindows.push_back(des);
}


void Handler::onMouseVisibilityChanged(bool b)
{

}

GUI::Window * Handler::window()
{
    return mWindow;
}


void Handler::registerKeyBinding(GUI::Key key,
	std::function<void()> f)
{
	assert(mKeyBindings.find(key) == mKeyBindings.end());
	mKeyBindings[key] = f;
}

}
}
