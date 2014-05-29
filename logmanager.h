#ifndef LOGGER_H
#define LOGGER_H

#include "prerequisites.h"
#include "singleton.h"
#include "log.h"

namespace Tsuki {
    class LogManager : public Singleton<LogManager>
    {
    public:
        LogManager(const char *logPath, const char *defLogName="tsuki", std::ostream &defStream=std::cerr);
        ~LogManager();
        static LogManager *instance();

        Log &createLog(const char *name);
        void removeLog(const char *name);
        Log &getDefaultLog();
        Log &getLog(const char *name);
    protected:
        const char *logPath;
        std::ostream &defStream;
        Log *defLog;
        std::map<const char *, Log *> logList;
    };
}


#endif // LOGGER_H
