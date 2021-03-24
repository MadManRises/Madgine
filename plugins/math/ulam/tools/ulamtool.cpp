#include "ulamtoolslib.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "ulamtool.h"

#include "imgui/imguiaddons.h"
#include "renderer/imroot.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "bigint.h"

UNIQUECOMPONENT(Engine::Tools::UlamTool);

namespace Engine {
namespace Tools {

    static int aLimit = 4;

    void textRender(Ulam &u, int &)
    {
        for (const Ulam::Triple &t : u) {
            ImGui::Text("%d", t.mValue);
        }
    }

    template <int n>
    void naryRender(Ulam &u, int &)
    {
        for (const Ulam::Triple &t : u) {
            std::stack<int> s;
            int i = t.mValue;
            while (i > 0) {
                s.push(i % n);
                i /= n;
            }
            std::stringstream ss;
            if constexpr (n == 3) {
                for (size_t j = 0; j < t.mCount2; ++j) {
                    ss << "  ";
                }
            }
            for (; !s.empty(); s.pop()) {
                ss << s.top() << " ";
            }
            ImGui::Text("%s", ss.str().c_str());
        }
    }

    size_t negativeTernary(std::stack<char> &s, BigInt i, size_t *lenPtr = nullptr)
    {
        size_t len = 0;
        size_t signCount = 0;
        while (i > 0) {
            switch (i % 3) {
            case 0:
                s.push('0');
                break;
            case 1:
                s.push('+');
                ++signCount;
                break;
            case 2:
                s.push('-');
                ++signCount;
                i += 3;
                break;
            }
            i /= 3;
            ++len;
        }
        if (lenPtr)
            *lenPtr = len;
        return signCount;
    }

    void negativeTernaryRender(Ulam &u, int &mHoveredLine)
    {
        size_t specialCases = 0;

        size_t highestCount2, count3, count2, highestLen;

        size_t steps = 0;

        int line = 0;

        ImGui::BeginColumns("col", 2);

        for (const Ulam::Triple &t : u) {
            std::stack<char> s;

            size_t signCount = negativeTernary(s, t.mValue);

            if (signCount % 2 == 1 && t.mValue != 1)
                continue;

            std::stringstream ss;
            for (size_t j = 0; j < (t.mCount2 + 2 - specialCases); ++j) {
                ss << "  ";
            }
            int topCount = 2;
            for (; !s.empty(); s.pop()) {
                ss << s.top() << " ";
                if (topCount > 0) {
                    if (s.top() == '-')
                        topCount = 0;
                    else if (s.top() == '+') {
                        if (--topCount == 0)
                            ++specialCases;
                    }
                }
            }
            char buffer[256];
            sprintf(buffer, "%10d | %s", t.mValue, ss.str().c_str());
            if (ImGui::Selectable(buffer, line == mHoveredLine))
                mHoveredLine = line;
            ++line;

            highestCount2 = t.mCount2 - specialCases;
            count2 = t.mCount2;
            count3 = t.mCount3;
            ++steps;
        }

        BigInt v = 1;
        for (size_t i = 1; i < steps; ++i) {
            std::stack<char> s;

            v <<= 1;

            highestLen = negativeTernary(s, v);

            std::stringstream ss;
            for (size_t j = 0; j < highestCount2 - s.size() + 3; ++j) {
                ss << "  ";
            }
            for (; !s.empty(); s.pop()) {
                ss << s.top() << " ";
            }
            char buffer[256];
            sprintf(buffer, "#%9zu | %s", i, ss.str().c_str());
            if (ImGui::Selectable(buffer, line == mHoveredLine))
                mHoveredLine = line;
            ++line;
        }

        ImGui::NextColumn();

        line = 0;

        std::stack<int> vals;

        for (const Ulam::Triple &t : u) {
            std::stack<char> s2;

            size_t len;
            size_t signCount = negativeTernary(s2, t.mValue, &len);
            std::stack<char> s;

            if (signCount % 2 == 1 && t.mValue != 1)
                continue;

            BigInt actualValue = t.mValue;
            while (!(actualValue > v))
                actualValue *= 3;
            actualValue /= 3;

            negativeTernary(s, v - actualValue, &len);

            std::stringstream ss;
            for (size_t j = 0; j < (highestCount2 + 3 - len); ++j) {
                ss << "  ";
            }
            for (; !s.empty(); s.pop()) {
                ss << s.top() << " ";
            }
            char buffer[256];
            sprintf(buffer, "%10d | %s", ((v - actualValue) > std::numeric_limits<uint32_t>::max()) ? 0 : static_cast<uint32_t>(v - actualValue), ss.str().c_str());
            if (ImGui::Selectable(buffer, line == mHoveredLine))
                mHoveredLine = line;
            ++line;

            v >>= 1;
            vals.push(t.mValue);
        }
        v = 2;
        vals.pop();
        for (; !vals.empty(); vals.pop()) {
            std::stack<char> s2;

            size_t signCount = negativeTernary(s2, vals.top());
            std::stack<char> s;

            if (signCount % 2 == 1 && vals.top() != 1)
                continue;

            size_t len;
            negativeTernary(s, v - vals.top(), &len);

            std::stringstream ss;
            for (size_t j = 0; j < (highestCount2 + 3 - len); ++j) {
                ss << "  ";
            }
            for (; !s.empty(); s.pop()) {
                ss << s.top() << " ";
            }
            char buffer[256];
            sprintf(buffer, "%10d | %s", ((v - vals.top()) > std::numeric_limits<uint64_t>::max()) ? 0 : static_cast<uint32_t>(v - vals.top()), ss.str().c_str());
            if (ImGui::Selectable(buffer, line == mHoveredLine))
                mHoveredLine = line;
            ++line;

            v <<= 1;
        }

        ImGui::EndColumns();

        ImGui::Text("%d/%d", count2, count3);
        double d1 = pow(2.0, count2);
        double d2 = pow(3.0, count3);
        ImGui::Text("%f", d1 / d2);
        ImGui::Text("%f", (d1 / d2 - u.mValue) / u.mValue);
    }

