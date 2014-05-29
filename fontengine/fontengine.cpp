#include "fontengine.h"
#include "shaderprogram.h"
#include "../textureatlas.h"
#include "../debug.h"
#include "utils.h"
#ifdef TSUKI_OS_WIN
    #include "win/winhacks.h"
#endif
#include <freetype/tttables.h>
#undef __FTERRORS_H__
#define FT_ERRORDEF( e, v, s )  { e, s },
#define FT_ERROR_START_LIST     {
#define FT_ERROR_END_LIST       { 0, 0 } };
const struct {
    int          code;
    const char*  message;
} FT_errors[] =
#include FT_ERRORS_H
;


namespace Tsuki {
/* predefined constants */
#define BUFFERSIZE 265
#define ATLASSIZE 512
#define CHARPOOL L" !\"#$%&'()*+,.-/0123456789:;<=>?»" \
                 L"@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_" \
                 L"`abcdefghijklmnopqrstuvwxyz{|}~"

    struct _size {
        unsigned int width;
        unsigned int height;
    };

    FontEngine::FontEngine() : error(0), nSymbols(0), pShaderProg(0) {
        defaultAtlasSize_m = ATLASSIZE;
        defaultCharpool_m = CHARPOOL;
        defaultOrigin = OriginBaseLeft;

        error = FT_Init_FreeType(&ftlib);
        if(error != 0)
            fatal("Can't initialize FontEngine\n: FT error (0x%02x): %s", FT_errors[error].code, FT_errors[error].message);

        // generate vbo id
        //glGenBuffers(1, &vboId);

        // initialize shader
        pShaderProg = new ShaderProgram();
        pShaderProg->addShaderFromFile("./data/shader/font.vert", GL_VERTEX_SHADER);
        pShaderProg->addShaderFromFile("./data/shader/font.frag", GL_FRAGMENT_SHADER);
        pShaderProg->link();

        posLoc = pShaderProg->getAttributeLocation("aPosition");
        texLoc = pShaderProg->getAttributeLocation("aTexCoord");
        gammaLoc = pShaderProg->getUniformLocation("aGamma");
        colorLoc = pShaderProg->getUniformLocation("aColor");

        //vertices = new DynBuffer(1024, 10, 1.25, 0.50);
        //vertices = (unsigned short *)malloc(sizeof(unsigned short)*(1024));
        //texVertices = (float *)malloc(sizeof(float)*(1024));
    }

    FontEngine::~FontEngine() {
        delete pShaderProg;
        //delete vertices;

        std::map<const char *, Font *>::iterator it = this->fonts.begin();
        while(it != this->fonts.end()) {
            delete it->second;
            ++it;
        }
        fonts.clear();

        for(unsigned int i = 0; i < Face::atlases.size(); ++i)
            delete Face::atlases[i];
        Face::atlases.clear();

        FT_Done_FreeType(ftlib);
    }

    template<> FontEngine* Singleton<FontEngine>::m_instance = 0;
    FontEngine *FontEngine::instance() {
        assert(m_instance);
        return m_instance;
    }

    // NOTE: use READ_ONLY macro ?
    FT_Library FontEngine::getFTLibrary() {
        return this->ftlib;
    }

    unsigned int FontEngine::getDefaultAtlasSize() {
        return this->defaultAtlasSize_m;
    }

    const wchar_t *FontEngine::getDefaultCharpool() {
        return this->defaultCharpool_m;
    }

    bool FontEngine::setDefaultAtlasSize(unsigned int size) {
        if(size < 1 || Utils::isNumberPowerOfTwo(size)) {
            this->defaultAtlasSize_m = size;
            return true;
        }
        return false;
    }

    // TODO: add \n some day
    Font *FontEngine::newFont(const char *fontfile, unsigned int size, const wchar_t *charpool) {
        Font *font;

        std::map<const char *, Font *>::iterator it;
        if((it = fonts.find(fontfile)) != fonts.end())
            font = it->second;
        else
            font = new Font();

        /* bind vbo to fill in font class */
        //glBindBuffer(GL_ARRAY_BUFFER, vboId);
        error = font->loadFromFile(fontfile, size, charpool);
        if(error != 0) {
            if(error == FE_FONT_ALREADY_LOADED)
                return font;
        }

        fonts[fontfile] = font;

        return font;
    }

