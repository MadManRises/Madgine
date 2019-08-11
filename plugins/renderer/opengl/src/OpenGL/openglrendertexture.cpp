#include "opengllib.h"

#include "openglrendertexture.h"

#include "Modules/math/matrix4.h"
#include "Modules/math/vector4.h"

#include "openglrenderwindow.h"

#include "gui/vertex.h"

#include "openglshaderloader.h"

#include "util/openglshader.h"

#include "Madgine/scene/camera.h"
#include "Madgine/scene/entity/components/transform.h"
#include "Madgine/scene/entity/entity.h"
#include "openglmesh.h"

namespace Engine {
namespace Render {

    OpenGLRenderTexture::OpenGLRenderTexture(OpenGLRenderWindow *window, uint32_t index, Scene::Camera *camera, const Vector2 &size)
        : RenderTarget(window, camera, size)
        , mIndex(index)
    {
        std::shared_ptr<OpenGLShader> vertexShader = OpenGLShaderLoader::load("scene_VS");
        std::shared_ptr<OpenGLShader> pixelShader = OpenGLShaderLoader::load("scene_PS");

        if (!mProgram.link(vertexShader.get(), pixelShader.get()))
            throw 0;

        mProgram.setUniform("lightColor", { 1.0f, 1.0f, 1.0f });
        mProgram.setUniform("lightDir", Vector3{ 0.0f, 0.0f, -1.0f }.normalisedCopy());

        mTexture.setWrapMode(GL_CLAMP_TO_EDGE);
        mTexture.setFilter(GL_NEAREST);

        glGenRenderbuffers(1, &mDepthRenderbuffer);
        glCheck();

        resize(size);

        glGenFramebuffers(1, &mFramebuffer);
        glCheck();
        glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);
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

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    OpenGLRenderTexture::~OpenGLRenderTexture()
    {
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

    void OpenGLRenderTexture::resize(const Vector2 &size)
    {
        GLsizei width = static_cast<GLsizei>(size.x);
        GLsizei height = static_cast<GLsizei>(size.y);

        mTexture.setData(width, height, nullptr);

        glBindRenderbuffer(GL_RENDERBUFFER, mDepthRenderbuffer);
        glCheck();
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
        glCheck();
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        RenderTarget::resize(size);
    }

    void OpenGLRenderTexture::render()
    {

        const Vector2 &size = getSize();

        glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);
        glCheck();
        glViewport(0, 0, static_cast<GLsizei>(size.x), static_cast<GLsizei>(size.y));
        glCheck();

        mProgram.bind();

        glClearColor(0.1f, 0.01f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float aspectRatio = size.x / size.y;

        mProgram.setUniform("vp", camera()->getViewProjectionMatrix(aspectRatio));

        for (Scene::Entity::Entity *e : camera()->visibleEntities()) {

            OpenGLMesh *mesh = e->getComponent<OpenGLMesh>();
            Scene::Entity::Transform *transform = e->getComponent<Scene::Entity::Transform>();
            if (mesh && mesh->isVisible() && transform) {
                OpenGLMeshData *meshData = mesh->data();
                if (meshData) {
                    mProgram.setUniform("m", transform->matrix());
                    mProgram.setUniform(
                        "anti_m",
                        transform->matrix()
                            .ToMat3()
                            .Inverse()
                            .Transpose());

                    glBindVertexArray(meshData->mVAO);
                    glCheck();

                    glDrawArrays(GL_TRIANGLES, 0, meshData->mVertexCount);
                    glCheck();
                }
            }
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}
}
