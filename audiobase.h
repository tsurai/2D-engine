#ifndef AUDIOBASE_H
#define AUDIOBASE_H

#include "prerequisites.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <stdint.h>

namespace Tsuki {
    class AudioBase {
    public:
        virtual ~AudioBase() { }

        virtual int open(const char *target) = 0;
        virtual void close() = 0;
        virtual void play() = 0;
        virtual void stop() = 0;

        struct {
            unsigned long size, chunkSize;
            short channels;
            unsigned long sampleRate, avgBytesPerSec;
            short bytesPerSample, bitsPerSample;
            long dataSize;
        } info;
    };
}

#endif // AUDIOBASE_H
