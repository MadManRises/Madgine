#include "opengllib.h"
#include "gui/widgets/toplevelwindow.h"
#include "openglrenderwindow.h"
#include "openglrenderer.h"


#include <iostream>

#undef min
#undef max

#include "Interfaces/debug/profiler/profiler.h"

#include "Interfaces/math/vector3.h"
#include "Interfaces/math/vector4.h"

#include "gui/widgets/widget.h"

#include "gui/vertex.h"

#include "openglrendertexture.h"

#include "openglshaderloader.h"

#include "util/openglshader.h"


namespace Engine {
	namespace Render {


		static GLuint vertexbuffer = 0;

		static GLuint vao;

		OpenGLRenderWindow::OpenGLRenderWindow(GUI::TopLevelWindow * w, ContextHandle context) :
			RenderWindow(w),
			mContext(context)
		{
			std::shared_ptr<OpenGLShader> vertexShader = OpenGLShaderLoader::load("ui_VS");
			std::shared_ptr<OpenGLShader> pixelShader = OpenGLShaderLoader::load("ui_PS");

			if (!mProgram.link(vertexShader.get(), pixelShader.get()))
				throw 0;

			mProgram.setUniform("textures[0]", 0);
			mProgram.setUniform("textures[1]", 1);

			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);

			// Generate 1 buffer, put the resulting identifier in vertexbuffer
			glGenBuffers(1, &vertexbuffer);

			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
			glVertexAttribPointer(
				0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				sizeof(GUI::Vertex),                  // stride
				(void*)0            // array buffer offset
			);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(
				1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
				4,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				sizeof(GUI::Vertex),                  // stride
				(void*)12            // array buffer offset
			);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(
				2,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
				2,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				sizeof(GUI::Vertex),                  // stride
				(void*)28            // array buffer offset
			);
			glEnableVertexAttribArray(2);			
			glVertexAttribIPointer(
				3,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
				1,                  // size
				GL_UNSIGNED_INT,           // type
				sizeof(GUI::Vertex),                  // stride
				(void*)36            // array buffer offset
			);			
			glEnableVertexAttribArray(3);			

			mDefaultTexture.setWrapMode(GL_CLAMP_TO_EDGE);			
			GLubyte borderColor[] = { 255, 255, 255, 255 };
			mDefaultTexture.setData(1, 1, borderColor);

			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
			glDepthFunc(GL_LESS);
			//glDepthRange(0.0, 1.0);

		}

		OpenGLRenderWindow::~OpenGLRenderWindow()
		{
			OpenGLRenderer::shutdownWindow(window()->window(), mContext);
		}

		void OpenGLRenderWindow::render()
		{
			PROFILE();

			updateRenderTargets();
			glActiveTexture(GL_TEXTURE0);
			mDefaultTexture.bind();

			Vector3 screenPos, screenSize;
			std::tie(screenPos, screenSize) = window()->getAvailableScreenSpace();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, static_cast<GLsizei>(screenSize.x), static_cast<GLsizei>(screenSize.y));

			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			mProgram.bind();

			
			std::vector<GUI::Vertex> vertices;

			std::queue<std::pair<GUI::Widget*, int>> q;
			for (GUI::Widget *w : window()->widgets()) {
				if (w->isVisible()) 
				{
					q.push(std::make_pair(w, 0));
				}				
			}
			while (!q.empty()) {

				GUI::Widget *w = q.front().first;
				int depth = q.front().second;
				q.pop();
				

				for (GUI::Widget *c : w->children()) {
					if (c->isVisible())
						q.push(std::make_pair(c, depth + 1));
				}


				std::vector<GUI::Vertex> localVertices = w->vertices(screenSize);

				std::move(localVertices.begin(), localVertices.end(), std::back_inserter(vertices));
				
			}			

			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

			glBindVertexArray(vao);

			// Draw the triangle !
			glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size())); // Starting from vertex 0; 3 vertices total -> 1 triangle			
			
		}


		std::unique_ptr<RenderTarget> OpenGLRenderWindow::createRenderTarget(Scene::Camera * camera, const Vector2& size)
		{
			return std::make_unique<OpenGLRenderTexture>(this, ++mTextureCount, camera, size);
		}

	}
}