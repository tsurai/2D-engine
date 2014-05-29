#include "audiosystem.h"
#include "audiobase.h"
#include "debug.h"

namespace Tsuki {
    AudioSystem::AudioSystem() : mIsInitialized(false) {
        /* initialize listener */
        listener.position[0] = 0.0;
        listener.position[1] = 0.0;
        listener.position[2] = 0.0;

        listener.velocity[0] = 0.0;
        listener.velocity[1] = 0.0;
        listener.velocity[2] = 0.0;

        listener.orientation[0] = 0.0;
        listener.orientation[1] = 0.0;
        listener.orientation[2] = 0.0;

        listener.orientation[3] = -1.0;
        listener.orientation[4] = 1.0;
        listener.orientation[5] = 0.0;
    }

    AudioSystem::~AudioSystem() {
        ALCcontext *context;

        alcMakeContextCurrent(NULL);
        alcDestroyContext(mContext);
        alcCloseDevice(mDevice);
    }

    template<> AudioSystem* Singleton<AudioSystem>::m_instance = 0;
    AudioSystem *AudioSystem::instance() {
        assert(m_instance);
        return m_instance;
    }

    int AudioSystem::initialize(const char *device) {
        if(mIsInitialized) {
            critical("[AudioSystem] already initialized");
            return -1;
        }

        if(!(mDevice = alcOpenDevice(device))) {
            critical("[AudioSystem] failed to open Output Device: %s",
                     ((device == NULL) ? alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER) : device));
            return -1;
        }

        mContext = alcCreateContext(mDevice, NULL);
        alcMakeContextCurrent(mContext);
        if(!mContext) {
            critical("[AudioSystem] failed to set context");
            return -2;
        }
    }

    void AudioSystem::test(AudioBase *sound) {
        ALuint source;
        ALuint buffer;
        ALuint frequency=sound->info.sampleRate;
        ALenum format=0;

        alGenBuffers(1, &buffer);
        alGenSources(1, &source);
        debug() << getErrorString(alGetError());

        if(sound->info.bitsPerSample == 8) {
            if(sound->info.channels == 1)
                format = AL_FORMAT_MONO8;
            else if(sound->info.channels == 2)
                format = AL_FORMAT_STEREO8;
        } else if(sound->info.bitsPerSample == 16) {
            if(sound->info.channels == 1)
                format = AL_FORMAT_MONO16;
            else if(sound->info.channels == 2)
                format = AL_FORMAT_STEREO16;
        } else {
            critical("[AudioSystem] unknown audio format");
            return;
        }

        //alBufferData(buffer, format, sound->getData(), sound->info.dataSize, frequency);
        debug() << getErrorString(alGetError());

        ALfloat SourcePos[] = { 0.0, 0.0, 0.0 };
        ALfloat SourceVel[] = { 0.0, 0.0, 0.0 };

        ALfloat ListenerPos[] = { 0.0, 0.0, 0.0 };
        ALfloat ListenerVel[] = { 0.0, 0.0, 0.0 };
        ALfloat ListenerOri[] = { 0.0, 0.0, -1.0,  0.0, 1.0, 0.0 };

        // Listener
        alListenerfv(AL_POSITION,   ListenerPos);
        alListenerfv(AL_VELOCITY,   ListenerVel);
        alListenerfv(AL_ORIENTATION,ListenerOri);

        // Source
        alSourcei(source, AL_BUFFER, buffer);
        alSourcef(source, AL_PITCH, 1.0f);
        alSourcef(source, AL_GAIN, 1.0f);
        alSourcefv(source, AL_POSITION, SourcePos);
        alSourcefv(source, AL_VELOCITY, SourceVel);
        alSourcei(source, AL_LOOPING, AL_FALSE);

        alSourcePlay(source);
    }

    const char **AudioSystem::listOutputDevices() {
        const char **bla;
        debug() << alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
        debug() << alcGetString(NULL, ALC_DEVICE_SPECIFIER);
        return bla;
    }

    const char **AudioSystem::listCaptureDevices() {
        const char **bla;
        debug() << alcGetString(NULL, ALC_CAPTURE_DEVICE_SPECIFIER);
        return bla;
    }

    bool AudioSystem::isInitialized() {
        return mIsInitialized;
    }

    const char *AudioSystem::getErrorString(ALenum err) {
        switch(err)
         {
             case AL_NO_ERROR:
                 return "AL_NO_ERROR";
             break;

             case AL_INVALID_NAME:
                 return "AL_INVALID_NAME";
             break;

             case AL_INVALID_ENUM:
                 return "AL_INVALID_ENUM";
             break;

             case AL_INVALID_VALUE:
                 return "AL_INVALID_VALUE";
             break;

             case AL_INVALID_OPERATION:
                 return "AL_INVALID_OPERATION";
             break;

             case AL_OUT_OF_MEMORY:
                 return "AL_OUT_OF_MEMORY";
             break;
         };
    }
}
