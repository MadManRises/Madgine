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
			onPointerDragAbort();
		}

		void GameHandlerBase::update(std::chrono::microseconds timeSinceLastFrame, Scene::ContextMask mask)
		{
			if (mContext & mask)
				update(timeSinceLastFrame);
		}

		void GameHandlerBase::fixedUpdate(std::chrono::microseconds timeStep, Scene::ContextMask mask)
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

		void GameHandlerBase::onPointerMove(const Input::PointerEventArgs& me)
		{
			//clampToWindow(me);
			if (mCurrentMouseButton != Input::MouseButton::NO_BUTTON)
			{
				//me.button = mCurrentMouseButton;
				if (!mDragging && mSingleClick &&
					fabs(me.position[0] - mDragStart[0]) + fabs(me.position[1] - mDragStart[1]) > mDragStartThreshold)
				{
					mSingleClick = false;
					if (mPointerDragModes[mCurrentMouseButton] != MouseDragMode::DISABLED)
					{
						mDragging = true;
						mWidget->captureInput();
						if (mPointerDragModes[mCurrentMouseButton] == MouseDragMode::ENABLED_HIDECURSOR)
						{
							mUI.hideCursor();
						}
						onPointerDragBegin(me);
					}
				}
			}
			if (mDragging)
			{
				onPointerDrag(me);
			}
			else if (mUI.isCursorVisible())
			{
				onPointerHover(me);
			}
		}

		void GameHandlerBase::onPointerDown(const Input::PointerEventArgs& me)
		{
			if (mCurrentMouseButton == Input::MouseButton::NO_BUTTON)
			{
				mCurrentMouseButton = me.button;
				mSingleClick = true;
				mDragStart = me.position;
			}
		}

		void GameHandlerBase::onPointerUp(const Input::PointerEventArgs& me)
		{
			//clampToWindow(me);
			if (me.button == mCurrentMouseButton)
			{
				if (mDragging)
				{
					mDragging = false;
					mWidget->releaseInput();
					if (mPointerDragModes[mCurrentMouseButton] == MouseDragMode::ENABLED_HIDECURSOR)
					{
						mUI.showCursor();
					}
					onPointerDragEnd(me);
				}
				else if (mSingleClick)
				{
					mSingleClick = false;
					onPointerClick(me);
				}
				mCurrentMouseButton = Input::MouseButton::NO_BUTTON;
			}
		}

		void GameHandlerBase::onPointerHover(const Input::PointerEventArgs& evt)
		{
		}

		void GameHandlerBase::onPointerClick(const Input::PointerEventArgs& evt)
		{
		}

		void GameHandlerBase::onPointerDragBegin(const Input::PointerEventArgs& evt)
		{
		}

		void GameHandlerBase::onPointerDrag(const Input::PointerEventArgs& evt)
		{
		}

		void GameHandlerBase::onPointerDragEnd(const Input::PointerEventArgs& evt)
		{
		}

		void GameHandlerBase::onPointerDragAbort()
		{
		}

		void GameHandlerBase::clampToWindow(Input::PointerEventArgs& me)
		{
			if (me.position[0] < 0.f) me.position[0] = 0.f;
			if (me.position[0] > 1.f) me.position[0] = 1.f;
			if (me.position[1] < 0.f) me.position[1] = 0.f;
			if (me.position[1] > 1.f) me.position[1] = 1.f;
		}

		void GameHandlerBase::setPointerDragMode(Input::MouseButton::MouseButton button, MouseDragMode mode)
		{
			mPointerDragModes[button] = mode;
		}

		const Vector2& GameHandlerBase::dragStart() const
		{
			return mDragStart;
		}

		void GameHandlerBase::update(std::chrono::microseconds timeSinceLastFrame)
		{
		}

		void GameHandlerBase::fixedUpdate(std::chrono::microseconds timeStep)
		{
		}
	}
} // namespace UI
