#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::ay38910
    @brief AY-3-8910 sound chip emulation (no I/O ports)
*/
#include "yakc/devices/sound.h"

namespace YAKC {

class ay8910 : public sound {
public:
    /// initialize the sound generator
    void init(int cpu_khz, int ay_cycle_length, int sound_hz);
    /// reset the sound generator
    void reset();
    /// step the sound generator, call after each CPU instruction
    void step(int cpu_cycles);

    /// write to selected register
    void write(uint8_t reg, uint8_t val);
    /// read from selected register
    uint8_t read(uint8_t reg) const;

    enum reg {
        TONE_PERIOD_A_FINE = 0,
        TONE_PERIOD_A_COARSE,
        TONE_PERIOD_B_FINE,
        TONE_PERIOD_B_COARSE,
        TONE_PERIOD_C_FINE,
        TONE_PERIOD_C_COARSE,
        NOISE_PERIOD,
        ENABLE,
        AMP_A,
        AMP_B,
        AMP_C,
        ENV_PERIOD_FINE,
        ENV_PERIOD_COARSE,
        ENV_SHAPE_CYCLE,
        IO_PORT_A,
        IO_PORT_B,

        NUM_REGS,
    };
    static_assert(NUM_REGS == 16, "AY8910 num regs!");

    uint8_t regs[NUM_REGS] = { };
    int tone_update_period = 0;
    int tone_update_count = 0;

    static const int num_channels = 3;
    struct channel_t {
        int count = 0;
        int period = 0;
        int bit = 0;
        int tone_disable = 0;
        int noise_disable = 0;
    } channels[num_channels];
    int noise_count = 0;
    int noise_period = 0;
    int noise_bit = 0;
    int noise_rng = 1;
    int env_count = 0;
    int env_period = 0;
    int env_cycle_count = 0;
    uint32_t env_volume = 0;
    int env_volume_add = 0;
};

} // namespace YAKC
