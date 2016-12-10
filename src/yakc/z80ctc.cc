//------------------------------------------------------------------------------
//  z80ctc.cc
//------------------------------------------------------------------------------
#include "z80ctc.h"
#include "z80bus.h"

namespace YAKC {

//------------------------------------------------------------------------------
void
z80ctc::init(z80bus* bus_) {
    YAKC_ASSERT(bus_);
    this->bus = bus_;
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
    if (downstream) {
        this->channels[3].int_ctrl.connect_irq_device(downstream);
    }
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
z80ctc::write(channel c, ubyte v) {
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
        this->bus->ctc_write(c);
    }
    else if ((v & CONTROL) == CONTROL_WORD) {
        // a control word
        chn.mode = v;
        if (!(chn.mode & CONSTANT_FOLLOWS)) {
            this->bus->ctc_write(c);
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
ubyte
z80ctc::read(channel c) {
    YAKC_ASSERT((c >= 0) && (c<num_channels));
    const channel_state& chn = channels[c];
    int val = chn.down_counter;
    if ((chn.mode & MODE) == MODE_TIMER) {
        val /= ((chn.mode & PRESCALER) == PRESCALER_256) ? 256 : 16;
    }
    return (ubyte) val;
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
z80ctc::update_timers(int ticks) {
    for (int c = 0; c < num_channels; c++) {
        channel_state& chn = channels[c];
        if (0 == (chn.mode & (RESET|CONSTANT_FOLLOWS))) {
            if (((chn.mode & MODE) == MODE_TIMER) && !chn.waiting_for_trigger) {
                chn.down_counter -= ticks;
                while (chn.down_counter <= 0) {
                    down_counter_callback(c);
                    chn.down_counter += down_counter_init(chn);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
void
z80ctc::update_counter(int chn_index) {
    channel_state& chn = this->channels[chn_index];
    if (0 == (chn.mode & (RESET|CONSTANT_FOLLOWS))) {
        if ((chn.mode & MODE) == MODE_COUNTER) {
            if (--chn.down_counter == 0) {
                down_counter_callback(chn_index);
                chn.down_counter = down_counter_init(chn);
            }
        }
        chn.waiting_for_trigger = false;
    }
}

//------------------------------------------------------------------------------
void
z80ctc::down_counter_callback(int chn_index) {
    channel_state& chn = this->channels[chn_index];
    if ((chn.mode & INTERRUPT) == INTERRUPT_ENABLED) {
        chn.int_ctrl.request_interrupt(chn.interrupt_vector);
    }
    this->bus->ctc_zcto(chn_index);
}

//------------------------------------------------------------------------------
void
z80ctc::ctrg(channel c) {
    this->update_counter(c);
}

//------------------------------------------------------------------------------
void
z80ctc::ctrg0(void* userdata) {
    z80ctc* self = (z80ctc*)userdata;
    self->update_counter(CTC0);
}

//------------------------------------------------------------------------------
void
z80ctc::ctrg1(void* userdata) {
    z80ctc* self = (z80ctc*)userdata;
    self->update_counter(CTC1);
}

//------------------------------------------------------------------------------
void
z80ctc::ctrg2(void* userdata) {
    z80ctc* self = (z80ctc*)userdata;
    self->update_counter(CTC2);
}

//------------------------------------------------------------------------------
void
z80ctc::ctrg3(void* userdata) {
    z80ctc* self = (z80ctc*)userdata;
    self->update_counter(CTC3);
}

} // namespace YAKC
