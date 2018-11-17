#pragma once

#include <OgreCommon.h>

#include <OgreRenderQueueListener.h>
#include <OgreSingleton.h>
#include <OgreTexture.h>

#include "ImguiRenderable.h"
#include "Madgine/input/inputlistener.h"
#include "../imguimanager.h"

namespace Ogre {
	class SceneManager;
	class TextureUnitState;
}

namespace Engine
{
	namespace Tools {

		std::unique_ptr<ImGuiManager> createOgreManager(App::ClientApplication &);


		class OgreImguiManager : public Ogre::RenderQueueListener, public ImGuiManager
		{
		public:

			OgreImguiManager(App::ClientApplication &app);
			~OgreImguiManager();

			virtual void init();
			virtual void finalize();

			virtual void newFrame(float deltaTime);

			//inherited from RenderQueueListener
			virtual void renderQueueEnded(uint8_t queueGroupId, const String& invocation, bool& repeatThisInvocation);


		protected:

			void createFontTexture();
			void createMaterial();

		protected:
			Ogre::SceneManager*				mSceneMgr;
			Ogre::TextureUnitState*           mTexUnit;
			int                         mLastRenderedFrame;

			ImGUIRenderable             mRenderable;
			Ogre::TexturePtr                  mFontTex;

			bool                        mFrameEnded;

		};
	}
}
