#include "../toolslib.h"

#include "testtool.h"

#include "imgui/imgui.h"

#include "imgui/imguiaddons.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"
#include "Modules/serialize/serializetable_impl.h"


#include "Interfaces/window/windowapi.h"

#include "Modules/math/atlas2.h"

#include "im3d/im3d.h"

#include "Modules/render/vertex.h"

#include "imageloaderlib.h"
#include "imageloader.h"


UNIQUECOMPONENT(Engine::Tools::TestTool);

namespace Engine {
namespace Tools {

    TestTool::TestTool(ImRoot &root)
        : Tool<TestTool>(root)
    {
    }

    template <size_t... Is>
    void renderValuetypeSizes(std::index_sequence<Is...>)
    {
        std::vector<std::pair<const char *, size_t>> data = {
            { typeid(std::get<Is>(std::declval<ValueType::Union>())).name(), sizeof(std::get<Is>(std::declval<ValueType::Union>())) }...
        };
        for (auto [name, size] : data) {
            ImGui::Text("  %s size: %lu", name, size);
        }
    }

    void TestTool::render()
    {

        if (ImGui::Begin("TestTool", &mVisible)) {
            /*if (ImGui::Button("Create Tool Window")) {
                Window::WindowSettings settings;
                static_cast<const ClientImRoot&>(*mRoot.parent()).window().createToolWindow(settings);
            }*/

			Resources::ImageLoader::load("Grass-01", true);

			Im3D::Text("Test_1234 Hallo\n wie gehts", Matrix4::IDENTITY);

            static std::vector<Render::Vertex> vertices;
            static std::vector<unsigned int> indices;
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

            Im3D::Mesh(IM3D_LINES, vertices.data(), vertices.size(), Matrix4::IDENTITY, indices.data(), indices.size());

            ImGui::DragFloat2("Scale", &ImGui::GetIO().DisplayFramebufferScale.x, 0.1f, 0.1f, 2.0f);

            ImGui::Text("ValueType size: %lu", sizeof(ValueType));
            renderValuetypeSizes(std::make_index_sequence<size_t(ValueType::Type::MAX_VALUETYPE_TYPE)>());
        }
        ImGui::End();
    }

    void TestTool::update()
    {

        ToolBase::update();
    }

    const char *TestTool::key() const
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

}
}

METATABLE_BEGIN(Engine::Tools::TestTool)
FUNCTION(logTest)
FUNCTION(logValue)
METATABLE_END(Engine::Tools::TestTool)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Tools::TestTool, Engine::Tools::ToolBase)
SERIALIZETABLE_END(Engine::Tools::TestTool)

RegisterType(Engine::Tools::TestTool);