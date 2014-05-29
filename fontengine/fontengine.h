#ifndef FONTENGINE_H
#define FONTENGINE_H

#include "../prerequisites.h"
#include "../singleton.h"
#include "font.h"
#include "fonterror.h"
#include <ft2build.h>
#include FT_FREETYPE_H

namespace Tsuki {
    struct _size;
    class Font;

    class FontEngine : public Singleton<FontEngine>
    {
    public:
        enum {
           OriginTopLeft = 0x1,
           OriginBaseLeft = 0x3,
           OriginBottomLeft = 0x4,
           OriginTopRight = 0x5,
           OriginBaseRight = 0x6,
           OriginBottomRight = 0x7
        } typedef text_origin;

        typedef struct {
            int x;
            int y;
            Font *font;
            unsigned short size;
            text_origin origin;
            const wchar_t *txt;
        } fe_string;

        FontEngine();
        ~FontEngine();

        static FontEngine *instance();

        FT_Library getFTLibrary();

        /* getter & setter */
        unsigned int getDefaultAtlasSize();
        bool setDefaultAtlasSize(unsigned int size);
        const wchar_t *getDefaultCharpool();
        bool setDefaultCharpool(const wchar_t *charpool);

        Font *newFont(const char *fontfile, unsigned int size, const wchar_t *charpool);
        inline Font *newFont(const char *fontfile, unsigned int size) {
            return newFont(fontfile, size, defaultCharpool_m);
        }

        _size getStringMetrics(Font *font, unsigned int size, const wchar_t *text);
        int getLastError() { return error; }
        const char *getErrorString();

        /* use wprint for better performance */
        int print(int x, int y, Font *font, unsigned short size, text_origin origin, const char *text);
        int print(int x, int y, Font *font, unsigned short size, text_origin origin, const char *format, ...);
        /*inline int print(int x, int y, Font *font, unsigned short size, const char *format, va_list args) {
            return print(x, y, font, size, defaultOrigin, format, args);
        }
        inline int print(int x, int y, unsigned short size, const char *format, va_list args) {
            return print(x, y, defFont, size, defaultOrigin, format, args);
        }*/

        int wprint(int x, int y, Font *font, unsigned short size, const wchar_t *format, ...);
        int wprint(int x, int y, Font *font, unsigned short size, text_origin origin, const wchar_t *format, ...);

        int drawTextBuffer();

        text_origin defaultOrigin;
        Font *defFont;
    private:
        FT_Library ftlib;
        FT_Face face;

        unsigned int vboId;
        int error;
        unsigned int nSymbols;
        unsigned int defaultAtlasSize_m;
        const wchar_t *defaultCharpool_m;
        std::map<const char *, Font *> fonts;
        std::list<fe_string *> textBuffer;
        unsigned int posLoc;
        unsigned int texLoc;
        unsigned int gammaLoc;
        unsigned int colorLoc;
        ShaderProgram *pShaderProg;

        DynBuffer *vertices;
        //unsigned short *vertices;
        //float *texVertices;

        int addTextToBuffer(int x, int y, Font *font, unsigned int size, const wchar_t *text, text_origin origin);
        //int drawText(int x, int y, Font *font, unsigned int size, const wchar_t *text, text_origin origin);
    };
}

#endif // FONTENGINE_H
