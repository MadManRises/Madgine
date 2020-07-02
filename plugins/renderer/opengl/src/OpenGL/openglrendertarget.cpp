#include "opengllib.h"

#include "openglrendercontext.h"

#include "openglrendertarget.h"

#include "openglmeshdata.h"
#include "openglmeshloader.h"

#include "Modules/math/rect2i.h"

#include "util/openglprogram.h"

namespace Engine {
namespace Render {

    OpenGLRenderTarget::OpenGLRenderTarget(OpenGLRenderContext *context)
        : RenderTarget(context)
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

    void OpenGLRenderTarget::renderMesh(MeshData *m, Program *p)
    {
        OpenGLMeshData *mesh = static_cast<OpenGLMeshData *>(m);
        OpenGLProgram *program = static_cast<OpenGLProgram *>(p);

        if (!mesh->mVAO)
            return;

        mesh->mVAO.bind();
        program->bind();

		program->verify();

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

        mesh->mVAO.unbind();
    }

    void OpenGLRenderTarget::renderVertices(Program *program, size_t groupSize, std::vector<Vertex> vertices, std::vector<unsigned short> indices)
    {
        if (!vertices.empty()) {
            OpenGLMeshData tempMesh;
            OpenGLMeshLoader::getSingleton().generate(tempMesh, groupSize, std::move(vertices), std::move(indices));

            renderMesh(&tempMesh, program);
        }
    }

    void OpenGLRenderTarget::renderVertices(Program *program, size_t groupSize, std::vector<Vertex2> vertices, std::vector<unsigned short> indices, TextureHandle texture)
    {
        if (!vertices.empty()) {
            OpenGLMeshData tempMesh;
            OpenGLMeshLoader::getSingleton().generate(tempMesh, groupSize, std::move(vertices), std::move(indices));

            tempMesh.mTextureHandle = texture;

            renderMesh(&tempMesh, program);
        }
    }

    void OpenGLRenderTarget::clearDepthBuffer()
    {
        glClear(GL_DEPTH_BUFFER_BIT);
        GL_CHECK();
    }
}
}
