#include "glwidget.h"
#include "gamestate.h"
#include "consolestate.h"
#include <QKeyEvent>
#ifdef Q_OS_LINUX
    #include <GL/glx.h>
#endif

typedef void (APIENTRY * EXTFUNCPTR)(int param);

GLWidget::GLWidget() {
    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

    isAspectRatioForced = false;
    isVsyncEnabled = false;
    ratioW = -1;
    ratioH = -1;
    cWidth_m = width();
    cHeight_m = height();
    cXoffset_m = 0;
    cYoffset_m = 0;

    fps = 0;
    frames = 0;
    fpsTimer.connect(&fpsTimer, SIGNAL(timeout()), this, SLOT(setFps()));
    fpsTimer.start(1000);
}

GLWidget::~GLWidget() {

}

bool GLWidget::setVsync(bool enable) {
    if((enable && isVsyncEnabled) || (!enable && !isVsyncEnabled))
        return true;

    EXTFUNCPTR swapInterval;
#ifdef Q_OS_LINUX
    if(strcasecmp("mesa", (const char *)glGetString(GL_RENDERER)) == 0)
        swapInterval = (EXTFUNCPTR)glXGetProcAddress((const GLubyte *)"glXSwapIntervalSGI");
    else
        swapInterval = (EXTFUNCPTR)glXGetProcAddress((const GLubyte *)"glXSwapIntervalMESA");
#endif
#ifdef Q_OS_WIN
    swapInterval = (EXTFUNCPTR)wglGetProcAddress("wglSwapIntervalEXT");
#endif
    if(swapInterval == NULL)
        return false;

    swapInterval(enable);
    isVsyncEnabled = enable;

    return true;
}

void GLWidget::initializeGL() {
    qDebug() << "initializeGL";
    glShadeModel(GL_SMOOTH); // Enable smooth shading
    glClearColor(0, 0, 0, 0); // Set the clear color to a black background

    glClearDepth(1.0f); // Depth buffer setup
    glEnable(GL_DEPTH_TEST); // Enable depth testing
    glDepthFunc(GL_LEQUAL); // Set type of depth test

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Really nice perspective calculations

    if (GLEW_OK != glewInit())
        qCritical() << "Glew initialization failed";

    this->setVsync(false);
    graph = new Graph("render", 1, 0, 50, 600, 100, 1, 1, 600);

    for(unsigned int i = 14; i < 17; ++i)
        if((font = FontEngine::instance()->newFont("./data/fonts/OpenSans-Regular.ttf", i)) == NULL)
            break;
}

void GLWidget::resizeGL(int w, int h) {
    // Prevent divide by zero (in the gluPerspective call)
    if (h == 0)
        h = 1;

    setFixedSize(w, h);
    float newRatio = (float)ratioW/(float)ratioH;
    float curRatio = (float)w/(float)h;

    if(isAspectRatioForced && (newRatio != curRatio)) {
        if(newRatio > curRatio) {
            cWidth_m = w;
            cHeight_m = (w/ratioW)*ratioH;
            glViewport(0, (h-cHeight_m)/2, w, cHeight_m); // Reset The Current Viewport
            glMatrixMode(GL_PROJECTION); // Select The Projection Matrix
            glLoadIdentity(); // Reset The Projection Matrix

            glOrtho(0, w, cHeight_m, 0, -1, 1);
            cYoffset_m = (h-cHeight_m)/2;
        } else if(newRatio < curRatio) {
            cHeight_m = h;
            cWidth_m = (h/ratioH)*ratioW;
            glViewport((w-cWidth_m)/2, 0, cWidth_m, h); // Reset The Current Viewport
            glMatrixMode(GL_PROJECTION); // Select The Projection Matrix
            glLoadIdentity(); // Reset The Projection Matrix

            glOrtho(0, cWidth_m, h, 0, -1, 1);
            cYoffset_m = (w-cWidth_m)/2;
        }
        glMatrixMode(GL_MODELVIEW); // Select The Modelview Matrix
        glLoadIdentity(); // Reset The Modelview Matrix
    } else {
        glViewport(0, 0, w, h); // Reset The Current Viewport
        glMatrixMode(GL_PROJECTION); // Select The Projection Matrix
        glLoadIdentity(); // Reset The Projection Matrix

        // Calculate The Aspect Ratio Of The Window
        glOrtho(0, w, h, 0, -1, 1);

        glMatrixMode(GL_MODELVIEW); // Select The Modelview Matrix
        glLoadIdentity(); // Reset The Modelview Matrix

        cWidth_m = w;
        cHeight_m = h;
        cXoffset_m = 0;
        cYoffset_m = 0;
    }
}

