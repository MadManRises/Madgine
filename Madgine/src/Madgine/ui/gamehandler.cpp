#include "../clientlib.h"
#include "gamehandler.h"
#include "uimanager.h"
#include "../gui/guisystem.h"
#include "../gui/widgets/widget.h"

namespace Engine
{
	namespace UI
	{
		const float GameHandlerBase::mDragStartThreshold = 0.01f;


		GameHandlerBase::GameHandlerBase(UIManager &ui, const std::string& windowName, Scene::ContextMask context) :
			Handler(ui, windowName),
			mCurrentMouseButton(Input::MouseButton::NO_BUTTON),
			mDragging(false),
			mSingleClick(false),
			mContext(context)
		{
		}

		void GameHandlerBase::abortDrag()
		{
			mDragging = false;
			mWidget->releaseInput();
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

		Scene::SceneManager &GameHandlerBase::sceneMgr(bool init) const
		{
			if (init)
			{
				checkInitState();
			}
			return mUI.sceneMgr(init);
		}

		GameHandlerBase& GameHandlerBase::getSelf(bool init)
		{
			if (init)
			{
				checkDependency();
			}
			return *this;
		}

		void GameHandlerBase::onMouseMove(Input::MouseEventArgs& me)
		{
			clampToWindow(me);
			if (mCurrentMouseButton != Input::MouseButton::NO_BUTTON)
			{
				me.button = mCurrentMouseButton;
				if (!mDragging && mSingleClick &&
					fabs(me.position[0] - mDragStart[0]) + fabs(me.position[1] - mDragStart[1]) > mDragStartThreshold)
				{
					mSingleClick = false;
					if (mMouseDragModes[mCurrentMouseButton] != MouseDragMode::DISABLED)
					{
						mDragging = true;
						mWidget->captureInput();
						if (mMouseDragModes[mCurrentMouseButton] == MouseDragMode::ENABLED_HIDECURSOR)
						{
							mUI.hideCursor();
						}
						onMouseDragBegin(me);
					}
				}
			}
			if (mDragging)
			{
				onMouseDrag(me);
			}
			else if (mUI.isCursorVisible())
			{
				onMouseHover(me);
			}
		}

		void GameHandlerBase::onMouseDown(Input::MouseEventArgs& me)
		{
			if (mCurrentMouseButton == Input::MouseButton::NO_BUTTON)
			{
				mCurrentMouseButton = me.button;
				mSingleClick = true;
				mDragStart = me.position;
			}
		}

		void GameHandlerBase::onMouseUp(Input::MouseEventArgs& me)
		{
			clampToWindow(me);
			if (me.button == mCurrentMouseButton)
			{
				if (mDragging)
				{
					mDragging = false;
					mWidget->releaseInput();
					if (mMouseDragModes[mCurrentMouseButton] == MouseDragMode::ENABLED_HIDECURSOR)
					{
						mUI.showCursor();
					}
					onMouseDragEnd(me);
				}
				else if (mSingleClick)
				{
					mSingleClick = false;
					onMouseClick(me);
				}
				mCurrentMouseButton = Input::MouseButton::NO_BUTTON;
			}
		}

		void GameHandlerBase::onMouseHover(const Input::MouseEventArgs& evt)
		{
		}

		void GameHandlerBase::onMouseClick(const Input::MouseEventArgs& evt)
		{
		}

		void GameHandlerBase::onMouseDragBegin(const Input::MouseEventArgs& evt)
		{
		}

		void GameHandlerBase::onMouseDrag(const Input::MouseEventArgs& evt)
		{
		}

		void GameHandlerBase::onMouseDragEnd(const Input::MouseEventArgs& evt)
		{
		}

		void GameHandlerBase::onMouseDragAbort()
		{
		}

		void GameHandlerBase::clampToWindow(Input::MouseEventArgs& me)
		{
			if (me.position[0] < 0.f) me.position[0] = 0.f;
			if (me.position[0] > 1.f) me.position[0] = 1.f;
			if (me.position[1] < 0.f) me.position[1] = 0.f;
			if (me.position[1] > 1.f) me.position[1] = 1.f;
		}

		void GameHandlerBase::setMouseDragMode(Input::MouseButton::MouseButton button, MouseDragMode mode)
		{
			mMouseDragModes[button] = mode;
		}

		const Vector2& GameHandlerBase::dragStart() const
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
