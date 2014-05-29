#include "audioogg.h"

namespace Tsuki {
    AudioOgg::AudioOgg() : isOpen(false) {

    }

    AudioOgg::~AudioOgg() {
        if(isOpen)
            close();
    }

    int AudioOgg::open(const char *target) {
        if(isOpen)
            close();

        int res;

        if(!(file = fopen(target, "rb"))) {
            return -1;
        }

        if((res = ov_open_callbacks(file, &oggStream, NULL, 0, OV_CALLBACKS_DEFAULT)) < 0) {
            fclose(file);
            return -2;
        }

        info = ov_info(&oggStream, -1);
        comment = ov_comment(&oggStream, -1);

        if(info->channels == 1)
            format = AL_FORMAT_MONO16;
        else
            format = AL_FORMAT_STEREO16;

        alGenBuffers(2, buffers);
        alGenSources(1, &source);

        alSource3f(source, AL_POSITION,        0.0, 0.0, 0.0);
        alSource3f(source, AL_VELOCITY,        0.0, 0.0, 0.0);
        alSource3f(source, AL_DIRECTION,       0.0, 0.0, 0.0);
        alSourcef (source, AL_ROLLOFF_FACTOR,  0.0          );
        alSourcei (source, AL_SOURCE_RELATIVE, AL_TRUE      );

        isOpen = true;

        return 0;
    }

    void AudioOgg::close() {
        if(!isOpen)
            return;

        alSourceStop(source);
        alDeleteBuffers(2, buffers);
        alDeleteSources(1, &source);

        ov_clear(&oggStream);

        fclose(file);
    }

    void AudioOgg::play() {
        if(!stream(buffers[0]))
            return;

        if(!stream(buffers[1]))
            return;

        alSourceQueueBuffers(source, 2, buffers);
        alSourcePlay(source);
    }

    void AudioOgg::stop() {

    }

    bool AudioOgg::stream(ALuint buffer) {
        int BUFFER_SIZE = 4096 * 8;
        char data[BUFFER_SIZE];
        int size = 0;
        int section;
        int result;

        while(size < BUFFER_SIZE) {
            result = ov_read(&oggStream, data + size, BUFFER_SIZE - size, 0, 2, 1, &section);

            if(result > 0)
                size += result;
            else
                if(result < 0)
                    return false;
                else
                    break;
        }

        if(size == 0)
            return false;

        alBufferData(buffer, format, data, size, info->rate);

        return true;
    }

    bool AudioOgg::update() {
        int processed;
        bool active = true;

        alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);

        while(processed--) {
            ALuint buffer;

            alSourceUnqueueBuffers(source, 1, &buffer);

            active = stream(buffer);

            alSourceQueueBuffers(source, 1, &buffer);

        }

        return active;
    }

    const void *AudioOgg::getData()
    {
        return NULL;
    }

}

