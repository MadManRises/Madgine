#pragma once

#include "renderdata.h"

#include "Madgine/fontloader/fontloader.h"

#include "Meta/serialize/hierarchy/serializabledataunit.h"

#include "Meta/math/rect2.h"
#include "Meta/math/atlas2.h"

#include "vertex.h"
#include "texturesettings.h"

namespace Engine {
namespace Widgets {

    struct TextRenderData : RenderData {

        std::string_view getFontName() const;
        void setFontName(std::string_view name);

        Render::FontLoader::Resource *getFont() const;
        void setFont(Render::FontLoader::Resource *font);

        bool available() const;
        std::pair<std::vector<Vertex>, TextureSettings> render(std::string_view text, Vector3 pos, Vector3 size, const Vector2 &screenSize, int cursorIndex = -1) const;
        std::vector<Vertex> renderSelection(std::string_view text, Vector3 pos, Vector3 size, const Vector2 &screenSize, const Atlas2::Entry &entry, int selectionStart, int selectionEnd, Vector4 color);
        float calculateWidth(std::string_view text, float z = 1.0f);
        Rect2 calculateBoundingBox(std::string_view text, Vector3 pos, Vector3 size);        

        static std::pair<std::vector<Vertex>, TextureSettings> renderText(std::string_view text, Vector3 pos, Vector2 size, const Render::Font *font, float fontSize, Vector2 pivot, const Vector2 &screenSize, int cursorIndex = -1);
        static std::vector<Vertex> renderSelection(std::string_view text, Vector3 pos, Vector2 size, const Render::Font *font, float fontSize, Vector2 pivot, const Vector2 &screenSize, const Atlas2::Entry &entry, int selectionStart, int selectionEnd, Vector4 color);
        static float calculateWidth(std::string_view text, const Render::Font *font, float fontSize);  
        static Rect2 calculateBoundingBox(std::string_view text, Vector3 pos, Vector2 size, const Render::Font *font, float fontSize, Vector2 pivot);

        int mFontSize = 16;

        Vector2 mPivot = { 0.5f, 0.5f };

    private:
        Render::FontLoader::Handle mFont;
    };

}
}