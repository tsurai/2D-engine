#include "log.h"
#include <cstdarg>

namespace Tsuki {
#define BUFFER_SIZE 256

    Log::Log(const char *filename, std::ostream &outputTarget, ios_base::openmode mode)
        : ForkStream(outputTarget, filename, mode) {

    }

    Log::~Log() {

    }

    //TODO: freed falschen buffer ka wieso
    int Log::logMessage(const char *format, ...) {
        va_list args;
        int ret;

        va_start(args, format);
        char *buf = (char *)calloc(1, BUFFER_SIZE);

        ret = vsprintf(buf, format, args);
        va_end(args);

        std::ostream::write(buf, ret);
        std::endl(*this);
        free(buf);

        return ret;
    }
}
