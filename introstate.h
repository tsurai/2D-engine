#ifndef INTROSTATE_H
#define INTROSTATE_H

#include "gamestate.h"
#include "fontengine/font.h"
#include <QGLShaderProgram>

using namespace Tsuki;

class IntroState : public GameState, public Singleton<IntroState>
{
    Q_OBJECT
public:
    IntroState();
    ~IntroState();
    void initialize();
    void cleanup();

    const char *getName();

    void start();
    void pause();
    void resume();

    void update();
    void draw();

    void setFlags(GameState::flags flags);

    bool eventFilter(QObject *source, QEvent *e);
private:
    float time;
    int subState;
    int yselection;
    int xselection;
    int maxySelection;
    bool useShaderInRT;

    int timeLocation;
    int resLocation;
    int transLocation;
    QGLShaderProgram *program;

    QTime t;
    GLuint logoID;
    GLuint framebuffer;
    GLuint bgTex;
    GLfloat transparency;
    GameState::flags stateFlags;
    Font *title;
    Font *menu;
signals:
    
public slots:
    
};

#endif // INTROSTATE_H
