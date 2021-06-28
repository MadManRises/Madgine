#pragma once

#include "toolbase.h"
#include "toolscollector.h"

#include "Madgine/nodegraph/nodegraphloader.h"
#include "Madgine/nodegraph/nodeinterpreter.h"

#include "Madgine/render/renderpass.h"

#include "Meta/keyvalue/valuetype.h"

#include "Madgine/render/camera.h"

#include "imgui/imguiaddons.h"

namespace Engine {
namespace Client {

    struct MADGINE_CLIENT_NODES_TOOLS_EXPORT NodeRendererTester : Tools::Tool<NodeRendererTester> {
        NodeRendererTester(Tools::ImRoot &root);
        NodeRendererTester(const NodeRendererTester &) = delete;

        virtual bool init() override;
        virtual void finalize() override;

        virtual void render() override;
        virtual void renderMenu() override;

        virtual std::string_view key() const override;

        struct NodeRenderPass : Render::RenderPass {

            virtual void render(Render::RenderTarget *target) override;

            virtual int priority() const override;

            NodeGraph::NodeGraphLoader::HandleType mHandle;

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

RegisterType(Engine::Client::NodeRendererTester);