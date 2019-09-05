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

namespace Engine {
namespace Render {

    OpenGLRenderTexture::OpenGLRenderTexture(OpenGLRenderWindow *window, uint32_t index, Scene::Camera *camera, const Vector2 &size)
        : RenderTarget(window, camera, { 0, 0 })
        , mIndex(index)
    {
        std::shared_ptr<OpenGLShader> vertexShader = OpenGLShaderLoader::load("scene_VS");
        std::shared_ptr<OpenGLShader> pixelShader = OpenGLShaderLoader::load("scene_PS");

        if (!mProgram.link(vertexShader.get(), pixelShader.get()))
            throw 0;

        mProgram.setUniform("lightColor", { 1.0f, 1.0f, 1.0f });
        mProgram.setUniform("lightDir", Vector3 { 0.0f, 0.0f, -1.0f }.normalisedCopy());

        mTexture.setWrapMode(GL_CLAMP_TO_EDGE);
        mTexture.setFilter(GL_NEAREST);

        glGenRenderbuffers(1, &mDepthRenderbuffer);
        glCheck();

        glGenFramebuffers(1, &mFramebuffer);
        glCheck();

        resize(size);

        glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);
        glCheck();
        glBindRenderbuffer(GL_RENDERBUFFER, mDepthRenderbuffer);
        glCheck();


		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthRenderbuffer);
        glCheck();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTexture.handle(), 0);
        //glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mTexture.handle(), 0);
        glCheck();


        GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers(1, DrawBuffers);
        glCheck();

        if (GLenum check = glCheckFramebufferStatus(GL_FRAMEBUFFER); check != GL_FRAMEBUFFER_COMPLETE)
            throw 0;

        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glCheck();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glCheck();
    }

    OpenGLRenderTexture::~OpenGLRenderTexture()
    {
        glDeleteFramebuffers(1, &mFramebuffer);
        glCheck();
        glDeleteRenderbuffers(1, &mDepthRenderbuffer);
        glCheck();
    }

    uint32_t OpenGLRenderTexture::textureId() const
    {
        glActiveTexture(GL_TEXTURE0 + mIndex);
        glCheck();
        mTexture.bind();
        return mIndex;
    }

    bool OpenGLRenderTexture::resize(const Vector2 &size)
    {
        if (!RenderTarget::resize(size))
            return false;
		
		GLsizei width = static_cast<GLsizei>(size.x);
        GLsizei height = static_cast<GLsizei>(size.y);

        assert(width > 0 && height > 0);

        mTexture.setData(width, height, nullptr);

        glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);
        glCheck();
        glBindRenderbuffer(GL_RENDERBUFFER, mDepthRenderbuffer);
        glCheck();

        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
        glCheck();


        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glCheck();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glCheck();

		return true;
    }

    void OpenGLRenderTexture::render()
    {

        setupProgram();

        for (RenderPass *pass : uniquePtrToPtr(preRenderPasses())) {
            pass->render(this, camera());
            mProgram.bind();
        }

        for (Scene::Entity::Entity *e : camera()->visibleEntities()) {

            OpenGLMesh *mesh = e->getComponent<OpenGLMesh>();
            Scene::Entity::Transform *transform = e->getComponent<Scene::Entity::Transform>();
            if (mesh && mesh->isVisible() && transform) {
                OpenGLMeshData *meshData = mesh->data();
                if (meshData) {
                    renderMesh(meshData, transform->matrix());
                }
            }
        }

        for (RenderPass *pass : uniquePtrToPtr(postRenderPasses())) {
            pass->render(this, camera());
            mProgram.bind();
        }

        resetProgram();
    }

    const OpenGLTexture &OpenGLRenderTexture::texture() const
    {
        return mTexture;
    }

    void OpenGLRenderTexture::setupProgram()
    {
        const Vector2 &size = getSize();

        glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);
        glCheck();
        glViewport(0, 0, static_cast<GLsizei>(size.x), static_cast<GLsizei>(size.y));
        glCheck();

        mProgram.bind();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glCheck();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glCheck();

        float aspectRatio = size.x / size.y;

        mProgram.setUniform("vp", camera()->getViewProjectionMatrix(aspectRatio));
    }

    void OpenGLRenderTexture::resetProgram()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

		constexpr GLenum modes[]
        {
            GL_POINTS,
                GL_LINES,
                GL_TRIANGLES
        };

		GLenum mode = modes[mesh->mGroupSize - 1];

        if (mesh->mIndices) {
            glDrawElements(mode, mesh->mElementCount, GL_UNSIGNED_INT, 0);
        } else
            glDrawArrays(mode, 0, mesh->mElementCount);
        glCheck();
    }

    void OpenGLRenderTexture::renderInstancedMesh(void *meshData, const std::vector<Matrix4> &transforms)
    {
        for (const Matrix4 &transform : transforms) {
            renderMesh(static_cast<OpenGLMeshData *>(meshData), transform);
		}
    }

    void OpenGLRenderTexture::renderVertices(size_t groupSize, Vertex *vertices, size_t vertexCount, unsigned int *indices, size_t indexCount)
    {
        OpenGLMeshData tempMesh = OpenGLMeshLoader::generate(groupSize, vertices, vertexCount, indices, indexCount);

        //setupProgram();

        renderMesh(&tempMesh);

        //resetProgram();
    }

    void OpenGLRenderTexture::clearDepthBuffer()
    {
        glClear(GL_DEPTH_BUFFER_BIT);
        glCheck();
    }
}
}
