#ifndef SCREENINFO_WIN_H
#define SCREENINFO_WIN_H

#include "prerequisites.h"
#include "screeninfo.h"
#include <wtypes.h>

namespace Tsuki {
    class ScreenInfo : public ScreenInfo_base {
    public:
        ScreenInfo() : hDesktop(GetDesktopWindow()) { }

        int getNumDisplays();
        int getWidth();
        int getHeight();
    private:
        const HWND hDesktop;
    };
}

#endif // SCREENINFO_WIN_H
