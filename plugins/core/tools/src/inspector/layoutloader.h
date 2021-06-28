#pragma once

#include "Madgine/resources/resourceloader.h"

namespace tinyxml2 {
class XMLDocument;
}

namespace Engine {
namespace Tools {

    struct XMLDocument {
        XMLDocument();
        ~XMLDocument();

        XMLDocument(const XMLDocument &other) = delete;
        XMLDocument(XMLDocument &&other);

        XMLDocument &operator=(XMLDocument &&other);

        tinyxml2::XMLDocument *operator->() const { return mPtr.get(); }

        std::unique_ptr<tinyxml2::XMLDocument> mPtr;
    };

    struct LayoutLoader : Resources::ResourceLoader<LayoutLoader, XMLDocument> {
        LayoutLoader();

        bool loadImpl(Data &data, ResourceDataInfo &info);
        void unloadImpl(Data &data, ResourceDataInfo &info);
    };

}
}

RegisterType(Engine::Tools::LayoutLoader);