#pragma once

#include "layoutloader.h"

namespace Engine {
namespace Tools {

    struct MADGINE_TOOLS_EXPORT InspectorLayout {
        InspectorLayout(LayoutLoader::HandleType document);

        tinyxml2::XMLElement *rootElement();

        std::vector<std::string> associations();

    private:
        LayoutLoader::HandleType mDocument;
    };

}
}
