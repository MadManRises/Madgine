#include "../../clientnodestoolslib.h"

#include "noderenderertester.h"

#include "imgui/imgui.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Madgine_Tools/imgui/clientimroot.h"

#include "Madgine/window/mainwindow.h"

#include "Madgine/render/rendercontext.h"
#include "Madgine/render/rendertarget.h"

#include "inspector/inspector.h"

#include "Meta/keyvalue/valuetype.h"

#include "Madgine_Tools/interactivecamera.h"

UNIQUECOMPONENT(Engine::Tools::NodeRendererTester);

METATABLE_BEGIN_BASE(Engine::Tools::NodeRendererTester, Engine::Tools::ToolBase)
METATABLE_END(Engine::Tools::NodeRendererTester)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Tools::NodeRendererTester, Engine::Tools::ToolBase)
SERIALIZETABLE_END(Engine::Tools::NodeRendererTester)

namespace Engine {
namespace Tools {

    NodeRendererTester::NodeRendererTester(Tools::ImRoot &root)
        : Tool<NodeRendererTester>(root)
    {
    }

    NodeRendererTester::~NodeRendererTester() = default;

    Threading::Task<bool> NodeRendererTester::init()
    {
        if (!co_await ToolBase::init())
            co_return false;

        mTexture = static_cast<const ClientImRoot &>(mRoot).window().getRenderer()->createRenderTexture({ 512, 512 });

        static_cast<ClientImRoot &>(mRoot).addRenderTarget(mTexture.get());

        mInspector = &getTool<Inspector>();

        co_return true;
    }

    Threading::Task<void> NodeRendererTester::finalize()
    {
        static_cast<ClientImRoot &>(mRoot).removeRenderTarget(mTexture.get());

        mTexture.reset();

        co_await ToolBase::finalize();
    }

    void NodeRendererTester::render()
    {
        if (ImGui::Begin("NodeRendererTester")) {
            //mTexture->render();

            ImGui::ImageButton((void *)mTexture->texture().mTextureHandle, { 512, 512 }, { 0, 0 }, { 1, 1 }, 0);
            ImGui::InteractiveView(mState);

            Tools::InteractiveCamera(mState, mCamera);

            ImGui::Text("Passes:");
            ImGui::SameLine();
            if (ImGui::Button("+")) {
                mTexture->addRenderPass(&mPasses.emplace_back());
            }
            if (ImGui::BeginTable("columns", 2, ImGuiTableFlags_Resizable)) {
                for (NodeRenderPass &pass : mPasses) {
                    TypedScopePtr ptr = pass.mHandle.resource();
                    if (mInspector->drawValue("pass", ptr, true).first) {
                        pass.mHandle = ptr.safe_cast<NodeGraph::NodeGraphLoader::Resource>();
                        size_t argCount = pass.mHandle->mDataProviderPins.size();
                        assert(argCount >= 1);
                        pass.mArguments.resize(argCount - 1);
                        for (uint32_t i = 1; i < argCount; ++i) {
                            ExtendedValueTypeDesc desc = pass.mHandle->dataProviderType({ 0, i });
                            if (desc == toValueTypeDesc<Engine::Vector4>()) {
                                pass.mArguments[i - 1] = Engine::Vector4 { Engine::Vector4::ZERO };
                            } else if (desc != Engine::toValueTypeDesc<Render::Camera *>()) {
                                pass.mArguments[i - 1].setType(desc);
                            } else {
                                pass.mArguments[i - 1] = &mCamera;
                            }
                        }
                    }
                    size_t i = 0;
                    for (ValueType &v : pass.mArguments) {
                        mInspector->drawValue(std::to_string(i++), v, true, false);
                    }
                }
                ImGui::EndTable();
            }
        }
        ImGui::End();
    }

    void NodeRendererTester::renderMenu()
    {
        ToolBase::renderMenu();
        if (mVisible) {
            if (ImGui::BeginMenu("NodeRendererTester")) {
                ImGui::EndMenu();
            }
        }
    }

    std::string_view NodeRendererTester::key() const
    {
        return "NodeRendererTester";
    }

    void NodeRendererTester::NodeRenderPass::render(Render::RenderTarget *target, size_t iteration)
    {
        if (mHandle) {
            mInterpreter.setGraph(&*mHandle);

            ArgumentList args { mArguments.size() + 1 };
            args[0] = ValueType { target };

            for (size_t i = 0; i < mArguments.size(); ++i) {
                args[i + 1] = mArguments[i].visit(overloaded {
                    [](const auto &v) {
                        return ValueType { v };
                    },
                    [](const OwnedScopePtr &p) { return ValueType {
                                                     p.get()
                                                 }; } });
            }

            IndexType<uint32_t> flow = 0;
            mInterpreter.interpret(flow, std::move(args));
        }
    }

    int NodeRendererTester::NodeRenderPass::priority() const
    {
        return 50;
    }

}
}
