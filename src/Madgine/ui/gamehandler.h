#pragma once

#include "handler.h"
#include "../uniquecomponent/uniquecomponentcollector.h"
#include "../scene/contextmasks.h"
#include "../uniquecomponent/uniquecomponent.h"

#include "Interfaces/scripting/types/scope.h"

#include "../input/inputevents.h"

namespace Engine
{
	namespace UI
	{
		class MADGINE_CLIENT_EXPORT GameHandlerBase : public Handler
		{
		public:
			GameHandlerBase(UIManager &ui, const std::string& windowName, Scene::ContextMask context = Scene::ContextMask::SceneContext);

			void abortDrag();

			void update(std::chrono::microseconds timeSinceLastFrame, Scene::ContextMask mask);
			void fixedUpdate(std::chrono::microseconds timeStep, Scene::ContextMask mask);

			Scene::SceneManager &sceneMgr(bool = true) const;

			GameHandlerBase &getSelf(bool = true);

		protected:
			void onMouseMove(const Input::MouseEventArgs& me) override;

			void onMouseDown(const Input::MouseEventArgs& me) override;

			void onMouseUp(const Input::MouseEventArgs& me) override;

			virtual void onMouseHover(const Input::MouseEventArgs& evt);

			virtual void onMouseClick(const Input::MouseEventArgs& evt);

			virtual void onMouseDragBegin(const Input::MouseEventArgs& evt);

			virtual void onMouseDrag(const Input::MouseEventArgs& evt);

			virtual void onMouseDragEnd(const Input::MouseEventArgs& evt);

			virtual void onMouseDragAbort();

			virtual void update(std::chrono::microseconds timeSinceLastFrame);
			virtual void fixedUpdate(std::chrono::microseconds timeStep);


			static void clampToWindow(Input::MouseEventArgs& me);

			enum class MouseDragMode
			{
				DISABLED = 0,
				ENABLED,
				ENABLED_HIDECURSOR
			};

			void setMouseDragMode(Input::MouseButton::MouseButton button, MouseDragMode mode);

			const Vector2& dragStart() const;

		private:
			static const float mDragStartThreshold;

			std::array<MouseDragMode, Input::MouseButton::BUTTON_COUNT> mMouseDragModes;

			Input::MouseButton::MouseButton mCurrentMouseButton;

			bool mDragging, mSingleClick;

			Vector2 mDragStart;

			Scene::ContextMask mContext;
		};


		using GameHandlerCollector = UniqueComponentCollector<GameHandlerBase, UIManager &>;

		template <class T>
		using GameHandler = Scripting::Scope<T, UniqueComponent<T, GameHandlerCollector>>;

		using GameHandlerContainer = UniqueComponentContainer<GameHandlerBase, UIManager &>;

	} // namespace UI

#ifndef STATIC_BUILD
	template struct MADGINE_CLIENT_EXPORT UniqueComponentRegistry<UI::GameHandlerBase, UI::UIManager &>;
#endif

}


RegisterType(Engine::UI::GameHandlerBase);
RegisterCollector(Engine::UI::GameHandlerCollector);