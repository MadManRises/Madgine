/*#pragma once

namespace Engine
{
	namespace Scene
	{
		class OGREMADGINE_EXPORT IngameOverlay : public Ogre::ManualObject, public Ogre::RenderTargetListener
		{
		public:
			IngameOverlay(const std::string& name);
			virtual ~IngameOverlay();

			void preViewportUpdate(const Ogre::RenderTargetViewportEvent& ev) override;
			void postViewportUpdate(const Ogre::RenderTargetViewportEvent& ev) override;

		private:
			bool mVisible;
		};
	}
}*/
