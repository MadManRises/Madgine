#include "../../widgetslib.h"

#include "textrenderdata.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

METATABLE_BEGIN(Engine::Widgets::TextRenderData)
MEMBER(mFontSize)
MEMBER(mPivot)
PROPERTY(Font, getFont, setFont)
METATABLE_END(Engine::Widgets::TextRenderData)

SERIALIZETABLE_BEGIN(Engine::Widgets::TextRenderData)
FIELD(mFontSize)
FIELD(mPivot)
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
        return renderText(text, pos, size.xy(), mFont, size.z * mFontSize, mPivot, screenSize, cursorIndex);
    }

    std::vector<Vertex> TextRenderData::renderSelection(std::string_view text, Vector3 pos, Vector3 size, const Vector2 &screenSize, int selectionStart, int selectionEnd, Vector4 color)
    {
        return renderSelection(text, pos, size.xy(), mFont, size.z * mFontSize, mPivot, screenSize, selectionStart, selectionEnd, color);
    }

    float TextRenderData::calculateWidth(std::string_view text, float z)
    {
        return calculateWidth(text, mFont, z * mFontSize);
    }

    Rect2 TextRenderData::calculateBoundingBox(std::string_view text, Vector3 pos, Vector3 size)
    {
        return calculateBoundingBox(text, pos, size.xy(), mFont, size.z * mFontSize, mPivot);
    }

    std::pair<std::vector<Vertex>, TextureSettings> TextRenderData::renderText(std::string_view text, Vector3 pos, Vector2 size, const Render::Font *font, float fontSize, Vector2 pivot, const Vector2 &screenSize, int cursorIndex)
    {
        std::vector<Vertex> result;

        size_t textLen = text.size();

        if (textLen == 0 && cursorIndex == -1)
            return {};

        float scaleX = fontSize / 5.0f / screenSize.x;
        float scaleY = fontSize / 5.0f / screenSize.y;

        float minY = font->mDescender / 64.0f * scaleY;
        float maxY = font->mAscender / 64.0f * scaleY;
        float fullHeight = maxY - minY;
        float fullWidth = calculateWidth(text, font, fontSize) / screenSize.x;

        float cursorX = (size.x - fullWidth) * pivot.x;
        float originY = (size.y - fullHeight) * pivot.y + maxY;

        for (size_t i = 0; i <= textLen; ++i) {

            if (i == cursorIndex) {
                const Render::Glyph &cursor = font->mGlyphs['|'];
                const Render::Glyph &ref = font->mGlyphs['D'];

                float height = ref.mSize.y * scaleY;

                float vPosX1 = cursorX - 1.5f * scaleX;
                float vPosX2 = cursorX + 1.5f * scaleX;
                float vPosY1 = originY - ref.mBearing.y * scaleY;
                float vPosY2 = vPosY1 + height;

                Vector3 v11 = { vPosX1, vPosY1, pos.z + 0.6f };
                Vector3 v12 = { vPosX2, vPosY1, pos.z + 0.6f };
                Vector3 v21 = { vPosX1, vPosY2, pos.z + 0.6f };
                Vector3 v22 = { vPosX2, vPosY2, pos.z + 0.6f };

                int uvWidth = cursor.mSize.x;
                int uvHeight = cursor.mSize.y;

                if (cursor.mFlipped)
                    std::swap(uvWidth, uvHeight);

                Vector2 uvTopLeft = { float(cursor.mUV.x) / font->mTextureSize.x, float(cursor.mUV.y) / font->mTextureSize.y };
                Vector2 uvBottomRight = { float(cursor.mUV.x + uvWidth) / font->mTextureSize.x,
                    float(cursor.mUV.y + uvHeight) / font->mTextureSize.y };

                Vector2 uvTopRight = { uvBottomRight.x, uvTopLeft.y };
                Vector2 uvBottomLeft = { uvTopLeft.x, uvBottomRight.y };

                if (cursor.mFlipped)
                    std::swap(uvTopRight, uvBottomLeft);

                result.push_back({ v11 + pos, { 1, 1, 1, 1 }, uvTopLeft });
                result.push_back({ v12 + pos, { 1, 1, 1, 1 }, uvTopRight });
                result.push_back({ v21 + pos, { 1, 1, 1, 1 }, uvBottomLeft });
                result.push_back({ v21 + pos, { 1, 1, 1, 1 }, uvBottomLeft });
                result.push_back({ v12 + pos, { 1, 1, 1, 1 }, uvTopRight });
                result.push_back({ v22 + pos, { 1, 1, 1, 1 }, uvBottomRight });
            }

            if (i == textLen)
                break;

            const Render::Glyph &g = font->mGlyphs[text[i]];

            float width = g.mSize.x * scaleX;
            float height = g.mSize.y * scaleY;

            float vPosX1 = cursorX + g.mBearing.x * scaleX;
            float vPosX2 = vPosX1 + width;
            float vPosY1 = originY - g.mBearing.y * scaleY;
            float vPosY2 = vPosY1 + height;

            Vector3 v11 = { vPosX1, vPosY1, pos.z + 0.5f };
            Vector3 v12 = { vPosX2, vPosY1, pos.z + 0.5f };
            Vector3 v21 = { vPosX1, vPosY2, pos.z + 0.5f };
            Vector3 v22 = { vPosX2, vPosY2, pos.z + 0.5f };

            int uvWidth = g.mSize.x;
            int uvHeight = g.mSize.y;

            if (g.mFlipped)
                std::swap(uvWidth, uvHeight);

            Vector2 uvTopLeft = { float(g.mUV.x) / font->mTextureSize.x, float(g.mUV.y) / font->mTextureSize.y };
            Vector2 uvBottomRight = { float(g.mUV.x + uvWidth) / font->mTextureSize.x,
                float(g.mUV.y + uvHeight) / font->mTextureSize.y };

            Vector2 uvTopRight = { uvBottomRight.x, uvTopLeft.y };
            Vector2 uvBottomLeft = { uvTopLeft.x, uvBottomRight.y };

            if (g.mFlipped)
                std::swap(uvTopRight, uvBottomLeft);

            result.push_back({ v11 + pos, { 1, 1, 1, 1 }, uvTopLeft });
            result.push_back({ v12 + pos, { 1, 1, 1, 1 }, uvTopRight });
            result.push_back({ v21 + pos, { 1, 1, 1, 1 }, uvBottomLeft });
            result.push_back({ v21 + pos, { 1, 1, 1, 1 }, uvBottomLeft });
            result.push_back({ v12 + pos, { 1, 1, 1, 1 }, uvTopRight });
            result.push_back({ v22 + pos, { 1, 1, 1, 1 }, uvBottomRight });

            cursorX += g.mAdvance / 64.0f * scaleX;
        }
        return { result, { { font->mTexture->mTextureHandle, Render::TextureType_2D }, TextureFlag_IsDistanceField } };
    }

    std::vector<Vertex> TextRenderData::renderSelection(std::string_view text, Vector3 pos, Vector2 size, const Render::Font *font, float fontSize, Vector2 pivot, const Vector2 &screenSize, int selectionStart, int selectionEnd, Vector4 color)
    {
        std::vector<Vertex> result;

        size_t textLen = text.size();

        float scaleX = fontSize / 5.0f / screenSize.x;
        float scaleY = fontSize / 5.0f / screenSize.y;

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

        float vPosY1 = originY - ref.mBearing.y * scaleY;
        float vPosY2 = vPosY1 + height;

        Vector3 v11 = { startX, vPosY1, pos.z + 0.4f };
        Vector3 v12 = { endX, vPosY1, pos.z + 0.4f };
        Vector3 v21 = { startX, vPosY2, pos.z + 0.4f };
        Vector3 v22 = { endX, vPosY2, pos.z + 0.4f };

        Vector2 uvTopLeft = { 0.0f, 0.0f };
        Vector2 uvBottomRight = { 1.0f, 1.0f };

        Vector2 uvTopRight = { uvBottomRight.x, uvTopLeft.y };
        Vector2 uvBottomLeft = { uvTopLeft.x, uvBottomRight.y };

        result.push_back({ v11 + pos, color, uvTopLeft });
        result.push_back({ v12 + pos, color, uvTopRight });
        result.push_back({ v21 + pos, color, uvBottomLeft });
        result.push_back({ v21 + pos, color, uvBottomLeft });
        result.push_back({ v12 + pos, color, uvTopRight });
        result.push_back({ v22 + pos, color, uvBottomRight });

        return result;
    }

    float TextRenderData::calculateWidth(std::string_view text, const Render::Font *font, float fontSize)
    {
        float scale = fontSize / 5.0f;

        float result = 0.0f;

        for (size_t i = 0; i < text.size(); ++i) {
            const Render::Glyph &g = font->mGlyphs[text[i]];

            result += g.mAdvance / 64.0f * scale;
        }

        return result;
    }

    Rect2 TextRenderData::calculateBoundingBox(std::string_view text, Vector3 pos, Vector2 size, const Render::Font *font, float fontSize, Vector2 pivot)
    {
        float scaleX = fontSize / 5.0f;
        float scaleY = fontSize / 5.0f;

        float minY = font->mDescender / 64.0f * scaleY;
        float maxY = font->mAscender / 64.0f * scaleY;
        float fullHeight = maxY - minY;
        float fullWidth = calculateWidth(text, font, fontSize);

        float cursorX = (size.x - fullWidth) * pivot.x;
        float baseY = (size.y - fullHeight) * pivot.y;

        return {
            pos.xy() + Vector2 { cursorX, baseY },
            { fullWidth, fullHeight }
        };
    }

}
}