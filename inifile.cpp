#include "inifile.h"
#include "common.h"
#include "debug.h"

namespace Tsuki {
    IniFile::IniFile(const char *filename, int mode, bool isUtf8=false) : mOpen(false), mUtf8(isUtf8) {
        if(filename)
            load(filename, mode, isUtf8);
    }

    void IniFile::load(const char *filename, int mode, bool isUtf8) {
        if(mOpen)
            close();

        if(!filename) {
            warning("[IniFile] Invalid Parameter");
            return;
        }

        f = fopen(filename, "w+");
        if(!f) {
            warning("[IniFile] Can't open file %s", filename);
            return;
        }

        if(mode == INI_BUFFER) {
            unsigned int ret = 0;
            unsigned int fsize = 0;

            fseek(f, 0, SEEK_END);
            fsize = ftell(f);
            fseek(f, 0, SEEK_SET);

            buffer = malloc(fsize);
            ret = fread(buffer, 1, fsize, f);
            if(ret != fsize) {
                warning("[IniFile] Can't read whole file (%d/%d)", ret, fsize);
                free(buffer);
                return;
            }
            fclose(f);
        }

        mOpen = true;
        mMode = mode;
        mUtf8 = isUtf8;
        mFilename = filename;
    }

    void IniFile::close() {
        if(!mOpen)
            return;

        if(buffer)
            free(buffer);
    }

    Any IniFile::getValue(const char *category, const char *key, const Any &def=Any()) {

        return def;
    }

    bool IniFile::isSpace(char *ch) {
        if(ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n')
            return true;
        else
            return false;
    }

    bool nextLine(void *data) {
        while(*data && *data != '\n')
            ++data;
        if(!*data)
            return false;
        else
            ++data;
        return true;
    }

    bool IniFile::parseEntry() {
        if(!mOpen)
            return false;

        void *& data = buffer;

        while(*data) {
            while(*data && isSpace(*data))
                ++data;
            else
                continue;

            if(*data == '#' || *data == ';')
                if(!nextLine)
                    return false;

            if(*data == '[') {

            }
        }
    }
}
