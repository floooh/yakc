//------------------------------------------------------------------------------
//  cpc.cc
//
//  Schematics:
//  - 646: http://www.cpcwiki.eu/imgs/6/6d/Schaltplan_cpc_464.jpg
//  - 664: http://www.cpcwiki.eu/index.php/File:CPC664_Schematic.png
//  - 6128: http://www.cpcwiki.eu/index.php/File:CPC6128_Schematic.png
//------------------------------------------------------------------------------
#include "cpc.h"
#include "yakc/util/filetypes.h"

#include <stdio.h>

namespace YAKC {

cpc_t cpc;

//------------------------------------------------------------------------------
//
//  the fixed hardware color palette
//
//  http://www.cpcwiki.eu/index.php/CPC_Palette
//  http://www.grimware.org/doku.php/documentations/devices/gatearray
//
//  index into this palette is the 'hardware color number' & 0x1F
//  order is ABGR
//
static uint32_t cpc_colors[32] = {
    0xff6B7D6E,         // #40 white
    0xff6D7D6E,         // #41 white
    0xff6BF300,         // #42 sea green
    0xff6DF3F3,         // #43 pastel yellow
    0xff6B0200,         // #44 blue
    0xff6802F0,         // #45 purple
    0xff687800,         // #46 cyan
    0xff6B7DF3,         // #47 pink
    0xff6802F3,         // #48 purple
    0xff6BF3F3,         // #49 pastel yellow
    0xff0DF3F3,         // #4A bright yellow
    0xffF9F3FF,         // #4B bright white
    0xff0605F3,         // #4C bright red
    0xffF402F3,         // #4D bright magenta
    0xff0D7DF3,         // #4E orange
    0xffF980FA,         // #4F pastel magenta
    0xff680200,         // #50 blue
    0xff6BF302,         // #51 sea green
    0xff01F002,         // #52 bright green
    0xffF2F30F,         // #53 bright cyan
    0xff010200,         // #54 black
    0xffF4020C,         // #55 bright blue
    0xff017802,         // #56 green
    0xffF47B0C,         // #57 sky blue
    0xff680269,         // #58 magenta
    0xff6BF371,         // #59 pastel green
    0xff04F571,         // #5A lime
    0xffF4F371,         // #5B pastel cyan
    0xff01026C,         // #5C red
    0xffF2026C,         // #5D mauve
    0xff017B6E,         // #5E yellow
    0xffF67B6E,         // #5F pastel blue
};

// first 32 bytes of the KC Compact color ROM
static uint32_t kcc_color_rom[32] = {
    0x15, 0x15, 0x31, 0x3d, 0x01, 0x0d, 0x11, 0x1d,
    0x0d, 0x3d, 0x3c, 0x3f, 0x0c, 0x0f, 0x1c, 0x1f,
    0x01, 0x31, 0x30, 0x33, 0x00, 0x03, 0x10, 0x13,
    0x05, 0x35, 0x34, 0x37, 0x04, 0x07, 0x14, 0x17
};

//------------------------------------------------------------------------------
bool
cpc_t::check_roms(system model, os_rom os) {
    if (system::cpc464 == model) {
        return roms.has(rom_images::cpc464_os) && roms.has(rom_images::cpc464_basic);
    }
    else if (system::cpc6128 == model) {
        return roms.has(rom_images::cpc6128_os) && roms.has(rom_images::cpc6128_basic);
    }
    else if (system::kccompact == model) {
        return roms.has(rom_images::kcc_os) && roms.has(rom_images::kcc_basic);
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
void
cpc_t::init_keymap() {
    /*
        http://cpctech.cpc-live.com/docs/keyboard.html
    
        CPC has a 10 columns by 8 lines keyboard matrix. The 10 columns
        are lit up by bits 0..3 of PPI port C connected to a 74LS145
        BCD decoder, and the lines are read through port A of the
        AY-3-8912 chip.
    */
    kbd_init(&board.kbd, 1);
    const char* keymap =
        // no shift
        "   ^08641 "
        "  [-97532 "
        "   @oure  "
        "  ]piytwq "
        "   ;lhgs  "
        "   :kjfda "
        "  \\/mnbc  "
        "   ., vxz "

        // shift
        "    _(&$! "
        "  {=)'%#\" "
        "   |OURE  "
        "  }PIYTWQ "
        "   +LHGS  "
        "   *KJFDA "
        "  `?MNBC  "
        "   >< VXZ ";
    YAKC_ASSERT(strlen(keymap) == 160);
    // shift key is on column 2, line 5
    kbd_register_modifier(&board.kbd, 0, 2, 5);
    // ctrl key is on column 2, line 7
    kbd_register_modifier(&board.kbd, 1, 2, 7);

    for (int shift = 0; shift < 2; shift++) {
        for (int col = 0; col < 10; col++) {
            for (int line = 0; line < 8; line++) {
                int c = keymap[shift*80 + line*10 + col];
                if (c != 0x20) {
                    kbd_register_key(&board.kbd, c, col, line, shift?(1<<0):0);
                }
            }
        }
    }

    // special keys
    kbd_register_key(&board.kbd, 0x20, 5, 7, 0);    // space
    kbd_register_key(&board.kbd, 0x08, 1, 0, 0);    // cursor left
    kbd_register_key(&board.kbd, 0x09, 0, 1, 0);    // cursor right
    kbd_register_key(&board.kbd, 0x0A, 0, 2, 0);    // cursor down
    kbd_register_key(&board.kbd, 0x0B, 0, 0, 0);    // cursor up
    kbd_register_key(&board.kbd, 0x01, 9, 7, 0);    // delete
    kbd_register_key(&board.kbd, 0x0C, 2, 0, 0);    // clr
    kbd_register_key(&board.kbd, 0x0D, 2, 2, 0);    // return
    kbd_register_key(&board.kbd, 0x03, 8, 2, 0);    // escape
}

//------------------------------------------------------------------------------
void
cpc_t::poweron(system m) {
    YAKC_ASSERT(int(system::any_cpc) & int(m));
    YAKC_ASSERT(!this->on);

    this->cur_model = m;
    this->on = true;
    this->joymask = 0;

    this->init_keymap();
    this->ga_init();

    // setup memory system
    clear(board.ram, sizeof(board.ram));
    mem_unmap_all(&board.mem);
    this->update_memory_mapping(true, true);

    // http://www.cpcwiki.eu/index.php/Format:TAP_tape_image_file_format
    if (m == system::cpc464) {
        this->casread_trap = 0x2836;
        this->casread_ret = 0x2872;
    }
    else {
        this->casread_trap = 0x29A6;
        this->casread_ret = 0x29E2;
    }

    // initialize hardware components, main clock frequency is 4 MHz
    board.freq_hz = 4000000;
    z80_init(&board.z80, cpu_tick);
    ay38912_init(&board.ay38912, 1000000, SOUND_SAMPLE_RATE, 0.5f);
    i8255_init(&board.i8255, ppi_in, ppi_out);
    mc6845_init(&board.mc6845, MC6845_TYPE_UM6845R);
    crt_init(&board.crt, CRT_PAL, 6, 32, max_display_width/16, max_display_height);
    this->tick_count = 0;

    // CPU start address
    board.z80.PC = 0x0000;
}

//------------------------------------------------------------------------------
void
cpc_t::poweroff() {
    YAKC_ASSERT(this->on);
    mem_unmap_all(&board.mem);
    this->on = false;
}

//------------------------------------------------------------------------------
void
cpc_t::reset() {
    mc6845_reset(&board.mc6845);
    crt_reset(&board.crt);
    ay38912_reset(&board.ay38912);
    i8255_reset(&board.i8255);
    z80_reset(&board.z80);
    board.z80.PC = 0x0000;
    this->joymask = 0;
    this->tick_count = 0;
    this->ga_init();
    mem_unmap_all(&board.mem);
    this->update_memory_mapping(true, true);
}

//------------------------------------------------------------------------------
uint64_t
cpc_t::exec(uint64_t start_tick, uint64_t end_tick) {
    YAKC_ASSERT(start_tick <= end_tick);
    uint32_t num_ticks = end_tick - start_tick;
    uint32_t ticks_executed = z80_exec(&board.z80, num_ticks);
    kbd_update(&board.kbd);
    return start_tick + ticks_executed;
}

//------------------------------------------------------------------------------
uint64_t
cpc_t::cpu_tick(int num_ticks, uint64_t pins) {
    // interrupt acknowledge?
    if ((pins & (Z80_M1|Z80_IORQ)) == (Z80_M1|Z80_IORQ)) {
        cpc.ga_int_ack();
    }

    // memory and IO requests
    if (pins & Z80_MREQ) {
        // CPU MEMORY REQUEST
        const uint16_t addr = Z80_GET_ADDR(pins);
        if (pins & Z80_RD) {
            Z80_SET_DATA(pins, mem_rd(&board.mem, addr));
        }
        else if (pins & Z80_WR) {
            mem_wr(&board.mem, addr, Z80_GET_DATA(pins));
        }
    }
    else if ((pins & Z80_IORQ) && (pins & (Z80_RD|Z80_WR))) {
        // CPU IO REQUEST
        pins = cpc.cpu_iorq(pins);
    }
    
    /*
        tick the gate array and audio chip at 1 MHz, and decide how many wait
        states must be injected, the CPC signals the wait line in 3 out of 4
        cycles:
    
         0: wait inactive
         1: wait active
         2: wait active
         3: wait active
    
        the CPU samples the wait line only on specific clock ticks
        during memory or IO operations, wait states are only injected
        if the 'wait active' happens on the same clock tick as the
        CPU would sample the wait line
    */
    int wait_scan_tick = -1;
    if (pins & Z80_MREQ) {
        // a memory request or opcode fetch, wait is sampled on second clock tick
        wait_scan_tick = 1;
    }
    else if (pins & Z80_IORQ) {
        if (pins & Z80_M1) {
            // an interrupt acknowledge cycle, wait is sampled on fourth clock tick
            wait_scan_tick = 3;
        }
        else {
            // an IO cycle, wait is sampled on third clock tick
            wait_scan_tick = 2;
        }
    }
    bool wait = false;
    uint32_t wait_cycles = 0;
    for (int i = 0; i<num_ticks; i++) {
        do {
            // CPC gate array sets the wait pin for 3 out of 4 clock ticks
            bool wait_pin = (cpc.tick_count++ & 3) != 0;
            wait = (wait_pin && (wait || (i == wait_scan_tick)));
            if (wait) {
                wait_cycles++;
            }
            // on every 4th clock cycle, tick the system
            if (!wait_pin) {
                if (ay38912_tick(&board.ay38912)) {
                    board.audiobuffer.write(board.ay38912.sample);
                }
                pins = cpc.ga_tick(pins);
            }
        }
        while (wait);
    }
    Z80_SET_WAIT(pins, wait_cycles);

    return pins;
}

//------------------------------------------------------------------------------
uint64_t
cpc_t::cpu_iorq(uint64_t pins) {
    /*
        CPU IO REQUEST

        For address decoding, see the main board schematics!
        also: http://cpcwiki.eu/index.php/Default_I/O_Port_Summary
    */
    /*
        Z80 to i8255 PPI pin connections:
            ~A11 -> CS (CS is active-low)
                A8 -> A0
                A9 -> A1
                RD -> RD
                WR -> WR
            D0..D7 -> D0..D7
    */
    if ((pins & Z80_A11) == 0) {
        // i8255 in/out
        uint64_t ppi_pins = (pins & Z80_PIN_MASK)|I8255_CS;
        if (pins & Z80_A9) { ppi_pins |= I8255_A1; }
        if (pins & Z80_A8) { ppi_pins |= I8255_A0; }
        if (pins & Z80_RD) { ppi_pins |= I8255_RD; }
        if (pins & Z80_WR) { ppi_pins |= I8255_WR; }
        pins = i8255_iorq(&board.i8255, ppi_pins) & Z80_PIN_MASK;
    }
    /*
        Z80 to MC6845 pin connections:

            ~A14 -> CS (CS is active low)
            A9  -> RW (high: read, low: write)
            A8  -> RS
        D0..D7  -> D0..D7
    */
    if ((pins & Z80_A14) == 0) {
        // 6845 in/out
        uint64_t vdu_pins = (pins & Z80_PIN_MASK)|MC6845_CS;
        if (pins & Z80_A9) { vdu_pins |= MC6845_RW; }
        if (pins & Z80_A8) { vdu_pins |= MC6845_RS; }
        pins = mc6845_iorq(&board.mc6845, vdu_pins) & Z80_PIN_MASK;
    }
    /*
        Gate Array Function (only writing to the gate array
        is possible, but the gate array doesn't check the
        CPU R/W pins, so each access is a write).

        This is used by the Arnold Acid test "OnlyInc", which
        access the PPI and gate array in the same IO operation
        to move data directly from the PPI into the gate array.
    */
    if ((pins & (Z80_A15|Z80_A14)) == Z80_A14) {
        // D6 and D7 select the gate array operation
        const uint8_t data = Z80_GET_DATA(pins);
        switch (data & ((1<<7)|(1<<6))) {
            case 0:
                // select pen:
                //  bit 4 set means 'select border', otherwise
                //  bits 0..3 contain the pen number
                cpc.ga_pen = data & 0x1F;
                break;
            case (1<<6):
                // select color for border or selected pen:
                if (cpc.ga_pen & (1<<4)) {
                    // border color
                    cpc.ga_border_color = cpc.ga_colors[data & 0x1F];
                }
                else {
                    cpc.ga_palette[cpc.ga_pen & 0x0F] = cpc.ga_colors[data & 0x1F];
                }
                break;
            case (1<<7):
                // select screen mode, ROM config and interrupt control
                //  - bits 0 and 1 select the screen mode
                //      00: Mode 0 (160x200 @ 16 colors)
                //      01: Mode 1 (320x200 @ 4 colors)
                //      02: Mode 2 (640x200 @ 2 colors)
                //      11: Mode 3 (160x200 @ 2 colors, undocumented)
                //
                //  - bit 2: disable/enable lower ROM
                //  - bit 3: disable/enable upper ROM
                //
                //  - bit 4: interrupt generation control
                //
                cpc.ga_config = data;
                cpc.ga_next_video_mode = data & 3;
                if (data & (1<<4)) {
                    cpc.ga_int_ctrl();
                }
                // only update ROM banks
                cpc.update_memory_mapping(true, false);
                break;
            case (1<<7)|(1<<6):
                // RAM memory management (only CPC6128)
                if (system::cpc6128 == cpc.cur_model) {
                    cpc.ga_ram_config = data;
                    // only update RAM banks
                    cpc.update_memory_mapping(false, true);
                }
                break;
        }
    }
    return pins;
}

//------------------------------------------------------------------------------
uint64_t
cpc_t::ppi_out(int port_id, uint64_t pins, uint8_t data) {
    /*
        i8255 PPI to AY-3-8912 PSG pin connections:
            PA0..PA7    -> D0..D7
                 PC7    -> BDIR
                 PC6    -> BC1
    */
    if (I8255_PORT_C == port_id) {
        // AY-3-8912 PSG function?
        if (data & ((1<<7)|(1<<6))) {
            uint64_t ay_pins = 0;
            if (data & (1<<7)) ay_pins |= AY38912_BDIR;
            if (data & (1<<6)) ay_pins |= AY38912_BC1;
            uint8_t ay_data = board.i8255.output[I8255_PORT_A];
            AY38912_SET_DATA(ay_pins, ay_data);
            ay38912_iorq(&board.ay38912, ay_pins);
        }

        // bits 0..3: select keyboard matrix line
        kbd_set_active_columns(&board.kbd, 1<<(data & 0x0F));

        // FIXME: cassette write data
        // cassette deck motor control
        /*
        if (data & (1<<4)) {
            if (!this->tape->is_playing()) {
                this->tape->play();
            }
        }
        else {
            if (this->tape->is_playing()) {
                this->tape->stop();
            }
        }
        */
    }
    else {
        //printf("cpc_t::ppi_out: write to port %d: 0x%02X!\n", port_id, data);
    }
    return pins;
}

//------------------------------------------------------------------------------
uint8_t
cpc_t::ppi_in(int port_id) {
    if (I8255_PORT_A == port_id) {
        // catch keyboard/joystick data which is normally in PSG PORT A
        if (board.ay38912.addr == AY38912_REG_IO_PORT_A) {
            if (board.kbd.active_columns & (1<<9)) {
                /*
                    joystick input is implemented like this:
                    - the keyboard column 9 is routed to the joystick
                      ports "COM1" pin, this means the joystick is only
                      "powered" when the keyboard line 9 is active
                    - the joysticks direction and fire pins are
                      connected to the keyboard matrix lines as
                      input to PSG port A
                    - thus, only when the keyboard matrix column 9 is sampled,
                      joystick input will be provided on the keyboard
                      matrix lines
                */
                return ~cpc.joymask;
            }
            else {
                return ~kbd_scan_lines(&board.kbd);
            }
        }
        else {
            // AY-3-8912 PSG function
            uint64_t ay_pins = 0;
            uint8_t ay_ctrl = board.i8255.output[I8255_PORT_C];
            if (ay_ctrl & (1<<7)) ay_pins |= AY38912_BDIR;
            if (ay_ctrl & (1<<6)) ay_pins |= AY38912_BC1;
            uint8_t ay_data = board.i8255.output[I8255_PORT_A];
            AY38912_SET_DATA(ay_pins, ay_data);
            ay_pins = ay38912_iorq(&board.ay38912, ay_pins);
            return AY38912_GET_DATA(ay_pins);
        }
    }
    else if (I8255_PORT_B == port_id) {
        //  Bit 7: cassette data input
        //  Bit 6: printer port ready (1=not ready, 0=ready)
        //  Bit 5: expansion port /EXP pin
        //  Bit 4: screen refresh rate (1=50Hz, 0=60Hz)
        //  Bit 3..1: distributor id (shown in start screen)
        //      0: Isp
        //      1: Triumph
        //      2: Saisho
        //      3: Solavox
        //      4: Awa
        //      5: Schneider
        //      6: Orion
        //      7: Amstrad
        //  Bit 0: vsync
        //
        uint8_t val = (1<<4) | (7<<1);    // 50Hz refresh rate, Amstrad
        // PPI Port B Bit 0 is directly wired to the 6845 VSYNC pin (see schematics)
        if (board.mc6845.vs) {
            val |= (1<<0);
        }
        return val;
    }
    else {
        // shouldn't happen
        printf("cpc_t::ppi_in: read from port %d\n", port_id);
        return 0xFF;
    }
}

//------------------------------------------------------------------------------
void
cpc_t::ga_init() {
    this->ga_config = 0;
    this->ga_next_video_mode = 1;
    this->ga_video_mode = 1;
    this->ga_ram_config = 0;
    this->ga_pen = 0;
    this->ga_border_color = 0;
    this->ga_hsync_irq_counter = 0;
    this->ga_hsync_after_vsync_counter = 0;
    this->ga_hsync_delay_counter = 2;
    this->ga_sync = false;
    this->ga_int = false;
    this->ga_crtc_pins = 0;
    clear(this->ga_palette, sizeof(this->ga_palette));
    
    // setup the color palette, this is different for CPC and KC Compact
    if (system::kccompact != this->cur_model) {
        // CPC models
        for (int i = 0; i < 32; i++) {
            this->ga_colors[i] = cpc_colors[i];
        }
    }
    else {
        // KC Compact
        for (int i = 0; i < 32; i++) {
            uint32_t rgba8 = 0xFF000000;
            const uint8_t val = kcc_color_rom[i];
            // color bits:
            //  xx|gg|rr|bb
            //
            const uint8_t b = val & 0x03;
            const uint8_t r = (val>>2) & 0x03;
            const uint8_t g = (val>>4) & 0x03;
            if (b == 0x03)     rgba8 |= 0x00FF0000;    // full blue
            else if (b != 0)   rgba8 |= 0x007F0000;    // half blue
            if (g == 0x03)     rgba8 |= 0x0000FF00;    // full green
            else if (g != 0)   rgba8 |= 0x00007F00;    // half green
            if (r == 0x03)     rgba8 |= 0x000000FF;    // full red
            else if (r != 0)   rgba8 |= 0x0000007F;    // half red
            this->ga_colors[i] = rgba8;
        }
    }
}

//------------------------------------------------------------------------------
void
cpc_t::ga_int_ctrl() {
    this->ga_hsync_irq_counter = 0;
    this->ga_int = false;
}

//------------------------------------------------------------------------------
void
cpc_t::ga_int_ack() {
    // on interrupt acknowledge from the CPU, clear the top bit from the
    // hsync counter, so the next interrupt can't occur closer then 
    // 32 HSYNC, and clear the gate array interrupt pin state
    this->ga_hsync_irq_counter &= 0x1F;
    this->ga_int = false;
}

//------------------------------------------------------------------------------
static bool falling_edge(uint64_t new_pins, uint64_t old_pins, uint64_t mask) {
    return 0 != (mask & (~new_pins & (new_pins ^ old_pins)));
}

//------------------------------------------------------------------------------
static bool rising_edge(uint64_t new_pins, uint64_t old_pins, uint64_t mask) {
    return 0 != (mask & (new_pins & (new_pins ^ old_pins)));
}

//------------------------------------------------------------------------------
uint64_t
cpc_t::ga_tick(uint64_t cpu_pins) {
    // http://cpctech.cpc-live.com/docs/ints.html
    // http://www.cpcwiki.eu/forum/programming/frame-flyback-and-interrupts/msg25106/#msg25106
    // https://web.archive.org/web/20170612081209/http://www.grimware.org/doku.php/documentations/devices/gatearray

    uint64_t crtc_pins = mc6845_tick(&board.mc6845);

    /*
        INTERRUPT GENERATION:

        From: https://web.archive.org/web/20170612081209/http://www.grimware.org/doku.php/documentations/devices/gatearray

        - On every falling edge of the HSYNC signal (from the 6845),
          the gate array will increment the counter by one. When the
          counter reaches 52, the gate array will raise the INT signal
          and reset the counter.
        - When the CPU acknowledges the interrupt, the gate array will
          reset bit5 of the counter, so the next interrupt can't occur
          closer than 32 HSync.
        - When a VSync occurs, the gate array will wait for 2 HSync and:
            - if the counter>=32 (bit5=1) then no interrupt request is issued
              and counter is reset to 0
            - if the counter<32 (bit5=0) then an interrupt request is issued
              and counter is reset to 0
        - This 2 HSync delay after a VSync is used to let the main program,
          executed by the CPU, enough time to sense the VSync...

        From: http://www.cpcwiki.eu/index.php?title=CRTC
        
        - The HSYNC is modified before being sent to the monitor. It happens
          2us after the HSYNC from the CRTC and lasts 4us when HSYNC length
          is greater or equal to 6.
        - The VSYNC is also modified before being sent to the monitor, it happens
          two lines after the VSYNC from the CRTC and stay 2 lines (same cut
          rule if VSYNC is lower than 4).

        NOTES:
            - the interrupt acknowledge is handled once per machine 
              cycle in cpu_tick()
            - the video mode will take effect *after the next HSYNC*
    */
    if (rising_edge(crtc_pins, this->ga_crtc_pins, MC6845_VS)) {
        this->ga_hsync_after_vsync_counter = 2;
    }
    if (falling_edge(crtc_pins, this->ga_crtc_pins, MC6845_HS)) {
        this->ga_video_mode = this->ga_next_video_mode;
        this->ga_hsync_irq_counter = (this->ga_hsync_irq_counter + 1) & 0x3F;

        // 2 HSync delay?
        if (this->ga_hsync_after_vsync_counter > 0) {
            this->ga_hsync_after_vsync_counter--;
            if (this->ga_hsync_after_vsync_counter == 0) {
                if (this->ga_hsync_irq_counter >= 32) {
                    this->ga_int = true;
                }
                this->ga_hsync_irq_counter = 0;
            }
        }
        // normal behaviour, request interrupt each 52 scanlines
        if (this->ga_hsync_irq_counter == 52) {
            this->ga_hsync_irq_counter = 0;
            this->ga_int = true;
        }
    }

    // generate HSYNC signal to monitor:
    //  - starts 2 ticks after HSYNC rising edge from CRTC
    //  - stays active for 4 ticks or less if CRTC HSYNC goes inactive earlier
    if (rising_edge(crtc_pins, this->ga_crtc_pins, MC6845_HS)) {
        this->ga_hsync_delay_counter = 3;
    }
    if (falling_edge(crtc_pins, this->ga_crtc_pins, MC6845_HS)) {
        this->ga_hsync_delay_counter = 0;
        this->ga_hsync_counter = 0;
        this->ga_sync = false;
    }
    if (this->ga_hsync_delay_counter > 0) {
        this->ga_hsync_delay_counter--;
        if (this->ga_hsync_delay_counter == 0) {
            this->ga_sync = true;
            this->ga_hsync_counter = 5;
        }
    }
    if (this->ga_hsync_counter > 0) {
        this->ga_hsync_counter--;
        if (this->ga_hsync_counter == 0) {
            this->ga_sync = false;
        }
    }

    // FIXME delayed VSYNC to monitor

    const bool vsync = (crtc_pins & MC6845_VS);
    crt_tick(&board.crt, this->ga_sync, vsync);
    this->ga_decode_video(crtc_pins);

    this->ga_crtc_pins = crtc_pins;

    if (this->ga_int) {
        cpu_pins |= Z80_INT;
    }
    return cpu_pins;
}

//------------------------------------------------------------------------------
void
cpc_t::ga_decode_pixels(uint32_t* dst, uint64_t crtc_pins) {

    // compute the source address from current CRTC ma (memory address)
    // and ra (raster address) like this:
    //
    // |ma12|ma11|ra2|ra1|ra0|ma9|ma8|...|ma2|ma1|ma0|0|
    //
    // Bits ma12 and m11 point to the 16 KByte page, and all
    // other bits are the index into that page.
    //
    const uint16_t ma = MC6845_GET_ADDR(crtc_pins);
    const uint8_t ra = MC6845_GET_RA(crtc_pins);
    const uint32_t page_index  = (ma>>12) & 3;
    const uint32_t page_offset = ((ma & 0x03FF)<<1) | ((ra & 7)<<11);
    const uint8_t* src = &(board.ram[page_index][page_offset]);
    uint8_t c;
    uint32_t p;
    if (0 == this->ga_video_mode) {
        // 160x200 @ 16 colors
        // pixel    bit mask
        // 0:       |3|7|
        // 1:       |2|6|
        // 2:       |1|5|
        // 3:       |0|4|
        for (int i = 0; i < 2; i++) {
            c = *src++;
            p = this->ga_palette[((c>>7)&0x1)|((c>>2)&0x2)|((c>>3)&0x4)|((c<<2)&0x8)];
            *dst++ = p; *dst++ = p; *dst++ = p; *dst++ = p;
            p = this->ga_palette[((c>>6)&0x1)|((c>>1)&0x2)|((c>>2)&0x4)|((c<<3)&0x8)];
            *dst++ = p; *dst++ = p; *dst++ = p; *dst++ = p;
        }
    }
    else if (1 == this->ga_video_mode) {
        // 320x200 @ 4 colors
        // pixel    bit mask
        // 0:       |3|7|
        // 1:       |2|6|
        // 2:       |1|5|
        // 3:       |0|4|
        for (int i = 0; i < 2; i++) {
            c = *src++;
            p = this->ga_palette[((c>>2)&2)|((c>>7)&1)];
            *dst++ = p; *dst++ = p;
            p = this->ga_palette[((c>>1)&2)|((c>>6)&1)];
            *dst++ = p; *dst++ = p;
            p = this->ga_palette[((c>>0)&2)|((c>>5)&1)];
            *dst++ = p; *dst++ = p;
            p = this->ga_palette[((c<<1)&2)|((c>>4)&1)];
            *dst++ = p; *dst++ = p;
        }
    }
    else if (2 == this->ga_video_mode) {
        // 640x200 @ 2 colors
        for (int i = 0; i < 2; i++) {
            c = *src++;
            for (int j = 7; j >= 0; j--) {
                *dst++ = this->ga_palette[(c>>j)&1];
            }
        }
    }
}

//------------------------------------------------------------------------------
void
cpc_t::ga_decode_video(uint64_t crtc_pins) {
    if (!this->debug_video) {
        if (board.crt.visible) {
            int dst_x = board.crt.pos_x * 16;
            int dst_y = board.crt.pos_y;
            YAKC_ASSERT((dst_x <= (max_display_width-16)) && (dst_y < max_display_height));
            uint32_t* dst = &(board.rgba8_buffer[dst_x + dst_y * max_display_width]);
            if (crtc_pins & MC6845_DE) {
                // decode visible pixels
                this->ga_decode_pixels(dst, crtc_pins);
            }
            else if (crtc_pins & (MC6845_HS|MC6845_VS)) {
                // blacker than black
                for (int i = 0; i < 16; i++) {
                    dst[i] = 0xFF000000;
                }
            }
            else {
                // border color
                for (int i = 0; i < 16; i++) {
                    dst[i] = this->ga_border_color;
                }
            }
        }
    }
    else {
        // debug mode
        int dst_x = board.crt.h_pos * 16;
        int dst_y = board.crt.v_pos;
        if ((dst_x <= (dbg_max_display_width-16)) && (dst_y < dbg_max_display_height)) {
            uint32_t* dst = &(board.rgba8_buffer[dst_x + dst_y * dbg_max_display_width]);
            if (!(crtc_pins & MC6845_DE)) {
                uint8_t r = 0x3F;
                uint8_t g = 0x3F;
                uint8_t b = 0x3F;
                if (crtc_pins & MC6845_HS) {
                    r = 0x7F; g = 0; b = 0;
                }
                if (this->ga_sync) {
                    r = 0xFF; g = 0; b = 0;
                }
                if (crtc_pins & MC6845_VS) {
                    g = 0x7F;
                }
                if (this->ga_int) {
                    b = 0xFF;
                }
                else if (0 == board.mc6845.scanline_ctr) {
                    r = g = b = 0x00;
                }
                for (int i = 0; i < 16; i++) {
                    if (i == 0) {
                        *dst++ = 0xFF000000;
                    }
                    else {
                        *dst++ = 0xFF<<24 | b<<16 | g<<8 | r;
                    }
                }
            }
            else {
                this->ga_decode_pixels(dst, crtc_pins);
            }
        }
    }
}

//------------------------------------------------------------------------------
// CPC6128 RAM block indices 
static int ram_config_table[8][4] = {
    { 0, 1, 2, 3 },
    { 0, 1, 2, 7 },
    { 4, 5, 6, 7 },
    { 0, 3, 2, 7 },
    { 0, 4, 2, 3 },
    { 0, 5, 2, 3 },
    { 0, 6, 2, 3 },
    { 0, 7, 2, 3 },
};

void
cpc_t::update_memory_mapping(bool upd_rom, bool upd_ram) {
    // index into RAM config array
    int ram_table_index;
    uint8_t* rom0_ptr,*rom1_ptr;
    if (system::kccompact == this->cur_model) {
        ram_table_index = 0;
        rom0_ptr = roms.ptr(rom_images::kcc_os);
        rom1_ptr = roms.ptr(rom_images::kcc_basic);
    }
    else if (system::cpc6128 == this->cur_model) {
        ram_table_index = this->ga_ram_config & 0x07;
        rom0_ptr = roms.ptr(rom_images::cpc6128_os);
        rom1_ptr = roms.ptr(rom_images::cpc6128_basic);
    }
    else {
        ram_table_index = 0;
        rom0_ptr = roms.ptr(rom_images::cpc464_os);
        rom1_ptr = roms.ptr(rom_images::cpc464_basic);
    }
    const int i0 = ram_config_table[ram_table_index][0];
    const int i1 = ram_config_table[ram_table_index][1];
    const int i2 = ram_config_table[ram_table_index][2];
    const int i3 = ram_config_table[ram_table_index][3];
    if (upd_ram) {
        // 0x4000..0x7FFF
        mem_map_ram(&board.mem, 0, 0x4000, 0x4000, board.ram[i1]);
        // 0x8000..0xBFFF
        mem_map_ram(&board.mem, 0, 0x8000, 0x4000, board.ram[i2]);
    }
    if (upd_rom) {
        // 0x0000..0x3FFF
        if (this->ga_config & (1<<2)) {
            // read/write from and to RAM bank
            mem_map_ram(&board.mem, 0, 0x0000, 0x4000, board.ram[i0]);
        }
        else {
            // read from ROM, write to RAM
            mem_map_rw(&board.mem, 0, 0x0000, 0x4000, rom0_ptr, board.ram[i0]);
        }
        // 0xC000..0xFFFF
        if (this->ga_config & (1<<3)) {
            // read/write from and to RAM bank
            mem_map_ram(&board.mem, 0, 0xC000, 0x4000, board.ram[i3]);
        }
        else {
            // read from ROM, write to RAM
            mem_map_rw(&board.mem, 0, 0xC000, 0x4000, rom1_ptr, board.ram[i3]);
        }
    }
}

//------------------------------------------------------------------------------
void
cpc_t::put_input(uint8_t ascii, uint8_t joy0_mask) {
    if (ascii && !joy0_mask) {
        kbd_key_down(&board.kbd, ascii);
        kbd_key_up(&board.kbd, ascii);
    }
    // setup the joystick bits which are overlayed
    // to the keyboard line input to the AY-3-8912 port
    this->joymask = 0;
    if (joy0_mask & joystick::up) {
        this->joymask |= (1<<0);
    }
    if (joy0_mask & joystick::down) {
        this->joymask |= (1<<1);
    }
    if (joy0_mask & joystick::left) {
        this->joymask |= (1<<2);
    }
    if (joy0_mask & joystick::right) {
        this->joymask |= (1<<3);
    }
    if (joy0_mask & joystick::btn0) {
        this->joymask |= (1<<4);
    }
    if (joy0_mask & joystick::btn1) {
        this->joymask |= (1<<5);
    }
}

//------------------------------------------------------------------------------
void
cpc_t::decode_audio(float* buffer, int num_samples) {
    board.audiobuffer.read(buffer, num_samples);
}

//------------------------------------------------------------------------------
const void*
cpc_t::framebuffer(int& out_width, int& out_height) {
    if (this->debug_video) {
        out_width = dbg_max_display_width;
        out_height = dbg_max_display_height;
    }
    else {
        out_width = max_display_width;
        out_height = max_display_height;
    }
    return board.rgba8_buffer;
}

//------------------------------------------------------------------------------
bool
cpc_t::load_sna(filesystem* fs, const char* name, filetype type, bool start) {
    auto fp = fs->open(name, filesystem::mode::read);
    if (!fp) {
        return false;
    }
    sna_header hdr;
    bool hdr_valid = false;
    if (fs->read(fp, &hdr, sizeof(hdr)) == sizeof(hdr)) {
        hdr_valid = true;
        const uint16_t dump_size = (hdr.dump_size_h<<8 | hdr.dump_size_l) & 0xFFFF;
        if (dump_size == 64) {
            YAKC_ASSERT(sizeof(board.ram) >= 0x10000);
            fs->read(fp, board.ram, 0x10000);
        }
        else {
            YAKC_ASSERT(sizeof(board.ram) >= 0x20000);
            fs->read(fp, board.ram, 0x20000);
        }
    }
    // CPU state
    auto& cpu = board.z80;
    cpu.F = hdr.F; cpu.A = hdr.A;
    cpu.C = hdr.C; cpu.B = hdr.B;
    cpu.E = hdr.E; cpu.D = hdr.D;
    cpu.L = hdr.L; cpu.H = hdr.H;
    cpu.R = hdr.R; cpu.I = hdr.I;
    cpu.IFF1 = (hdr.IFF1 & 1) != 0;
    cpu.IFF2 = (hdr.IFF2 & 1) != 0;
    cpu.IX = (hdr.IX_h<<8 | hdr.IX_l) & 0xFFFF;
    cpu.IY = (hdr.IY_h<<8 | hdr.IY_l) & 0xFFFF;
    cpu.SP = (hdr.SP_h<<8 | hdr.SP_l) & 0xFFFF;
    cpu.PC = (hdr.PC_h<<8 | hdr.PC_l) & 0xFFFF;
    cpu.IM = hdr.IM;
    cpu.AF_ = (hdr.A_<<8 | hdr.F_) & 0xFFFF;
    cpu.BC_ = (hdr.B_<<8 | hdr.C_) & 0xFFFF;
    cpu.DE_ = (hdr.D_<<8 | hdr.E_) & 0xFFFF;
    cpu.HL_ = (hdr.H_<<8 | hdr.L_) & 0xFFFF;
    // gate array state
    for (int i = 0; i < 16; i++) {
        this->ga_palette[i] = this->ga_colors[hdr.pens[i] & 0x1F];
    }
    this->ga_border_color = this->ga_colors[hdr.pens[16] & 0x1F];
    this->ga_pen = hdr.selected_pen & 0x1F;
    this->ga_config = hdr.gate_array_config & 0x3F;
    this->ga_next_video_mode = hdr.gate_array_config & 3;
    this->ga_ram_config = hdr.ram_config & 0x3F;
    this->update_memory_mapping(true, true);
    auto& vdg = board.mc6845;
    for (int i = 0; i < 18; i++) {
        vdg.reg[i] = hdr.crtc_regs[i];
    }
    vdg.sel = hdr.crtc_selected;
    // FIXME: rom_config
    auto& ppi = board.i8255;
    ppi.output[I8255_PORT_A] = hdr.ppi_a;
    ppi.output[I8255_PORT_B] = hdr.ppi_b;
    ppi.output[I8255_PORT_C] = hdr.ppi_c;
    for (int i = 0; i < 16; i++) {
        // latch AY-3-8912 register address
        ay38912_iorq(&board.ay38912, AY38912_BDIR|AY38912_BC1|(i<<16));
        // write AY-3-8912 register value
        ay38912_iorq(&board.ay38912, AY38912_BDIR|(hdr.psg_regs[i]<<16));
    }
    // latch AY-3-8912 selected address
    ay38912_iorq(&board.ay38912, AY38912_BDIR|AY38912_BC1|(hdr.psg_selected<<16));
    fs->close(fp);
    fs->rm(name);
    if (!hdr_valid) {
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------
bool
cpc_t::load_bin(filesystem* fs, const char* name, filetype type, bool start) {
    auto fp = fs->open(name, filesystem::mode::read);
    if (!fp) {
        return false;
    }
    cpcbin_header hdr;
    bool hdr_valid = false;
    if (fs->read(fp, &hdr, sizeof(hdr)) == sizeof(hdr)) {
        hdr_valid = true;
    }
    if (hdr_valid) {
        const uint16_t load_addr = (hdr.load_addr_h<<8)|hdr.load_addr_l;
        const uint16_t start_addr = (hdr.start_addr_h<<8)|hdr.start_addr_l;
        const uint16_t len = (hdr.length_h<<8)|hdr.length_l;
        uint8_t val;
        for (uint16_t i=0; i < len; i++) {
            fs->read(fp, &val, sizeof(val));
            mem_wr(&board.mem, load_addr+i, val);
        }
        auto& cpu = board.z80;
        cpu.IFF1 = cpu.IFF2 = true;
        cpu.C = 0;  // hmm "ROM select number"
        cpu.HL = start_addr;
        cpu.PC = 0xBD16;        // MC START PROGRAM
    }
    fs->close(fp);
    fs->rm(name);
    return true;
}

//------------------------------------------------------------------------------
bool
cpc_t::quickload(filesystem* fs, const char* name, filetype type, bool start) {
    if (filetype::cpc_sna == type) {
        return this->load_sna(fs, name, type, start);
    }
    else if (filetype::cpc_bin == type) {
        return this->load_bin(fs, name, type, start);
    }
    return false;
}

//------------------------------------------------------------------------------
/*
void
cpc::casread() {
    auto& cpu = this->board->z80;
    bool success = false;
    // read the next block
    uint16_t len = 0;
    this->tape->read(&len, sizeof(len));
    uint8_t sync = 0;
    this->tape->read(&sync, sizeof(sync));
    if (sync == cpu.A) {
        success = true;
        this->tape->inc_counter(1);
        for (uint16_t i = 0; i < (len-1); i++) {
            uint8_t val;
            this->tape->read(&val, sizeof(val));
            cpu.mem.w8(cpu.HL++, val);
        }
    }
    cpu.F = success ? 0x45 : 0x00;
    cpu.PC = this->casread_ret;
}
*/

//------------------------------------------------------------------------------
const char*
cpc_t::system_info() const {
    return "FIXME!";
}

} // namespace YAKC
