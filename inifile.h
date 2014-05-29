#ifndef INIFILE_H
#define INIFILE_H

#include "prerequisites.h"

namespace Tsuki {
    typedef enum {
        INI_BUFFER=0x1,
        INI_STREAM=0x2
    } INI_MODE;

    class IniFile
    {
    public:
        IniFile(const char *filename, int mode, bool isUtf8);
        ~IniFile();

        void load(const char *filename, int mode, bool isUtf8);
        void close();

        inline bool isOpen() { return mOpen; }
        Any getValue(const char *category, const char *key, const Any &def);
    private:
        bool nextLine(void *data);
        bool isSpace(char ch);
        bool parseEntry();

        FILE *f;
        const char *mFilename;
        bool mOpen;
        bool mUtf8;
        int mMode;
        void *buffer;
    };
}
#endif // INIFILE_H
