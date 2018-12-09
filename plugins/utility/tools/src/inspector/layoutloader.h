#pragma once

#include "Madgine/resources/resourceloader.h"

namespace Engine {
	namespace Tools {

		class LayoutLoader : public Resources::ResourceLoader<LayoutLoader, tinyxml2::XMLDocument> {
			
		public:
			LayoutLoader(Resources::ResourceManager &mgr);

		private:
			virtual std::shared_ptr<Data> load(ResourceType * res) override;
		};

	}
}

RegisterClass(Engine::Tools::LayoutLoader);