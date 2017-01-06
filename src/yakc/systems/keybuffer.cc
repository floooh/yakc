//------------------------------------------------------------------------------
//  keybuffer.cc
//------------------------------------------------------------------------------
#include "keybuffer.h"

namespace YAKC {

//------------------------------------------------------------------------------
keybuffer::keybuffer() :
advance_delay(1),
advance_count(0),
write_pos(0),
read_pos(0) {
    clear(this->buf, sizeof(this->buf));
}

//------------------------------------------------------------------------------
void
keybuffer::init(int delay) {
    this->advance_delay = delay;
    this->reset();
}

//------------------------------------------------------------------------------
void
keybuffer::reset() {
    advance_count = 0;
    write_pos = 0;
    read_pos = 0;
    clear(buf, sizeof(buf));
}

//------------------------------------------------------------------------------
void
keybuffer::advance() {
    advance_count++;
    if (advance_count > advance_delay) {
        advance_count = 0;
        if (read_pos != write_pos) {
            read_pos = (read_pos + 1) & wrap_mask;
        }
    }
}

//------------------------------------------------------------------------------
void
keybuffer::write(uint8_t key) {
    if (key != buf[write_pos]) {
        write_pos = (write_pos + 1) & wrap_mask;
        buf[write_pos] = key;
        if (read_pos == write_pos) {
            read_pos = (read_pos + 1) & wrap_mask;
        }
    }
}

//------------------------------------------------------------------------------
uint8_t
keybuffer::read() {
    return buf[read_pos];
}

} // namespace YAKC
