//------------------------------------------------------------------------------
//  yakc.cc
//------------------------------------------------------------------------------
#include "yakc.h"

namespace YAKC {

//------------------------------------------------------------------------------
void
yakc::init(const ext_funcs& sys_funcs, const sound_funcs& snd_funcs) {
    func = sys_funcs;
    this->cpu_ahead = false;
    this->cpu_behind = false;
    this->abs_cycle_count = 0;
    this->overflow_cycles = 0;
    this->kc85.init(&this->board);
    this->z1013.init(&this->board);
    this->z9001.init(&this->board);
    this->zx.init(&this->board);
    this->kc85.audio.setup_callbacks(snd_funcs);
    this->z9001.setup_sound_funcs(snd_funcs);
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
}

//------------------------------------------------------------------------------
bool
yakc::switchedon() const {
    return this->kc85.on || this->z1013.on || this->z9001.on || this->zx.on;
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
}

//------------------------------------------------------------------------------
void
yakc::clear_daisychain() {
    this->board.cpu.connect_irq_device(nullptr);
    this->board.ctc.init_daisychain(nullptr);
    this->board.pio.int_ctrl.connect_irq_device(nullptr);
    this->board.pio2.int_ctrl.connect_irq_device(nullptr);
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
}

//------------------------------------------------------------------------------
bool
yakc::is_device(device mask) const {
    return 0 != (int(this->model) & int(mask));
}

//------------------------------------------------------------------------------
void
yakc::onframe(int speed_multiplier, int micro_secs, uint64_t min_cycle_count, uint64_t max_cycle_count) {
    // FIXME: the speed multiplier isn't currently working because of the min/max cycle count limiter!
    YAKC_ASSERT(speed_multiplier > 0);
    this->cpu_ahead = false;
    this->cpu_behind = false;
    if (!this->board.dbg.paused) {
        // compute the end-cycle-count for the current frame
        if (this->abs_cycle_count == 0) {
            this->abs_cycle_count = min_cycle_count;
        }
        const int64_t num_cycles = this->board.clck.cycles(micro_secs*speed_multiplier) - this->overflow_cycles;
        uint64_t abs_end_cycles = this->abs_cycle_count + num_cycles;
        if ((max_cycle_count != 0) && (abs_end_cycles > max_cycle_count)) {
            abs_end_cycles = max_cycle_count;
            this->cpu_ahead = true;
        }
        else if ((min_cycle_count != 0) && (abs_end_cycles < min_cycle_count)) {
            abs_end_cycles = min_cycle_count;
            this->cpu_behind = true;
        }
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
        YAKC_ASSERT(this->abs_cycle_count >= abs_end_cycles);
        this->overflow_cycles = uint32_t(this->abs_cycle_count - abs_end_cycles);
    }
}

//------------------------------------------------------------------------------
void
yakc::put_key(ubyte ascii) {
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
        this->zx.put_key(ascii);
    }
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
    else {
        return "no info available";
    }
}

//------------------------------------------------------------------------------
z80bus*
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
    else if (this->zx.on) {
        this->zx.border_color(out_red, out_green, out_blue);
    }
    else {
        out_red = out_green = out_blue = 0.0f;
    }
}

} // namespace YAKC
