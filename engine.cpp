#include "engine.h"
#include "common.h"
#include "logmanager.h"
#include "glwindow.h"
#include "statemanager.h"
#include "fontengine/fontengine.h"
#include "debug.h"
#include "states/introstate.h"
#include <GL/glfw.h>

namespace Tsuki {
    Engine::Engine(const char *logPath) : mGLWindow(0), mStateManager(0), mFontManager(0) {
        running = false;
        mInitialized = false;
        mLogManager = new LogManager(logPath);

        char *tmp = (char *)calloc(1, 128);
        sprintf(tmp, "version %d.%d.%d \"%s\"", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_NAME);
        mVersionString = const_cast<const char *>(tmp);
    }

    Engine::~Engine() {
        if(mGLWindow) delete mGLWindow;
        if(mStateManager) delete mStateManager;
        if(mFontManager) delete mFontManager;

        free((void *)mVersionString);
        debug() << "engine destroyed...byebye";
    }

    template<> Engine* Singleton<Engine>::m_instance = 0;
    Engine *Engine::instance() {
        assert(m_instance);
        return m_instance;
    }

    void Engine::initialize(const char *title, int width, int height, bool fullscreen, int framerate, bool debug) {
        Tsuki::debug() << "initialize Engine...";

        mGLWindow = new GLWindow(title, width, height, fullscreen, framerate);
        mStateManager = new StateManager();

        mInitialized = true;
        Tsuki::debug() << "...done";
    }

    void Engine::start(GameState *state) {
        debug() << "starting Engine...";

        if(running || !mInitialized)
            return;

        mGLWindow->show();
       // mGLWindow->setForcedAspectRatio(16, 9);
        mGLWindow->setVsyncEnabled(false);

        mFontManager = FontEngine::instance();
        mStateManager->pushState(state);

        double t = 0.0;
        const double dt = 0.01;

        double currentTime = glfwGetTime();
        double accumulator = 0.0;

        debug() << "...entering game loop";
        running = true;
        while(running) {
            double newTime = glfwGetTime();
            double frameTime = newTime - currentTime;
            if(frameTime > 0.25)
                frameTime = 0.25;
            currentTime = newTime;

            accumulator += frameTime;
            while(accumulator >= dt) {
                update(t, dt);
                t += dt;
                accumulator -= dt;
            }

            // interpolation
            //const double alpha = accumulator / dt;
            draw();
        }
    }

    void Engine::update(double t, const double dt) {
        mStateManager->getCurState()->update(t, dt);

        // Check if ESC key was pressed or window was closed
        /*if(glfwGetKey(GLFW_KEY_ESC) || !glfwGetWindowParam(GLFW_OPENED))
            this->stop();*/
    }

    void Engine::draw() {
        if(running) {
            mGLWindow->draw();

            glerror = glGetError();
            if(running && (glerror != GL_NO_ERROR))
                fatal() << "openGL error: " << glerror;
        }
    }

    GLWindow *Engine::getWindow() {
        return mGLWindow;
    }

    void Engine::stop() {
        //debug() << "stopping Engine...";
        running = false;
    }
}
