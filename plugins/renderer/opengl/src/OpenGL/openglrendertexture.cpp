#include "opengllib.h"

#include "openglrendertexture.h"


#include "Interfaces/math/vector4.h"

#include "openglrenderwindow.h"

#include "client/gui/vertex.h"

#include "openglshaderloader.h"

#include "openglshader.h"

#include "Interfaces/math/matrix4.h"

#include "client/render/camera.h"

namespace Engine {
	namespace Render {

		static GLuint vao;


		struct Vertex {
			Vector3 mPos;
			Vector4 mColor;
			Vector3 mNormal;
		};

		OpenGLRenderTexture::OpenGLRenderTexture(OpenGLRenderWindow * window, uint32_t index, Camera *camera, const Vector2 & size) :
			RenderTarget(window, camera, size),
			mIndex(index),
			mWindow(window)
		{
			std::shared_ptr<OpenGLShader> vertexShader = OpenGLShaderLoader::load("scene_VS");
			std::shared_ptr<OpenGLShader> pixelShader = OpenGLShaderLoader::load("scene_PS");
			
			if (!mProgram.link(vertexShader.get(), pixelShader.get()))
				throw 0;

			mProgram.setUniform("lightColor", { 1.0f,0.9f,0.8f });
			mProgram.setUniform("lightDir", Vector3{ -1.0f,-0.5f,1.0f }.normalisedCopy());

			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);

			// Generate 1 buffer, put the resulting identifier in vertexbuffer
			glGenBuffers(1, &mVertexbuffer);

			glBindBuffer(GL_ARRAY_BUFFER, mVertexbuffer);
			glVertexAttribPointer(
				0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				sizeof(Vertex),                  // stride
				(void*)0            // array buffer offset
			);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(
				1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
				4,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				sizeof(Vertex),                  // stride
				(void*)12            // array buffer offset
			);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(
				2,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				sizeof(Vertex),                  // stride
				(void*)28            // array buffer offset
			);
			glEnableVertexAttribArray(2);
			
						
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

		void OpenGLRenderTexture::resize(const Vector2 & size)
		{
			GLsizei width = static_cast<GLsizei>(size.x);
			GLsizei height = static_cast<GLsizei>(size.y);

			mTexture.setData(width, height, nullptr);

			glBindRenderbuffer(GL_RENDERBUFFER, mDepthRenderbuffer);
			glCheck();
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
			glCheck();

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

			static Quaternion test;
			
			test = Quaternion(0.001f, Vector3::UNIT_Y) * test * Quaternion(0.0001f, Vector3::UNIT_X);

			mProgram.setUniform("vp", camera()->getViewProjectionMatrix(aspectRatio));
			mProgram.setUniform("m", Matrix4{ test.toMatrix() });

			Vertex vertices[] = {
				{{-0.5f, 0.0f, -0.5f}, {0.25,0.5,0.75,1.0}, {0.0f, -1.0f, 0.0f}},
				{ {0.5f, 0.0f, -0.5f}, {0.25,0.5,0.75,1.0}, {0.0f, -1.0f, 0.0f}},
				{ {0.0f, 0.0f, 0.25f}, {0.25,0.5,0.75,1.0}, {0.0f, -1.0f, 0.0f}},
				{{-0.5f, 0.0f, -0.5f}, {0.5,0.5,0.75,1.0}, {0.0f, 0.3f, -0.5f}},
				{ {0.5f, 0.0f, -0.5f}, {0.5,0.5,0.75,1.0}, {0.0f, 0.3f, -0.5f}},
				{ {0.0f, 0.75f, 0.0f}, {0.5,0.5,0.75,1.0}, {0.0f, 0.3f, -0.5f}},
				{{-0.5f, 0.0f, -0.5f}, {0.25,0,0.75,1.0}, {-0.25f, 0.25f, 0.25f}},
				{ {0.0f, 0.0f, 0.25f}, {0.25,0,0.75,1.0}, {-0.25f, 0.25f, 0.25f}},
				{ {0.0f, 0.75f, 0.0f}, {0.25,0,0.75,1.0}, {-0.25f, 0.25f, 0.25f}},
				{{0.5f, 0.0f, -0.5f}, {0.5,0,0.75,1.0}, {0.25f, 0.25f, 0.25f}},
				{ {0.0f, 0.0f, 0.25f}, {0.5,0,0.75,1.0}, {0.25f, 0.25f, 0.25f}},
				{ {0.0f, 0.75f, 0.0f}, {0.5,0,0.75,1.0}, {0.25f, 0.25f, 0.25f}},
			};

			glBindBuffer(GL_ARRAY_BUFFER, mVertexbuffer);
			glCheck();

			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * 12, vertices, GL_STATIC_DRAW);
			glCheck();

			glBindVertexArray(vao);
			glCheck();

			// Draw the triangle !
			glDrawArrays(GL_TRIANGLES, 0, 12); // Starting from vertex 0; 3 vertices total -> 1 triangle		
			glCheck();
		}

	}
}
