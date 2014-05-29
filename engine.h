#ifndef ENGINE_H
#define ENGINE_H

#include "prerequisites.h"
#include "singleton.h"

namespace Tsuki {
    class Engine : public Singleton<Engine>
    {
        friend class GameState;
    public:
        Engine(const char *logPath);
        ~Engine();
        static Engine *instance();

        /* core functions */
        void initialize(const char *title, int width=800, int height=600, bool fullscreen=false, int framerate=0, bool debug=true);
        void start(GameState *state);
        void stop();
        void update(double t, const double dt);
        void draw();

        inline bool isRunning() { return running; }
        inline bool isInitialized() { return mInitialized; }

        GLWindow *getWindow();
    protected:
        /* subsystems */
        LogManager *mLogManager;
        GLWindow *mGLWindow;
        StateManager *mStateManager;
        FontEngine *mFontManager;

        int glerror;
        const char *mVersionString;
        bool mInitialized;
        bool running;
    };
}

#endif // ENGINE_H
