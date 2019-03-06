#pragma once


namespace Engine {
	namespace GUI {

		struct Vertex {
			Vector3 mPos;
			Vector4 mColor;
			Vector2 mUV = {};
			uint32_t mTextureIndex = 0;
		};

	}
}