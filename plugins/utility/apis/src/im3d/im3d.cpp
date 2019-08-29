#include "../apislib.h"

#include "im3d.h"

#include "Modules/threading/workgroup.h"

#include "Madgine/render/vertex.h"

#include "im3d_internal.h"

namespace Engine {
namespace Apis {

    namespace Im3D {

        Threading::WorkgroupLocal<Im3DContext> sContext;

        Im3DContext *GetCurrentContext()
        {
            return &sContext;
        }

        void NewFrame()
        {
            sContext->mTriangleIndices.clear();
            sContext->mTriangleVertices.clear();
        }

        void Mesh()
        {
            /*auto &ref = sContext->mTriangles.emplace_back();
            ref = { {
                { { 0, 0, 0 }, { 1, 0, 0, 1 }, { 0, 0, 1 } },
                { { 1, 0, 0 }, { 0, 1, 0, 1 }, { 0, 0, 1 } },
                { { 0, 1, 0 }, { 0, 0, 1, 1 }, { 0, 0, 1 } },
            } };*/
        }

    }

}
}