#pragma once

namespace Engine {
namespace Apis {
    namespace Im3D {

		
        struct Im3DContext {
            std::vector<Render::Vertex> mTriangleVertices;
            std::vector<unsigned int> mTriangleIndices;
        };

    }
}
}