#ifndef STATEMANAGER_H
#define STATEMANAGER_H

#include "prerequisites.h"
#include "singleton.h"

//TODO: make functions static StateManager::pushState() instead of StateManager::getinstace()->pushState()

/*!
 * \brief The StateManager manages every GameState the game posseses including
 *       creation, initialization, event redirecting and closing.
 *
 * The StateManager is an core class that is absolutly requiered to run a game.
 * It manages all GameState Objects that implement an actual game with its different States.
 *
 */
namespace Tsuki {
    class StateManager : public Singleton<StateManager> {
    public:
        /*!
         * \brief Constructor
         *
         * Prepares the class for usage
         */
        StateManager();

        /*!
         * \brief Deconstructor
         *
         * Frees all data allocated by the StateManager
         */
        ~StateManager();

        /*!
         * \brief instance Returns the current instance of the StateManager
         * \overload
         *
         */
        static StateManager *instance();

        /*!
         * \brief Pushes a GameState on the top of the stack and starts it
         *
         * \param state GameState to push
         *
         * Pushes a GameState on top of the stack, installs the window eventfilter and starts it for usage.
         * The GameState that has been on top before gets paused and removed from the eventfilter.
         */
        void pushState(GameState *state);

        /*!
         * \brief Replaces the GameState on the top of the stack with the GameState given as parameter state.
         *
         * \param state GameState with which to replace the current GameState
         *
         * Replaces the current GameState with an other GameState given by the parameter state. This function
         * stops the top state, cleans it up and completly removes it from memory.
        */
        void changeState(GameState *state);
        /*!
         * \brief Pops the top GameState of the stack
         *
         * Pops the top GameState of the stack and resumes the state that has been pushed before it\n.
         * The GameState will NOT be removed if there are less than one states currently on the stack. This ensures
         * that we can never have an empty stack and thus breaking the game by having no state to proccess.
         */
        void popState();
        /*!
         \brief

         \param state
         \param stateIndex
        */
        void changeState(GameState *state, int stateIndex);
        /*!
         \brief

         \param state
         \param stateIndex
        */
        void insertState(GameState *state, int stateIndex);
        /*!
         \brief

         \param stateIndex
        */
        void removeState(int stateIndex);

        /*!
         \brief

         \return GameState
        */
        GameState *getCurState();
        /*!
         \brief

         \param i
         \return GameState
        */
        GameState *getState(int i);

        /*!
         \brief

         \return int
        */
        int count();
    private:
        std::vector<GameState *> stateStack; /*!< TODO */
    };
}

#endif // STATEMANAGER_H
