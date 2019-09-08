#include "opengllib.h"

#include "openglfontloader.h"

#include "openglfontdata.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"

#include "Modules/math/atlas2.h"
#include "Modules/math/vector2i.h"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace Engine {
namespace Render {

    OpenGLFontLoader::OpenGLFontLoader()
        : ResourceLoader({ ".ttf" })
    {
    }

    std::shared_ptr<OpenGLFontData> OpenGLFontLoader::loadImpl(ResourceType *res)
    {

        FT_Library ft;
        if (FT_Init_FreeType(&ft)) {
            LOG_ERROR("FREETYPE: Could not init FreeType Library");
            return {};
        }

        FT_Face face;
        if (FT_New_Face(ft, res->path().c_str(), 0, &face)) {
            LOG_ERROR("FREETYPE: Failed to load font");
        }

        FT_Set_Pixel_Sizes(face, 0, 48);

        std::shared_ptr<OpenGLFontData> data = std::make_shared<OpenGLFontData>();

        std::vector<Vector2i> sizes;
        sizes.resize(128);

        for (GLubyte c = 0; c < 128; c++) {
            // Load character glyph
            if (FT_Load_Char(face, c, FT_LOAD_DEFAULT)) {
                LOG_ERROR("FREETYTPE: Failed to load Glyph");
                sizes[c] = { 0, 0 };
                continue;
            }
            sizes[c] = { static_cast<int>(face->glyph->bitmap.width), static_cast<int>(face->glyph->bitmap.rows) };
        }

        constexpr int UNIT_SIZE = 256;

        Atlas2 atlas({ UNIT_SIZE, UNIT_SIZE });
        atlas.addBin({ 0, 0 });

        int areaSize = 1;

        auto expand = [&]() {
            for (int i = 0; i < areaSize; ++i) {
                for (int j = 0; j < areaSize; ++j) {
                    atlas.addBin({ j * UNIT_SIZE, (areaSize + i) * UNIT_SIZE });
                    atlas.addBin({ (areaSize + j) * UNIT_SIZE, i * UNIT_SIZE });
                    atlas.addBin({ (areaSize + j) * UNIT_SIZE, (areaSize + i) * UNIT_SIZE });
                }
            }
            areaSize *= 2;
        };

        std::vector<Atlas2::Entry> entries = atlas.insert(
            sizes, expand, true);

        data->mTextureSize = { areaSize * UNIT_SIZE,
            areaSize * UNIT_SIZE };
        data->mTexture.setData(data->mTextureSize, nullptr);

        for (GLubyte c = 0; c < 128; c++) {

            // Load character glyph
            if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
                LOG_ERROR("FREETYTPE: Failed to load Glyph");
                continue;
            }

            data->mGlyphs[c].mSize = sizes[c];
            data->mGlyphs[c].mUV = entries[c].mArea.mTopLeft;
            data->mGlyphs[c].mFlipped = entries[c].mFlipped;
            data->mGlyphs[c].mAdvance = face->glyph->advance.x;
            data->mGlyphs[c].mBearingY = face->glyph->bitmap_top;

            Vector2i size = sizes[c];
            Vector2i pos = entries[c].mArea.mTopLeft;
            if (entries[c].mFlipped)
                std::swap(size.x, size.y);
            pos = { pos.x, areaSize * UNIT_SIZE - pos.y - size.y };

            std::unique_ptr<Vector4[]> colors = std::make_unique<Vector4[]>(size.x * size.y);
            for (int y = 0; y < size.y; ++y) {
                for (int x = 0; x < size.x; ++x) {
                    int index = (size.y - y - 1) * size.x + x;
                    int sourceIndex = entries[c].mFlipped ? x * size.y + y : y * size.x + x;

                    colors[index] = Vector4 { 1, 1, 1, 1 } * (face->glyph->bitmap.buffer[sourceIndex] / 255.0f);
                }
            }

            data->mTexture.setSubData(pos, size, colors.get());
        }

        FT_Done_Face(face);
        FT_Done_FreeType(ft);

        return data;
    }

}
}

UNIQUECOMPONENT(Engine::Render::OpenGLFontLoader);

METATABLE_BEGIN(Engine::Render::OpenGLFontLoader)
MEMBER(mResources)
METATABLE_END(Engine::Render::OpenGLFontLoader)

METATABLE_BEGIN(Engine::Render::OpenGLFontLoader::ResourceType)
METATABLE_END(Engine::Render::OpenGLFontLoader::ResourceType)

RegisterType(Engine::Render::OpenGLFontLoader);