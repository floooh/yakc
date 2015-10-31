//------------------------------------------------------------------------------
//  kc85.cc
//------------------------------------------------------------------------------
#include "kc85.h"

namespace yakc {

//------------------------------------------------------------------------------
kc85::kc85() :
cur_model(kc_model::none),
is_paused(false) {
    memset(this->ram0, 0, sizeof(this->ram0));
    memset(this->ram1, 0, sizeof(this->ram1));
    memset(this->irm0, 0, sizeof(this->irm0));
    memset(this->irm1, 0, sizeof(this->irm1));
    memset(this->rom0, 0, sizeof(this->rom0));
}

//------------------------------------------------------------------------------
void
kc85::switchon(kc_model m) {
    YAKC_ASSERT(kc_model::none != m);
    YAKC_ASSERT(!this->on);
    this->cur_model = m;
    this->on = true;

    if (kc_model::kc85_3 == m) {
        this->cpu.mem.map(0, this->ram0, sizeof(this->ram0), memory::type::ram);
        this->cpu.mem.map(2, this->irm0, sizeof(this->irm0), memory::type::ram);
        this->cpu.mem.map(3, this->rom0, sizeof(this->rom0), memory::type::rom);
    }
    this->cpu.reset();
}

//------------------------------------------------------------------------------
void
kc85::switchoff() {
    YAKC_ASSERT(this->on);
    this->on = false;
}

//------------------------------------------------------------------------------
bool
kc85::ison() const {
    return this->on;
}

//------------------------------------------------------------------------------
kc85::kc_model
kc85::model() const {
    return this->cur_model;
}

//------------------------------------------------------------------------------
void
kc85::reset() {
    this->cpu.reset();
}

//------------------------------------------------------------------------------
void
kc85::onframe(int micro_secs) {
    if (!this->is_paused) {
        // FIXME
    }
}

//------------------------------------------------------------------------------
void
kc85::pause(bool b) {
    this->is_paused = b;
}

//------------------------------------------------------------------------------
bool
kc85::paused() const {
    return this->is_paused;
}

//------------------------------------------------------------------------------
void
kc85::step() {
    this->cpu.step();
}

//------------------------------------------------------------------------------
bool
kc85::blink_state() const {
    // FIXME
    return true;
}

} // namespace yakc
