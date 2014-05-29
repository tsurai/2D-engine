#include "font.h"
#include "face.h"
#include <ft2build.h>
#include "fontengine.h"
#include FT_FREETYPE_H

namespace Tsuki {
    Font::Font() {
        faces.clear();
    }

    Font::~Font() {
        std::map<unsigned int, Face *>::iterator it = this->faces.begin();
        while(it != this->faces.end()) {
            delete it->second;
            ++it;
        }
        this->faces.clear();
    }

    int Font::loadSize(unsigned int size) {
        /* is Face already loaded */
        if(faces.count(size) == 0)
            return FE_FONT_ALREADY_LOADED;

        FT_Face ftface = NULL;
        int error = 0;

        /* load font faces from file */
        error = FT_New_Face(FontEngine::instance()->getFTLibrary(), this->filename_m, 0, &ftface);
        if(error != 0)
            return error;

        /* select charmap encoding */
        error = FT_Select_Charmap(ftface, FT_ENCODING_UNICODE);
        if(error != 0)
            return error;

        Face *face = new Face();
        error = face->load(ftface, size, FontEngine::instance()->getDefaultCharpool(), -1);
        if(error != 0)
            return error;

        this->faces[size] = face;

        return FE_OK;
    }

    face_metrics_t Font::getFaceMetrics(unsigned int size) {
        face_metrics_t ret;
        std::map<unsigned int, Face *>::iterator it;
        if((it = this->faces.find(size)) == this->faces.end())
            return ret;

        Face *face = it->second;
        ret.size = face->size();
        ret.ascender = face->ascender();
        ret.descender = face->descender();
        ret.linegap = face->linegap();
        ret.maxHeight = face->maxHeight();
        ret.maxAdvanceHeight = face->maxAdvanceHeight();
        ret.maxAvanceWidth = face->maxAdvanceWidth();

        return ret;
    }

    int Font::loadFromFile(const char *filename, unsigned int size, const wchar_t *charpool) {
        if(!filename || !charpool)
            return FE_INVALID_PARAMETER;

        /* is face already loaded */
        if(faces.count(size) == 1)
            return FE_FONT_ALREADY_LOADED;

        FT_Face ftface = NULL;
        int error = 0;

        // load font faces from file
        error = FT_New_Face(FontEngine::instance()->getFTLibrary(), filename, 0, &ftface);
        if(error != 0)
            return error;

        // TODO: feature to select encodings
        // select charmap encoding
        error = FT_Select_Charmap(ftface, FT_ENCODING_UNICODE);
        if(error != 0)
            return error;

        // get truetype os2 table
        this->tt_os2_m = (TT_OS2 *)FT_Get_Sfnt_Table(ftface, ft_sfnt_os2);
        if(!this->tt_os2_m && (this->tt_os2_m->version == 0xFFFF))
            this->tt_os2_m = NULL;

        this->filename_m = filename;
        this->familyname_m = ftface->family_name;
        this->stylename_m = ftface->style_name;
        this->weight_m = tt_os2_m->usWeightClass;

        Face *face = new Face();
        error = face->load(ftface, size, charpool, -1);
        if(error != 0)
            return error;

        this->faces[size] = face;

        return FE_OK;
    }

    // TODO: add loadByName stuff sometime
    /*
    int Font::loadByName(const char *familyname, bool bold, bool italic) {

        return 0;
    }*/
}
