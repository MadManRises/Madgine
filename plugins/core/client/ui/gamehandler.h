#pragma once

#include "handler.h"
#include "Interfaces/uniquecomponent/uniquecomponentdefine.h"
#include "Madgine/scene/contextmasks.h"

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


			static void clampToWindow(Input::PointerEventArgs& me);

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

			Scene::ContextMask mContext;
		};


		/*using GameHandlerCollector = UniqueComponentCollector<GameHandlerBase, UIManager &>;

		template <class T>
		using GameHandler = Scripting::Scope<T, UniqueComponent<T, GameHandlerCollector>>;

		using GameHandlerContainer = UniqueComponentContainer<GameHandlerBase, UIManager &>;*/

	} // namespace UI
	/*
#ifndef STATIC_BUILD
	MADGINE_CLIENT_TEMPLATE_INSTANTIATION struct UniqueComponentRegistry<UI::GameHandlerBase, UI::UIManager &>;
#endif
*/
}

DEFINE_UNIQUE_COMPONENT(Engine::UI, GameHandlerBase, UIManager&, GameHandler, MADGINE_CLIENT);

RegisterType(Engine::UI::GameHandlerBase);
//RegisterCollector(Engine::UI::GameHandlerCollector);