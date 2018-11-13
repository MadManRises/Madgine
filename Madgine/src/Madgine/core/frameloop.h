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

			virtual bool singleFrame(std::chrono::microseconds timeSinceLastFrame = 0us);

			std::chrono::nanoseconds fixedRemainder() const;

			void addFrameListener(FrameListener* listener);
			void removeFrameListener(FrameListener* listener);

			void shutdown();
			bool isShutdown();

		protected:
			void startLoop();
			
			virtual bool init();
			virtual void finalize();

			bool sendFrameStarted(std::chrono::microseconds timeSinceLastFrame);
			bool sendFrameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Scene::ContextMask context = Scene::ContextMask::SceneContext);
			bool sendFrameFixedUpdate(std::chrono::microseconds timeSinceLastFrame, Scene::ContextMask context = Scene::ContextMask::SceneContext);
			bool sendFrameEnded(std::chrono::microseconds timeSinceLastFrame);
			
		private:
			bool mShutdown;

			std::vector<FrameListener*> mListeners;


			std::chrono::microseconds mTimeBank;

			static constexpr std::chrono::microseconds FIXED_TIMESTEP{ 15000 };
		};

		
	}
}