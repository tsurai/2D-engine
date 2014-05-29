#include "imageformat.h"
#include "debug.h"
#include <GL/gl.h>
#include <GL/glext.h>

namespace Tsuki {
    Bitmap::Bitmap() {

    }

    Bitmap::~Bitmap() {

    }

    void Bitmap::load(const char *filename) {
        unsigned char *data;

        if(!filename) {
            warning("Invalid parameter");
            return;
        }
        this->filename = filename;

        FILE *f = fopen(filename, "rb");
        if(!f) {
            warning("Can't open Bitmap file %s", filename);
            return;
        }

        /* get file size */
        fseek(f, 0, SEEK_END);
        size = ftell(f);
        fseek(f, 0, SEEK_SET);

        /* read file into buffer */
        debug() << size;
        unsigned int ret;
        data = (unsigned char *)malloc(size);
        ret = fread(data, 1, size, f);
        if(ret != size) {
            warning("Couldn't read whole file %d/%d\n%s", ret, size, filename);
            return;
        }
        fclose(f);

        loadFromMemory(data);

        free(data);
    }

    void Bitmap::loadFromMemory(unsigned char *data) {
        /* parse file header */
        if(memcmp(data, "BM", 2) != 0) {
            warning("Invalid Bitmap format\n%s", filename);
            return;
        }

        if(memcmp(data+2, &size, 2) != 0) {
            warning("File is corrupted\n%s", filename);
            return;
        }
        dataOffset = (int)(data[0x0A]);

        /* parse dip header */
        type = (int)(data[0x0E]);
        width = (int)(data[0x12]);
        height = (int)(data[0x16]);

        bpp = (short)(data[0x1C]);
        if((bpp != 1) && (bpp != 4) && (bpp != 8) && (bpp != 16) && (bpp != 24) && (bpp != 32)) {
            warning("Invalid color depth\n%s", filename);
            return;
        }

        compression = (int)(data[0x1E]);
        if(compression == UNKNOWN_COMP || compression == BI_JPEG || compression == BI_PNG) {
            warning("Unsupported compression value\n%s", filename);
            return;
        }

        if((height < 0) && ((compression == BI_RLE8 && bpp != 8)
                || (compression == BI_RLE4 && bpp != 4)
                || (compression == BI_BITFIELDS && (bpp != 16 || bpp != 32)))) {
            warning("Corrupted DIP header\n%s", filename);
            return;
        }

        if(compression != BI_RGB)
            imgSize = (int)(data[0x22]);
        else
            imgSize = width * height * bpp / 8;

        /* ain't giving a shit about optimal color tables... expect crash sometime */
        nColors = (int)(data[0x2E]);
        if(nColors == 0) {
            if(bpp == 0)
                nColors = 2;
            else if(bpp == 4)
                nColors = 16;
            else if(bpp == 8)
                nColors = 256;
        }

        /* SKIP - ignore color bitmasks for now */

        /* jump to end of the header */
        data = data+14+type;

        /* parse color table */
        if(bpp == 1 || bpp == 4 || bpp == 8) {
            colorTable = (unsigned char *)malloc(4*nColors);
            memcpy(colorTable, data, 4*nColors);
        }

        /* jump to pixel data */
        debug() << imgSize;

        //data = data+dataOffset;
        imgData = (unsigned char *)malloc(imgSize);
        memcpy(imgData, data, imgSize);
    }

    int Bitmap::createTexture() {
        glTexId = 0;

        glEnable(GL_TEXTURE_2D);
        glGenTextures(1, &glTexId);
        glBindTexture(GL_TEXTURE_2D, glTexId);

        if(bpp == 1) {
            //glTexImage2D(GL_TEXTURE_2D, 0, 2, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, imgData);
        } else if(bpp == 24) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, imgData);
        } else if(bpp == 32) {

        }

        return glTexId;
    }
}
