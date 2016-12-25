#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::sound_ay38910
    @brief AY-3-8910 sound chip emulation (no I/O ports)
*/
#include "yakc/sound.h"

namespace YAKC {

class sound_ay8910 : public sound {
public:
    /// initialize the sound generator
    void init(int cpu_khz, int ay_cycle_length, int sound_hz);
    /// reset the sound generator
    void reset();
    /// step the sound generator, call after each CPU instruction
    void step(int cpu_cycles);

    /// write to selected register
    void write(ubyte reg, ubyte val);
    /// read from selected register
    ubyte read(ubyte reg) const;

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

    ubyte regs[NUM_REGS] = { };
    int tone_length = 0;
    int tone_count = 0;

    static const int num_channels = 3;
    struct {
        int count = 0;
        int period = 0;
        bool bit = false;
    } channels[num_channels];
};

} // namespace YAKC
