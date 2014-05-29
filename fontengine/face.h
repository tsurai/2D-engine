#ifndef FACE_H
#define FACE_H

#include "../prerequisites.h"
#include "../common.h"
#include <ft2build.h>
#include FT_FREETYPE_H

namespace Tsuki {
    typedef struct {
        unsigned int width;
        unsigned int height;
        int offsetx;
        int offsety;
        float advancex;
        float advancey;
    } glyph_metrics_t;

    typedef struct {
        unsigned int size;
        int height;
        int linegap;
        int ascender;
        int descender;
        int maxAvanceWidth;
        int maxAdvanceHeight;
        int maxHeight;
    } face_metrics_t;

    class Face
    {
    public:
        friend class FontEngine;

        Face();
        ~Face();

        int load(FT_Face face, unsigned int size, const wchar_t *charpool, int atlasId);

        READ_ONLY(unsigned int, size)
        READ_ONLY(int, height)
        READ_ONLY(int, linegap)
        READ_ONLY(int, ascender)
        READ_ONLY(int, descender)
        READ_ONLY(int, maxAdvanceWidth)
        READ_ONLY(int, maxAdvanceHeight)
        READ_ONLY(int, maxHeight)
    private:
        typedef struct : public glyph_metrics_t {
            wchar_t charcode;
            float u0;
            float v0;
            float u1;
            float v1;
            int regionx;
            int regiony;
        } glyph_t;

        int reroll(TextureAtlas *atlas);

        TextureAtlas *atlas;
        std::map<wchar_t, glyph_t *> glyphs;
        static std::vector<TextureAtlas *> atlases;
    };
}

#endif // FACE_H
