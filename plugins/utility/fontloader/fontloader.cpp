#include "fontloaderlib.h"

#include "Modules/keyvalue/metatable_impl.h"

#include "fontloader.h"

#include "Modules/math/atlas2.h"
#include "Modules/math/vector2i.h"

#include "Modules/filesystem/filemanager.h"
#include "Modules/serialize/formatter/safebinaryformatter.h"
#include "Modules/serialize/streams/serializestream.h"

#include "Modules/generic/areaview.h"
#include "Modules/generic/bytebuffer.h"

#include "Modules/math/vector3.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H

#undef INFINITE
#include "msdfgen.h"

#include "core/edge-coloring.h"

UNIQUECOMPONENT(Engine::Render::FontLoader)

METATABLE_BEGIN_BASE(Engine::Render::FontLoader::ResourceType, Engine::Resources::ResourceBase)
METATABLE_END(Engine::Render::FontLoader::ResourceType)

RegisterType(Engine::Render::FontLoader)
    METATABLE_BEGIN(Engine::Render::FontLoader)
        MEMBER(mResources)
            METATABLE_END(Engine::Render::FontLoader)

                namespace msdfgen
{

    struct FtContext {
        Point2 position;
        Shape *shape;
        Contour *contour;
    };

    static Point2 ftPoint2(const FT_Vector &vector)
    {
        return Point2(vector.x / 64., vector.y / 64.);
    }

    static int ftMoveTo(const FT_Vector *to, void *user)
    {
        FtContext *context = reinterpret_cast<FtContext *>(user);
        context->contour = &context->shape->addContour();
        context->position = ftPoint2(*to);
        return 0;
    }

    static int ftLineTo(const FT_Vector *to, void *user)
    {
        FtContext *context = reinterpret_cast<FtContext *>(user);
        context->contour->addEdge(new LinearSegment(context->position, ftPoint2(*to)));
        context->position = ftPoint2(*to);
        return 0;
    }

    static int ftConicTo(const FT_Vector *control, const FT_Vector *to, void *user)
    {
        FtContext *context = reinterpret_cast<FtContext *>(user);
        context->contour->addEdge(new QuadraticSegment(context->position, ftPoint2(*control), ftPoint2(*to)));
        context->position = ftPoint2(*to);
        return 0;
    }

    static int ftCubicTo(const FT_Vector *control1, const FT_Vector *control2, const FT_Vector *to, void *user)
    {
        FtContext *context = reinterpret_cast<FtContext *>(user);
        context->contour->addEdge(new CubicSegment(context->position, ftPoint2(*control1), ftPoint2(*control2), ftPoint2(*to)));
        context->position = ftPoint2(*to);
        return 0;
    }
}

namespace Engine {
namespace Render {

    FontLoader::FontLoader()
        : ResourceLoader({ ".msdf", ".ttf" })
    {
    }

