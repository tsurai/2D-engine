#ifndef GLWINDOW_H
#define GLWINDOW_H

#include "prerequisites.h"
#include "common.h"
#ifdef TSUKI_OS_WIN
    #define GLEW_STATIC
#endif
#include <GL/glew.h>
#include <GL/glfw.h>

namespace Tsuki {
    class GLWindow
    {
        // TODO: screenmode enum - full, window, borderless
    public:
        GLWindow(const char *title, unsigned int width=800, unsigned int height=600, bool fullscreen=false, unsigned int framerate=0, ParamList *param=NULL);
        ~GLWindow();

        void initializeGL();
        void draw();

        void show();
        void center();
        void resize(int width, int height);
        void setForcedAspectRatio(bool force);
        void setForcedAspectRatio(unsigned int ratioW, unsigned int ratioH);
        void setPosition(int x, int y);
        void setFullscreen(bool fullscreen);
        void setVsyncEnabled(bool vsync);

        /* information query functions */
        unsigned int width();
        unsigned int height();
        unsigned int cWidth();
        unsigned int cHeight();
    protected:
        void applyAspectRatio();

        const char *mTitle;
        unsigned int mWidth;
        unsigned int mHeight;
        unsigned int mCWidth;
        unsigned int mCHeight;
        bool mFullscreen;
        bool mAspectRatio;
        unsigned int mFramerate;
        bool mIsOpen;
        bool mVSync;
        unsigned int mRatioW;
        unsigned int mRatioH;
        unsigned int mFPS;
        ScreenInfo *mScreen;
        //double lastFrame;
        Font *mfDebug;
        unsigned int glerror;
    };
}

#endif // GLWINDOW_H
