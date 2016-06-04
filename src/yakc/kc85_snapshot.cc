//------------------------------------------------------------------------------
//  kc85_snapshot.cc
//------------------------------------------------------------------------------
#include "kc85_snapshot.h"

namespace YAKC {

//------------------------------------------------------------------------------
bool
kc85_snapshot::is_snapshot(const state_t& state) {
    return (state.magic == 'YAKC') && (state.version == 1);
}

//------------------------------------------------------------------------------
void
kc85_snapshot::take_snapshot(const kc85& kc, state_t& state) {
    memset(&state, 0, sizeof(state));
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
void
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
void
kc85_snapshot::write_clock_state(const kc85& kc, state_t& state) {
    clock& clk = kc.board->clck;
    state.clock.base_freq_khz = clk.base_freq_khz;
    for (int i = 0; i < 4; i++) {
        state.clock.timers[i].freq_hz = clk.timers[i].freq_hz;
        state.clock.timers[i].count   = clk.timers[i].count;
        state.clock.timers[i].value   = clk.timers[i].value;
    }
}

//------------------------------------------------------------------------------
void
kc85_snapshot::apply_clock_state(const state_t& state, kc85& kc) {
    clock& clk = kc.board->clck;
    clk.base_freq_khz = state.clock.base_freq_khz;
    for (int i = 0; i < 4; i++) {
        clk.timers[i].freq_hz = state.clock.timers[i].freq_hz;
        clk.timers[i].count   = state.clock.timers[i].count;
        clk.timers[i].value   = state.clock.timers[i].value;
    }
}

//------------------------------------------------------------------------------
void
kc85_snapshot::write_kc_state(const kc85& kc, state_t& state) {
    state.kc.model = (ubyte) kc.cur_model;
    state.kc.caos  = (ubyte) kc.cur_caos;
    state.kc.io84 = kc.io84;
    state.kc.io86 = kc.io86;
    state.kc.pio_a = kc.pio_a;
    state.kc.pio_b = kc.pio_b;
}

//------------------------------------------------------------------------------
void
kc85_snapshot::apply_kc_state(const state_t& state, kc85& kc) {
    kc.cur_model = (device) state.kc.model;
    kc.cur_caos  = (os_rom) state.kc.caos;
    kc.io84      = state.kc.io84;
    kc.io86      = state.kc.io86;
    kc.pio_a     = state.kc.pio_a;
    kc.pio_b     = state.kc.pio_b;
    kc.update_rom_pointers();
}

//------------------------------------------------------------------------------
void
kc85_snapshot::write_cpu_state(const kc85& kc, state_t& state) {
    const z80& cpu = kc.board->cpu;
    state.cpu.AF  = cpu.AF;
    state.cpu.BC  = cpu.BC;
    state.cpu.DE  = cpu.DE;
    state.cpu.HL  = cpu.HL;
    state.cpu.WZ  = cpu.WZ;
    state.cpu.AF_ = cpu.AF_;
    state.cpu.BC_ = cpu.BC_;
    state.cpu.DE_ = cpu.DE_;
    state.cpu.HL_ = cpu.HL_;
    state.cpu.WZ_ = cpu.WZ_;
    state.cpu.IX  = cpu.IX;
    state.cpu.IY  = cpu.IY;
    state.cpu.SP  = cpu.SP;
    state.cpu.PC  = cpu.PC;
    state.cpu.I   = cpu.I;
    state.cpu.R   = cpu.R;
    state.cpu.IM  = cpu.IM;
    state.cpu.HALT = cpu.HALT;
    state.cpu.IFF1 = cpu.IFF1;
    state.cpu.IFF2 = cpu.IFF2;
    state.cpu.INV  = cpu.INV;
    state.cpu.irq_received = cpu.irq_received;
    state.cpu.enable_interrupt = cpu.enable_interrupt;
}

//------------------------------------------------------------------------------
void
kc85_snapshot::apply_cpu_state(const state_t& state, kc85& kc) {
    z80& cpu = kc.board->cpu;
    cpu.AF = state.cpu.AF;
    cpu.BC = state.cpu.BC;
    cpu.DE = state.cpu.DE;
    cpu.HL = state.cpu.HL;
    cpu.WZ = state.cpu.WZ;
    cpu.AF_ = state.cpu.AF_;
    cpu.BC_ = state.cpu.BC_;
    cpu.DE_ = state.cpu.DE_;
    cpu.HL_ = state.cpu.HL_;
    cpu.WZ_ = state.cpu.WZ_;
    cpu.IX  = state.cpu.IX;
    cpu.IY  = state.cpu.IY;
    cpu.SP  = state.cpu.SP;
    cpu.PC  = state.cpu.PC;
    cpu.I   = state.cpu.I;
    cpu.R   = state.cpu.R;
    cpu.IM  = state.cpu.IM;
    cpu.HALT = 0 != state.cpu.HALT;
    cpu.IFF1 = 0 != state.cpu.IFF1;
    cpu.IFF2 = 0 != state.cpu.IFF2;
    cpu.INV  = 0 != state.cpu.INV;
    cpu.irq_received = 0 != state.cpu.irq_received;
    cpu.enable_interrupt = 0 != state.cpu.enable_interrupt;
}

//------------------------------------------------------------------------------
void
kc85_snapshot::write_intctrl_state(const z80int& src, state_t::intctrl_t& dst) {
    dst.enabled = src.int_enabled;
    dst.requested = src.int_requested;
    dst.request_data = src.int_request_data;
    dst.pending = src.int_pending;
}

//------------------------------------------------------------------------------
void
kc85_snapshot::apply_intctrl_state(const state_t::intctrl_t& src, z80int& dst) {
    dst.int_enabled = 0 != src.enabled;
    dst.int_requested = 0 != src.requested;
    dst.int_request_data = src.request_data;
    dst.int_pending = 0 != src.pending;
}

//------------------------------------------------------------------------------
void
kc85_snapshot::write_ctc_state(const kc85& kc, state_t& state) {
    for (int c = 0; c < 4; c++) {
        auto& dst = state.ctc.chn[c];
        const auto& src = kc.board->ctc.channels[c];

        dst.down_counter = src.down_counter;
        dst.mode = src.mode;
        dst.constant = src.constant;
        dst.waiting_for_trigger = src.waiting_for_trigger;
        dst.interrupt_vector = src.interrupt_vector;
        write_intctrl_state(src.int_ctrl, dst.intctrl);
    }
}

//------------------------------------------------------------------------------
void
kc85_snapshot::apply_ctc_state(const state_t& state, kc85& kc) {
    for (int c = 0; c < 4; c++) {
        auto& dst = kc.board->ctc.channels[c];
        const auto& src = state.ctc.chn[c];

        dst.down_counter = src.down_counter;
        dst.mode = src.mode;
        dst.constant = src.constant;
        dst.waiting_for_trigger = 0 != src.waiting_for_trigger;
        dst.interrupt_vector = src.interrupt_vector;
        apply_intctrl_state(src.intctrl, dst.int_ctrl);
    }
}

//------------------------------------------------------------------------------
void
kc85_snapshot::write_pio_state(const kc85& kc, state_t& state) {
    const z80pio& pio = kc.board->pio;
    for (int i = 0; i < 2; i++) {
        state.pio.port[i] = pio.port[i];
    }
    write_intctrl_state(pio.int_ctrl, state.pio.intctrl);
}

//------------------------------------------------------------------------------
void
kc85_snapshot::apply_pio_state(const state_t& state, kc85& kc) {
    z80pio& pio = kc.board->pio;
    for (int i = 0; i < 2; i++) {
        pio.port[i] = state.pio.port[i];
    }
    apply_intctrl_state(state.pio.intctrl, pio.int_ctrl);
}

//------------------------------------------------------------------------------
void
kc85_snapshot::write_video_state(const kc85& kc, state_t& state) {
    state.video.cur_pal_line = kc.video.cur_pal_line;
    state.video.irm_control = kc.video.irm_control;
    state.video.pio_blink_flag = kc.video.pio_blink_flag;
    state.video.ctc_blink_flag = kc.video.ctc_blink_flag;
}

//------------------------------------------------------------------------------
void
kc85_snapshot::apply_video_state(const state_t& state, kc85& kc) {
    kc.video.model = (device) state.kc.model;
    kc.video.cur_pal_line = state.video.cur_pal_line;
    kc.video.irm_control = state.video.irm_control;
    kc.video.pio_blink_flag = 0 != state.video.pio_blink_flag;
    kc.video.ctc_blink_flag = 0 != state.video.ctc_blink_flag;
}

//------------------------------------------------------------------------------
void
kc85_snapshot::write_audio_state(const kc85& kc, state_t& state) {
    state.audio.volume = kc.audio.volume;
    for (int c = 0; c < 2; c++) {
        state.audio.chn[c].ctc_mode = kc.audio.channels[c].ctc_mode;
        state.audio.chn[c].ctc_constant = kc.audio.channels[c].ctc_constant;
    }
}

//------------------------------------------------------------------------------
void
kc85_snapshot::apply_audio_state(const state_t& state, kc85& kc) {
    kc.audio.reset();
    kc.audio.volume = state.audio.volume;
    for (int c = 0; c < 2; c++) {
        kc.audio.channels[c].ctc_mode = state.audio.chn[c].ctc_mode;
        kc.audio.channels[c].ctc_constant = state.audio.chn[c].ctc_constant;
    }
}

//------------------------------------------------------------------------------
void
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
void
kc85_snapshot::apply_exp_state(const state_t& state, kc85& kc) {
    for (int s = 0; s < 2; s++) {
        const auto& slot = state.exp.slots[s];
        if (kc.exp.slot_occupied(slot.slot_addr)) {
            kc.exp.remove_module(slot.slot_addr, kc.board->cpu.mem);
        }
        kc.exp.insert_module(slot.slot_addr, (kc85_exp::module_type)slot.module_type);
        kc.exp.update_control_byte(slot.slot_addr, slot.control_byte);
    }
}

//------------------------------------------------------------------------------
void
kc85_snapshot::write_memory_state(const kc85& kc, state_t& state) {
    static_assert(sizeof(kc.ram) == sizeof(state.ram), "general RAM size mismatch");
    static_assert(sizeof(kc.video.irm) == sizeof(state.irm), "video RAM size mismatch");

    memcpy(state.ram, kc.ram, sizeof(kc.ram));
    memcpy(state.irm, kc.video.irm, sizeof(kc.video.irm));

    // copy content of RAM modules
    const auto& slot08 = kc.exp.slot_by_addr(0x08);
    if (slot08.mod.mem_ptr && slot08.mod.mem_owned) {
        memcpy(state.ram8, slot08.mod.mem_ptr, slot08.mod.mem_size);
    }
    const auto& slot0C = kc.exp.slot_by_addr(0x0C);
    if (slot0C.mod.mem_ptr && slot0C.mod.mem_owned) {
        memcpy(state.ramC, slot0C.mod.mem_ptr, slot0C.mod.mem_size);
    }
}

//------------------------------------------------------------------------------
void
kc85_snapshot::apply_memory_state(const state_t& state, kc85& kc) {
    static_assert(sizeof(kc.ram) == sizeof(state.ram), "general RAM size mismatch");
    static_assert(sizeof(kc.video.irm) == sizeof(state.irm), "video RAM size mismatch");

    memcpy(kc.ram, state.ram, sizeof(state.ram));
    memcpy(kc.video.irm, state.irm, sizeof(state.irm));
    const auto& slot08 = kc.exp.slot_by_addr(0x08);
    if (slot08.mod.mem_ptr && slot08.mod.mem_owned) {
        memcpy(slot08.mod.mem_ptr, state.ram8, slot08.mod.mem_size);
    }
    const auto& slot0C = kc.exp.slot_by_addr(0x0C);
    if (slot0C.mod.mem_ptr && slot0C.mod.mem_owned) {
        memcpy(slot0C.mod.mem_ptr, state.ramC, slot0C.mod.mem_size);
    }
    kc.update_bank_switching();
}

} // namespace YAKC
