#pragma once

#include "Modules/render/vertex.h"

namespace Engine {
namespace Widgets {

	using Vertex = Compound<
		Render::VertexPos_3D, 
		Render::VertexColor, 
		Render::VertexUV>;

}
}