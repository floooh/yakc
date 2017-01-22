//------------------------------------------------------------------------------
//  yakc.cc
//------------------------------------------------------------------------------
#include "yakc.h"

namespace YAKC {

//------------------------------------------------------------------------------
void
yakc::init(const ext_funcs& sys_funcs) {
    func = sys_funcs;
    this->cpu_ahead = false;
    this->cpu_behind = false;
    this->abs_cycle_count = 0;
    this->overflow_cycles = 0;
    this->kc85.init(&this->board, &this->roms);
    this->z1013.init(&this->board, &this->roms);
    this->z9001.init(&this->board, &this->roms);
    this->zx.init(&this->board, &this->roms);
    this->cpc.init(&this->board, &this->roms);
    this->bbcmicro.init(&this->board, &this->roms);
}

//------------------------------------------------------------------------------
void
yakc::add_rom(rom_images::rom type, const uint8_t* ptr, int size) {
    this->roms.add(type, ptr, size);
}

//------------------------------------------------------------------------------
bool
yakc::check_roms(device m, os_rom os) {
    if (is_device(m, device::any_kc85)) {
        return kc85::check_roms(this->roms, m, os);
    }
    else if (is_device(m, device::any_z1013)) {
        return z1013::check_roms(this->roms, m, os);
    }
    else if (is_device(m, device::any_z9001)) {
        return z9001::check_roms(this->roms, m, os);
    }
    else if (is_device(m, device::any_zx)) {
        return zx::check_roms(this->roms, m, os);
    }
    else if (is_device(m, device::any_cpc)) {
        return cpc::check_roms(this->roms, m, os);
    }
    else if (is_device(m, device::any_bbcmicro)) {
        return bbcmicro::check_roms(this->roms, m, os);
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
void
yakc::poweron(device m, os_rom rom) {
    YAKC_ASSERT(!this->kc85.on && !this->z1013.on);
    this->clear_daisychain();
    this->model = m;
    this->os = rom;
    this->abs_cycle_count = 0;
    this->overflow_cycles = 0;
    if (this->is_device(device::any_kc85)) {
        this->kc85.poweron(m, rom);
    }
    else if (this->is_device(device::any_z1013)) {
        this->z1013.poweron(m);
    }
    else if (this->is_device(device::any_z9001)) {
        this->z9001.poweron(m, rom);
    }
    else if (this->is_device(device::any_zx)) {
        this->zx.poweron(m);
    }
    else if (this->is_device(device::any_cpc)) {
        this->cpc.poweron(m);
    }
    else if (this->is_device(device::any_bbcmicro)) {
        this->bbcmicro.poweron(m);
    }
}

//------------------------------------------------------------------------------
void
yakc::poweroff() {
    if (this->kc85.on) {
        this->kc85.poweroff();
    }
    if (this->z1013.on) {
        this->z1013.poweroff();
    }
    if (this->z9001.on) {
        this->z9001.poweroff();
    }
    if (this->zx.on) {
        this->zx.poweroff();
    }
    if (this->cpc.on) {
        this->cpc.poweroff();
    }
    if (this->bbcmicro.on) {
        this->bbcmicro.poweroff();
    }
}

//------------------------------------------------------------------------------
bool
yakc::switchedon() const {
    return this->kc85.on || this->z1013.on || this->z9001.on || this->zx.on || this->cpc.on || this->bbcmicro.on;
}

//------------------------------------------------------------------------------
void
yakc::reset() {
    this->overflow_cycles = 0;
    if (this->kc85.on) {
        this->kc85.reset();
    }
    if (this->z1013.on) {
        this->z1013.reset();
    }
    if (this->z9001.on) {
        this->z9001.reset();
    }
    if (this->zx.on) {
        this->zx.reset();
    }
    if (this->cpc.on) {
        this->cpc.reset();
    }
    if (this->bbcmicro.on) {
        this->bbcmicro.reset();
    }
}

//------------------------------------------------------------------------------
void
yakc::clear_daisychain() {
    this->board.z80cpu.connect_irq_device(nullptr);
    this->board.z80ctc.init_daisychain(nullptr);
    this->board.z80pio.int_ctrl.connect_irq_device(nullptr);
    this->board.z80pio2.int_ctrl.connect_irq_device(nullptr);
}

//------------------------------------------------------------------------------
void
yakc::on_context_switched() {
    this->clear_daisychain();
    if (this->is_device(device::any_kc85)) {
        this->kc85.on_context_switched();
    }
    else if (this->is_device(device::any_z1013)) {
        this->z1013.on_context_switched();
    }
    else if (this->is_device(device::any_z9001)) {
        this->z9001.on_context_switched();
    }
    else if (this->is_device(device::any_zx)) {
        this->zx.on_context_switched();
    }
    else if (this->is_device(device::any_cpc)) {
        this->cpc.on_context_switched();
    }
    else if (this->is_device(device::any_bbcmicro)) {
        this->bbcmicro.on_context_switched();
    }
}

//------------------------------------------------------------------------------
bool
yakc::is_device(device mask) const {
    return 0 != (int(this->model) & int(mask));
}

//------------------------------------------------------------------------------
bool
yakc::is_device(device model, device mask) {
    return 0 != (int(model) & int(mask));
}

//------------------------------------------------------------------------------
cpu
yakc::cpu_type() const {
    if (this->is_device(device::any_bbcmicro)) {
        return cpu::mos6502;
    }
    else {
        return cpu::z80;
    }
}

//------------------------------------------------------------------------------
void
yakc::step(int micro_secs, uint64_t audio_cycle_count) {
    uint64_t min_cycle_count = 0;
    uint64_t max_cycle_count = 0;
    if (audio_cycle_count > 0) {
        const uint64_t cpu_min_ahead_cycles = (this->board.clck.base_freq_khz*1000)/100;
        const uint64_t cpu_max_ahead_cycles = (this->board.clck.base_freq_khz*1000)/25;
        min_cycle_count = audio_cycle_count + cpu_min_ahead_cycles;
        max_cycle_count = audio_cycle_count + cpu_max_ahead_cycles;
    }
    this->cpu_ahead = false;
    this->cpu_behind = false;
    // compute the end-cycle-count for the current frame
    if (this->abs_cycle_count == 0) {
        this->abs_cycle_count = min_cycle_count;
    }
    const int64_t num_cycles = this->board.clck.cycles(micro_secs) - this->overflow_cycles;
    uint64_t abs_end_cycles = this->abs_cycle_count + num_cycles;
    if ((max_cycle_count != 0) && (abs_end_cycles > max_cycle_count)) {
        abs_end_cycles = max_cycle_count;
        this->cpu_ahead = true;
    }
    else if ((min_cycle_count != 0) && (abs_end_cycles < min_cycle_count)) {
        abs_end_cycles = min_cycle_count;
        this->cpu_behind = true;
    }
    if (!this->board.dbg.paused) {
        if (this->kc85.on) {
            this->abs_cycle_count = this->kc85.step(this->abs_cycle_count, abs_end_cycles);
        }
        else if (this->z1013.on) {
            this->abs_cycle_count = this->z1013.step(this->abs_cycle_count, abs_end_cycles);
        }
        else if (this->z9001.on) {
            this->abs_cycle_count = this->z9001.step(this->abs_cycle_count, abs_end_cycles);
        }
        else if (this->zx.on) {
            this->abs_cycle_count = this->zx.step(this->abs_cycle_count, abs_end_cycles);
        }
        else if (this->cpc.on) {
            this->abs_cycle_count = this->cpc.step(this->abs_cycle_count, abs_end_cycles);
        }
        else if (this->bbcmicro.on) {
            this->abs_cycle_count = this->bbcmicro.step(this->abs_cycle_count, abs_end_cycles);
        }
        YAKC_ASSERT(this->abs_cycle_count >= abs_end_cycles);
        this->overflow_cycles = uint32_t(this->abs_cycle_count - abs_end_cycles);
    }
    else {
        this->abs_cycle_count = abs_end_cycles;
        this->overflow_cycles = 0;
    }
}

//------------------------------------------------------------------------------
void
yakc::put_input(uint8_t ascii, uint8_t joy0_mask) {
    if (!this->joystick_enabled) {
        joy0_mask = 0;
    }
    if (this->kc85.on) {
        this->kc85.put_key(ascii);
    }
    if (this->z1013.on) {
        this->z1013.put_key(ascii);
    }
    if (this->z9001.on) {
        this->z9001.put_key(ascii);
    }
    if (this->zx.on) {
        this->zx.put_input(ascii, joy0_mask);
    }
    if (this->cpc.on) {
        this->cpc.put_input(ascii, joy0_mask);
    }
}

//------------------------------------------------------------------------------
void
yakc::enable_joystick(bool b) {
    this->joystick_enabled = b;
}

//------------------------------------------------------------------------------
bool
yakc::is_joystick_enabled() const {
    return this->joystick_enabled;
}

//------------------------------------------------------------------------------
const char*
yakc::system_info() const {
    if (this->kc85.on) {
        return this->kc85.system_info();
    }
    else if (this->z1013.on) {
        return this->z1013.system_info();
    }
    else if (this->z9001.on) {
        return this->z9001.system_info();
    }
    else if (this->zx.on) {
        return this->zx.system_info();
    }
    else if (this->cpc.on) {
        return this->cpc.system_info();
    }
    else if (this->bbcmicro.on) {
        return this->bbcmicro.system_info();
    }
    else {
        return "no info available";
    }
}

//------------------------------------------------------------------------------
system_bus*
yakc::get_bus() {
    if (this->is_device(device::any_kc85)) {
        return &this->kc85;
    }
    else if (this->is_device(device::any_z1013)) {
        return &this->z1013;
    }
    else if (this->is_device(device::any_z9001)) {
        return &this->z9001;
    }
    else if (this->is_device(device::any_zx)) {
        return &this->zx;
    }
    else if (this->is_device(device::any_cpc)) {
        return &this->cpc;
    }
    else if (this->is_device(device::any_bbcmicro)) {
        return &this->bbcmicro;
    }
    else {
        return nullptr;
    }
}

//------------------------------------------------------------------------------
void
yakc::border_color(float& out_red, float& out_green, float& out_blue) {
    if (this->z9001.on) {
        this->z9001.border_color(out_red, out_green, out_blue);
    }
    else {
        out_red = out_green = out_blue = 0.0f;
    }
}

//------------------------------------------------------------------------------
void
yakc::fill_sound_samples(float* buffer, int num_samples) {
    if (this->kc85.on) {
        return this->kc85.decode_audio(buffer, num_samples);
    }
    else if (this->z9001.on) {
        return this->z9001.decode_audio(buffer, num_samples);
    }
    else if (this->zx.on) {
        return this->zx.decode_audio(buffer, num_samples);
    }
    else if (this->cpc.on) {
        return this->cpc.decode_audio(buffer, num_samples);
    }
    else {
        clear(buffer, num_samples * sizeof(float));
    }
}

//------------------------------------------------------------------------------
const void*
yakc::framebuffer(int& out_width, int& out_height) {
    if (this->kc85.on) {
        return this->kc85.framebuffer(out_width, out_height);
    }
    else if (this->z9001.on) {
        return this->z9001.framebuffer(out_width, out_height);
    }
    else if (this->z1013.on) {
        return this->z1013.framebuffer(out_width, out_height);
    }
    else if (this->zx.on) {
        return this->zx.framebuffer(out_width, out_height);
    }
    else if (this->cpc.on) {
        return this->cpc.framebuffer(out_width, out_height);
    }
    else if (this->bbcmicro.on) {
        return this->bbcmicro.framebuffer(out_width, out_height);
    }
    else {
        out_width = 0;
        out_height = 0;
        return nullptr;
    }
}

} // namespace YAKC
