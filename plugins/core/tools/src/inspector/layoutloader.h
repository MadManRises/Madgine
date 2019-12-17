#pragma once

#include "Modules/resources/resourceloader.h"

namespace Engine {
namespace Tools {

    class LayoutLoader : public Resources::ResourceLoader<LayoutLoader, tinyxml2::XMLDocument> {

    public:
        LayoutLoader();

    private:
        virtual bool loadImpl(Data &data, ResourceType *res) override;
        virtual void unloadImpl(Data &data) override;
    };

}
}