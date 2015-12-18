#pragma once
//------------------------------------------------------------------------------
/**
    @class kc85_snapshot
    @brief helper functions for taking and applying machine state snapshots
*/
#include "core/common.h"
#include "core/kc85.h"

namespace yakc {

class kc85_snapshot {
public:
    /// snapshot state
    #pragma pack(push,1)
    struct state_t {
        int magic;
        int version;

        // clock state
        struct clock_t {
            int base_freq_khz;
            struct timer_t {
                int freq_hz;
                int count;
                int value;
            } timers[4];
        } clock;
        static_assert((sizeof(clock_t)&3)==0, "clock_t odd size!");

        // central KC85 state
        struct kc_t {
            ubyte model;
            ubyte caos;
            ubyte io84;
            ubyte io86;
        } kc;
        static_assert((sizeof(kc_t)&3)==0, "kc_t odd size!");

        // cpu state
        struct cpu_t {
            uword AF, BC, DE, HL;
            uword AF_, BC_, DE_, HL_;
            uword IX, IY, SP, PC;
            ubyte I, R, IM;
            ubyte HALT, IFF1, IFF2;

            ubyte INV;
            ubyte irq_received;
            ubyte enable_interrupt;
            ubyte padding[3];
        } cpu;
        static_assert((sizeof(cpu_t)&3)==0, "cpu_t odd size!");

        // interrupt controller data
        struct intctrl_t {
            ubyte enabled;
            ubyte requested;
            ubyte request_data;
            ubyte pending;
        };

        // ctc state
        struct ctc_t {
            struct chn_t {
                int down_counter;
                ubyte mode;
                ubyte constant;
                ubyte waiting_for_trigger;
                ubyte interrupt_vector;
                intctrl_t intctrl;
            } chn[4];
        } ctc;
        static_assert((sizeof(ctc_t)&3)==0, "ctc_t odd size!");

        // pio state
        struct pio_t {
            struct chn_t {
                ubyte interrupt_vector;
                ubyte interrupt_enabled;
                ubyte mode;
                ubyte padding;
            } chn[2];
            intctrl_t intctrl;
        } pio;
        static_assert((sizeof(pio_t)&3)==0, "pio_t odd size!");

        // video state
        struct video_t {
            uword cur_pal_line;
            ubyte irm_control;
            ubyte pio_blink_flag;
            ubyte ctc_blink_flag;
            ubyte padding[3];
        } video;
        static_assert((sizeof(video_t)&3)==0, "video_t odd size!");

        // audio state
        struct audio_t {
            int volume;
            struct channel_t {
                ubyte ctc_mode;
                ubyte ctc_constant;
            } chn[2];
        } audio;
        static_assert((sizeof(audio_t)&3)==0, "audio_t odd size!");

        // expansion module system
        struct exp_t {
            struct slot_t {
                ubyte slot_addr;        // 0x08 or 0x0C
                ubyte module_type;      // kc85_exp::module_type
                ubyte control_byte;
                ubyte padding;
            } slots[2];
        } exp;
        static_assert((sizeof(exp_t)&3)==0, "exp_t odd size!");

        // system RAM banks
        ubyte ram[4][0x4000];
        // IRM RAM banks
        ubyte irm[4][0x4000];
        // optional slot 0x08 RAM banks
        ubyte ram8[4][0x4000];
        // optional slot 0x0C RAM banks
        ubyte ramC[4][0x4000];
    };
    static_assert((sizeof(state_t)&3)==0, "state odd size!");
    #pragma pack(pop)

    /// record a snapshot into a memory buffer
    static void take_snapshot(const kc85& kc, state_t& state);
    /// apply a snapshot from memory buffer
    static void apply_snapshot(const state_t& state, kc85& kc);
    /// test if state_t contains a valid snapshot
    static bool is_snapshot(const state_t& state);

