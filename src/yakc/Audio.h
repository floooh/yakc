#pragma once
//------------------------------------------------------------------------------
/**
    @class Audio.h
    @brief implement audio playback callback
*/
#include "KC85Oryol.h"
#include "soloud.h"
#include "Core/Containers/Queue.h"

class Audio {
public:
    /// setup audio playback
    void Setup(yakc::kc85& kc);
    /// shutdown audio playback
    void Discard();
    /// call at end of frame
    void Update(int micro_secs);

private:
    class kcAudio;

    class kcAudioInstance : public SoLoud::AudioSourceInstance {
    public:
        kcAudioInstance(kcAudio* aParent);
        virtual void getAudio(float* aBuffer, unsigned int aSamples);
        virtual bool hasEnded();

        kcAudio* parent;
        uint32_t freqCounter[2];
    };
    class kcAudio : public SoLoud::AudioSource {
    public:
        kcAudio() {
            this->mChannels = 2;
            this->freq[0] = 0;
            this->freq[1] = 0;
        };
        virtual kcAudioInstance* createInstance() {
            return new kcAudioInstance(this);
        };
        uint32_t freq[2];
    };

    /// convert 1.75MHz ticks to audio ticks, including a base delay
    unsigned int asAudioTicks(unsigned int z80Cycles) const;
    /// callback to start sound or change frequency
    static void cb_sound(void* userdata, uint32_t delay_cycles, int channel, int hz);
    /// callback to stop sound
    static void cb_stop(void* userdata, uint32_t delay_cycles, int channel);
    /// callback to change volume
    static void cb_volume(void* userdata, uint32_t delay_cycles, int vol);

    static const int BaseDelayTicks = 44100 / 15;
    yakc::kc85* kc = nullptr;
    SoLoud::Soloud soloud;
    kcAudio audioSource;
    int audioHandle = 0;
};