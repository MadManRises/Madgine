#include "libinclude.h"

#include "IngameOverlay.h"

#include "UI\UIManager.h"

namespace Engine {
	namespace OGRE {

		IngameOverlay::IngameOverlay(const std::string &name):
			Ogre::ManualObject(name)
		{
			setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY - 1);
			setUseIdentityProjection(true);
			setUseIdentityView(true);
			setQueryFlags(0);

			UI::UIManager::getSingleton().renderWindow()->addListener(this);
		}

		IngameOverlay::~IngameOverlay()
		{
			UI::UIManager::getSingleton().renderWindow()->removeListener(this);
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