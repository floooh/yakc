//------------------------------------------------------------------------------
//  z80ctc.cc
//------------------------------------------------------------------------------
#include "z80ctc.h"
#include "yakc/core/system_bus.h"

namespace YAKC {

//------------------------------------------------------------------------------
void
z80ctc::init(int id_) {
    this->id = id_;
    for (auto& chn : channels) {
        chn = channel_state();
    }
}

//------------------------------------------------------------------------------
void
z80ctc::init_daisychain(z80int* downstream) {
    for (int i = 0; i < 3; i++) {
        this->channels[i].int_ctrl.connect_irq_device(&this->channels[i+1].int_ctrl);
    }
    this->channels[3].int_ctrl.connect_irq_device(downstream);
}

//------------------------------------------------------------------------------
void
z80ctc::reset() {
    // don't clear callbacks on reset
    for (auto& chn : channels) {
        chn.mode = RESET;
        chn.constant = 0;
        chn.down_counter = 0;
        chn.waiting_for_trigger = false;
    }
}

//------------------------------------------------------------------------------
void
z80ctc::write(system_bus* bus, channel c, uint8_t v) {
    YAKC_ASSERT((c >= 0) && (c<num_channels));

    channel_state& chn = channels[c];
    if (chn.mode & CONSTANT_FOLLOWS) {
        // time constant value following a control word
        chn.constant = v;
        chn.down_counter = down_counter_init(chn);
        chn.mode &= ~(CONSTANT_FOLLOWS|RESET);
        if ((chn.mode & MODE) == MODE_TIMER) {
            chn.waiting_for_trigger = (chn.mode & TRIGGER) == TRIGGER_PULSE;
        }
        if (bus) {
            bus->ctc_write(this->id, c);
        }
    }
    else if ((v & CONTROL) == CONTROL_WORD) {
        // a control word
        chn.mode = v;
        if (!(chn.mode & CONSTANT_FOLLOWS)) {
            if (bus) {
                bus->ctc_write(this->id, c);
            }
        }
    }
    else {
        // an interrupt vector for the whole CTC must be written
        // to channel 0, interrupt vectors for following channels
        // are computed from the base vector plus 2 byte per channel
        if (CTC0 == c) {
            for (int i = 0; i < num_channels; i++) {
                channels[i].interrupt_vector = (v & 0xF8) + 2*i;
            }
        }
    }
}

//------------------------------------------------------------------------------
uint8_t
z80ctc::read(channel c) {
    YAKC_ASSERT((c >= 0) && (c<num_channels));
    const channel_state& chn = channels[c];
    int val = chn.down_counter;
    if ((chn.mode & MODE) == MODE_TIMER) {
        val /= ((chn.mode & PRESCALER) == PRESCALER_256) ? 256 : 16;
    }
    return (uint8_t) val;
}

//------------------------------------------------------------------------------
int
z80ctc::down_counter_init(const channel_state& chn) const {
    int c = (0 == chn.constant) ? 0x100 : chn.constant;
    if ((chn.mode & MODE) == MODE_TIMER) {
        c *= ((chn.mode & PRESCALER) == PRESCALER_256) ? 256 : 16;
    }
    return c;
}

//------------------------------------------------------------------------------
void
z80ctc::step(system_bus* bus, int ticks) {
    for (int c = 0; c < num_channels; c++) {
        channel_state& chn = channels[c];
        if (0 == (chn.mode & (RESET|CONSTANT_FOLLOWS))) {
            if (((chn.mode & MODE) == MODE_TIMER) && !chn.waiting_for_trigger) {
                chn.down_counter -= ticks;
                while (chn.down_counter <= 0) {
                    down_counter_callback(bus, c);
                    chn.down_counter += down_counter_init(chn);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
void
z80ctc::update_counter(system_bus* bus, int chn_index) {
    channel_state& chn = this->channels[chn_index];
    if (0 == (chn.mode & (RESET|CONSTANT_FOLLOWS))) {
        if ((chn.mode & MODE) == MODE_COUNTER) {
            if (--chn.down_counter == 0) {
                down_counter_callback(bus, chn_index);
                chn.down_counter = down_counter_init(chn);
            }
        }
        chn.waiting_for_trigger = false;
    }
}

//------------------------------------------------------------------------------
void
z80ctc::down_counter_callback(system_bus* bus, int chn_index) {
    channel_state& chn = this->channels[chn_index];
    if ((chn.mode & INTERRUPT) == INTERRUPT_ENABLED) {
        chn.int_ctrl.request_interrupt(bus, chn.interrupt_vector);
    }
    if (bus) {
        bus->ctc_zcto(this->id, chn_index);
    }
}

//------------------------------------------------------------------------------
void
z80ctc::ctrg(system_bus* bus, channel c) {
    YAKC_ASSERT(bus);
    this->update_counter(bus, c);
}

} // namespace YAKC
