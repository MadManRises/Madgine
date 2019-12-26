#pragma once

#include "Modules/resources/resourceloader.h"

namespace Engine {
namespace Tools {

	struct XMLDocument {
        XMLDocument();

		XMLDocument(XMLDocument &&) = default;

		XMLDocument &operator=(XMLDocument &&) = default;

		tinyxml2::XMLDocument *operator->() const { return mPtr.get(); }

        std::unique_ptr<tinyxml2::XMLDocument> mPtr;
	};

    class LayoutLoader : public Resources::ResourceLoader<LayoutLoader, XMLDocument> {

    public:
        LayoutLoader();

        bool loadImpl(Data &data, ResourceType *res);
        void unloadImpl(Data &data, ResourceType *res);
    };

}
}