#pragma once

#include "OgrePrerequisites.h"
#include "OgreRenderable.h"
#include <OgreRenderOperation.h>

struct ImDrawVert;
typedef unsigned short ImDrawIdx;

using namespace Ogre;

namespace Engine
{
	namespace Tools
	{
		class SceneManager;
		class ImGUIRenderable : public Renderable
		{
		protected:
			RenderOperation mRenderOp;
			void initImGUIRenderable(void);

		public:
			ImGUIRenderable();
			~ImGUIRenderable();

			void updateVertexData(const ImDrawVert* vtxBuf, const ImDrawIdx* idxBuf, unsigned int vtxCount, unsigned int idxCount);
			Real getSquaredViewDepth(const Camera* cam) const { (void)cam; return 0; }

			virtual const MaterialPtr& getMaterial(void) const { return mMaterial; }
			virtual void getWorldTransforms(Matrix4* xform) const { *xform = mXform; }
			virtual void getRenderOperation(RenderOperation& op) { op = mRenderOp; }
			virtual const LightList& getLights(void) const;

			MaterialPtr              mMaterial;
			Matrix4                  mXform;
			int                      mVertexBufferSize;
			int                      mIndexBufferSize;

		};
		/** @} */
		/** @} */

	}
}

