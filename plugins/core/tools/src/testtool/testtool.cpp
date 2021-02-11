#include "../toolslib.h"

#include "testtool.h"

#include "imgui/imgui.h"

#include "imgui/imguiaddons.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Interfaces/window/windowapi.h"

#include "Meta/math/atlas2.h"

#include "im3d/im3d.h"

#include "render/vertex.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

UNIQUECOMPONENT(Engine::Tools::TestTool);

METATABLE_BEGIN_BASE(Engine::Tools::TestTool, Engine::Tools::ToolBase)
FUNCTION(logTest)
FUNCTION(logValue, value)
FUNCTION(dummy)
METATABLE_END(Engine::Tools::TestTool)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Tools::TestTool, Engine::Tools::ToolBase)
SERIALIZETABLE_END(Engine::Tools::TestTool)

namespace Engine {
namespace Tools {

    TestTool::TestTool(ImRoot &root)
        : Tool<TestTool>(root)
    {
    }

    template <size_t... Is>
    void renderValuetypeSizes(std::index_sequence<Is...>)
    {
        std::vector<std::pair<const char *, unsigned int>> data = {
            { typeid(std::get<Is>(std::declval<ValueType::Union>())).name(), sizeof(std::get<Is>(std::declval<ValueType::Union>())) }...
        };
        for (auto [name, size] : data) {
            ImGui::Text("  %s size: %u", name, size);
        }
    }

    template <typename T, typename... Bases>
    void renderVTableLayout(T& t) {
        if (ImGui::TreeNode(typeid(T).name())) {
            ImGui::Text("size: %lu", static_cast<unsigned long>(sizeof(T)));
            uintptr_t *vtable = *reinterpret_cast<uintptr_t**>(&t);
            if (ImGui::TreeNode("vtbl", "vtable: %p", vtable)) {
                ImGui::TreePop();
            }
            (renderVTableLayout<Bases>(t), ...);
            ImGui::TreePop();
        }
    }

    void TestTool::render()
    {

        if (ImGui::Begin("TestTool", &mVisible)) {
            /*if (ImGui::Button("Create Tool Window")) {
                Window::WindowSettings settings;
                static_cast<const ClientImRoot&>(*mRoot.parent()).window().createToolWindow(settings);
            }*/

            //Resources::ImageLoader::load("Grass-01", true);

            Im3D::Text("Test_1234 Hallo\n wie gehts", Matrix4 { Matrix4::IDENTITY });

            static std::vector<Render::Vertex> vertices;
            static std::vector<unsigned short> indices;
            if (ImGui::Button("Update Atlas")) {
                vertices.clear();
                indices.clear();

                constexpr size_t COUNT = 80;
                Vector2i array[COUNT];
                for (int i = 0; i < COUNT; ++i) {
                    array[i] = { rand() % 128,
                        rand() % 128 };
                }
                Vector2i origin { 0, 0 };
                Atlas2 atlas { { 512, 512 } };

                std::vector<Atlas2::Entry> entries = atlas.insert({ array, array + COUNT }, [&]() { atlas.addBin(origin); origin.x += 512; });

                for (const Atlas2::Entry &entry : entries) {
                    vertices.push_back({ { float(entry.mArea.mTopLeft.x), float(entry.mArea.mTopLeft.y), 0 }, { 1, 1, 1, 1 }, { 0, 0, 1 } });
                    vertices.push_back({ { float(entry.mArea.mTopLeft.x + entry.mArea.mSize.x), float(entry.mArea.mTopLeft.y), 0 }, { 1, 1, 1, 1 }, { 0, 0, 1 } });
                    vertices.push_back({ { float(entry.mArea.mTopLeft.x), float(entry.mArea.mTopLeft.y + entry.mArea.mSize.y), 0 }, { 1, 1, 1, 1 }, { 0, 0, 1 } });
                    vertices.push_back({ { float(entry.mArea.mTopLeft.x + entry.mArea.mSize.x), float(entry.mArea.mTopLeft.y + entry.mArea.mSize.y), 0 }, { 1, 1, 1, 1 }, { 0, 0, 1 } });
                }

                for (Render::Vertex &v : vertices) {
                    v.mPos *= 0.01f;
                }

                for (size_t i = 0; i < COUNT; ++i) {
                    indices.push_back(4 * i + 0);
                    indices.push_back(4 * i + 1);
                    indices.push_back(4 * i + 0);
                    indices.push_back(4 * i + 2);
                    indices.push_back(4 * i + 1);
                    indices.push_back(4 * i + 3);
                    indices.push_back(4 * i + 2);
                    indices.push_back(4 * i + 3);
                }
            }

            Im3D::Mesh(IM3D_LINES, vertices.data(), vertices.size(), {}, indices.data(), indices.size());

            ImGui::DragInt("Sphere-Detail", &mSphereDetail, 1.0f, 0, 100);

            //Im3D::Sphere({ 0, 0, 0 }, 1.0f, Im3D::SphereParameters { static_cast<size_t>(mSphereDetail) });

            Im3D::Arrow(0.15f, Vector3::ZERO, Vector3::UNIT_X);

            ImGui::DragFloat2("Scale", &ImGui::GetIO().DisplayFramebufferScale.x, 0.1f, 0.1f, 2.0f);

            if (ImGui::TreeNode("ValueType Sizes")) {
                ImGui::Text("ValueType size: %u", (unsigned int)sizeof(ValueType));
                renderValuetypeSizes(std::make_index_sequence<size_t(ValueTypeEnum::MAX_VALUETYPE_TYPE)>());
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("VTable")) {

                struct Base1 {
                    virtual void b1() { }
                };

                struct Base2 {
                    virtual void b2() { }
                };

                struct Base3 : Base2 {
                    virtual void b3() { }
                    virtual void b2() override { }
                };

                struct Derived : Base1, Base3 {
                    virtual void b1() override { }
                };

                Base3 b;
                renderVTableLayout<Base3, Base2>(b);
                Derived d;
                renderVTableLayout<Derived, Base3, Base2, Base1>(d);


                ImGui::TreePop();
            }
        }
        ImGui::End();
    }

    void TestTool::update()
    {

        ToolBase::update();
    }

    std::string_view TestTool::key() const
    {
        return "TestTool";
    }

    void TestTool::logTest()
    {
        LOG("Test");
    }

    void TestTool::logValue(const ValueType &v)
    {
        LOG(v.toString());
    }

    int TestTool::dummy()
    {
        return 42;
    }

}
}
