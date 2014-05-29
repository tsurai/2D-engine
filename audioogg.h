#ifndef AUDIOOGG_H
#define AUDIOOGG_H

#include "prerequisites.h"
#include "audiobase.h"
#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisenc.h>
#include <vorbis/vorbisfile.h>

namespace Tsuki {
    class AudioOgg : public AudioBase
    {
    public:
        AudioOgg();
        ~AudioOgg();

        int open(const char *target);
        void close();
        void play();
        void stop();
        bool isPlaying();
        bool update();
        const void *getData();
    private:
        bool stream(ALuint buffer);
        bool isOpen;

        FILE *file;
        OggVorbis_File oggStream;
        vorbis_info *info;
        vorbis_comment *comment;
        ALuint buffers[2];
        ALuint source;
        ALenum format;
    };
}

#endif // AUDIOOGG_H
