#include "statemanager.h"
#include "gamestate.h"
#include "debug.h"
#include <vector>

namespace Tsuki {
    StateManager::StateManager() {
        debug() << "initialize StateManager";
        stateStack.empty();
    }

    StateManager::~StateManager() {
        debug() << "Destroying StateManager";

        while(!stateStack.empty()) {
            stateStack.back()->cleanup();
            delete stateStack.back();
            stateStack.pop_back();
        }
    }

    template<> StateManager* Singleton<StateManager>::m_instance = 0;
    StateManager *StateManager::instance() {
        assert(m_instance);
        return m_instance;
    }

    void StateManager::changeState(GameState *state) {
        // cleanup and pop current state
        if(!stateStack.empty()) {
            //debug("Changing state from %s to %s", stateStack.back()->getName(), state->getName());

            stateStack.back()->cleanup();
            delete stateStack.back();
            stateStack.pop_back();
        }

        // setup new state
        stateStack.push_back(state);
        stateStack.back()->initialize();
        stateStack.back()->start();
    }

    void StateManager::pushState(GameState *state) {
        // pause current state
        if(!stateStack.empty()) {
            stateStack.back()->pause();
        }
        //debug("Push %s", state->getName());

        // setup new state
        stateStack.push_back(state);
        stateStack.back()->initialize();
        stateStack.back()->start();
    }

    void StateManager::popState() {
        // cleanup and pop current state
        /* we need at least one state left on the stack because we can't run without */
        if(stateStack.size() > 1) {
            //debug("Pop %s", stateStack.back()->getName());

            stateStack.back()->cleanup();
            delete stateStack.back();
            stateStack.pop_back();

            // resume previous state
            stateStack.back()->resume();
        }
    }

    void StateManager::changeState(GameState *state, int stateIndex) {
        // cleanup and pop current state
        if(!stateStack.empty() && stateIndex <= (int)stateStack.size()-1) {
            //debug("Changing from %s to %s", stateStack.at(stateIndex)->getName(), state->getName());

            stateStack.at(stateIndex)->cleanup();
            delete stateStack.at(stateIndex);
            stateStack.erase(stateStack.begin()+stateIndex);
        }

        // setup new state
        stateStack.insert(stateStack.begin()+stateIndex, state);
        stateStack.at(stateIndex)->initialize();
        stateStack.at(stateIndex)->start();
    }

    void StateManager::insertState(GameState *state, int stateIndex) {
        stateStack.at(stateIndex-1)->pause();

        //debug("Push %s", state->getName());
        stateStack.insert(stateStack.begin()+stateIndex, state);
        stateStack.at(stateIndex)->initialize();
        stateStack.at(stateIndex)->start();
    }

    void StateManager::removeState(int stateIndex) {
        // cleanup and pop current state
        /* we need at least one NON CONSOLE state left on the stack because
           the console never clears the screen */
        if(((int)stateStack.size() < 2) || (stateIndex >= (int)stateStack.size()))
            return;

        /* technicaly no pop but we gonna 'ignore' the console state */
        //debug("Pop %s", stateStack.at(stateIndex)->getName());
        stateStack.at(stateIndex)->cleanup();
        delete stateStack.at(stateIndex);
        stateStack.erase(stateStack.begin()+stateIndex);

        // resume previous state
        stateStack.at(stateIndex-1)->resume();
    }

    GameState *StateManager::getCurState() {
        if(stateStack.empty())
            fatal() << "State stack is empty! Exiting now...";
        return stateStack.back();
    }

    GameState *StateManager::getState(int i) {
        return stateStack.at(i);
    }

    int StateManager::count() {
        return stateStack.size();
    }
}
