#ifndef SCREENINFO_H
#define SCREENINFO_H

namespace Tsuki {
    class ScreenInfo_base {
    public:
        virtual ~ScreenInfo_base() { }

        virtual int getNumDisplays() = 0;
        virtual int getWidth() = 0;
        virtual int getHeight() = 0;
    };
}

#ifdef TSUKI_OS_WIN
    #include "win/screeninfo_win.h"
#elif defined TSUKI_OS_LINUX
    #include "unix/screeninfo_unix.h"
#endif

#endif // SCREENINFO_H
