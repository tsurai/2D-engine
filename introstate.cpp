#include "introstate.h"
#include "consolestate.h"
#include "playstate.h"
#include "engine.h"
#include <QKeyEvent>
#include "fmod.h"
#include "fmod_errors.h"

template<> IntroState* Singleton<IntroState>::m_instance = 0;

IntroState::IntroState() {
    //window->deleteTexture(logoID);
}

IntroState::~IntroState() {
    qDebug() << this->getName() << "destroyed";
}


void ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        exit(-1);
    }
}

FMOD_SYSTEM *fsystem;
FMOD_SOUND   *sound;
FMOD_SOUND   *sound_menu;
FMOD_CHANNEL *channel = 0;

void IntroState::initialize() {
    paused = false;
    useShaderInRT = false;
    subState = 0;
    yselection = 0;
    xselection = 0;
    maxySelection = 4;
    transparency = 0;

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    // load shader
    QGLShader shader(QGLShader::Fragment);
    shader.compileSourceFile("data/frag3.fsh");
    program = new QGLShaderProgram(window->context());
    program->addShader(&shader);
    program->link();

    timeLocation = program->uniformLocation("time");
    resLocation = program->uniformLocation("resolution");
    transLocation = program->uniformLocation("trans");

    // draw the shader to a texture and meassure the time it takes
    glGenTextures(1, &bgTex);
    glBindTexture(GL_TEXTURE_2D, bgTex);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window->cWidth(), window->cHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glGenFramebuffersEXT(1, &framebuffer);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffer);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, bgTex, 0);

    if(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) != GL_FRAMEBUFFER_COMPLETE_EXT)
        qCritical() << "Framebuffer not ready";

    // bind out framebuffer and setup the view
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffer);
    glPushAttrib(GL_VIEWPORT_BIT);

    glViewport(0, 0, window->cWidth(), window->cHeight());
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, window->cWidth(), window->cHeight(), 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // meassure the time the shader needs
    QTime timer = QTime::currentTime();
    timer.start();

    program->bind();
    program->setUniformValue(resLocation, window->size());
    program->setUniformValue(timeLocation, 0.01f);
    program->setUniformValue(transLocation, 1.0f);
    glBegin(GL_QUADS);
        glVertex2i(0, 0); // Top Left Of The Texture and Quad
        glVertex2i(0, window->cHeight()); // Bottom Left Of The Texture and Quad
        glVertex2i(window->cWidth(), window->cHeight()); // Bottom Right Of The Texture and Quad
        glVertex2i(window->cWidth(), 0); // Top Right Of The Texture and Quad
    glEnd();
    program->release();

    int time = timer.elapsed();

    glPopAttrib();
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

    // fast enough to use in realtime?
    if(time <= 200) {
        glDeleteTextures(1, &bgTex);
        useShaderInRT = true;
    }
    glDeleteFramebuffersEXT(1, &framebuffer);

    title = FontEngine::instance()->newFont("./data/fonts/kochi-mincho-subst.ttf", 26, L"やぶれたゆめ");
    title = FontEngine::instance()->newFont("./data/fonts/kochi-mincho-subst.ttf", 64, L"破れた夢");
    menu = FontEngine::instance()->newFont("./data/fonts/Lato-Light.ttf", 17);
    menu = FontEngine::instance()->newFont("./data/fonts/Lato-Light.ttf", 22);


    FMOD_RESULT       result;
    int               key;
    unsigned int      version;

    result = FMOD_System_Create(&fsystem);
    ERRCHECK(result);
    result = FMOD_System_GetVersion(fsystem, &version);
    ERRCHECK(result);

        if (version < FMOD_VERSION)
        {
            printf("Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
        }

        result = FMOD_System_Init(fsystem, 2, FMOD_INIT_NORMAL, 0);
        ERRCHECK(result);

        result = FMOD_System_CreateSound(fsystem, "./data/menu.wav", FMOD_SOFTWARE | FMOD_LOOP_OFF | FMOD_2D, 0, &sound_menu);
        ERRCHECK(result);

        result = FMOD_System_CreateStream(fsystem, "./data/test.mp3", FMOD_HARDWARE | FMOD_LOOP_NORMAL | FMOD_2D, 0, &sound);
        ERRCHECK(result);

        result = FMOD_System_PlaySound(fsystem, FMOD_CHANNEL_FREE, sound, false, &channel);
        ERRCHECK(result);

}

void IntroState::cleanup() {

}

const char *IntroState::getName() {
    return "Introstate";
}

void IntroState::start() {
    paused = false;
    /* other stuff */
}

void IntroState::pause() {
    paused = true;
    /* other stuff */
}

void IntroState::resume() {
    paused = false;
    /* other stuff */
}

void IntroState::update() {
    if(paused)
        return;

    if(transparency < 1.0)
        transparency += 0.004;

    time += 0.003f;
}

