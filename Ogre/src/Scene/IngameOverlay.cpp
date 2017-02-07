#include "madginelib.h"

#include "IngameOverlay.h"

#include "App\Application.h"

namespace Engine {
	namespace Scene {

		IngameOverlay::IngameOverlay(const std::string &name):
			Ogre::ManualObject(name)
		{
			setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY - 1);
			setUseIdentityProjection(true);
			setUseIdentityView(true);
			setQueryFlags(0);

			App::Application::getSingleton().renderWindow()->addListener(this);
		}

		IngameOverlay::~IngameOverlay()
		{
			App::Application::getSingleton().renderWindow()->removeListener(this);
		}

		void IngameOverlay::preViewportUpdate(const Ogre::RenderTargetViewportEvent & ev)
		{
			mVisible = isVisible();
			setVisible(false);
		}

		void IngameOverlay::postViewportUpdate(const Ogre::RenderTargetViewportEvent & ev)
		{
			setVisible(mVisible);
		}

	}
}