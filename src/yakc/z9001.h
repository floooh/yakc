#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::z9001
    @brief wrapper class for Z9001, KC85/1 and KC87
    
    KEYBOARD MATRIX:
        PIO2-A => kbd matrix columns
        PIO2-B => kbd matrix lines

        Writing to PIO2-B to trigger an interrupt and then returning the 
        ~column on PIO2-A and ~line on PIO2-B will start outputting that 
        character to the console.
        
        http://www.sax.de/~zander/z9001/z9sch_5.pdf
        http://www.sax.de/~zander/z9001/z9sch_1.pdf
*/
#include "yakc/breadboard.h"
#include "yakc/keybuffer.h"
#include "yakc/z80bus.h"
#include "yakc/rom_images.h"

namespace YAKC {

class z9001 : public z80bus {
public:
    /// the main board
    breadboard* board = nullptr;
    /// rom image store
    rom_images* roms = nullptr;

    /// one-time setup
    void init(breadboard* board, rom_images* roms);
    /// check if required roms are loaded
    static bool check_roms(const rom_images& roms, device model, os_rom os);
    /// setup audio callbacks
    void setup_sound_funcs(const sound_funcs& funcs);
    /// init the memory map
    void init_memory_mapping();

    /// power-on the device
    void poweron(device m, os_rom os);
    /// power-off the device
    void poweroff();
    /// reset the device
    void reset();
    /// get info about emulated system
    const char* system_info() const;
    /// called after snapshot restore
    void on_context_switched();
    /// get current border color
    void border_color(float& out_red, float& out_green, float& out_blue);

    /// process a number of cycles, return final processed tick
    uint64_t step(uint64_t start_tick, uint64_t end_tick);

    /// the z80 out callback
    virtual void cpu_out(uword port, ubyte val) override;
    /// the z80 in callback
    virtual ubyte cpu_in(uword port) override;
    /// CTC write callback (used for audio)
    virtual void ctc_write(int ctc_id, int chn_id) override;
    /// CTC ZCTO callback (used to trigger CTC channel 3)
    virtual void ctc_zcto(int ctc_id, int chn_id) override;
    /// PIO output callback
    virtual void pio_out(int pio_id, int port_id, ubyte val) override;
    /// PIO input callback
    virtual ubyte pio_in(int pio_id, int port_id) override;
    /// request a CPU interrupt
    virtual void irq() override;
    /// clock timer triggered
    virtual void timer(int timer_id) override;

    /// put a key as ASCII code
    void put_key(ubyte ascii);
    /// handle key input (called from onframe())
    void handle_key();

    /// decode an entire frame into RGBA8Buffer
    void decode_video();

    static const int video_ram_page = 4;
    static const int color_ram_page = 5;

    device cur_model = device::kc87;
    os_rom cur_os = os_rom::kc87_os_2;
    bool on = false;
    uint64_t cur_tick = 0;

    keybuffer keybuf;
    uint64_t key_mask = 0;              // (column<<8)|line bits for currently pressed key
    uint8_t kbd_column_mask = 0;        // PIO2-A keyboard matrix column mask
    uint8_t kbd_line_mask = 0;          // PIO2-B keyboard matrix line mask
    static const int max_num_keys = 128;
    uint64_t key_map[max_num_keys];     // complete keyboard matrix state for each ascii code

    bool blink_flipflop = false;
    uint8_t brd_color = 0;              // border color byte extracted from PIO1-A
    uint32_t blink_counter = 0;
    uint32_t rgba8_buffer[320*192];     // decoded linear RGBA8 video buffer

    sound_funcs sound_cb;               // external sound callbacks
    ubyte ctc0_mode = z80ctc::RESET;    // CTC0 state for audio output
    ubyte ctc0_constant = 0;
};

} // namespace YAKC
