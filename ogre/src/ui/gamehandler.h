#pragma once

#include "handler.h"
#include "ogreuniquecomponentcollector.h"
#include "scene/contextmasks.h"
#include "uniquecomponent.h"

#include "scripting/types/scope.h"

namespace Engine
{
	namespace UI
	{
		class OGREMADGINE_EXPORT GameHandlerBase : public Handler
		{
		public:
			GameHandlerBase(const std::string& windowName, Scene::ContextMask context = Scene::ContextMask::SceneContext);

			void abortDrag();

			void update(float timeSinceLastFrame, Scene::ContextMask mask);
			void fixedUpdate(float timeStep, Scene::ContextMask mask);

			Scene::OgreSceneManager* sceneMgr() const;

		protected:
			void onMouseMove(GUI::MouseEventArgs& me) override;

			void onMouseDown(GUI::MouseEventArgs& me) override;

			void onMouseUp(GUI::MouseEventArgs& me) override;

			virtual void onMouseHover(const GUI::MouseEventArgs& evt);

			virtual void onMouseClick(const GUI::MouseEventArgs& evt);

			virtual void onMouseDragBegin(const GUI::MouseEventArgs& evt);

			virtual void onMouseDrag(const GUI::MouseEventArgs& evt);

			virtual void onMouseDragEnd(const GUI::MouseEventArgs& evt);

			virtual void onMouseDragAbort();

			virtual void update(float timeSinceLastFrame);
			virtual void fixedUpdate(float timeStep);


			static void clampToWindow(GUI::MouseEventArgs& me);

			enum class MouseDragMode
			{
				DISABLED = 0,
				ENABLED,
				ENABLED_HIDECURSOR
			};

			void setMouseDragMode(GUI::MouseButton::MouseButton button, MouseDragMode mode);

			const std::array<float, 2>& dragStart() const;

		private:
			static const float mDragStartThreshold;

			std::array<MouseDragMode, GUI::MouseButton::BUTTON_COUNT> mMouseDragModes;

			GUI::MouseButton::MouseButton mCurrentMouseButton;

			bool mDragging, mSingleClick;

			std::array<float, 2> mDragStart;

			Scene::ContextMask mContext;

			Scene::OgreSceneManager* mSceneMgr;
		};


		using GameHandlerCollector = OgreUniqueComponentCollector<GameHandlerBase>;

		template <class T>
		using GameHandler = Scripting::Scope<T, UniqueComponent<T, GameHandlerCollector>>;
	} // namespace UI
}
