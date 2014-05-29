#ifndef GLOBALS_H
#define GLOBALS_H

/* enable list */
#define ASSERTIONS_ENABLED 1

/* global defines */
#define READ_ONLY(type,varname) private: type varname##_m; public: inline type varname() { return varname##_m; };

#ifdef ASSERTIONS_ENABLED
    #define ASSERT(expr) if(expr) { } else { \
        qCritical("ASSERTION \"%s\" is false in [%s:%d]", #expr, __FILE__, __LINE__); \
        raise(SIGABRT); }
#else
#define ASSERT(expr)
#endif


/* global includes */
#include "logger.h"

#endif // GLOBALS_H
