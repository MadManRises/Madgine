
#include "Interfaces/math/vector4.h"
#include "util/openglbuffer.h"

namespace Engine
{
	namespace Render {

		struct Vertex {
			Vector3 mPos;
			Vector4 mColor;
			Vector3 mNormal;
		};

		struct MADGINE_OPENGL_EXPORT OpenGLMeshData
		{
			OpenGLBuffer mVertices;
			size_t mVertexCount;
			GLuint mVAO;
		};

	}
}