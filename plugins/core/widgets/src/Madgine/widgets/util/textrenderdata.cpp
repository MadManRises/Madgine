#include "../../widgetslib.h"

#include "textrenderdata.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

METATABLE_BEGIN_BASE(Engine::Widgets::TextRenderData, Engine::Widgets::RenderData)
MEMBER(mFontSize)
MEMBER(mPivot)
MEMBER(mColor)
PROPERTY(Font, getFont, setFont)
METATABLE_END(Engine::Widgets::TextRenderData)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Widgets::TextRenderData, Engine::Widgets::RenderData)
FIELD(mFontSize)
FIELD(mPivot)
FIELD(mColor)
ENCAPSULATED_FIELD(mFont, getFontName, setFontName)
SERIALIZETABLE_END(Engine::Widgets::TextRenderData)

namespace Engine {
namespace Widgets {

    std::string_view TextRenderData::getFontName() const
    {
        return mFont.name();
    }

    void TextRenderData::setFontName(std::string_view name)
    {
        mFont.load(name);
    }

    Render::FontLoader::Resource *TextRenderData::getFont() const
    {
        return mFont.resource();
    }

    void TextRenderData::setFont(Render::FontLoader::Resource *font)
    {
        mFont = font;
    }

    bool TextRenderData::available() const
    {
        return mFont && mFont.available();
    }

    std::pair<std::vector<Vertex>, TextureSettings> TextRenderData::render(std::string_view text, Vector3 pos, Vector3 size, const Vector2 &screenSize, int cursorIndex) const
    {
        return renderText(text, pos, size.xy(), mFont, size.z * mFontSize, mColor, mPivot, screenSize, cursorIndex);
    }

    std::vector<Vertex> TextRenderData::renderSelection(std::string_view text, Vector3 pos, Vector3 size, const Vector2 &screenSize, const Atlas2::Entry &entry, int selectionStart, int selectionEnd, Vector4 color)
    {
        return renderSelection(text, pos, size.xy(), mFont, size.z * mFontSize, mPivot, screenSize, entry, selectionStart, selectionEnd, color);
    }

    float TextRenderData::calculateWidth(std::string_view text, float z)
    {
        return calculateWidth(text, mFont, z * mFontSize);
    }

    Rect2 TextRenderData::calculateBoundingBox(std::string_view text, Vector2 pos, Vector3 size)
    {
        return calculateBoundingBox(text, pos, size.xy(), mFont, size.z * mFontSize, mPivot);
    }

    std::pair<std::vector<Vertex>, TextureSettings> TextRenderData::renderText(std::string_view text, Vector3 pos, Vector2 size, const Render::Font *font, float fontSize, Color3 color, Vector2 pivot, const Vector2 &screenSize, int cursorIndex)
    {
        std::vector<Vertex> result;

        size_t textLen = text.size();

        if (textLen == 0 && cursorIndex == -1)
            return {};

        float scaleX = fontSize / 64.0f / screenSize.x;
        float scaleY = fontSize / 64.0f / screenSize.y;

        float minY = font->mDescender / 64.0f * scaleY;
        float maxY = font->mAscender / 64.0f * scaleY;
        float fullHeight = maxY - minY;
        float fullWidth = calculateWidth(text, font, fontSize) / screenSize.x;

        float cursorX = (size.x - fullWidth) * pivot.x;
        float originY = (size.y - fullHeight) * pivot.y + maxY;

        const Render::Glyph &ref = font->mGlyphs['D'];

        float cursorHeight = ref.mSize.y * scaleY;

        for (size_t i = 0; i <= textLen; ++i) {

            if (i == cursorIndex) {
                const Render::Glyph &cursor = font->mGlyphs['|'];
                
                float width = 3.0f * scaleX;

                float startX = cursorX - 1.5f * scaleX;
                float startY = originY - ref.mBearing.y * scaleY;

                renderQuadUV(result, { pos.x + startX, pos.y + startY, pos.z + 0.6f }, { width, cursorHeight }, { color, 1 }, { cursor.mUV, cursor.mSize }, font->mTextureSize, cursor.mFlipped);                
            }

            if (i == textLen)
                break;

            const Render::Glyph &g = font->mGlyphs[text[i]];

            float width = g.mSize.x * scaleX;
            float height = g.mSize.y * scaleY;

            float startX = cursorX + g.mBearing.x * scaleX;
            float startY = originY - g.mBearing.y * scaleY;

            renderQuadUV(result, { pos.x + startX, pos.y + startY, pos.z + 0.5f }, { width, height }, { color, 1 }, { g.mUV, g.mSize }, font->mTextureSize, g.mFlipped);

            cursorX += g.mAdvance / 64.0f * scaleX;
        }
        return { result, { { font->mTexture->mTextureHandle, Render::TextureType_2D }, TextureFlag_IsDistanceField } };
    }

    std::vector<Vertex> TextRenderData::renderSelection(std::string_view text, Vector3 pos, Vector2 size, const Render::Font *font, float fontSize, Vector2 pivot, const Vector2 &screenSize, const Atlas2::Entry &entry, int selectionStart, int selectionEnd, Vector4 color)
    {
        std::vector<Vertex> result;

        size_t textLen = text.size();

        float scaleX = fontSize / 64.0f / screenSize.x;
        float scaleY = fontSize / 64.0f / screenSize.y;

        float minY = font->mDescender / 64.0f * scaleY;
        float maxY = font->mAscender / 64.0f * scaleY;
        float fullHeight = maxY - minY;
        float fullWidth = calculateWidth(text, font, fontSize) / screenSize.x;

        float cursorX = (size.x - fullWidth) * pivot.x;
        float originY = (size.y - fullHeight) * pivot.y + maxY;

        float startX;
        float endX;

        for (size_t i = 0; i <= textLen; ++i) {
            if (i == selectionStart)
                startX = cursorX;

            if (i == selectionEnd)
                endX = cursorX;

            if (i == textLen)
                break;

            const Render::Glyph &g = font->mGlyphs[text[i]];

            cursorX += g.mAdvance / 64.0f * scaleX;
        }

        const Render::Glyph &ref = font->mGlyphs['D'];

        float height = ref.mSize.y * scaleY;

        float startY = originY - ref.mBearing.y * scaleY;

        renderQuadUV(result, { pos.x + startX, pos.y + startY, pos.z + 0.4f }, { endX - startX, height }, color, entry.mArea, { 2048, 2048 }, entry.mFlipped);

        return result;
    }

    float TextRenderData::calculateWidth(std::string_view text, const Render::Font *font, float fontSize)
    {
        float scale = fontSize / 64.0f;

        float result = 0.0f;

        for (char c : text) {
            const Render::Glyph &g = font->mGlyphs[c];

            result += g.mAdvance / 64.0f * scale;
        }

        return result;
    }

    Rect2 TextRenderData::calculateBoundingBox(std::string_view text, Vector2 pos, Vector2 size, const Render::Font *font, float fontSize, Vector2 pivot)
    {
        float scale = fontSize / 64.0f;

        float minY = font->mDescender / 64.0f * scale;
        float maxY = font->mAscender / 64.0f * scale;
        float fullHeight = maxY - minY;
        float fullWidth = calculateWidth(text, font, fontSize);

        float cursorX = (size.x - fullWidth) * pivot.x;
        float baseY = (size.y - fullHeight) * pivot.y;

        return {
            pos + Vector2 { cursorX, baseY },
            { fullWidth, fullHeight }
        };
    }

}
}