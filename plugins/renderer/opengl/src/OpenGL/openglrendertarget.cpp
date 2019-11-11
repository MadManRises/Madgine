#include "opengllib.h"

#include "openglrendertarget.h"

#include "Modules/math/matrix4.h"
#include "Modules/math/vector4.h"

#include "openglrenderwindow.h"

#include "Madgine/gui/vertex.h"

#include "openglshaderloader.h"

#include "util/openglshader.h"

#include "Madgine/render/camera.h"

#include "Madgine/render/renderpass.h"
#include "openglmeshdata.h"
#include "openglmeshloader.h"

#include "openglfontdata.h"
#include "openglfontloader.h"

#include "openglrendercontext.h"

namespace Engine {
namespace Render {

    OpenGLRenderTarget::OpenGLRenderTarget(OpenGLRenderContext *context)
        : RenderTarget(context)
    {
    }

    OpenGLRenderTarget::OpenGLRenderTarget(OpenGLRenderContext *context, dont_create_t)
        : RenderTarget(context)
        , mTempBuffer(dont_create)
    {
    }

    OpenGLRenderTarget::~OpenGLRenderTarget()
    {
    }

    void OpenGLRenderTarget::beginFrame()
    {
        const Vector2i &screenSize = size();

        glViewport(0, 0, static_cast<GLsizei>(screenSize.x), static_cast<GLsizei>(screenSize.y));
        GL_CHECK();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        GL_CHECK();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        GL_CHECK();

        RenderTarget::beginFrame();
    }

    void OpenGLRenderTarget::setRenderSpace(const Rect2i &space)
    {
        const Vector2i &screenSize = size();

        glViewport(space.mTopLeft.x, screenSize.y - (space.mTopLeft.y + space.mSize.y), space.mSize.x, space.mSize.y);
        GL_CHECK();
    }

    void OpenGLRenderTarget::renderMesh(Resources::MeshData *m)
    {
        OpenGLMeshData *mesh = static_cast<OpenGLMeshData *>(m);

        mesh->mVAO.bind();

        constexpr GLenum modes[] {
            GL_POINTS,
            GL_LINES,
            GL_TRIANGLES
        };

        GLenum mode = modes[mesh->mGroupSize - 1];

        if (mesh->mIndices) {
            glDrawElements(mode, mesh->mElementCount, GL_UNSIGNED_SHORT, 0);
        } else
            glDrawArrays(mode, 0, mesh->mElementCount);
        GL_CHECK();
    }

    /*void OpenGLRenderTexture::renderInstancedMesh(RenderPassFlags flags, Resources::MeshData *meshData, const std::vector<Matrix4> &transforms)
    {
        setupProgram(flags);

        for (const Matrix4 &transform : transforms) {
            renderMesh(meshData, transform);
        }
    }*/

    void OpenGLRenderTarget::renderVertices(RenderPassFlags flags, size_t groupSize, Vertex *vertices, size_t vertexCount, unsigned short *indices, size_t indexCount)
    {
        if ((!indices && vertexCount > 0) || indexCount > 0) {
            std::shared_ptr<Resources::MeshData> tempMesh = OpenGLMeshLoader::getSingleton().generate(groupSize, vertices, vertexCount, indices, indexCount);

            //setupProgram(flags);

            renderMesh(tempMesh.get());
        }
    }

    void OpenGLRenderTarget::renderVertices(RenderPassFlags flags, size_t groupSize, Vertex2 *vertices, size_t vertexCount, unsigned short *indices, size_t indexCount, unsigned int textureId)
    {
        if ((!indices && vertexCount > 0) || indexCount > 0) {
            std::shared_ptr<Resources::MeshData> tempMesh = OpenGLMeshLoader::getSingleton().generate(groupSize, vertices, vertexCount, indices, indexCount);

            //setupProgram(flags, textureId);

            renderMesh(tempMesh.get());
        }
    }

    void OpenGLRenderTarget::clearDepthBuffer()
    {
        glClear(GL_DEPTH_BUFFER_BIT);
        GL_CHECK();
    }
}
}
