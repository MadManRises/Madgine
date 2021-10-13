#include "opengllib.h"

#include "openglrendercontext.h"

#include "openglrendertarget.h"

#include "meshloader.h"
#include "openglmeshdata.h"
#include "openglmeshloader.h"

#include "Meta/math/rect2i.h"

#include "util/openglprogram.h"

namespace Engine {
namespace Render {

    OpenGLRenderTarget::OpenGLRenderTarget(OpenGLRenderContext *context, bool global, std::string name, size_t iterations)
        : RenderTarget(context, global, name, iterations)
    {
    }

    OpenGLRenderTarget::~OpenGLRenderTarget()
    {
    }

    void OpenGLRenderTarget::beginIteration(size_t iteration)
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
        glPushDebugGroupKHR(GL_DEBUG_SOURCE_APPLICATION, 0, -1, tag);
    }

    void OpenGLRenderTarget::popAnnotation()
    {
        glPopDebugGroupKHR();
    }

    void OpenGLRenderTarget::setRenderSpace(const Rect2i &space)
    {
        const Vector2i &screenSize = size();

        glViewport(space.mTopLeft.x, screenSize.y - (space.mTopLeft.y + space.mSize.y), space.mSize.x, space.mSize.y);
        GL_CHECK();
    }

    void OpenGLRenderTarget::renderMesh(GPUMeshData *m, Program *p, const GPUMeshData::Material *material)
    {
        OpenGLMeshData *mesh = static_cast<OpenGLMeshData *>(m);
        OpenGLProgram *program = static_cast<OpenGLProgram *>(p);

        if (!mesh->mVAO)
            return;

        program->bind(&mesh->mVAO);

        program->verify();

        if (material)
            bindTextures({ { material->mDiffuseHandle, TextureType_2D } });

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

        program->unbind(&mesh->mVAO);
    }

    void OpenGLRenderTarget::renderMeshInstanced(size_t count, GPUMeshData *m, Program *p, const GPUMeshData::Material *material)
    {
        OpenGLMeshData *mesh = static_cast<OpenGLMeshData *>(m);
        OpenGLProgram *program = static_cast<OpenGLProgram *>(p);

        if (!mesh->mVAO)
            return;

        program->bind(&mesh->mVAO);

        program->verify();

        if (material)
            bindTextures({ { material->mDiffuseHandle, TextureType_2D } });

        constexpr GLenum modes[] {
            GL_POINTS,
            GL_LINES,
            GL_TRIANGLES
        };

        GLenum mode = modes[mesh->mGroupSize - 1];

        if (mesh->mIndices) {
            glDrawElementsInstanced(mode, mesh->mElementCount, GL_UNSIGNED_SHORT, 0, count);
        } else
            glDrawArraysInstanced(mode, 0, mesh->mElementCount, count);
        GL_CHECK();

        program->unbind(&mesh->mVAO);
    }

    void OpenGLRenderTarget::renderVertices(Program *program, size_t groupSize, std::vector<Vertex> vertices, std::vector<unsigned short> indices)
    {
        if (!vertices.empty()) {
            OpenGLMeshData tempMesh;
            OpenGLMeshLoader::getSingleton().generate(tempMesh, { groupSize, std::move(vertices), std::move(indices) });

            renderMesh(&tempMesh, program);
        }
    }

    void OpenGLRenderTarget::renderVertices(Program *program, size_t groupSize, std::vector<Vertex2> vertices, std::vector<unsigned short> indices, const GPUMeshData::Material *material)
    {
        if (!vertices.empty()) {
            OpenGLMeshData tempMesh;
            OpenGLMeshLoader::getSingleton().generate(tempMesh, { groupSize, std::move(vertices), std::move(indices) });

            renderMesh(&tempMesh, program, material);
        }
    }

    void OpenGLRenderTarget::clearDepthBuffer()
    {
        glClear(GL_DEPTH_BUFFER_BIT);
        GL_CHECK();
    }

    void OpenGLRenderTarget::bindTextures(const std::vector<TextureDescriptor> &tex, size_t offset)
    {
        for (size_t i = 0; i < tex.size(); ++i) {
            glActiveTexture(GL_TEXTURE0 + offset + i);
            GLenum type;
            switch (tex[i].mType) {
            case TextureType_2D:
                type = GL_TEXTURE_2D;
                break;
            case TextureType_2DMultiSample:
                type = GL_TEXTURE_2D_MULTISAMPLE;
                break;
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
