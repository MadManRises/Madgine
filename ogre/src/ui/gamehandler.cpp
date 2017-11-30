#include "ogrelib.h"
#include "gamehandler.h"
#include "ui/uimanager.h"
#include "gui/guisystem.h"
#include "gui/windows/window.h"
#include "scene/ogrescenemanager.h"

namespace Engine
{
	namespace UI
	{
		const float GameHandlerBase::mDragStartThreshold = 0.01f;


		GameHandlerBase::GameHandlerBase(const std::string& windowName, Scene::ContextMask context) :
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

		void GameHandlerBase::update(float timeSinceLastFrame, Scene::ContextMask mask)
		{
			if (mContext & mask)
				update(timeSinceLastFrame);
		}

		void GameHandlerBase::fixedUpdate(float timeStep, Scene::ContextMask mask)
		{
			if (mContext & mask)
				fixedUpdate(timeStep);
		}

		Scene::OgreSceneManager* GameHandlerBase::sceneMgr() const
		{
			return mSceneMgr;
		}

		void GameHandlerBase::onMouseMove(GUI::MouseEventArgs& me)
		{
			clampToWindow(me);
			if (mCurrentMouseButton != GUI::MouseButton::NO_BUTTON)
			{
				me.button = mCurrentMouseButton;
				if (!mDragging && mSingleClick &&
					fabs(me.position[0] - mDragStart[0]) + fabs(me.position[1] - mDragStart[1]) > mDragStartThreshold)
				{
					mSingleClick = false;
					if (mMouseDragModes[mCurrentMouseButton] != MouseDragMode::DISABLED)
					{
						mDragging = true;
						mWindow->captureInput();
						if (mMouseDragModes[mCurrentMouseButton] == MouseDragMode::ENABLED_HIDECURSOR)
						{
							mUI->hideCursor();
						}
						onMouseDragBegin(me);
					}
				}
			}
			if (mDragging)
			{
				onMouseDrag(me);
			}
			else if (mUI->isCursorVisible())
			{
				onMouseHover(me);
			}
		}

		void GameHandlerBase::onMouseDown(GUI::MouseEventArgs& me)
		{
			if (mCurrentMouseButton == GUI::MouseButton::NO_BUTTON)
			{
				mCurrentMouseButton = me.button;
				mSingleClick = true;
				mDragStart = me.position;
			}
		}

		void GameHandlerBase::onMouseUp(GUI::MouseEventArgs& me)
		{
			clampToWindow(me);
			if (me.button == mCurrentMouseButton)
			{
				if (mDragging)
				{
					mDragging = false;
					mWindow->releaseInput();
					if (mMouseDragModes[mCurrentMouseButton] == MouseDragMode::ENABLED_HIDECURSOR)
					{
						mUI->showCursor();
					}
					onMouseDragEnd(me);
				}
				else if (mSingleClick)
				{
					mSingleClick = false;
					onMouseClick(me);
				}
				mCurrentMouseButton = GUI::MouseButton::NO_BUTTON;
			}
		}

		void GameHandlerBase::onMouseHover(const GUI::MouseEventArgs& evt)
		{
		}

		void GameHandlerBase::onMouseClick(const GUI::MouseEventArgs& evt)
		{
		}

		void GameHandlerBase::onMouseDragBegin(const GUI::MouseEventArgs& evt)
		{
		}

		void GameHandlerBase::onMouseDrag(const GUI::MouseEventArgs& evt)
		{
		}

		void GameHandlerBase::onMouseDragEnd(const GUI::MouseEventArgs& evt)
		{
		}

		void GameHandlerBase::onMouseDragAbort()
		{
		}

		void GameHandlerBase::clampToWindow(GUI::MouseEventArgs& me)
		{
			if (me.position[0] < 0.f) me.position[0] = 0.f;
			if (me.position[0] > 1.f) me.position[0] = 1.f;
			if (me.position[1] < 0.f) me.position[1] = 0.f;
			if (me.position[1] > 1.f) me.position[1] = 1.f;
		}

		void GameHandlerBase::setMouseDragMode(GUI::MouseButton::MouseButton button, MouseDragMode mode)
		{
			mMouseDragModes[button] = mode;
		}

		const std::array<float, 2>& GameHandlerBase::dragStart() const
		{
			return mDragStart;
		}

		void GameHandlerBase::update(float timeSinceLastFrame)
		{
		}

		void GameHandlerBase::fixedUpdate(float timeStep)
		{
		}
	}
} // namespace UI
