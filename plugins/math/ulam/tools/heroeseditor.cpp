#include "ulamtoolslib.h"

#include "heroeseditor.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#include "imgui/imguiaddons.h"
#include "renderer/imroot.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

UNIQUECOMPONENT(Engine::Tools::HeroesEditor);

namespace Engine {
namespace Tools {

    HeroesEditor::HeroesEditor(ImRoot &root)
        : Tool<HeroesEditor>(root)
        , mMap(R"(C:\Users\Bub\Dropbox\Dos\HEROES2\MAPS\othersid.mp2)",
              R"(C:\Users\Bub\Dropbox\Dos\HEROES2\MAPS\othersid - Kopie.mp2)")
    {
    }

    ImVec4 toColor(unsigned short type)
    {

        if (type < 0x20) {
            return { 0.0f, 0.0f, 1.0f, 1.0f };
        } else if (type < 0x60) {
            return { 0.1f, 0.4f, 0.1f, 1.0f };
        } else if (type < 0xd0) {
            return { 0.1f, 0.2f, 0.2f, 1.0f };
        } else if (type < 0x140) {
            return { 0.8f, 0.8f, 0.0f, 1.0f };
        } else if (type < 0x170) {
            return { 0.5f, 0.3f, 0.0f, 1.0f };
        } else if (type < 0x230) {
            return { 0.9f, 0.9f, 0.6f, 1.0f };
        }

        return { 1.0f, 0.0f, 0.0f, 1.0f };
    }

    void HeroesEditor::draw(const char *name, std::vector<unsigned char> &buffer)
    {
        if (ImGui::Begin(name, &mVisible, ImGuiWindowFlags_HorizontalScrollbar)) {

            unsigned char *data = buffer.data();

            if (mOffset + mStride * sMapSize * sMapSize > mMap.mBuffer.size())
                ImGui::Text("Too Big!");
            else {
                for (int i = 0; i < sMapSize + 1; ++i) {
                    if (i > mLimity) {
                        break;
                    }
                    for (int j = 0; j < sMapSize + 1; ++j) {
                        if (j > mLimitx) {
                            break;
                        }
                        int index = i * (sMapSize + 1) + j;
                        unsigned short value = *reinterpret_cast<const unsigned short *>(data + (index * mStride + mOffset));
                        ImVec4 col = toColor(value);
                        std::string name = "col" + std::to_string(index);
                        if (ImGui::ColorButton(name.c_str(), col)) {
                            mModifyX = j;
                            mModifyY = i;
                            ImGui::OpenPopup("modifyField");
                        }

                        if (ImGui::IsItemHovered()) {
                            ImGui::BeginTooltip();
                            ImGui::Text("(%d, %d) - %4x", j, i, value);
                            for (int c = 2; c < mStride; ++c) {

                                int value = data[index * mStride + mOffset + c];
                                ImGui::Text("%2x", value);
                                ImGui::SameLine();
                            }
                            ImGui::EndTooltip();
                        }
                        ImGui::SameLine();
                    }
                    ImGui::Text("");
                }
            }

            if (ImGui::BeginPopup("modifyField")) {
                int index = mModifyY * (sMapSize + 1) + mModifyX;
                unsigned short value = *reinterpret_cast<const unsigned short *>(data + (index * mStride + mOffset));
                ImGui::Text("(%d, %d) - %4x", mModifyX, mModifyY, value);
                for (int c = 2; c < mStride; ++c) {
                    int value = data[index * mStride + mOffset + c];
                    ImGui::SetNextItemWidth(30.0f);
                    std::string name = "##int" + std::to_string(c);
                    if (ImGui::DragInt(name.c_str(), &value, 1.0f, 0, 255)) {
                        data[index * mStride + mOffset + c] = value;
                    }
                    ImGui::SameLine();
                }
                ImGui::EndPopup();
            }
        }
        ImGui::End();
    }

    void HeroesEditor::render()
    {
        draw("Map", mMap.mBuffer);

        //draw("Map2", mMap.mBuffer2);

        //draw("Map3", mMap.mBuffer3);
    }

    void HeroesEditor::renderMenu()
    {
        ToolBase::renderMenu();
        if (mVisible) {
            if (ImGui::BeginMenu("HeroesEditor")) {

                if (ImGui::MenuItem("Save Map"))
                    mMap.save();
                if (ImGui::MenuItem("Save Map2"))
                    mMap.save2();
                ImGui::EndMenu();
            }
        }
    }

    std::string_view HeroesEditor::key() const
    {
        return "HeroesEditor";
    }
}
}

METATABLE_BEGIN_BASE(Engine::Tools::HeroesEditor, Engine::Tools::ToolBase)
MEMBER(mOffset)
MEMBER(mStride)
MEMBER(mLimitx)
MEMBER(mLimity)
METATABLE_END(Engine::Tools::HeroesEditor)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Tools::HeroesEditor, Engine::Tools::ToolBase)
SERIALIZETABLE_END(Engine::Tools::HeroesEditor)
