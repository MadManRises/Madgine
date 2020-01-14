#pragma once

#include "Modules/resources/resourceloader.h"

#include "../tinyxml/tinyxml2.h"

namespace Engine {
namespace Tools {

    struct XMLDocument {
        XMLDocument();

        XMLDocument(XMLDocument &&) = default;

        XMLDocument &operator=(XMLDocument &&) = default;

        tinyxml2::XMLDocument *operator->() const { return mPtr.get(); }

        std::unique_ptr<tinyxml2::XMLDocument> mPtr;
    };

    struct LayoutLoader : Resources::ResourceLoader<LayoutLoader, XMLDocument> {
        LayoutLoader();

        bool loadImpl(Data &data, ResourceType *res);
        void unloadImpl(Data &data, ResourceType *res);
    };

}
}