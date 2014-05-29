#ifndef WINHACKS_H
#define WINHACKS_H

#ifdef __MINGW32__
#include <cstring>
#include <cstdarg>
#include <cwchar>

/* contains hacks to get shit working on windows when it fucks up again */
int swprintf(wchar_t *buf, size_t length, const wchar_t *fmt, ...) {
    va_list args;
    int i;

    va_start(args, fmt);
    i=_vsnwprintf(buf,length,fmt,args);
    va_end(args);
    return i;
}

inline int vswprintf(wchar_t *buf, size_t length, const wchar_t *fmt, va_list args) {
    return _vsnwprintf(buf,length,fmt,args);
}

int sprintf(char *buf, size_t length, const char *fmt, ...) {
    va_list args;
    int i;

    va_start(args, fmt);
    i=_vsnprintf(buf,length,fmt,args);
    va_end(args);
    return i;
}

inline int vsprintf(char *buf, size_t length, const char *fmt, va_list args) {
    return _vsnprintf(buf,length,fmt,args);
}

#endif //__MINGW32__

#endif // WINHACKS_H
