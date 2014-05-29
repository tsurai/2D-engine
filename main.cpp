#include "engine.h"
#include "states/introstate.h"
#include "states/playstate.h"
#include "imageformat.h"
#include "debug.h"
#include "audiosystem.h"
#include "audiowave.h"
#include "audioogg.h"

using namespace Tsuki;

int main( void )
{

    Engine *engine = new Engine("./logs");
    engine->initialize("qtp Engine", 800, 600, false);
/*
    AudioSystem *as = new AudioSystem();
    as->initialize();
    /*AudioWave *wave = new AudioWave();
    wave->open("./data/sound.wav");
    as->test(wave);

    AudioOgg *ogg = new AudioOgg();
    ogg->open("./data/sound.ogg");
    ogg->play();

    while(ogg->update()) {

    }

    delete ogg;*/

    engine->start(new IntroState());
    delete engine;
    //delete as;

    return 0;
}
