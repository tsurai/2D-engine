#ifndef IMAGEFORMAT_H
#define IMAGEFORMAT_H

#include "prerequisites.h"

namespace Tsuki {
    typedef enum {
        BITMAPCOREHEADER=12,
        BITMAPINFOHEADER=40,
        BITMAPV2INFOHEADER=52,
        BITMAPV3INFOHEADER=56,
        BITMAPCOREHEADER2=64,
        BITMAPV4HEADER=108,
        BITMAPV5HEADER=124,
        UNKNOWN_TYPE
    } BMP_TYPE;

    typedef enum {
        BI_RGB=0x0,
        BI_RLE8=0x1,
        BI_RLE4=0x2,
        BI_BITFIELDS=0x3,
        BI_JPEG=0x4,
        BI_PNG=0x5,
        BI_ALPHABITFIELDS=0x6,
        UNKNOWN_COMP
    } BMP_COMP;

    class Bitmap {
    public:
        Bitmap();
        ~Bitmap();

        void load(const char *filename);
        void loadFromMemory(unsigned char *data);
        int createTexture();
    private:
        const char *filename;
        unsigned int size;
        unsigned int dataOffset;
        int width;
        int height;
        unsigned short bpp;
        unsigned int imgSize;
        unsigned int compression;
        unsigned int type;
        unsigned int nColors;
        unsigned int glTexId;

        unsigned char *colorTable;
        unsigned char *imgData;
    };
}
#endif // IMAGEFORMAT_H
