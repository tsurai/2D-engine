#ifndef AUDIOWAVE_H
#define AUDIOWAVE_H

#include "audiobase.h"

namespace Tsuki {
    class AudioWave : public AudioBase
    {
    public:
        AudioWave();
        int open(const char *target);
        void close();
        void play();
        void stop();
        const void *getData();
        bool loadWavFile(const std::string filename, ALuint *buffer, ALsizei *size, ALsizei *frequency, ALenum *format);
    private:
        FILE *file;
        char *data;
        bool isOpen;
    };
}

#endif // AUDIOWAVE_H
