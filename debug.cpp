#include "debug.h"

namespace Tsuki {
    int _debug(Debug::type_t type, const char *format, va_list args) {
        char *buf = (char *)calloc(1, 256);

        int offset;
        switch(type) {
        case Debug::WARNING:
            strcpy(buf, "[Warning] ");
            offset = 10;
            break;
        case Debug::CRITICAL:
            strcpy(buf, "[Critical] ");
            offset = 11;
            break;
        case Debug::FATAL:
            strcpy(buf, "[Fatal] ");
            offset = 8;
            break;
        case Debug::NORMAL:
            strcpy(buf, "[Debug] ");
            offset = 8;
        default:
            break;
        }

        vsprintf(buf+offset, format, args);
        int ret = LogManager::instance()->getDefaultLog().logMessage(buf);
        free(buf);

        return ret;
    }

    //inline Debug debug() { return Debug(Debug::NORMAL); }
    int debug(const char *format, ...) {
       va_list args;
       va_start(args, format);
       int ret = _debug(Debug::NORMAL, format, args);
       va_end(args);
       return ret;
    }

    //inline Debug warning() { return Debug(Debug::WARNING); }
    int warning(const char *format, ...) {
        va_list args;
        va_start(args, format);
        int ret = _debug(Debug::WARNING, format, args);
        va_end(args);
        return ret;
    }

    //inline Debug critical() { return Debug(Debug::CRITICAL); }
    int critical(const char *format, ...) {
        va_list args;
        va_start(args, format);
        int ret = _debug(Debug::CRITICAL, format, args);
        va_end(args);
        return ret;
    }

    //inline Debug fatal() { return Debug(Debug::FATAL); }
    int fatal(const char *format, ...) {
        va_list args;
        va_start(args, format);
        int ret = _debug(Debug::FATAL, format, args);
        va_end(args);
        return ret;
    }
}
