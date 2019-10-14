#include "opengllib.h"

#include "openglrendertexture.h"

#include "Modules/math/matrix4.h"
#include "Modules/math/vector4.h"

#include "openglrenderwindow.h"

#include "Madgine/gui/vertex.h"

#include "openglshaderloader.h"

#include "util/openglshader.h"

#include "Madgine/scene/camera.h"
#include "Madgine/scene/entity/components/transform.h"
#include "Madgine/scene/entity/entity.h"
#include "openglmesh.h"

#include "Madgine/render/renderpass.h"

#include "openglfontdata.h"
#include "openglfontloader.h"

#include "Madgine/app/application.h"
#include "Madgine/scene/scenemanager.h"

namespace Engine {
namespace Render {

    OpenGLRenderTexture::OpenGLRenderTexture(OpenGLRenderWindow *window, Scene::Camera *camera, const Vector2 &size)
        : RenderTarget(window, camera, { 0, 0 })
    {
        std::shared_ptr<OpenGLShader> vertexShader = OpenGLShaderLoader::load("scene_VS");
        std::shared_ptr<OpenGLShader> pixelShader = OpenGLShaderLoader::load("scene_PS");

        if (!mProgram.link(vertexShader.get(), pixelShader.get()))
            throw 0;

        mProgram.setUniform("lightColor", { 1.0f, 1.0f, 1.0f });
        mProgram.setUniform("lightDir", Vector3 { 0.1f, 0.1f, 1.0f }.normalizedCopy());

        mTexture.setWrapMode(GL_CLAMP_TO_EDGE);
        mTexture.setFilter(GL_NEAREST);

        glGenRenderbuffers(1, &mDepthRenderbuffer);
        GL_CHECK();

        glGenFramebuffers(1, &mFramebuffer);
        GL_CHECK();

        resize(size);

        glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);
        GL_CHECK();
        glBindRenderbuffer(GL_RENDERBUFFER, mDepthRenderbuffer);
        GL_CHECK();

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthRenderbuffer);
        GL_CHECK();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTexture.handle(), 0);
        //glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mTexture.handle(), 0);
        GL_CHECK();

#if !OPENGL_ES
        GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers(1, DrawBuffers);
        GL_CHECK();
#endif

        if (GLenum check = glCheckFramebufferStatus(GL_FRAMEBUFFER); check != GL_FRAMEBUFFER_COMPLETE)
            throw 0;

        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        GL_CHECK();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        GL_CHECK();
    }

    OpenGLRenderTexture::~OpenGLRenderTexture()
    {
        glDeleteFramebuffers(1, &mFramebuffer);
        GL_CHECK();
        glDeleteRenderbuffers(1, &mDepthRenderbuffer);
        GL_CHECK();
    }

    uint32_t OpenGLRenderTexture::textureId() const
    {
        return mTexture.handle();
	}

    bool OpenGLRenderTexture::resize(const Vector2 &size)
    {
        if (!RenderTarget::resize(size))
            return false;

        GLsizei width = static_cast<GLsizei>(size.x);
        GLsizei height = static_cast<GLsizei>(size.y);

        assert(width > 0 && height > 0);

        mTexture.setData({ width, height }, nullptr);

        glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);
        GL_CHECK();
        glBindRenderbuffer(GL_RENDERBUFFER, mDepthRenderbuffer);
        GL_CHECK();

        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
        GL_CHECK();

        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        GL_CHECK();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        GL_CHECK();

        return true;
    }

    void OpenGLRenderTexture::render()
    {
        const Vector2 &size = getSize();

        glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);
        GL_CHECK();
        glViewport(0, 0, static_cast<GLsizei>(size.x), static_cast<GLsizei>(size.y));
        GL_CHECK();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        GL_CHECK();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        GL_CHECK();

        float aspectRatio = size.x / size.y;

        mProgram.setUniform("v", camera()->getViewMatrix());
        mProgram.setUniform("p", camera()->getProjectionMatrix(aspectRatio));

        for (RenderPass *pass : uniquePtrToPtr(preRenderPasses())) {
            pass->render(this, camera());
        }

		//TODO Culling
        for (Scene::Entity::Entity &e : App::Application::getSingleton().getGlobalAPIComponent<Scene::SceneManager>().entities()) {

            OpenGLMesh *mesh = e.getComponent<OpenGLMesh>();
            Scene::Entity::Transform *transform = e.getComponent<Scene::Entity::Transform>();
            if (mesh && mesh->isVisible() && transform) {
                OpenGLMeshData *meshData = mesh->data();
                if (meshData) {
                    setupProgram(0, meshData->mTexture.handle());
                    renderMesh(meshData, transform->matrix());
                }
            }
        }

        for (RenderPass *pass : uniquePtrToPtr(postRenderPasses())) {
            pass->render(this, camera());
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    const OpenGLTexture &OpenGLRenderTexture::texture() const
    {
        return mTexture;
    }

    void OpenGLRenderTexture::setupProgram(RenderPassFlags flags, unsigned int textureId)
    {

        mProgram.bind();

        mProgram.setUniform("hasLight", !(flags & RenderPassFlags_NoLighting));

        mProgram.setUniform("hasDistanceField", !!(flags & RenderPassFlags_DistanceField));

        mProgram.setUniform("hasTexture", textureId != 0);
        if (textureId) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textureId);
            GL_CHECK();
        }
    }

    void OpenGLRenderTexture::renderMesh(OpenGLMeshData *mesh, const Matrix4 &transformMatrix)
    {
        mProgram.setUniform("m", transformMatrix);
        mProgram.setUniform(
            "anti_m",
            transformMatrix
                .ToMat3()
                .Inverse()
                .Transpose());

        mesh->mVAO.bind();

        constexpr GLenum modes[] {
            GL_POINTS,
            GL_LINES,
            GL_TRIANGLES
        };

        GLenum mode = modes[mesh->mGroupSize - 1];

        if (mesh->mIndices) {
            glDrawElements(mode, mesh->mElementCount, GL_UNSIGNED_INT, 0);
        } else
            glDrawArrays(mode, 0, mesh->mElementCount);
        GL_CHECK();
    }

    void OpenGLRenderTexture::renderInstancedMesh(RenderPassFlags flags, void *meshData, const std::vector<Matrix4> &transforms)
    {
        setupProgram(flags);

        for (const Matrix4 &transform : transforms) {
            renderMesh(static_cast<OpenGLMeshData *>(meshData), transform);
        }
    }

    void OpenGLRenderTexture::renderVertices(RenderPassFlags flags, size_t groupSize, Vertex *vertices, size_t vertexCount, unsigned int *indices, size_t indexCount)
    {
        if ((!indices && vertexCount > 0) || indexCount > 0) {
            OpenGLMeshData tempMesh = OpenGLMeshLoader::generate(groupSize, vertices, vertexCount, indices, indexCount);

            setupProgram(flags);

            renderMesh(&tempMesh);
        }
    }

    void OpenGLRenderTexture::renderVertices(RenderPassFlags flags, size_t groupSize, Vertex2 *vertices, size_t vertexCount, unsigned int *indices, size_t indexCount, unsigned int textureId)
    {
        if ((!indices && vertexCount > 0) || indexCount > 0) {
            OpenGLMeshData tempMesh = OpenGLMeshLoader::generate(groupSize, vertices, vertexCount, indices, indexCount);

            setupProgram(flags, textureId);

            renderMesh(&tempMesh);
        }
    }

    void OpenGLRenderTexture::clearDepthBuffer()
    {
        glClear(GL_DEPTH_BUFFER_BIT);
        GL_CHECK();
    }
}
}