    /// write the clock state
    static void write_clock_state(const kc85& kc, state_t& state);
    /// apply clock state
    static void apply_clock_state(const state_t& state, kc85& kc);
    /// write the toplevel KC state
    static void write_kc_state(const kc85& kc, state_t& state);
    /// apply toplevel KC state
    static void apply_kc_state(const state_t& state, kc85& kc);
    /// write the cpu state
    static void write_cpu_state(const kc85& kc, state_t& state);
    /// apply cpu state
    static void apply_cpu_state(const state_t& state, kc85& kc);
    /// write interrupt controller state
    static void write_intctrl_state(const z80int& src, state_t::intctrl_t& dst);
    /// apply interrupt controller state
    static void apply_intctrl_state(const state_t::intctrl_t& src, z80int& dst);
    /// write the ctc state
    static void write_ctc_state(const kc85& kc, state_t& state);
    /// apply ctc state
    static void apply_ctc_state(const state_t& state, kc85& kc);
    /// write the pio state
    static void write_pio_state(const kc85& kc, state_t& state);
    /// apply pio state
    static void apply_pio_state(const state_t& state, kc85& kc);
    /// write the video state
    static void write_video_state(const kc85& kc, state_t& state);
    /// apply video state
    static void apply_video_state(const state_t& state, kc85& kc);
    /// write the audio state
    static void write_audio_state(const kc85& kc, state_t& state);
    /// apply audio state
    static void apply_audio_state(const state_t& state, kc85& kc);
    /// write the expansion system state
    static void write_exp_state(const kc85& kc, state_t& state);
    /// apply expansion system state
    static void apply_exp_state(const state_t& state, kc85& kc);
    /// write memory state
    static void write_memory_state(const kc85& kc, state_t& state);
    /// apply memory state
    static void apply_memory_state(const state_t& state, kc85& kc);
};

//------------------------------------------------------------------------------
inline bool
kc85_snapshot::is_snapshot(const state_t& state) {
    return (state.magic == 'YAKC') && (state.version == 1);
}

//------------------------------------------------------------------------------
inline void
kc85_snapshot::take_snapshot(const kc85& kc, state_t& state) {
    YAKC_MEMSET(&state, 0, sizeof(state));
    state.magic = 'YAKC';
    state.version = 1;
    write_clock_state(kc, state);
    write_kc_state(kc, state);
    write_cpu_state(kc, state);
    write_ctc_state(kc, state);
    write_pio_state(kc, state);
    write_video_state(kc, state);
    write_audio_state(kc, state);
    write_exp_state(kc, state);
    write_memory_state(kc, state);
}

//------------------------------------------------------------------------------
inline void
kc85_snapshot::apply_snapshot(const state_t& state, kc85& kc) {
    YAKC_ASSERT(is_snapshot(state));
    apply_clock_state(state, kc);
    apply_kc_state(state, kc);
    apply_cpu_state(state, kc);
    apply_ctc_state(state, kc);
    apply_pio_state(state, kc);
    apply_video_state(state, kc);
    apply_audio_state(state, kc);
    apply_exp_state(state, kc);
    apply_memory_state(state, kc);
}

//------------------------------------------------------------------------------
inline void
kc85_snapshot::write_clock_state(const kc85& kc, state_t& state) {
    state.clock.base_freq_khz = kc.clck.base_freq_khz;
    for (int i = 0; i < 4; i++) {
        state.clock.timers[i].freq_hz = kc.clck.timers[i].freq_hz;
        state.clock.timers[i].count   = kc.clck.timers[i].count;
        state.clock.timers[i].value   = kc.clck.timers[i].value;
    }
}

//------------------------------------------------------------------------------
inline void
kc85_snapshot::apply_clock_state(const state_t& state, kc85& kc) {
    kc.clck.base_freq_khz = state.clock.base_freq_khz;
    for (int i = 0; i < 4; i++) {
        kc.clck.timers[i].freq_hz = state.clock.timers[i].freq_hz;
        kc.clck.timers[i].count   = state.clock.timers[i].count;
        kc.clck.timers[i].value   = state.clock.timers[i].value;
    }
}

//------------------------------------------------------------------------------
inline void
kc85_snapshot::write_kc_state(const kc85& kc, state_t& state) {
    state.kc.model = (ubyte) kc.cur_model;
    state.kc.caos  = (ubyte) kc.cur_caos;
    state.kc.io84 = kc.io84;
    state.kc.io86 = kc.io86;
}

//------------------------------------------------------------------------------
inline void
kc85_snapshot::apply_kc_state(const state_t& state, kc85& kc) {
    kc.cur_model = (kc85_model) state.kc.model;
    kc.cur_caos  = (kc85_caos) state.kc.caos;
    kc.io84      = state.kc.io84;
    kc.io86      = state.kc.io86;
    kc.update_rom_pointers();
}

//------------------------------------------------------------------------------
inline void
kc85_snapshot::write_cpu_state(const kc85& kc, state_t& state) {
    state.cpu.AF  = kc.cpu.state.AF;
    state.cpu.BC  = kc.cpu.state.BC;
    state.cpu.DE  = kc.cpu.state.DE;
    state.cpu.HL  = kc.cpu.state.HL;
    state.cpu.AF_ = kc.cpu.state.AF_;
    state.cpu.BC_ = kc.cpu.state.BC_;
    state.cpu.DE_ = kc.cpu.state.DE_;
    state.cpu.HL_ = kc.cpu.state.HL_;
    state.cpu.IX  = kc.cpu.state.IX;
    state.cpu.IY  = kc.cpu.state.IY;
    state.cpu.SP  = kc.cpu.state.SP;
    state.cpu.PC  = kc.cpu.state.PC;
    state.cpu.I   = kc.cpu.state.I;
    state.cpu.R   = kc.cpu.state.R;
    state.cpu.IM  = kc.cpu.state.IM;
    state.cpu.HALT = kc.cpu.state.HALT;
    state.cpu.IFF1 = kc.cpu.state.IFF1;
    state.cpu.IFF2 = kc.cpu.state.IFF2;
    state.cpu.INV  = kc.cpu.state.INV;
    state.cpu.irq_received = kc.cpu.irq_received;
    state.cpu.enable_interrupt = kc.cpu.enable_interrupt;
}

//------------------------------------------------------------------------------
inline void
kc85_snapshot::apply_cpu_state(const state_t& state, kc85& kc) {
    kc.cpu.state.AF = state.cpu.AF;
    kc.cpu.state.BC = state.cpu.BC;
    kc.cpu.state.DE = state.cpu.DE;
    kc.cpu.state.HL = state.cpu.HL;
    kc.cpu.state.AF_ = state.cpu.AF_;
    kc.cpu.state.BC_ = state.cpu.BC_;
    kc.cpu.state.DE_ = state.cpu.DE_;
    kc.cpu.state.HL_ = state.cpu.HL_;
    kc.cpu.state.IX  = state.cpu.IX;
    kc.cpu.state.IY  = state.cpu.IY;
    kc.cpu.state.SP  = state.cpu.SP;
    kc.cpu.state.PC  = state.cpu.PC;
    kc.cpu.state.I   = state.cpu.I;
    kc.cpu.state.R   = state.cpu.R;
    kc.cpu.state.IM  = state.cpu.IM;
    kc.cpu.state.HALT = state.cpu.HALT;
    kc.cpu.state.IFF1 = state.cpu.IFF1;
    kc.cpu.state.IFF2 = state.cpu.IFF2;
    kc.cpu.state.INV  = state.cpu.INV;
    kc.cpu.irq_received = state.cpu.irq_received;
    kc.cpu.enable_interrupt = state.cpu.enable_interrupt;
}

//------------------------------------------------------------------------------
inline void
kc85_snapshot::write_intctrl_state(const z80int& src, state_t::intctrl_t& dst) {
    dst.enabled = src.int_enabled;
    dst.requested = src.int_requested;
    dst.request_data = src.int_request_data;
    dst.pending = src.int_pending;
}

//------------------------------------------------------------------------------
inline void
kc85_snapshot::apply_intctrl_state(const state_t::intctrl_t& src, z80int& dst) {
    dst.int_enabled = src.enabled;
    dst.int_requested = src.requested;
    dst.int_request_data = src.request_data;
    dst.int_pending = src.pending;
}

//------------------------------------------------------------------------------
inline void
kc85_snapshot::write_ctc_state(const kc85& kc, state_t& state) {
    for (int c = 0; c < 4; c++) {
        auto& dst = state.ctc.chn[c];
        const auto& src = kc.ctc.channels[c];

        dst.down_counter = src.down_counter;
        dst.mode = src.mode;
        dst.constant = src.constant;
        dst.waiting_for_trigger = src.waiting_for_trigger;
        dst.interrupt_vector = src.interrupt_vector;
        write_intctrl_state(src.int_ctrl, dst.intctrl);
    }
}

//------------------------------------------------------------------------------
inline void
kc85_snapshot::apply_ctc_state(const state_t& state, kc85& kc) {
    for (int c = 0; c < 4; c++) {
        auto& dst = kc.ctc.channels[c];
        const auto& src = state.ctc.chn[c];

        dst.down_counter = src.down_counter;
        dst.mode = src.mode;
        dst.constant = src.constant;
        dst.waiting_for_trigger = src.waiting_for_trigger;
        dst.interrupt_vector = src.interrupt_vector;
        apply_intctrl_state(src.intctrl, dst.int_ctrl);
    }
}

//------------------------------------------------------------------------------
inline void
kc85_snapshot::write_pio_state(const kc85& kc, state_t& state) {
    for (int c = 0; c < 2; c++) {
        auto& dst = state.pio.chn[c];
        const auto& src = kc.pio.channel_state[c];

        dst.interrupt_vector = src.interrupt_vector;
        dst.interrupt_enabled = src.interrupt_enabled;
        dst.mode = src.mode;
    }
    write_intctrl_state(kc.pio.int_ctrl, state.pio.intctrl);
}

//------------------------------------------------------------------------------
inline void
kc85_snapshot::apply_pio_state(const state_t& state, kc85& kc) {
    for (int c = 0; c < 2; c++) {
        auto& dst = kc.pio.channel_state[c];
        const auto& src = state.pio.chn[c];

        dst.interrupt_vector = src.interrupt_vector;
        dst.interrupt_enabled = src.interrupt_enabled;
        dst.mode = src.mode;
    }
    apply_intctrl_state(state.pio.intctrl, kc.pio.int_ctrl);
}

//------------------------------------------------------------------------------
inline void
kc85_snapshot::write_video_state(const kc85& kc, state_t& state) {
    state.video.cur_pal_line = kc.video.cur_pal_line;
    state.video.irm_control = kc.video.irm_control;
    state.video.pio_blink_flag = kc.video.pio_blink_flag;
    state.video.ctc_blink_flag = kc.video.ctc_blink_flag;
}

//------------------------------------------------------------------------------
inline void
kc85_snapshot::apply_video_state(const state_t& state, kc85& kc) {
    kc.video.cur_pal_line = state.video.cur_pal_line;
    kc.video.irm_control = state.video.irm_control;
    kc.video.pio_blink_flag = state.video.pio_blink_flag;
    kc.video.ctc_blink_flag = state.video.ctc_blink_flag;
}

//------------------------------------------------------------------------------
inline void
kc85_snapshot::write_audio_state(const kc85& kc, state_t& state) {
    state.audio.volume = kc.audio.volume;
    for (int c = 0; c < 2; c++) {
        state.audio.chn[c].ctc_mode = kc.audio.channels[c].ctc_mode;
        state.audio.chn[c].ctc_constant = kc.audio.channels[c].ctc_constant;
    }
}

//------------------------------------------------------------------------------
inline void
kc85_snapshot::apply_audio_state(const state_t& state, kc85& kc) {
    kc.audio.volume = state.audio.volume;
    for (int c = 0; c < 2; c++) {
        kc.audio.channels[c].ctc_mode = state.audio.chn[c].ctc_mode;
        kc.audio.channels[c].ctc_constant = state.audio.chn[c].ctc_constant;
    }
}

//------------------------------------------------------------------------------
inline void
kc85_snapshot::write_exp_state(const kc85& kc, state_t& state) {
    for (int s = 0; s < 2; s++) {
        auto& dst = state.exp.slots[s];
        const auto& src = kc.exp.slots[s];

        dst.slot_addr = src.slot_addr;
        dst.module_type = src.mod.type;
        dst.control_byte = src.control_byte;
    }
}

//------------------------------------------------------------------------------
inline void
kc85_snapshot::apply_exp_state(const state_t& state, kc85& kc) {
    for (int s = 0; s < 2; s++) {
        const auto& slot = state.exp.slots[s];
        kc.exp.insert_module(slot.slot_addr, (kc85_exp::module_type)slot.module_type);
        kc.exp.update_control_byte(slot.slot_addr, slot.control_byte);
    }
}

//------------------------------------------------------------------------------
inline void
kc85_snapshot::write_memory_state(const kc85& kc, state_t& state) {
    static_assert(sizeof(kc.ram) == sizeof(state.ram), "general RAM size mismatch");
    static_assert(sizeof(kc.video.irm) == sizeof(state.irm), "video RAM size mismatch");

    YAKC_MEMCPY(state.ram, kc.ram, sizeof(kc.ram));
    YAKC_MEMCPY(state.irm, kc.video.irm, sizeof(kc.video.irm));
}

//------------------------------------------------------------------------------
inline void
kc85_snapshot::apply_memory_state(const state_t& state, kc85& kc) {
    static_assert(sizeof(kc.ram) == sizeof(state.ram), "general RAM size mismatch");
    static_assert(sizeof(kc.video.irm) == sizeof(state.irm), "video RAM size mismatch");

    YAKC_MEMCPY(kc.ram, state.ram, sizeof(state.ram));
    YAKC_MEMCPY(kc.video.irm, state.irm, sizeof(state.irm));

    kc.update_bank_switching();
}

} // namespace yakc