    _size FontEngine::getStringMetrics(Font *font, unsigned int size, const wchar_t *text) {
        _size strSize;
        strSize.width = 0;
        strSize.height = 0;

        if(text == NULL || !font)
            return strSize;

        if(font->faces.count(size) == 0)
            return strSize;

        Face *face = font->faces.at(size);

        Face::glyph_t *glyph;
        std::map<wchar_t, Face::glyph_t *>::iterator it;
        unsigned int length = wcslen(text);

        for(unsigned int i = 0; i < length; ++i) {
            it = face->glyphs.find(text[i]);
            if(it == face->glyphs.end())
                continue;

            glyph = it->second;
            strSize.width += glyph->width;
            if(i != length-2)
                strSize.width += (int)glyph->advancex-glyph->width;
            if(strSize.height < glyph->height)
                strSize.height = glyph->height;
        }

        return strSize;
    }

    // TODO: implement getErrorString()
    const char *FontEngine::getErrorString() {
        return "";
    }

    int FontEngine::addTextToBuffer(int x, int y, Font *font, unsigned int size, const wchar_t *text, FontEngine::text_origin origin) {
        if(text == NULL)
            return FE_INVALID_PARAMETER;

        fe_string *buf = new fe_string;
        buf->x = x;
        buf->y = y;
        buf->font = font;
        buf->size = size;
        buf->txt = text;
        buf->origin = origin;
        textBuffer.push_back(buf);

        return FE_OK;
    }

    // TODO: add color support
    int FontEngine::drawTextBuffer() {
        int n = 0;
        int m = 0;
        int bufOffset = 0;
        int indOffset = 0;
        int ret = FE_OK;

        // allocate buffer memory
        unsigned short *indices = (unsigned short *)malloc(sizeof(unsigned short)*(nSymbols*6));

        //vertices->requestData((sizeof(unsigned short)+sizeof(float))*(nSymbols*8));
        //unsigned short *vert = (unsigned short *)vertices->getData();
        //float *texVert = (float *)((char *)vertices->getData()+(sizeof(unsigned short)*(nSymbols*8)));
        unsigned short *vert = (unsigned short *)malloc(sizeof(unsigned short)*nSymbols*8);
        float *texVert = (float *)malloc(sizeof(float)*nSymbols*8);

        while(!textBuffer.empty()) {
            fe_string *buf = textBuffer.front();
            int x = buf->x;
            int y = buf->y;
            Font *font = buf->font;
            unsigned int size = buf->size;
            const wchar_t *text = buf->txt;
            FontEngine::text_origin origin = buf->origin;

            if(font->faces.count(size) == 0) {
                ret = FE_FONT_NOT_LOADED;
                warning("FontEngine: font %s in size %d not loaded", font->filename(), size);

                nSymbols -= wcslen(buf->txt);
                free((void *)textBuffer.front()->txt);
                delete textBuffer.front();
                textBuffer.pop_front();

                continue;
            }

            Face *face = font->faces.at(size);

            // check if atlas is loaded
            GLint texid;
            glEnable(GL_TEXTURE_2D);
            glEnable(GL_BLEND);
            glDisable(GL_DEPTH_TEST);

            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            glGetIntegerv(GL_TEXTURE_BINDING_2D, &texid);
            if(texid != (int)face->atlas->glid)
                glBindTexture(GL_TEXTURE_2D, face->atlas->glid);

            int penx = x;
            int peny = y;

            // set initial pen y coordiante
            // HACK: shouldn't manipulate parameter
            if(origin == OriginTopLeft || origin == OriginTopRight)
                peny = y += face->ascender_m + face->descender_m + 1;
            else if(origin == OriginBottomLeft || origin == OriginBottomRight)
                peny = y += face->descender_m;

            // HACK: ugly way to get linegaps with right origin to work
            unsigned int length = wcslen(text);
            if(origin > 4) {
                for(unsigned int i = 0; i < length; ++i)
                    if(text[i] == '\n')
                        peny += face->linegap_m;
            }

            Face::glyph_t *glyph;
            std::map<wchar_t, Face::glyph_t *>::iterator it;

            for(unsigned int i = 0; i < length; ++i) {
                // line break
                if(text[i] == '\n') {
                    if(origin < 5) {
                        peny += face->linegap_m;
                        penx = x;
                    } else {
                        peny -= face->linegap_m;
                        penx = x;
                    }
                    continue;
                }

                it = face->glyphs.find(text[(origin < 5) ? i : length-(i+1)]);
                if(it == face->glyphs.end()) {
                    ret = FE_GLYPH_NOT_CACHED;
                    warning("FontEngine: glyph %lc(0x%x) not cached", text[(origin < 5) ? i : length-(i+1)], (int)text[(origin < 5) ? i : length-(i+1)]);
                    --nSymbols;
                    continue;
                }

                unsigned short x0;
                unsigned short y0;
                unsigned short x1;
                unsigned short y1;

                glyph = it->second;
                float u0 = glyph->u0;
                float v0 = glyph->v0;
                float u1 = glyph->u1;
                float v1 = glyph->v1;

                // right aligned
                if(origin > 4)
                    penx -= (int)glyph->advancex;

                x0 = (unsigned short)(penx + glyph->offsetx);
                y0 = (unsigned short)(peny - glyph->offsety);
                x1 = (unsigned short)(x0 + glyph->width);
                y1 = (unsigned short)(y0 + glyph->height);

                vert[n] = x0;
                texVert[n++] = u0;
                vert[n] = y0;
                texVert[n++] = v0;
                vert[n] = x0;
                texVert[n++] = u0;
                vert[n] = y1;
                texVert[n++] = v1;
                vert[n] = x1;
                texVert[n++] = u1;
                vert[n] = y1;
                texVert[n++] = v1;
                vert[n] = x1;
                texVert[n++] = u1;
                vert[n] = y0;
                texVert[n++] = v0;

                indices[m++] = indOffset;
                indices[m++] = indOffset+1;
                indices[m++] = indOffset+2;
                indices[m++] = indOffset;
                indices[m++] = indOffset+2;
                indices[m++] = indOffset+3;
                indOffset += 4;

                // left aligned
                if(origin < 5)
                    penx += (int)glyph->advancex;
            }
            free((void *)textBuffer.front()->txt);
            delete textBuffer.front();
            textBuffer.pop_front();
        }

        pShaderProg->bind();
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);


