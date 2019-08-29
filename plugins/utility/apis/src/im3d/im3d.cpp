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

        void Mesh(const Render::Vertex *vertices, size_t vertexCount, const Matrix4 &transform, const unsigned int *indices, size_t indexCount)
        {
            Im3DContext *c = &sContext;

            unsigned int indexBase = c->mTriangleVertices.size();

            std::transform(vertices, vertices + vertexCount, std::back_inserter(sContext->mTriangleVertices), [&](const Render::Vertex &v) {
                Render::Vertex result = v;
                result.mPos = (transform * Vector4 { v.mPos, 1.0f }).xyz();
                return result;
			});

            if (indices) {
                assert(indexCount % 3 == 0);
                std::transform(indices, indices + indexCount, std::back_inserter(c->mTriangleIndices), [&](unsigned int i) { return i + indexBase; });
            } else {
                assert(vertexCount % 3 == 0);
                size_t oldIndexCount = c->mTriangleIndices.size();
                c->mTriangleIndices.resize(oldIndexCount + vertexCount);
                std::iota(c->mTriangleIndices.begin() + oldIndexCount, c->mTriangleIndices.end(), indexBase);
            }
        }

    }

}
}