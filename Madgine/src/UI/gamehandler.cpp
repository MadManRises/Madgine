#include "madginelib.h"
#include "gamehandler.h"
#include "ui\UIManager.h"
#include "gui\GUISystem.h"
#include "gui\Windows\Window.h"
#include "Scene\ogrescenemanager.h"

namespace Engine {
namespace UI {


const float GameHandlerBase::mDragStartSquaredThreshold = 0.001f;


GameHandlerBase::GameHandlerBase(const std::string &windowName, App::ContextMask context) :
    Handler(windowName),
    mCurrentMouseButton(GUI::MouseButton::NO_BUTTON),
    mDragging(false),
    mSingleClick(false),
    mContext(context),
	mSceneMgr(&Scene::OgreSceneManager::getSingleton())
{

}

void GameHandlerBase::abortDrag()
{
    mDragging = false;
	mWindow->releaseInput();    
    onMouseDragAbort();
}

void GameHandlerBase::update(float timeSinceLastFrame, App::ContextMask mask)
{
    if (mContext & mask)
        update(timeSinceLastFrame);
}

Scene::OgreSceneManager *GameHandlerBase::sceneMgr() {
	return mSceneMgr;
}

void GameHandlerBase::onMouseMove(GUI::MouseEventArgs &me)
{
    clampToWindow(me);
    if (mCurrentMouseButton != GUI::MouseButton::NO_BUTTON){
        me.button = mCurrentMouseButton;
        if (!mDragging && mSingleClick &&
                me.position.squaredDistance(mDragStart) > mDragStartSquaredThreshold){
            mSingleClick = false;
			if (mMouseDragModes[mCurrentMouseButton] != MouseDragMode::DISABLED) {
				mDragging = true;
				mWindow->captureInput();
				if (mMouseDragModes[mCurrentMouseButton] == MouseDragMode::ENABLED_HIDECURSOR) {
					mUI->hideCursor();
				}
				onMouseDragBegin(me);
			}
        }
    }
    if (mDragging) {
		onMouseDrag(me);
    } else if (mUI->isCursorVisible()) {
        onMouseHover(me);
    }

}

void GameHandlerBase::onMouseDown(GUI::MouseEventArgs &me)
{
    if (mCurrentMouseButton == GUI::MouseButton::NO_BUTTON){
        mCurrentMouseButton = me.button;
        mSingleClick = true;
        mDragStart = me.position;
    }

    

}

void GameHandlerBase::onMouseUp(GUI::MouseEventArgs &me)
{
    clampToWindow(me);
    if (me.button == mCurrentMouseButton) {
        if (mDragging) {
            mDragging = false;
			mWindow->releaseInput();
            if (mMouseDragModes[mCurrentMouseButton] == MouseDragMode::ENABLED_HIDECURSOR){
                mUI->showCursor();
            }
            onMouseDragEnd(me);
        } else if (mSingleClick) {
            mSingleClick = false;
            onMouseClick(me);
        }
        mCurrentMouseButton = GUI::MouseButton::NO_BUTTON;
    }

    

}

void GameHandlerBase::onMouseHover(const GUI::MouseEventArgs &evt)
{
    
}

void GameHandlerBase::onMouseClick(const GUI::MouseEventArgs &evt)
{
}

void GameHandlerBase::onMouseDragBegin(const GUI::MouseEventArgs &evt)
{

}

void GameHandlerBase::onMouseDrag(const GUI::MouseEventArgs &evt)
{

}

void GameHandlerBase::onMouseDragEnd(const GUI::MouseEventArgs &evt)
{

}

void GameHandlerBase::onMouseDragAbort()
{

}

void GameHandlerBase::clampToWindow(GUI::MouseEventArgs &me)
{
    if (me.position.x < 0.f) me.position.x = 0.f;
    if (me.position.x > 1.f) me.position.x = 1.f;
    if (me.position.y < 0.f) me.position.y = 0.f;
    if (me.position.y > 1.f) me.position.y = 1.f;
}

void GameHandlerBase::setMouseDragMode(GUI::MouseButton::MouseButton button, GameHandlerBase::MouseDragMode mode)
{
    mMouseDragModes[button] = mode;
}

const Ogre::Vector2 &GameHandlerBase::dragStart()
{
    return mDragStart;
}

void GameHandlerBase::update(float timeSinceLastFrame)
{

}


}
} // namespace UI