        glUniform1f(gammaLoc, 1.75);
        glVertexAttribPointer(posLoc, 2, GL_SHORT, GL_FALSE, bufOffset, vert);
        glEnableVertexAttribArray(posLoc);

        glVertexAttribPointer(texLoc, 2, GL_FLOAT, GL_FALSE, bufOffset, texVert);
        glEnableVertexAttribArray(texLoc);

        bufOffset = n;

        glDrawElements(GL_TRIANGLES, nSymbols*6, GL_UNSIGNED_SHORT, indices);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        pShaderProg->unbind();

        free(indices);

        // TODO: free loop here for error case
        textBuffer.clear();
        nSymbols = 0;

        return ret;
    }

    int FontEngine::print(int x, int y, Font *font, unsigned short size, FontEngine::text_origin origin, const char *format, ...) {
        va_list args;
        int ret;

        va_start(args, format);
        char *buf = (char *)calloc(1, BUFFERSIZE);

        ret = vsprintf(buf, format, args);
        if(ret < 1)
            return ret;

        nSymbols += ret;
        va_end(args);

        wchar_t *wbuf = (wchar_t *)calloc(ret+1, sizeof(wchar_t));
        mbstowcs(wbuf, buf, (ret+1)*sizeof(wchar_t));
        free(buf);

        return this->addTextToBuffer(x, y, font, size, wbuf, origin);
    }

    int FontEngine::wprint(int x, int y, Font *font, unsigned short size, const wchar_t *format, ...) {
        va_list args;
        int ret;

        va_start(args, format);
        wchar_t *buf = (wchar_t *)calloc(1, BUFFERSIZE);

        if((ret = vswprintf(buf, BUFFERSIZE, format, args)) < 1)
            return ret;

        nSymbols += ret;
        va_end(args);

        return this->addTextToBuffer(x, y, font, size, buf, defaultOrigin);
    }

    int FontEngine::wprint(int x, int y, Font *font, unsigned short size, FontEngine::text_origin origin, const wchar_t *format, ...) {
        va_list args;
        int ret;

        va_start(args, format);
        wchar_t *buf = (wchar_t *)calloc(1, BUFFERSIZE);

        if((ret = vswprintf(buf, BUFFERSIZE, format, args)) < 1)
            return ret;

        nSymbols += ret;
        va_end(args);

        return this->addTextToBuffer(x, y, font, size, buf, origin);
    }
}
