#ifndef FONT_H
#define FONT_H

#include "../prerequisites.h"
#include "face.h"
#include <freetype/tttables.h>

namespace Tsuki {
    class Font
    {
    public:
        Font();
        ~Font();

        READ_ONLY(const char *, filename)
        READ_ONLY(const char *, familyname)
        READ_ONLY(const char *, stylename)
        READ_ONLY(unsigned int, weight)
        READ_ONLY(TT_OS2 *, tt_os2)

        int loadSize(unsigned int size);
        face_metrics_t getFaceMetrics(unsigned int size);
    private:
        int loadFromFile(const char *filename, unsigned int size, const wchar_t *charpool);
        int loadByName(const char *familyname, bool bold=false, bool italic=false);

        std::map<unsigned int, Face *> faces;

        friend class FontEngine;
    };
}

#endif // FONT_H
