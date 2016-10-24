#include "libinclude.h"
#include "guihandler.h"
#include "windownames.h"
#include "Database/translationunit.h"
#include "Database/exceptionmessages.h"
#include "GUI\GUISystem.h"
#include "UI\UIManager.h"
#include "GUI\Windows\Window.h"


namespace Engine {
namespace UI {

GuiHandlerBase::GuiHandlerBase(const std::string &windowName, WindowType type, const std::string &layoutFile, const std::string &parentName) :
    Handler(windowName),
    mLayoutFile(layoutFile),
	mParentName(parentName),
	mType(type),
	mInitialized(false),
	mOrder(layoutFile.empty() ? 2 : (parentName != WindowNames::rootWindow ? 1 : 0)),
	mContext(App::ContextMask::NoContext)	
{
	setInitialisationOrder(2);
}


void GuiHandlerBase::sizeChanged()
{
}

void GuiHandlerBase::init(int order)
{
    if (mOrder == order)
        init();
}

void GuiHandlerBase::init()
{
	if (mInitialized)
		return;

	GUI::Window *window = nullptr;

    if (!mLayoutFile.empty()){
		window = mUI->gui()->loadLayout(mLayoutFile, mParentName);
		if (!window){
			LOG_ERROR(Database::Exceptions::guiHandlerInitializationFailed(mWindowName));
			return;
		}
    }
	if (window)
		Handler::init(window);
	else
		Handler::init();
	mInitialized = true;
}

void GuiHandlerBase::open()
{
    switch(mType){
    case WindowType::MODAL_OVERLAY:
        mUI->openModalWindow(this);
        break;
    case WindowType::NONMODAL_OVERLAY:
		mUI->openWindow(this);
        break;
    case WindowType::ROOT_WINDOW:
		mUI->swapCurrentRoot(this);
        break;
    }
}

void GuiHandlerBase::close()
{
	switch (mType) {
	case WindowType::MODAL_OVERLAY:
		mUI->closeModalWindow(this);
		break;
	case WindowType::NONMODAL_OVERLAY:
		mUI->closeWindow(this);
		break;
	case WindowType::ROOT_WINDOW:
		throw 0;
		break;
	}
}

bool GuiHandlerBase::isOpen()
{
	return mWindow->isVisible();
}


bool GuiHandlerBase::isRootWindow()
{
    return mType == WindowType::ROOT_WINDOW;
}

App::ContextMask GuiHandlerBase::context()
{
	return mContext;
}

void GuiHandlerBase::setInitialisationOrder(int order)
{
	mOrder = order;
}

void GuiHandlerBase::setContext(App::ContextMask context)
{
	mContext = context;
}



} // namespace GuiHandler
} // namespace Cegui

