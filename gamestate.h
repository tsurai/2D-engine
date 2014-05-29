#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "prerequisites.h"
#include "engine.h"
#include "glwindow.h"
#include "statemanager.h"

namespace Tsuki {
    class GameState {
    public:
        virtual ~GameState() { }

        virtual void initialize() = 0;
        virtual void cleanup() = 0;

        virtual const char *getName() = 0;

        virtual void start() = 0;
        virtual void pause() = 0;
        virtual void resume() = 0;

        virtual void update(double t, const double dt) = 0;
        virtual void draw() = 0;

        bool isPaused() {
            return paused;
        }

        inline void exit() {
            Engine::instance()->mStateManager->popState();
        }

        inline void changeState(GameState *state) {
            Engine::instance()->mStateManager->changeState(state);
        }

        inline void pushState(GameState *state) {
            Engine::instance()->mStateManager->pushState(state);
        }

    protected:
        GameState() { window = Engine::instance()->getWindow(); }
        bool paused;
        GLWindow *window;
    private:
        GameState(const GameState &);
        GameState& operator=(const GameState &);
    };
}

#endif // GAMESTATE_H
