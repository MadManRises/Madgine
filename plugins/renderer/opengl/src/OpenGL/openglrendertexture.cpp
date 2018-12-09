#include "opengllib.h"

#include "openglrendertexture.h"


#include "glad.h"

#include "Interfaces/math/vector4.h"

#include "openglrenderwindow.h"

#include "Madgine/gui/vertex.h"

#include <iostream>

#include "Madgine/resources/resourcemanager.h"
#include "openglshaderloader.h"

#include "openglshader.h"

#include "Interfaces/math/matrix4.h"

#include "Madgine/render/camera.h"

namespace Engine {
	namespace Render {

		static unsigned int vao;


		struct Vertex {
			Vector3 mPos;
			Vector4 mColor;
			Vector3 mNormal;
		};

		OpenGLRenderTexture::OpenGLRenderTexture(OpenGLRenderWindow * window, uint32_t index, Camera *camera, const Vector2 & size) :
			RenderTarget(window, camera, size),
			mIndex(index)
		{
			std::shared_ptr<OpenGLShader> vertexShader = Resources::ResourceManager::getSingleton().load<OpenGLShaderLoader>("scene_VS");
			std::shared_ptr<OpenGLShader> pixelShader = Resources::ResourceManager::getSingleton().load<OpenGLShaderLoader>("scene_PS");
			
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
			
			
			auto check = []() {int e = glGetError(); if (e) exit(e); };			
						
			mTexture.setWrapMode(GL_CLAMP_TO_EDGE);
			mTexture.setFilter(GL_NEAREST);

			glGenRenderbuffers(1, &mDepthRenderbuffer);

			resize(size);
			
			glGenFramebuffers(1, &mFramebuffer);
			check();			
			glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);
			check();

			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthRenderbuffer);			
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mTexture.handle(), 0);
			check();			

			GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
			glDrawBuffers(1, DrawBuffers);
			check();

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				throw 0;
			
		}

		OpenGLRenderTexture::~OpenGLRenderTexture()
		{
			glDeleteRenderbuffers(1, &mDepthRenderbuffer);
		}

		uint32_t OpenGLRenderTexture::textureId() const
		{
			glActiveTexture(GL_TEXTURE0 + mIndex);
			mTexture.bind();
			return mIndex;
		}

		void OpenGLRenderTexture::resize(const Vector2 & size)
		{
			size_t width = size_t(size.x);
			size_t height = size_t(size.y);

			mTexture.setData(width, height, nullptr);

			glBindRenderbuffer(GL_RENDERBUFFER, mDepthRenderbuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size.x, size.y);

			RenderTarget::resize(size);
		}

		void OpenGLRenderTexture::render()
		{
			auto check = []() {int e = glGetError(); if (e) exit(e); };

			const Vector2 &size = getSize();

			glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);
			check();
			glViewport(0, 0, size.x, size.y);			
			check();

			mProgram.bind();

			glClearColor(0.1f, 0.01f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			float aspectRatio = float(size.x) / float(size.y);

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
			check();

			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * 12, vertices, GL_STATIC_DRAW);
			check();

			glBindVertexArray(vao);
			check();

			// Draw the triangle !
			glDrawArrays(GL_TRIANGLES, 0, 12); // Starting from vertex 0; 3 vertices total -> 1 triangle		
			check();	
		}

	}
}
