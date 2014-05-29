#ifndef AUDIOSYSTEM_H
#define AUDIOSYSTEM_H

#include "prerequisites.h"
#include "singleton.h"
#include <AL/al.h>
#include <AL/alc.h>

namespace Tsuki {
    class AudioSystem : public Singleton<AudioSystem>
    {
    public:
        AudioSystem();
        ~AudioSystem();
        AudioSystem *instance();
        int initialize(const char *device=NULL);
        bool isInitialized();
        void test(AudioBase *sound);
        const char **listOutputDevices();
        const char **listCaptureDevices();

    private:
        struct listener_t {
            ALfloat position[3];
            ALfloat velocity[3];
            ALfloat orientation[6];
        } listener;

        bool mIsInitialized;
        ALCdevice *mDevice;
        ALCcontext *mContext;

        const char *getErrorString(ALenum err);
    };
}

#endif // AUDIOSYSTEM_H
