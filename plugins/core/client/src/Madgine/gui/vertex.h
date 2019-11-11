#pragma once

#include "Modules/render/vertex.h"

namespace Engine {
namespace GUI {

	using Vertex = Compound<
		Render::VertexPos_3D, 
		Render::VertexColor, 
		Render::VertexUV>;

}
}