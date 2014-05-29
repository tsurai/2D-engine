#include "logmanager.h"

namespace Tsuki {
    LogManager::LogManager(const char *logPath, const char *defLogName, std::ostream &defStream)
        : logPath(logPath), defStream(defStream)
    {
        char filename[FILENAME_MAX];
        sprintf(filename,"%s/%s.log", logPath, defLogName);
        defLog = new Log(filename, defStream);
    }

    LogManager::~LogManager() {

    }

    template<> LogManager* Singleton<LogManager>::m_instance = 0;
    LogManager *LogManager::instance() {
        assert(m_instance);
        return m_instance;
    }

    Log &LogManager::createLog(const char *name) {
        if(logList.count(name) != 0)
            return *logList.find(name)->second;

        char filename[FILENAME_MAX];
        sprintf(filename,"%s/%s.log", logPath, name);

        logList.insert(std::pair<const char *, Log *>(name, new Log(filename, defStream)));

        return *logList.find(name)->second;
    }

    void LogManager::removeLog(const char *name) {
        std::map<const char *, Log *>::iterator it;
        if((it = logList.find(name)) == logList.end())
            return;
        delete it->second;
        logList.erase(it);
    }

    Log &LogManager::getDefaultLog() {
        return *defLog;
    }

    Log &LogManager::getLog(const char *name) {
        std::map<const char *, Log *>::iterator it;
        if((it = logList.find(name)) != logList.end())
            return *it->second;
        else
            return *defLog;
    }
}
