#ifndef PREREQUISITES_H
#define PREREQUISITES_H

namespace Tsuki {
    #define VERSION_MAJOR 0
    #define VERSION_MINOR 1
    #define VERSION_PATCH 0
    #define VERSION_NAME "Fubaku"

    class Engine;
    class GLWindow;
    class GameState;
    class StateManager;
    class LogManager;
    class Log;
    class IniFile;
    class ScreenInfo;
    class AudioBase;
    class Font;
    class FontAtlas;
    class FontEngine;
    class TextureAtlas;
    class ShaderProgram;
    class DynBuffer;
    class Any;
}

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <vector>
#include <map>
#include <list>
#include <set>
#include <queue>
#include <deque>
#include <string>
#include <ostream>
#include <fstream>
#include <iostream>
#include <sstream>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "platform.h"

#endif // PREREQUISITES_H
