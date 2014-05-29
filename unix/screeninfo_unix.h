#ifndef SCREENINFO_UNIX_H
#define SCREENINFO_UNIX_H

#include "prerequisites.h"
#include "screeninfo.h"
#include <X11/Xlib.h>

namespace Tsuki {
    class ScreenInfo : public ScreenInfo_base {
    public:
        ScreenInfo() : display(XOpenDisplay(0)) { }
        ~ScreenInfo() { }

        int getNumDisplays();
        int getWidth();
        int getHeight();
    private:
        Display *display;
    };
}

#endif // SCREENINFO_UNIX_H
