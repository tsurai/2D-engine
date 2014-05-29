TEMPLATE = app
CONFIG -= qt

QMAKE_CXXFLAGS += -Wl,-subsystem,windows,-pthread

SOURCES += main.cpp \
    glwindow.cpp \
    logmanager.cpp \
    log.cpp \
    engine.cpp \
    statemanager.cpp \
    debug.cpp \
    textureatlas.cpp \
    fontengine/face.cpp \
    fontengine/fontengine.cpp \
    fontengine/font.cpp \
    shaderprogram.cpp \
    tilemap.cpp \
    imageformat.cpp \
    #inifile.cpp \
    audiowave.cpp \
    audiosystem.cpp \
    audioogg.cpp


LIBS += -lglfw

win32 {
    LIBS += -lglu32 -lopenGL32 -lopenAL32 -logg -lvorbis -lvorbisfile -lfreetyoe
    SOURCES += win/screeninfo_win.cpp \
        thirdparty/glew.c
    HEADERS += win/screeninfo_win.h
} else {
    unix {
        LIBS += -lGLEW -lGL -lfreetype
        SOURCES += unix/screeninfo_unix.cpp
        HEADERS += unix/screeninfo_unix.h
        INCLUDEPATH += "/usr/include/freetype2/"
    } else {
        DEFINES += OS_UNKNOWN
    }
}

HEADERS += \
    glwindow.h \
    singleton.h \
    common.h \
    logmanager.h \
    prerequisites.h \
    log.h \
    engine.h \
    debug.h \
    screeninfo.h \
    statemanager.h \
    gamestate.h \
    platform.h \
    textureatlas.h \
    fontengine/face.h \
    fontengine/fonterror.h \
    fontengine/fontengine.h \
    fontengine/font.h \
    shaderprogram.h \
    tilemap.h \
    imageformat.h \
    audiowave.h \
    audiosystem.h \
    audioogg.h \
    audiobase.h