    std::set<int> rowStep(std::set<int> as, int b)
    {
        std::set<int> vs;
        for (int a : as) {
            if (a < 0)
                continue;
            int v = 3 * a + b;
            if (v == 0) {
                vs.insert(-1);
            }
            int c = 1;
            while (c <= v) {
                if (v - c <= aLimit) {
                    vs.insert(v - c);
                }
                c <<= 1;
            }
        }
        return vs;
    }

    void rowRenderHelper(std::set<int> as, int b, int depth, int maxDepth)
    {
        std::stringstream ss;
        if (depth != maxDepth)
            ss << "  ";
        for (size_t i = 0; i < maxDepth - depth; ++i) {
            ss << "  | ";
        }
        ImGui::Text("%s%d |", ss.str().c_str(), b);

        std::set<int> vs = rowStep(as, b);
        if (depth > 0) {
            for (int i = 0; i < 3; ++i) {
                rowRenderHelper(vs, i, depth - 1, maxDepth);
            }
        } else {
            for (int va : vs) {
                ImGui::SameLine();
                if (va >= 0)
                    ImGui::Text("%d", va);
                else
                    ImGui::Text("-");
            }
        }
    }

    void rowRender(Ulam &u, int &mHoveredLine)
    {
        static bool collapsed = false;
        ImGui::Checkbox("collapsed", &collapsed);

        for (int a = 0; a <= aLimit; ++a) {
            for (int b = 0; b < 3; ++b) {
                ImGui::Text("%d", a);
                ImGui::SameLine();
                rowRenderHelper({ a }, b, u.mValue, u.mValue);
            }
        }
    }

    void rowRender2Helper(int a, std::set<int> as, int b, int depth, int maxDepth)
    {
        int pivot = pow(3, depth);

        if (b % pivot == 0 && a == 0) {
            ImGui::Text("%d |", b / pivot);
        } else {
            ImGui::Text("  |");            
        }
        ImGui::SameLine();

        std::set<int> vs = rowStep(as, b / pivot);

        if (depth > 0) {
            rowRender2Helper(a, vs, b - (b / pivot) * pivot, depth - 1, depth);
        } else {
            ImGui::Text("%d |", a);
            for (int va : vs) {
                if (va >= 0) {
                    ImGui::SameLine();
                    ImGui::Text("%d", va);
                }
            }
        }
    }

    void rowRender2(Ulam &u, int &mHoveredLine)
    {
        static bool collapsed = false;
        ImGui::Checkbox("collapsed", &collapsed);

        ImGui::DragInt("Limit", &aLimit);

        for (int b = 0; b < pow(3, u.mValue + 1); ++b) {
            for (int a = 0; a <= aLimit; ++a) {
                rowRender2Helper(a, { a }, b, u.mValue, u.mValue);
            }
        }
    }

    UlamTool::UlamTool(ImRoot &root)
        : Tool<UlamTool>(root)
    {
        mFunction = negativeTernaryRender;
    }

    void UlamTool::render()
    {
        if (ImGui::Begin("Ulam", &mVisible)) {
            ImGui::DragInt("Start", &mUlam.mValue);
            if (ImGui::BeginCombo("Presentation: ", mMode)) {
                if (ImGui::Selectable("text", mFunction == textRender)) {
                    mFunction = textRender;
                    mMode = "text";
                }
                if (ImGui::Selectable("binary", mFunction == naryRender<2>)) {
                    mFunction = naryRender<2>;
                    mMode = "binary";
                }
                if (ImGui::Selectable("ternary", mFunction == naryRender<3>)) {
                    mFunction = naryRender<3>;
                    mMode = "ternary";
                }
                if (ImGui::Selectable("negative ternary", mFunction == negativeTernaryRender)) {
                    mFunction = negativeTernaryRender;
                    mMode = "negative ternary";
                }
                if (ImGui::Selectable("row", mFunction == rowRender)) {
                    mFunction = rowRender;
                    mMode = "row";
                }
                if (ImGui::Selectable("row2", mFunction == rowRender2)) {
                    mFunction = rowRender2;
                    mMode = "row2";
                }
                ImGui::EndCombo();
            }

            ImGui::Separator();

            mFunction(mUlam, mHoveredLine);
        }
        ImGui::End();
    }

    std::string_view UlamTool::key() const
    {
        return "Ulam";
    }
}
}

METATABLE_BEGIN_BASE(Engine::Tools::UlamTool, Engine::Tools::ToolBase)
METATABLE_END(Engine::Tools::UlamTool)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Tools::UlamTool, Engine::Tools::ToolBase)
SERIALIZETABLE_END(Engine::Tools::UlamTool)
