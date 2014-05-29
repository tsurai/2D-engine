#include "screeninfo_unix.h"

namespace Tsuki {
    int ScreenInfo::getNumDisplays() {
        return XScreenCount(display);
    }

    int ScreenInfo::getWidth() {
        return XDisplayWidth(display, 0);
    }

    int ScreenInfo::getHeight() {
        return XDisplayHeight(display, 0);
    }
}