void GLWidget::drawGL() {
    // isolates the state transformations from system drawings
    // NOTE: could cause problems when the wrong matrix is selected
    glPushMatrix();
        StateManager::instance()->getCurState()->draw();
    glPopMatrix();

    //graph->update(0, ProfileManager::instance()->getLastFrameTime(0));
    //graph->update(1, ProfileManager::instance()->getLastFrameTime(1));
    //graph->draw();

    // should rather be part of the engine class
    if(Engine::instance()->isDebugMode()) {
        FontEngine::instance()->print(0, 0, font, 16, FontEngine::OriginTopLeft, "Current state: %s%s", StateManager::instance()->getCurState()->getName() ,(StateManager::instance()->getCurState()->isPaused() ? " (paused)" : ""));
        FontEngine::instance()->wprint(0, cHeight_m, font, 16, FontEngine::OriginBaseLeft, L"States on stack: %d", StateManager::instance()->count());
        FontEngine::instance()->wprint(cWidth_m, 0, font, 16, FontEngine::OriginTopRight, L"FPS: %d", fps);
    }
    FontEngine::instance()->drawTextBuffer();
    frames++;

    this->swapBuffers();
}

/*
 * Misc funcs
 */
void GLWidget::changeResolution(int width, int height)
{
    assert((width > 1) && (height > 1));
    setFixedSize(width, height);
}

void GLWidget::forceAspectRatio(bool isForced) {
    if(ratioW <= 0 || ratioH <= 0)
        return;

    isAspectRatioForced = isForced;
}

bool GLWidget::setForcedAspectRatio(int w, int h) {
    if(w == 0 || h == 0)
        return false;

    float newRatio = (float)w/(float)h;
    float curRatio = (float)width()/(float)height();

    if(newRatio == curRatio)
        return false;

    ratioW = w;
    ratioH = h;

    return true;
}

// TODO: recover mouse position
void GLWidget::setMouseLock(bool enable) {
#ifdef Q_OS_WIN
    if(enable) {
        RECT r;
        r.top = geometry().top();
        r.left = geometry().left();
        r.bottom = geometry().bottom();
        r.right = geometry().right();
        ClipCursor(&r);
        isMouseLocked = true;
    } else {
        unsetCursor();
        isMouseLocked = false;
    }
#endif
}

/*
 * Events
 */
void GLWidget::keyPressEvent(QKeyEvent *e) {
    switch(e->key()) {
    case Qt::Key_AsciiCircum:
        if(QString(Engine::instance()->stateManager->getCurState()->metaObject()->className()).trimmed() != "ConsoleState") {
            getStateManager->pushState(new ConsoleState());
        }
        break;
    case Qt::Key_F1:
        if(!windowState().testFlag(Qt::WindowFullScreen)) {
            windowedWidth = width();
            windowedHeight = height();
            setWindowState(windowState() ^ Qt::WindowFullScreen);
        } else {
            setWindowState(windowState() ^ Qt::WindowFullScreen);
            resizeGL(windowedWidth, windowedHeight);
        }
        break;
    default:
        QGLWidget::keyPressEvent(e);
    }
}

void GLWidget::changeEvent(QEvent *e) {
    switch(e->type()) {
    case QEvent::ActivationChange:
        if(this->isActiveWindow() && isMouseLocked)
            this->setMouseLock(true);
    case QEvent::WindowStateChange:
        //hide mouse cursor in fullscreen mode
        if(windowState().testFlag(Qt::WindowFullScreen)) {
            setCursor(Qt::BlankCursor);
        } else
            unsetCursor();
        break;
    default:
        QGLWidget::changeEvent(e);
        break;
    }
}

void GLWidget::closeEvent(QCloseEvent *) {
    emit closed();
}

void GLWidget::setFps() {
    fps = frames;
    frames = 0;
}
