#ifndef LOG_H
#define LOG_H

#include "prerequisites.h"
#include "common.h"

namespace Tsuki {
    class Log : public ForkStream
    {
        friend class debug;
    public:
        Log(const char *filename, std::ostream &outputTarget=std::cerr, ios_base::openmode mode = ios_base::app);
        ~Log();

        int logMessage(const char *format, ...);
        //void logMessage(const char *msg, bool suppressFileOutput=false, bool supressDebugOutput=false);
    protected:
        const char *fname;
    };
}

#endif // LOG_H
