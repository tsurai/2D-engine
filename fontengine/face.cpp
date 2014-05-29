#include "../textureatlas.h"
#include "debug.h"
#include "face.h"
#include "fonterror.h"
#include <algorithm>
#include <GL/gl.h>
#include <GL/glext.h>

namespace Tsuki {
    typedef struct {
        unsigned short x, y;
        float u, v;
        short advx, advy;
    } font_vert;

    std::vector<TextureAtlas *> Face::atlases;

    Face::Face() {
        glyphs.clear();
    }

    Face::~Face() {
        std::map<wchar_t, glyph_t *>::iterator it = this->glyphs.begin();
        while(it != this->glyphs.end()) {
            delete it->second;
            ++it;
        }
        this->glyphs.clear();
    }

    int Face::load(FT_Face face, unsigned int size, const wchar_t *charpool, int atlasId) {
        if(!face || !charpool)
            return FE_INVALID_PARAMETER;

        int error;
        size_t hres = 64;
        FT_Matrix matrix = { (int)((1.0/hres)   * 0x10000L),
                             (int)((0.0)      * 0x10000L),
                             (int)((0.0)      * 0x10000L),
                             (int)((1.0)      * 0x10000L) };

        error = FT_Set_Char_Size(face, (int)(size << 6), 0, 72*hres, 72);
        if(error != 0)
            return error;

        FT_Set_Transform(face, &matrix, NULL);

        TextureAtlas *atlas;
        if(Face::atlases.empty())
            atlas = new TextureAtlas(/*FontEngine::instance()->getDefaultAtlasSize()*/ 512, /*FontEngine::instance()->getDefaultAtlasSize()*/ 512, 1);
        else {
            if(atlasId < 0 || (int)Face::atlases.size() > atlasId)
                atlas = (atlasId < 0) ? Face::atlases.back() : Face::atlases.at(atlasId);
            else
                return FE_INVALID_ATLAS_ID;
        }

        int atlas_width = atlas->getWidth();
        int atlas_height = atlas->getHeight();
        int atlas_depth = atlas->getDepth();
        FT_GlyphSlot slot;
        FT_Bitmap bitmap;

        FT_Int32 flags = FT_LOAD_RENDER;
        flags |= FT_LOAD_FORCE_AUTOHINT;

        FT_Size_Metrics metrics = face->size->metrics;
        this->size_m = size;
        this->height_m = (metrics.height  >> 6);
        this->ascender_m = (metrics.ascender >> 6);
        this->descender_m = (metrics.descender >> 6);
        this->linegap_m = (height_m - ascender_m - descender_m) * 1.75f;

        // NOTE: linegap calculation seems to be semi fine -> control this stuff
        unsigned int legnth = wcslen(charpool);
        for(unsigned int i = 0; i < legnth; i++) {
            error = FT_Load_Char(face, charpool[i], flags);
            if(error != 0)
                return error;

            slot = face->glyph;
            bitmap = slot->bitmap;

            rect_t region = atlas->getRect(bitmap.width/atlas_depth+1, bitmap.rows+1);
            if(region.x < 0) {
                critical() << "Texture Atlas is full";
                this->reroll(atlas);
                return FE_ATLAS_IS_FULL;
            }

            //font_vert *vert = new font_vert[2];
            glyph_t *glyph = new glyph_t;
            glyph->charcode = charpool[i];
            glyph->width = (int)(bitmap.width/atlas_depth);
            glyph->height = bitmap.rows;
            glyph->offsetx = slot->bitmap_left;
            glyph->offsety = slot->bitmap_top;
            glyph->regionx = region.x;
            glyph->regiony = region.y;
            glyph->u0 = region.x/(float)atlas_width;
            glyph->v0 = region.y/(float)atlas_height;
            glyph->u1 = (region.x + glyph->width)/(float)atlas_width;
            glyph->v1 = (region.y + glyph->height)/(float)atlas_height;
            glyph->advancex = slot->advance.x/64.0f;
            glyph->advancey = slot->advance.y/64.0f;

            this->glyphs[charpool[i]] = glyph;

            /* fill vbo */
            //glBufferSubData(GL_ARRAY_BUFFER, (atlas->getNodeCount()*2)*sizeof(font_vert), sizeof(font_vert), )

            atlas->setRect(region.x, region.y, bitmap.width/atlas_depth, bitmap.rows, bitmap.buffer, bitmap.pitch);
        }

        atlas->upload();
        this->atlas = atlas;

        // TODO: manage the fucking atlases
        //std::vector<TextureAtlas *>::iterator it;
        if(find(Face::atlases.begin(), Face::atlases.end(), atlas) == Face::atlases.end())
            Face::atlases.push_back(atlas);

        FT_Done_Face(face);

        return FE_OK;
    }

    int Face::reroll(TextureAtlas *atlas) {
        if(!atlas)
            return FE_INVALID_PARAMETER;

        glyph_t *glyph;
        std::map<wchar_t, glyph_t *>::iterator it = this->glyphs.begin();
        while(it != this->glyphs.end()) {
            glyph = it->second;
            atlas->unsetRect(glyph->regionx, glyph->regiony, glyph->width, glyph->height);
            delete glyph;
            ++it;
        }

        return FE_OK;
    }
}
