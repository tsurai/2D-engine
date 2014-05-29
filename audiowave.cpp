#include "audiowave.h"
#include <stdint.h>
#include "debug.h"

namespace Tsuki {
    AudioWave::AudioWave()
    {
    }
    int AudioWave::open(const char *target) {
        int res;

        if(!(file = fopen(target, "rb"))) {
            return -1;
        }

        char type[4];
        uint32_t size, chunkSize;
        short formatType, channels;
        uint32_t sampleRate, avgBytesPerSec;
        short bytesPerSample, bitsPerSample;
        long dataSize;

        /* read header */
        fread(type, sizeof(char), 4, file);
        if(memcmp(type, "RIFF", 4) != 0) {
            return -1;
        }

        fread(&size, sizeof(unsigned long), 1, file);
        fread(type, sizeof(char), 4, file);
        if(memcmp(type, "WAVE", 4) != 0) {
            return -2;
        }

        fread(type, sizeof(char), 4, file);
        if(memcmp(type, "fmt ", 4) != 0) {
            return -3;
        }

        fread(&chunkSize, sizeof(uint32_t), 1, file);
        fread(&formatType, sizeof(short), 1, file);
        fread(&info.channels, sizeof(short), 1, file);
        fread(&info.sampleRate, sizeof(uint32_t), 1, file);
        fread(&avgBytesPerSec, sizeof(uint32_t), 1, file);
        fread(&info.bytesPerSample, sizeof(short), 1, file);
        fread(&info.bitsPerSample, sizeof(short), 1, file);


        //info.bitsPerSample = info.bytesPerSample * 8;
        debug() << info.channels;
        debug() << info.sampleRate;
        debug() << info.bitsPerSample;

        /* skip fact chunk (whatever that is good for) */
        fread(type, sizeof(char), 4, file);
        debug() << type;
        if(memcmp(type, "fact", 4) == 0) {
            fseek(file, 8, SEEK_CUR);
            fread(type, sizeof(char), 4, file);
        } else if(memcmp(type, "data", 4) != 0) {
            return -4;
        }

        fread(&info.dataSize, sizeof(long), 1, file);
        int sizee = info.dataSize;
        debug() << "size: " << sizee;

        data = (char *)malloc(info.dataSize);
        fread(data, sizeof(char), info.dataSize, file);

        fclose(file);

        return 0;
    }

    void AudioWave::close() {

    }

    void AudioWave::play() {

    }

    void AudioWave::stop() {

    }

    const void *AudioWave::getData() {
        return data;
    }
}

