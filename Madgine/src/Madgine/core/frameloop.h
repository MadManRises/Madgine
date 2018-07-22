#pragma once

#include "../scene/contextmasks.h"
#include "../madgineobject.h"

namespace Engine
{
	namespace Core
	{
		class MADGINE_BASE_EXPORT FrameLoop : public MadgineObject
		{
		public:
			
			FrameLoop();
			virtual ~FrameLoop() = default;

			virtual int go();

			virtual bool singleFrame();

			float fixedRemainder() const;

			void addFrameListener(FrameListener* listener);
			void removeFrameListener(FrameListener* listener);

		protected:
			virtual bool init();
			virtual void finalize();

			bool sendFrameStarted(float timeSinceLastFrame);
			bool sendFrameRenderingQueued(float timeSinceLastFrame, Scene::ContextMask context = Scene::ContextMask::SceneContext);
			bool sendFrameFixedUpdate(float timeSinceLastFrame, Scene::ContextMask context = Scene::ContextMask::SceneContext);
			bool sendFrameEnded(float timeSinceLastFrame);
			
		private:
			std::vector<FrameListener*> mListeners;

			float mTimeBank;

			static constexpr float FIXED_TIMESTEP = 0.015f;
		};

		
	}
}