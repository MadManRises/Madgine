#pragma once

#include "renderdata.h"

#include "Madgine/fontloader/fontloader.h"

#include "Meta/serialize/hierarchy/serializabledataunit.h"

#include "Meta/math/atlas2.h"
#include "Meta/math/color4.h"
#include "Meta/math/rect2.h"

#include "texturesettings.h"
#include "vertex.h"
#include "layouts/sizeconstraints.h"

namespace Engine {
namespace Widgets {

    struct TextRenderData : RenderData {

        std::string_view getFontName() const;
        void setFontName(std::string_view name);

        Render::FontLoader::Resource *getFont() const;
        void setFont(Render::FontLoader::Resource *font);

        bool available() const;
        void render(WidgetsRenderData &renderData, std::string_view text, Vector3 pos, Vector3 size, int cursorIndex = -1) const;
        void renderSelection(WidgetsRenderData &renderData, std::string_view text, Vector3 pos, Vector3 size, const Atlas2::Entry &entry, int selectionStart, int selectionEnd, Color4 color);
        float calculateWidth(std::string_view text, float z = 1.0f);
        float calculateHeight(float z = 1.0f);
        Rect2 calculateBoundingBox(std::string_view text, Vector2 pos, Vector3 size);
        SizeConstraints calculateHeightConstraints(float z = 1.0f);

        static void renderText(WidgetsRenderData &renderData, std::string_view text, Vector3 pos, Vector2 size, const Render::Font *font, float fontSize, Color4 color, Vector2 pivot, int cursorIndex = -1);
        static void renderSelection(WidgetsRenderData &renderData, std::string_view text, Vector3 pos, Vector2 size, const Render::Font *font, float fontSize, Vector2 pivot, const Atlas2::Entry &entry, int selectionStart, int selectionEnd, Color4 color);
        static float calculateWidth(std::string_view text, const Render::Font *font, float fontSize);
        static float calculateHeight(const Render::Font *font, float fontSize);
        static Rect2 calculateBoundingBox(std::string_view text, Vector2 pos, Vector2 size, const Render::Font *font, float fontSize, Vector2 pivot);
        static SizeConstraints calculateHeightConstraints(const Render::Font *font, float fontSize);

        int mFontSize = 16;

        Vector2 mPivot = { 0.5f, 0.5f };

        Color4 mColor = { 1, 1, 1, 1 };

    private:
        Render::FontLoader::Handle mFont;
        Resources::ImageLoader::Resource *mImage = nullptr;
    };

}
}