    bool FontLoader::loadImpl(Font &font, ResourceType *res)
    {
        font.mTexture.create(res->name(), FORMAT_UNSIGNED_BYTE);
        font.mTexture.setMinMode(MIN_NEAREST);
        font.mTextureHandle = font.mTexture->mTextureHandle;

        if (res->path().extension() == ".msdf") {

            Filesystem::FileManager cache("msdf_cache");
            Serialize::SerializeInStream in = cache.openRead(res->path().parentPath() / (res->name() + ".msdf"), std::make_unique<Serialize::SafeBinaryFormatter>());
            assert(in);
            in >> font.mGlyphs;
            in >> font.mTextureSize;
            ByteBuffer b;
            in >> b;
            font.mTexture.setData(font.mTextureSize, b);

        } else if (res->path().extension() == ".ttf") {
            LOG("Creating Cache for " << res->path());

            FT_Library ft;
            if (FT_Init_FreeType(&ft)) {
                LOG_ERROR("FREETYPE: Could not init FreeType Library");
                return false;
            }

            std::vector<unsigned char> fontBuffer = res->readAsBlob();

            FT_Face face;
            if (FT_New_Memory_Face(ft, fontBuffer.data(), fontBuffer.size(), 0, &face)) {
                FT_Done_FreeType(ft);
                LOG_ERROR("FREETYPE: Failed to load font");
                return false;
            }

            FT_Set_Pixel_Sizes(face, 0, 64);

            std::vector<Vector2i> sizes;
            sizes.resize(128);
            std::vector<Vector2i> extendedSizes;
            extendedSizes.resize(128);

            for (unsigned char c = 0; c < 128; c++) {
                // Load character glyph
                if (FT_Load_Char(face, c, FT_LOAD_DEFAULT)) {
                    LOG_ERROR("FREETYTPE: Failed to load Glyph");
                    sizes[c] = { 0, 0 };
                    extendedSizes[c] = { 0, 0 };
                    continue;
                }
                sizes[c] = { static_cast<int>(face->glyph->bitmap.width) + 2, static_cast<int>(face->glyph->bitmap.rows) + 2 };
                extendedSizes[c] = { static_cast<int>(face->glyph->bitmap.width) + 3, static_cast<int>(face->glyph->bitmap.rows) + 3 };
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
                extendedSizes, expand, true);

            font.mTextureSize = { areaSize * UNIT_SIZE,
                areaSize * UNIT_SIZE };
            size_t byteSize = font.mTextureSize.x * font.mTextureSize.y;
            std::unique_ptr<std::array<unsigned char, 4>[]> texBuffer = std::make_unique<std::array<unsigned char, 4>[]>(byteSize);
            AreaView<std::array<unsigned char, 4>, 2> tex { texBuffer.get(), { static_cast<size_t>(font.mTextureSize.x), static_cast<size_t>(font.mTextureSize.y) } };
            tex.flip(1);

            for (unsigned char c = 0; c < 128; c++) {

                // Load character glyph
                if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
                    LOG_ERROR("FREETYTPE: Failed to load Glyph");
                    continue;
                }

                std::unique_ptr<Vector3[]> buffer = std::make_unique<Vector3[]>(sizes[c].x * sizes[c].y);
                AreaView<Vector3, 2> bufferView { buffer.get(), { static_cast<size_t>(sizes[c].x), static_cast<size_t>(sizes[c].y) } };

                msdfgen::BitmapRef<float, 3>
                    bm { buffer[0].ptr(), sizes[c].x, sizes[c].y };

                msdfgen::Shape shape;
                shape.inverseYAxis = true;

                msdfgen::FtContext context = {};
                context.shape = &shape;
                FT_Outline_Funcs ftFunctions;
                ftFunctions.move_to = &msdfgen::ftMoveTo;
                ftFunctions.line_to = &msdfgen::ftLineTo;
                ftFunctions.conic_to = &msdfgen::ftConicTo;
                ftFunctions.cubic_to = &msdfgen::ftCubicTo;
                ftFunctions.shift = 0;
                ftFunctions.delta = 0;
                FT_Outline_Decompose(&face->glyph->outline, &ftFunctions, &context);

                msdfgen::edgeColoringSimple(shape, 3);
                msdfgen::generateMSDF(bm, shape, 4.0, { 1, 1 }, { static_cast<double>(-face->glyph->bitmap_left + 1), static_cast<double>(sizes[c].y - face->glyph->bitmap_top - 1) });

                font.mGlyphs[c].mSize = sizes[c];
                font.mGlyphs[c].mUV = entries[c].mArea.mTopLeft;
                font.mGlyphs[c].mFlipped = entries[c].mFlipped;
                font.mGlyphs[c].mAdvance = face->glyph->advance.x;
                font.mGlyphs[c].mBearingY = face->glyph->bitmap_top - 1;

                Vector2i size = sizes[c];
                if (entries[c].mFlipped)
                    std::swap(size.x, size.y);
                Vector2i pos = { entries[c].mArea.mTopLeft.x, entries[c].mArea.mTopLeft.y };

                AreaView<std::array<unsigned char, 4>, 2> targetView = tex.subArea({ static_cast<size_t>(pos.x), static_cast<size_t>(pos.y) }, { static_cast<size_t>(size.x), static_cast<size_t>(size.y) });
                if (entries[c].mFlipped)
                    targetView.swapAxis(0, 1);

                std::transform(bufferView.begin(), bufferView.end(), targetView.begin(),
                    [](const Vector3 &v) {
                        return std::array<unsigned char, 4> {
                            static_cast<unsigned char>(clamp(v.x, 0.0f, 1.0f) * 255),
                            static_cast<unsigned char>(clamp(v.y, 0.0f, 1.0f) * 255),
                            static_cast<unsigned char>(clamp(v.z, 0.0f, 1.0f) * 255),
                            255
                        };
                    });
            }

            FT_Done_Face(face);
            FT_Done_FreeType(ft);

            font.mTexture.setData(font.mTextureSize, { texBuffer.get(), 4 * byteSize });

            Filesystem::FileManager cache("msdf_cache");
            Serialize::SerializeOutStream out = cache.openWrite(res->path().parentPath() / (res->name() + ".msdf"), std::make_unique<Serialize::SafeBinaryFormatter>());
            if (out) {
                out << font.mGlyphs;
                out << font.mTextureSize;
                out << ByteBuffer { texBuffer.get(), 4 * byteSize };
            }

        } else {
            std::terminate();
        }

        return true;
    }

    void FontLoader::unloadImpl(Font &font, ResourceType *res)
    {
    }

}
}