#include "opengllib.h"

#include "openglrendercontext.h"

#include "openglrendertarget.h"

#include "Madgine/meshloader/meshdata.h"
#include "openglmeshdata.h"
#include "openglmeshloader.h"

#include "Meta/math/rect2i.h"

#include "util/openglpipelineinstance.h"

#include "Madgine/render/material.h"

namespace Engine {
namespace Render {

    OpenGLRenderTarget::OpenGLRenderTarget(OpenGLRenderContext *context, bool global, std::string name, size_t iterations)
        : RenderTarget(context, global, name, iterations)
    {
    }

    OpenGLRenderTarget::~OpenGLRenderTarget()
    {
    }

    void OpenGLRenderTarget::beginIteration(size_t iteration) const
    {
        RenderTarget::beginIteration(iteration);

        Vector2i screenSize = size();

        if (screenSize.x <= 0)
            screenSize.x = 1;
        if (screenSize.y <= 0)
            screenSize.y = 1;

        glViewport(0, 0, static_cast<GLsizei>(screenSize.x), static_cast<GLsizei>(screenSize.y));
        GL_CHECK();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        GL_CHECK();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        GL_CHECK();
    }

    void OpenGLRenderTarget::pushAnnotation(const char *tag)
    {
#if OPENGL_ES
        //glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, tag);
#else
        if (glPushDebugGroupKHR)
            glPushDebugGroupKHR(GL_DEBUG_SOURCE_APPLICATION, 0, -1, tag);
#endif
    }

    void OpenGLRenderTarget::popAnnotation()
    {
#if OPENGL_ES
        //glPopDebugGroup();
#else
        if (glPopDebugGroupKHR)
            glPopDebugGroupKHR();
#endif
    }

    void OpenGLRenderTarget::setRenderSpace(const Rect2i &space)
    {
        const Vector2i &screenSize = size();

        glViewport(space.mTopLeft.x, screenSize.y - (space.mTopLeft.y + space.mSize.y), space.mSize.x, space.mSize.y);
        GL_CHECK();
    }

    void OpenGLRenderTarget::renderMesh(const GPUMeshData *m, const PipelineInstance *p, const Material *material)
    {
        const OpenGLMeshData *mesh = static_cast<const OpenGLMeshData *>(m);
        const OpenGLPipelineInstance *pipeline = static_cast<const OpenGLPipelineInstance *>(p);

        mesh->mVertices.bind();
        static_cast<OpenGLRenderContext *>(context())->bindFormat(mesh->mFormat);
#if !OPENGL_ES || OPENGL_ES >= 310
#    if !OPENGL_ES
        if (glBindVertexBuffer)
#    endif
            glBindVertexBuffer(0, mesh->mVertices.handle(), 0, mesh->mVertexSize);
#endif

        pipeline->bind();

        pipeline->verify();

        if (material)
            bindTextures({ { material->mDiffuseTexture, TextureType_2D } });

        constexpr GLenum modes[] {
            GL_POINTS,
            GL_LINES,
            GL_TRIANGLES
        };

        GLenum mode = modes[mesh->mGroupSize - 1];

        if (mesh->mIndices) {
            mesh->mIndices.bind();
            glDrawElements(mode, mesh->mElementCount, GL_UNSIGNED_INT, 0);
        } else
            glDrawArrays(mode, 0, mesh->mElementCount);
        GL_CHECK();

        static_cast<OpenGLRenderContext *>(context())->unbindFormat();
    }

    void OpenGLRenderTarget::renderMeshInstanced(size_t count, const GPUMeshData *m, const PipelineInstance *p, const Material *material)
    {
        const OpenGLMeshData *mesh = static_cast<const OpenGLMeshData *>(m);
        const OpenGLPipelineInstance *pipeline = static_cast<const OpenGLPipelineInstance *>(p);

        mesh->mVertices.bind();
        static_cast<OpenGLRenderContext *>(context())->bindFormat(mesh->mFormat);
#if !OPENGL_ES || OPENGL_ES >= 310
#    if !OPENGL_ES
        if (glBindVertexBuffer)
#    endif
            glBindVertexBuffer(0, mesh->mVertices.handle(), 0, mesh->mVertexSize);
#endif

        pipeline->bind();

        pipeline->verify();

        if (material)
            bindTextures({ { material->mDiffuseTexture, TextureType_2D } });

        constexpr GLenum modes[] {
            GL_POINTS,
            GL_LINES,
            GL_TRIANGLES
        };

        GLenum mode = modes[mesh->mGroupSize - 1];

        if (mesh->mIndices) {
            mesh->mIndices.bind();
            glDrawElementsInstanced(mode, mesh->mElementCount, GL_UNSIGNED_SHORT, 0, count);
        } else
            glDrawArraysInstanced(mode, 0, mesh->mElementCount, count);
        GL_CHECK();

        static_cast<OpenGLRenderContext *>(context())->unbindFormat();
    }

    void OpenGLRenderTarget::clearDepthBuffer()
    {
        glClear(GL_DEPTH_BUFFER_BIT);
        GL_CHECK();
    }

    void OpenGLRenderTarget::bindTextures(const std::vector<TextureDescriptor> &tex, size_t offset) const
    {
        for (size_t i = 0; i < tex.size(); ++i) {
            glActiveTexture(GL_TEXTURE0 + offset + i);
            GLenum type;
            switch (tex[i].mType) {
            case TextureType_2D:
                type = GL_TEXTURE_2D;
                break;
#if MULTISAMPLING
            case TextureType_2DMultiSample:
                type = GL_TEXTURE_2D_MULTISAMPLE;
                break;
#endif
            case TextureType_Cube:
                type = GL_TEXTURE_CUBE_MAP;
                break;
            default:
                throw 0;
            }
            glBindTexture(type, tex[i].mTextureHandle);
            GL_CHECK();
        }
    }

}
}
