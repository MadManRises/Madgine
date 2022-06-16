#pragma once

#include "../toolbase.h"
#include "../toolscollector.h"

#include "Interfaces/filesystem/path.h"

namespace Engine {
namespace Tools {

    struct Filesystem : Tool<Filesystem> {
        SERIALIZABLEUNIT(Filesystem);

        Filesystem(ImRoot &root);

        virtual void render() override;

        std::string_view key() const override;

    private:
        Engine::Filesystem::Path mCurrentPath = ".";
        Engine::Filesystem::Path mSelectedPath = ".";
    };

}
}

REGISTER_TYPE(Engine::Tools::Filesystem)
