#include "screeninfo_win.h"

namespace Tsuki {
    int ScreenInfo::getNumDisplays() {
        return 0;
    }

    int ScreenInfo::getWidth() {
        RECT rect;
        GetWindowRect(hDesktop, &rect);

        return rect.right;
    }

    int ScreenInfo::getHeight() {
        RECT rect;
        GetWindowRect(hDesktop, &rect);

        return rect.bottom;
    }
}
