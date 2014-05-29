#ifndef DEBUG_H
#define DEBUG_H

#include "common.h"
#include "logmanager.h"
#include <cstdarg>

namespace Tsuki {
    class Debug
    {
    public:
        typedef enum {
            NORMAL,
            WARNING,
            CRITICAL,
            FATAL
        } type_t;

        inline Debug(type_t type=NORMAL) : m_type(type), os(*new std::ostream(LogManager::instance()->getDefaultLog().rdbuf())) { writePrefix(); }
        inline ~Debug() { os << std::endl; delete &os; if(m_type == FATAL) raise(SIGABRT); }

        template<class T>
        Debug &operator <<(const T &t) {
             os << t;
             return *this;
        }

        Debug &operator<<(std::ostream& ( *pf )(std::ostream&)) {
            os << pf;
            return *this;
        }
    protected:
        void writePrefix() {
            switch(m_type) {
            case NORMAL:
                os.write("[Debug] ", 8);
                break;
            case WARNING:
                os.write("[Warning] ", 10);
                break;
            case CRITICAL:
                os.write("[Critical] ", 11);
                break;
            case FATAL:
                os.write("[Fatal] ", 8);
                break;
            }
        }

        type_t m_type;
        std::ostream &os;
    };

    extern int _debug(Debug::type_t type, const char *format, va_list args);

    inline Debug debug() { return Debug(Debug::NORMAL); }
    extern int debug(const char *format, ...);

    inline Debug warning() { return Debug(Debug::WARNING); }
    extern int warning(const char *format, ...);

    inline Debug critical() { return Debug(Debug::CRITICAL); }
    extern int critical(const char *format, ...);

    inline Debug fatal() { return Debug(Debug::FATAL); }
    extern int fatal(const char *format, ...);

    #ifndef NOASSERTIONS
        #define ASSERT(expr) if(expr) { } else { \
            fatal("ASSERTION \"%s\" is false in [%s:%d]", #expr, __FILE__, __LINE__); }
    #else
    #define ASSERT(expr)
    #endif
}

#endif // DEBUG_H
