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

#include "Madgine_Tools/inspector/inspector.h"

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
        if (ImGui::Begin("NodeRendererTester", &mVisible)) {

            ImGui::ImageButton((void *)mTexture->texture(), { 512, 512 }, { 0, 0 }, { 1, 1 }, 0);
            //ImGui::InteractiveView(mState);

            //Tools::InteractiveCamera(mState, mCamera);

            ImGui::Text("Passes:");
            ImGui::SameLine();
            if (ImGui::Button("+")) {
                mPasses.emplace_back();
            }
            if (ImGui::BeginTable("columns", 2, ImGuiTableFlags_Resizable)) {
                for (NodeRenderPass &pass : mPasses) {
                    /* if (pass.mLoadingHandle && pass.mLoadingHandle.available()) {

                        if (pass.mPass)
                            mTexture->removeRenderPass(pass.mPass);

                        pass.mHandle = std::move(pass.mLoadingHandle);
                        pass.mLoadingHandle.reset();

                        pass.mInterpreter.setGraph(&*pass.mHandle);

                        size_t argCount = pass.mHandle->mDataProviderPins.size();
                        pass.mInterpreter.arguments().resize(argCount);
                        for (uint32_t i = 0; i < argCount; ++i) {
                            ExtendedValueTypeDesc desc = pass.mHandle->dataProviderType({ 0, i });
                            if (static_cast<ValueTypeDesc>(desc) != pass.mInterpreter.arguments()[i].type()) {
                                if (desc == toValueTypeDesc<Engine::Vector4>()) {
                                    pass.mInterpreter.arguments()[i] = Engine::Vector4 { Engine::Vector4::ZERO };
                                } else {
                                    pass.mInterpreter.arguments()[i].setType(desc);
                                }
                            }
                        }

                        ValueType retVal;
                        pass.mInterpreter.read(retVal, 0);
                        pass.mPass = retVal.as<Render::RenderPass *>();

                        if (pass.mPass)
                            mTexture->addRenderPass(pass.mPass);
                    }

                    TypedScopePtr ptr = pass.mHandle.resource();
                    if (mInspector->drawValue("pass", ptr, true).first) {
                        pass.mLoadingHandle = ptr.safe_cast<NodeGraph::NodeGraphLoader::Resource>();
                    }
                    size_t i = 0;
                    for (ValueType &v : pass.mInterpreter.arguments()) {
                        mInspector->drawValue(std::to_string(i++), v, true, false);
                    }
                    if (NodeGraph::NodeGraphLoader::Handle newHandle = pass.mHandle.refresh()) {
                        pass.mLoadingHandle = newHandle;
                    }*/
                    throw 0;
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

}
}
