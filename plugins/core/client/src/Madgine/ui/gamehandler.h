#pragma once

#include "handler.h"
#include "Modules/uniquecomponent/uniquecomponentdefine.h"
#include "../threading/contextmasks.h"

#include "../input/inputevents.h"

namespace Engine
{
	namespace UI
	{
		class MADGINE_CLIENT_EXPORT GameHandlerBase : public Handler
		{
		public:
			GameHandlerBase(UIManager &ui, Threading::ContextMask context = Threading::ContextMask::SceneContext);

			void abortDrag();

			void update(std::chrono::microseconds timeSinceLastFrame, Threading::ContextMask mask);
			void fixedUpdate(std::chrono::microseconds timeStep, Threading::ContextMask mask);

			GameHandlerBase &getSelf(bool = true);

		protected:
			void onPointerMove(const Input::PointerEventArgs& me) override;

			void onPointerDown(const Input::PointerEventArgs& me) override;

			void onPointerUp(const Input::PointerEventArgs& me) override;

			virtual void onPointerHover(const Input::PointerEventArgs& evt);

			virtual void onPointerClick(const Input::PointerEventArgs& evt);

			virtual void onPointerDragBegin(const Input::PointerEventArgs& evt);

			virtual void onPointerDrag(const Input::PointerEventArgs& evt);

			virtual void onPointerDragEnd(const Input::PointerEventArgs& evt);

			virtual void onPointerDragAbort();

			virtual void update(std::chrono::microseconds timeSinceLastFrame);
			virtual void fixedUpdate(std::chrono::microseconds timeStep);


			void clampToWindow(Input::PointerEventArgs& me);

			enum class MouseDragMode
			{
				DISABLED = 0,
				ENABLED,
				ENABLED_HIDECURSOR
			};

			void setPointerDragMode(Input::MouseButton::MouseButton button, MouseDragMode mode);

			const Vector2& dragStart() const;

		private:
			static const float mDragStartThreshold;

			std::array<MouseDragMode, Input::MouseButton::BUTTON_COUNT> mPointerDragModes;

			Input::MouseButton::MouseButton mCurrentMouseButton;

			bool mDragging, mSingleClick;

			Vector2 mDragStart;

			Threading::ContextMask mContext;
		};


	} // namespace UI

}

DECLARE_UNIQUE_COMPONENT(Engine::UI, GameHandler, GameHandlerBase, UIManager &);

namespace Engine
{
	namespace UI
	{

		template <typename T>
		using GameHandler = GameHandlerComponent<T>;

	}
}
