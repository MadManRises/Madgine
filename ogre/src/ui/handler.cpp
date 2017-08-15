#include "madginelib.h"
#include "handler.h"
#include "ui/uimanager.h"
#include "gui/guisystem.h"
#include "gui/windows/window.h"


namespace Engine {
namespace UI {

Handler::Handler(const std::string &windowName) :
    mWindow(nullptr),
    mUI(&UI::UIManager::getSingleton()),
    mWindowName(windowName)
{

}


bool Handler::installToWindow(GUI::Window * w)
{
    mWindow = w;

	w->registerHandlerEvents(this, 
		std::bind(&Handler::injectMouseMove, this, std::placeholders::_1),
		std::bind(&Handler::injectMouseDown, this, std::placeholders::_1),
		std::bind(&Handler::injectMouseUp, this, std::placeholders::_1),
		std::bind(&Handler::injectMouseMove, this, std::placeholders::_1),
		std::bind(&Handler::injectKeyPress, this, std::placeholders::_1));
	w->registerEvent(this, GUI::EventType::WindowResized, std::bind(&Handler::sizeChanged, this));

	for (const WindowDescriber &des : mWindows) {
		GUI::Window * window = w->getChildRecursive(des.mWindowName, des.mClass);
		
		if (!window) {
			LOG_ERROR(Exceptions::windowNotFound(des.mWindowName));
			return false;
		}
		
		if (des.mVar) {			
			*des.mVar = window;
		}
		
		for (auto &event : des.mEvents) {
			//TODO
			window->registerEvent(this, event.first, event.second);
		}		
	}

	mWindows.clear();

	return true;
}


void Handler::sizeChanged()
{
}

bool Handler::init()
{
	if (!MadgineObject::init())
		return false;
	if (!installToWindow(mUI->gui()->getWindowByName(mWindowName))) {
		return false;
	}
	return true;
}

void Handler::finalize()
{
	if (mWindow) {
		mWindow->unregisterAllEvents(this);
	}
	MadgineObject::finalize();
}

bool Handler::init(GUI::Window *window)
{
	if (installToWindow(window)) {
		return MadgineObject::init();
	}
	else {
		return false;
	}
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