void IntroState::draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear screen and depth buffer
    glColor4f(1.0f, 1.0f, 1.0f, transparency);
    glLoadIdentity(); // Reset current modelview matrix

    if(useShaderInRT) {
        program->bind();
        program->setUniformValue(resLocation, window->size());
        program->setUniformValue(timeLocation, time);
        program->setUniformValue(transLocation, transparency);

        glBegin(GL_QUADS);
            glVertex2i(0, 0);
            glVertex2i(0, window->cHeight());
            glVertex2i(window->cWidth(), window->cHeight());
            glVertex2i(window->cWidth(), 0);
        glEnd();
        program->release();
    } else {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, bgTex);
        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 1.0f); glVertex2i(0, 0);
            glTexCoord2f(0.0f, 0.0f); glVertex2i(0, window->cHeight());
            glTexCoord2f(1.0f, 0.0f); glVertex2i(window->cWidth(), window->cHeight());
            glTexCoord2f(1.0f, 1.0f); glVertex2i(window->cWidth(), 0);
        glEnd();
    }

    glDisable(GL_TEXTURE_2D);
    int xoffset = (int)window->cWidth()*0.67;
    int yoffset = window->cHeight()-75;
    glBegin(GL_LINES);
        glVertex2i(xoffset, 70);
        glVertex2i(xoffset, yoffset);
    glEnd();

    xoffset-=10;
    FontEngine::instance()->wprint(xoffset, 75, title, 64, FontEngine::OriginTopRight, L"%ls", L"破れた夢");
    FontEngine::instance()->wprint(xoffset, 140, title, 26, FontEngine::OriginTopRight, L"%ls", L"やぶれたゆめ");

    FontEngine::instance()->wprint(xoffset, yoffset, menu, 22, FontEngine::OriginBottomRight, L"%ls %ls", ((!subState && yselection == 3) ? L"»" : L""), L"Exit");
    FontEngine::instance()->wprint(xoffset, yoffset-=32, menu, 22, FontEngine::OriginBottomRight, L"%ls %ls", ((!subState && yselection == 2) ? L"»" : L""), L"Settings");
    FontEngine::instance()->wprint(xoffset, yoffset-=32, menu, 22, FontEngine::OriginBottomRight, L"%ls %ls", ((!subState && yselection == 1) ? L"»" : L""), L"Load Game");
    FontEngine::instance()->wprint(xoffset, yoffset-=32, menu, 22, FontEngine::OriginBottomRight, L"%ls %ls", ((!subState && yselection == 0) ? L"»" : L""), L"New Game");

    // settings menu
    if(subState == 1) {
        yoffset = 75;
        FontEngine::instance()->wprint(xoffset+20+((yselection == maxySelection-1) ? ((!xselection) ? 20 : 75) : 0),
                                       yoffset+((yselection == maxySelection-1) ? yselection*25+5 : yselection*25), menu, 17, FontEngine::OriginTopLeft, L"»");
        FontEngine::instance()->wprint(xoffset+30, yoffset, menu, 17, FontEngine::OriginTopLeft, L"Resolution");
        FontEngine::instance()->wprint(window->cWidth()-30, yoffset, menu, 17, FontEngine::OriginTopRight, L"1024 x 768");
        FontEngine::instance()->wprint(xoffset+30, yoffset+=25, menu, 17, FontEngine::OriginTopLeft, L"Vsync");
        FontEngine::instance()->wprint(window->cWidth()-30, yoffset, menu, 17, FontEngine::OriginTopRight, L"off");
        FontEngine::instance()->wprint(xoffset+30, yoffset+=25, menu, 17, FontEngine::OriginTopLeft, L"Debugmode");
        FontEngine::instance()->wprint(window->cWidth()-30, yoffset, menu, 17, FontEngine::OriginTopRight, L"on");
        FontEngine::instance()->wprint(xoffset+30+20, yoffset+=30, menu, 17, FontEngine::OriginTopLeft, L"Back");
        FontEngine::instance()->wprint(xoffset+30+75, yoffset, menu, 17, FontEngine::OriginTopLeft, L"Accept");
    }
}

void IntroState::setFlags(GameState::flags flags) {
    stateFlags = flags;
}

bool IntroState::eventFilter(QObject *source, QEvent *e) {
    Q_UNUSED(source);
    if(e->type() == QMouseEvent::MouseButtonPress && stateFlags.testFlag(GameState::FILTER_MOUSE)) {

    }

    if(e->type() == QKeyEvent::KeyPress && stateFlags.testFlag(GameState::FILTER_KEYBOARD)) {
        FMOD_CHANNEL *channel;
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(e);
        switch(keyEvent->key()) {
        case Qt::Key_M:
            float vol;
            FMOD_Channel_GetVolume(channel, &vol);
            if(vol == 0.0)
                FMOD_Channel_SetVolume(channel, 1.0f);
            else
                FMOD_Channel_SetVolume(channel, 0.0f);
            break;
        case Qt::Key_Escape:
            if(subState == 1) {
                subState = 0;
                yselection = 2;
                maxySelection = 4;
            }
            break;
        case Qt::Key_Pause:
            if(!paused)
                pause();
            else
                resume();
            break;
        case Qt::Key_Up:

            FMOD_System_PlaySound(fsystem, FMOD_CHANNEL_FREE, sound_menu, false, &channel);
            FMOD_Channel_SetVolume(channel, 0.05f);
            yselection = (!yselection) ? maxySelection-1 : yselection-1;
            break;
        case Qt::Key_Down:
            FMOD_System_PlaySound(fsystem, FMOD_CHANNEL_FREE, sound_menu, false, &channel);
            FMOD_Channel_SetVolume(channel, 0.05f);
            yselection = (yselection+1) % maxySelection;
            break;
        case Qt::Key_Right:
            break;
        case Qt::Key_Left:
            break;
        case Qt::Key_Return:
        case Qt::Key_Enter:
            if(subState == 0) {
                switch(yselection) {
                case 0:
                    changeState(new PlayState());
                    break;
                case 1:
                    break;
                case 2:
                    subState = 1;
                    yselection = 0;
                    maxySelection = 4;
                    break;
                case 3:
                    Engine::instance()->stop();
                    break;
                }
            }
            break;
        }
    }
    return false;
}
