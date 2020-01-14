#pragma once

#include "../toolscollector.h"

namespace Engine {
namespace Tools {

    struct ImGuiDemo : Tool<ImGuiDemo> {
        ImGuiDemo(ImRoot &root);

        void render() override;

        const char *key() const override;
    };

}
}
