#pragma once

namespace Engine {
	namespace Scene {

		class MADGINE_EXPORT IngameOverlay : public Ogre::ManualObject, public Ogre::RenderTargetListener
		{
		public:
			IngameOverlay(const std::string &name);
			virtual ~IngameOverlay();

			virtual void preViewportUpdate(const Ogre::RenderTargetViewportEvent &ev) override;
			virtual void postViewportUpdate(const Ogre::RenderTargetViewportEvent &ev) override;

			private:
				bool mVisible;
		};

	}
}
