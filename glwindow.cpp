#include "glwindow.h"
#include "debug.h"
#include "engine.h"
#include "statemanager.h"
#include "gamestate.h"
#include "screeninfo.h"
#include "fontengine/fontengine.h"

namespace Tsuki {
    typedef void (APIENTRY * EXTFUNCPTR)(int param);

    void cbResize(int width, int height) {
        Engine::instance()->getWindow()->resize(width, height);
    }

    void cbClose(void) {
        Engine::instance()->stop();
    }

    GLWindow::GLWindow(const char *title, unsigned int width, unsigned int height, bool fullscreen, unsigned int framerate, ParamList *param)
        : mTitle(title), mWidth(width), mHeight(height), mFullscreen(fullscreen), mAspectRatio(false), mFramerate(framerate), mIsOpen(false),
          mVSync(false), mRatioW(0), mRatioH(0), mFPS(0)
    {
        UNUSED(param);
        debug() << "initialize GLWindow";
        if(!glfwInit())
            fatal() << "failed to initialize glfw";

        glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, true);

        mScreen = new ScreenInfo();
    }

    GLWindow::~GLWindow() {
        glfwTerminate();
        delete mScreen;
    }

    void GLWindow::initializeGL() {
        debug() << "initializeGL";

        if(glewInit() != GLEW_OK)
            fatal() << "failed to initialize glew";

        if(!GLEW_VERSION_2_1)
            fatal() << "OpenGL 2.1 not supported";

        glShadeModel(GL_SMOOTH);
        glClearColor(0, 0, 0, 0);

        glClearDepth(1.0f);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

        resize(mWidth, mHeight);

        // has to be initialized here because it needs a GL context to load shader
        if(!Engine::instance()->isRunning()) {
            new FontEngine();
            mfDebug = FontEngine::instance()->newFont("./data/fonts/DroidSansMono.ttf", 14);
        }
    }

    void GLWindow::draw() {
        StateManager::instance()->getCurState()->draw();

        if(/*Engine::instance()->isDebugMode()*/ true) {
            FontEngine::instance()->print(0, 0, mfDebug, 14, FontEngine::OriginTopLeft, "Current state: %s%s", StateManager::instance()->getCurState()->getName() ,(StateManager::instance()->getCurState()->isPaused() ? " (paused)" : ""));
            FontEngine::instance()->wprint(0, mCHeight, mfDebug, 14, FontEngine::OriginBaseLeft, L"States on stack: %d", StateManager::instance()->count());
            FontEngine::instance()->wprint(mCWidth, 0, mfDebug, 14, FontEngine::OriginTopRight, L"FPS: %d", mFPS);
        }
        FontEngine::instance()->drawTextBuffer();

        glfwSwapBuffers();

        static double frames;
        static double lastFrame;
        double now = glfwGetTime();
        ++frames;
        if(now-lastFrame > 1.0) {
            lastFrame = now;
            mFPS = frames;
            frames = 0;
        }
    }

    void GLWindow::show() {
        if(mIsOpen)
            return;

        if(!glfwOpenWindow(mWidth, mHeight, 0, 0, 0, 0, 0, 0, (mFullscreen) ? GLFW_FULLSCREEN : GLFW_WINDOW))
            fatal() << "failed to create glfw window";

        glfwSetWindowTitle(mTitle);
        initializeGL();

        center();
        glfwSetWindowSizeCallback((GLFWwindowsizefun)(cbResize));
        glfwSetWindowCloseCallback((GLFWwindowclosefun)(cbClose));

        mIsOpen = true;
    }

    void GLWindow::center() {
        glfwSetWindowPos((mScreen->getWidth()-mWidth)/2, (mScreen->getHeight()-mHeight)/2);
    }

    void GLWindow::resize(int width, int height) {
        if(width == 0 || height == 0)
            return;

        mWidth = width;
        mHeight = height;
        glfwSetWindowSize(width, height);

        if(mAspectRatio) {
            applyAspectRatio();
        } else {
            glViewport(0, 0, width, height);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();

            glOrtho(0, width, height, 0, -1, 1);

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            mCWidth = width;
            mCHeight = height;
        }
    }

    void GLWindow::setForcedAspectRatio(bool force) {
        if(force)
            applyAspectRatio();
        mAspectRatio = force;
    }

    void GLWindow::setForcedAspectRatio(unsigned int ratioW, unsigned int ratioH) {
        if(ratioW == 0 || ratioW == mRatioW || ratioH == 0 || ratioH == mRatioH)
            return;

        mRatioW = ratioW;
        mRatioH = ratioH;
        applyAspectRatio();
    }

    void GLWindow::setPosition(int x, int y) {
        glfwSetWindowSize(x, y);
    }

    void GLWindow::setFullscreen(bool fullscreen) {
        if(fullscreen == mFullscreen)
            return;
/*
        // save current hints
        // WARNING: only valid for current glfw version
        int cGlfwHints = 14;
        int hints[cGlfwHints*2];
        int hintId = GLFW_REFRESH_RATE;
        for(int i = 0; i < cGlfwHints*2; i++) {
            hints[i] = hintId++;
            hints[++i] = glfwGetWindowParam(hintId);
        }

        // close window and reset hints
        glfwCloseWindow();
        for(int i = 0; i < cGlfwHints*2; i++) {
            glfwOpenWindowHint(hints[i], hints[++i]);
        }
        mIsOpen = false;
        mFullscreen = fullscreen;
        show();*/
    }

    void GLWindow::setVsyncEnabled(bool vsync) {

        EXTFUNCPTR swapInterval;
#ifdef TSUKI_OS_LINUX
        if(strcasecmp("mesa", (const char *)glGetString(GL_RENDERER)) == 0)
            swapInterval = (EXTFUNCPTR)glfwGetProcAddress("glxSwapIntervalSGI");
        else
            swapInterval = (EXTFUNCPTR)glfwGetProcAddress("glxSwapIntervalMESA");
#endif
#ifdef TSUKI_OS_WIN
        swapInterval = (EXTFUNCPTR)glfwGetProcAddress("wglSwapIntervalEXT");
#endif
        if(swapInterval != NULL) {
            swapInterval(vsync);
            mVSync = vsync;
        }

        //glfwSwapInterval(0);
    }

    void GLWindow::applyAspectRatio() {
        if(mRatioW == 0 || mRatioH == 0)
            return;

        float newRatio = (double)mRatioW/(double)mRatioH;
        float oldRatio = (double)mWidth/(double)mHeight;

        if(newRatio > oldRatio) {
            mCWidth = mWidth;
            mCHeight = (mWidth/mRatioW)*mRatioH;
            glViewport(0, (mHeight-mCHeight)/2, mWidth, mCHeight); // Reset The Current Viewport
            glMatrixMode(GL_PROJECTION); // Select The Projection Matrix
            glLoadIdentity(); // Reset The Projection Matrix

            glOrtho(0, mWidth, mCHeight, 0, -1, 1);
            //cYoffset_m = (mHeight-mCHeight)/2;
        } else if(newRatio < oldRatio) {
            mCHeight = mHeight;
            mCWidth = (mHeight/mRatioH)*mRatioW;
            glViewport((mWidth-mCWidth)/2, 0, mCWidth, mHeight); // Reset The Current Viewport
            glMatrixMode(GL_PROJECTION); // Select The Projection Matrix
            glLoadIdentity(); // Reset The Projection Matrix

            glOrtho(0, mCWidth, mHeight, 0, -1, 1);
            //cYoffset_m = (mWidth-mCWidth)/2;
        }
        glMatrixMode(GL_MODELVIEW); // Select The Modelview Matrix
        glLoadIdentity(); // Reset The Modelview Matrix

    }

    unsigned int GLWindow::width() {
        return mWidth;
    }

    unsigned int GLWindow::height() {
        return mHeight;
    }

    unsigned int GLWindow::cWidth() {
        return mCWidth;
    }

    unsigned int GLWindow::cHeight() {
        return mCHeight;
    }
}
