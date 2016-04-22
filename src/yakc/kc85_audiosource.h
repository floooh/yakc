#pragma once
//------------------------------------------------------------------------------
/**
    @class kc85_audiosource.h
    @brief implements a custom SoLoud audio source for KC85 sound generation
*/
#include "soloud.h"
#include "Core/Threading/RWLock.h"

class kc85_audiosource;

class kc85_audioinstance : public SoLoud::AudioSourceInstance {
public:
    kc85_audioinstance(kc85_audiosource* aParent);
    virtual void getAudio(float* aBuffer, unsigned int aSamples);
    virtual bool hasEnded();

    kc85_audiosource* parent;
};

class kc85_audiosource : public SoLoud::AudioSource {
public:
    kc85_audiosource();
    virtual SoLoud::AudioSourceInstance* createInstance();

    struct op {
        uint64_t cycle_pos = (uint64_t)-1;
        uint32_t hz = 0;
        uint32_t vol = 0;
    };
    struct channel {
        static const int size = 512;    // must be 2^N
        Oryol::RWLock lock;
        uint32_t write_pos = 0;
        uint32_t read_pos = 0;
        uint16_t phase_counter = 0;
        bool overflow = false;
        op ops[size];

        // push a new audio-op into the ringbuffer (called from main thread)
        void push(const op& in_op) {
            Oryol::ScopedWriteLock l(this->lock);
            this->ops[this->write_pos++] = in_op;
            this->write_pos &= (size-1);
            if (this->write_pos == this->read_pos) {
                // ringbuffer overflow
                this->read_pos = (this->read_pos+1) & (size-1);
                this->overflow = true;
            }
            else {
                this->overflow = false;
            }
            // always write a new 'infinity-op' past the newest write-pos
            this->ops[this->write_pos] = op();
        };
        // get the current audio op (called from audio thread)
        void peek(op& out_op) {
            // called from audio-thread
            Oryol::ScopedWriteLock l(this->lock);
            out_op = this->ops[this->read_pos];
        };
        // advance to next audio op if time is right (called from audio thread)
        void peek_next(uint64_t cur_cycle_pos, op& out_op) {
            Oryol::ScopedWriteLock l(this->lock);
            uint32_t next_read_pos = (this->read_pos+1) & (this->size-1);
            if (cur_cycle_pos >= this->ops[next_read_pos].cycle_pos) {
                this->read_pos = next_read_pos;
                out_op = this->ops[this->read_pos];
            }
        };
    };

    #if ORYOL_HAS_ATOMIC
    std::atomic<uint32_t> cpu_clock_speed;
    std::atomic<uint64_t> sample_cycle_count;
    #else
    uint32_t cpu_clock_speed;
    uint64_t sample_cycle_count;
    #endif
    uint32_t sample_rate = 44100;               // audio sample rate in Hz
    channel channels[2];
};
