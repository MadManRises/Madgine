#pragma once

#include "Madgine_Tools/toolbase.h"
#include "Madgine_Tools/toolscollector.h"

#include "Madgine/nodegraph/nodegraphloader.h"
#include "Madgine/nodegraph/nodeinterpreter.h"

#include "Madgine/render/renderpass.h"

#include "Madgine/render/camera.h"

#include "imgui/imguiaddons.h"

namespace Engine {
namespace Tools {

    struct MADGINE_CLIENT_NODES_TOOLS_EXPORT NodeRendererTester : Tools::Tool<NodeRendererTester> {
        NodeRendererTester(Tools::ImRoot &root);
        NodeRendererTester(const NodeRendererTester &) = delete;
        ~NodeRendererTester();

        virtual Threading::Task < bool> init() override;
        virtual Threading::Task < void> finalize() override;

        virtual void render() override;
        virtual void renderMenu() override;

        virtual std::string_view key() const override;

        struct NodeRenderPass : Render::RenderPass {

            virtual void render(Render::RenderTarget *target, size_t iteration) override;

            virtual int priority() const override;

            NodeGraph::NodeGraphLoader::Handle mHandle;

            NodeGraph::NodeInterpreter mInterpreter;

            ArgumentList mArguments;
        };

    private:
        std::unique_ptr<Render::RenderTarget> mTexture;

        Render::Camera mCamera;

        std::vector<NodeRenderPass> mPasses;

        Tools::Inspector *mInspector;

        ImGui::InteractiveViewState mState;
    };
}
}

REGISTER_TYPE(Engine::Tools::NodeRendererTester)