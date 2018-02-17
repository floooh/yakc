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

    // joystick (just use some unused upper ascii codes)
    kbd_register_key(&board.kbd, 0xF0, 9, 2, 0);    // joystick left
    kbd_register_key(&board.kbd, 0xF1, 9, 3, 0);    // joystick right
    kbd_register_key(&board.kbd, 0xF2, 9, 0, 0);    // joystick down
    kbd_register_key(&board.kbd, 0xF3, 9, 1, 0);    // joystick up
    kbd_register_key(&board.kbd, 0xF4, 9, 5, 0);    // joystick fire0
    kbd_register_key(&board.kbd, 0xF5, 9, 4, 0);    // joystick fire1
}

//------------------------------------------------------------------------------
void
cpc_t::poweron(system m) {
    YAKC_ASSERT(int(system::any_cpc) & int(m));
    YAKC_ASSERT(!this->on);

    this->cur_model = m;
    this->on = true;

    this->init_keymap();

    // setup memory system
    clear(board.ram, sizeof(board.ram));
    mem_unmap_all(&board.mem);
    this->ga_config = 0x00;     // enable both ROMs
    this->ram_config = 0x00;    // standard RAM bank config (0,1,2,3)
    this->update_memory_mapping();

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
    crt_init(&board.crt, CRT_PAL, 2, 32, cpc_video::max_display_width/16, cpc_video::max_display_height);
    this->video.init(m);
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
    this->video.reset();
    mc6845_reset(&board.mc6845);
    crt_reset(&board.crt);
    ay38912_reset(&board.ay38912);
    i8255_reset(&board.i8255);
    this->ga_config = 0;
    this->ram_config = 0;
    z80_reset(&board.z80);
    board.z80.PC = 0x0000;
    mem_unmap_all(&board.mem);
    this->update_memory_mapping();
    this->tick_count = 0;
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
        cpc.video.interrupt_acknowledge();
    }

    /*
        decide how many wait states must be injected, the CPC signals
        the wait line in 3 out of 4 cycles:
    
         0: wait inactive
         1: wait active
         2: wait active
         3: wait active
    
        the CPU samples the wait line only on specific clock ticks
        during memory or IO operations, a wait states are only injected
        if the 'wait active' happens on the same clock tick as the
        CPU would sample the wait line
    */
    int wait_sample_tick = -1;
    if (pins & Z80_MREQ) {
        // a memory request or opcode fetch, wait is sampled on second clock tick
        wait_sample_tick = 1;
    }
    else if (pins & Z80_IORQ) {
        if (pins & Z80_M1) {
            // an interrupt acknowledge cycle, wait is sampled on fourth clock tick
            wait_sample_tick = 3;
        }
        else {
            // an IO cycle, wait is sampled on third clock tick
            wait_sample_tick = 2;
        }
    }
    bool wait = false;
    uint32_t wait_cycles = 0;
    for (int i = 0; i<num_ticks; i++) {
        do {
            // CPC gate array sets the wait pin for 3 out of 4 clock ticks
            bool wait_pin = (cpc.tick_count++ & 3) != 0;
            wait = (wait_pin && (wait || (i == wait_sample_tick)));
            if (wait) {
                wait_cycles++;
            }
            // on every 4th clock cycle, tick the system
            if (!wait_pin) {
                if (ay38912_tick(&board.ay38912)) {
                    board.audiobuffer.write(board.ay38912.sample);
                }
                pins = cpc.video.tick(pins);
            }
        }
        while (wait);
    }
    Z80_SET_WAIT(pins, wait_cycles);

    // memory and IO requests
    if (pins & Z80_MREQ) {
        const uint16_t addr = Z80_GET_ADDR(pins);
        if (pins & Z80_RD) {
            Z80_SET_DATA(pins, mem_rd(&board.mem, addr));
        }
        else if (pins & Z80_WR) {
            mem_wr(&board.mem, addr, Z80_GET_DATA(pins));
        }
    }
    else if (pins & Z80_IORQ) {
        if (pins & Z80_RD) {
            uint8_t data = cpc.cpu_in(pins);
            Z80_SET_DATA(pins, data);
        }
        else if (pins & Z80_WR) {
            cpc.cpu_out(pins);
        }
    }
    return pins;
}

//------------------------------------------------------------------------------
void
cpc_t::cpu_out(uint64_t pins) {
    // http://cpcwiki.eu/index.php/Default_I/O_Port_Summary
    const uint16_t addr = Z80_GET_ADDR(pins);
    const uint8_t data = Z80_GET_DATA(pins);
    if (0 == (addr & (1<<15))) {
        // Gate Array or RAM configuration
        if (0 != (addr & (1<<14))) {
            // Gate Array
            switch (data & 0xC0) {
                // select pen
                case 0x00:
                    this->video.select_pen(data);
                    break;
                // assign color to selected pen
                case 0x40:
                    this->video.assign_color(data);
                    break;
                // select screen mode, rom config, interrupt ctrl
                case 0x80:
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
                    this->ga_config = data;
                    this->video.set_video_mode(data & 3);
                    if (data & (1<<4)) {
                        this->video.interrupt_control();
                    }
                    this->update_memory_mapping();
                    break;
            }
        }
        // CPC6128 RAM configuration
        if (((data & 0xC0) == 0xC0) && (system::cpc6128 == this->cur_model)) {
            this->ram_config = data;
            this->update_memory_mapping();
        }
    }
    if (0 == (addr & (1<<14))) {
        // CRTC function
        // FIXME: check schematics for actual pin connections!
        uint64_t crtc_pins = (pins & Z80_PIN_MASK)|MC6845_CS|MC6845_RW;
        const uint16_t crtc_func = addr & 0x0300;
        if (crtc_func == 0x0000) {
            // 0xBCxx: select CRTC register
            mc6845_iorq(&board.mc6845, crtc_pins|MC6845_RS);
        }
        else if (crtc_func == 0x0100) {
            // 0xBDxx: write to selected CRTC register
            mc6845_iorq(&board.mc6845, crtc_pins);
        }
        else {
            printf("WARNING: unknown CRTC function\n");
        }
        return;
    }
    if (0 == (addr & (1<<13))) {
        // FIXME: ROM select
        printf("OUT ROM Select: %02x\n", data);
    }
    if (0 == (addr & (1<<12))) {
        // FIXME: printer port
        printf("OUT Printer Port: %02x\n", data);
    }
    if (0 == (addr & (1<<11))) {
        // 8255 PPI
        uint64_t ppi_pins = (pins & Z80_PIN_MASK)|I8255_CS|I8255_WR;
        if (addr & 0x0100) { ppi_pins |= I8255_A0; }
        if (addr & 0x0200) { ppi_pins |= I8255_A1; }
        i8255_iorq(&board.i8255, ppi_pins);
    }
    if (0 == (addr & (1<<10))) {
        // FIXME: peripheral soft reset
        printf("OUT Peripheral Soft Reset: %02x\n", data);
    }
}

//------------------------------------------------------------------------------
uint8_t
cpc_t::cpu_in(uint64_t pins) {
    const uint16_t addr = Z80_GET_ADDR(pins);
    if (0 == (addr & (1<<14))) {
        // CRTC function
        // FIXME: untested
        // FIXME: check actual board schematics for pin connections
        const uint16_t crtc_func = addr & 0x0300;
        uint64_t crtc_pins = (pins & Z80_PIN_MASK)|MC6845_CS;
        if (crtc_func == 0x0200) {
            // 0xBExx: read status register on type 1 CRTC
            crtc_pins |= MC6845_RS;
            crtc_pins = mc6845_iorq(&board.mc6845, crtc_pins);
            uint8_t data = MC6845_GET_DATA(crtc_pins);
            return data;
        }
        else if (crtc_func == 0x0300) {
            // 0xBFxx: read from selected CRTC register
            crtc_pins = mc6845_iorq(&board.mc6845, crtc_pins);
            uint8_t data = MC6845_GET_DATA(crtc_pins);
            return data;
        }
        else {
            //printf("IN: CRTC unknown function!\n");
            return 0xFF;
        }
    }
    if (0 == (addr & (1<<11))) {
        uint64_t ppi_pins = (pins & Z80_PIN_MASK)|I8255_CS;
        if (addr & 0x0100) { ppi_pins |= I8255_A0; }
        if (addr & 0x0200) { ppi_pins |= I8255_A1; }
        ppi_pins = i8255_iorq(&board.i8255, ppi_pins);
        return I8255_GET_DATA(ppi_pins);
    }
    if (0 == (addr & (1<<10))) {
        // FIXME: Expansion Peripherals
    }
    // fallthrough
    return 0x00;
}

//------------------------------------------------------------------------------
uint64_t
cpc_t::ppi_out(int port_id, uint64_t pins, uint8_t data) {
    if (I8255_PORT_C == port_id) {
        // PSG function
        if (data & 0xC0) {
            // PSG function
            uint64_t ay_pins = AY38912_BDIR;    // select or write
            if (data & 0x40) { ay_pins |= AY38912_BC1; }   // select register
            AY38912_SET_DATA(ay_pins, board.i8255.output[I8255_PORT_A]);
            ay38912_iorq(&board.ay38912, ay_pins);
        }
        // FIXME: cassette write data
        // bits 0..3: select keyboard matrix line
        kbd_set_active_columns(&board.kbd, 1<<(data & 0x0F));

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
    return pins;
}

//------------------------------------------------------------------------------
uint8_t
cpc_t::ppi_in(int port_id) {
    if (I8255_PORT_A == port_id) {
        // catch keyboard data which is normally in PSG PORT A
        if (board.ay38912.addr == AY38912_REG_IO_PORT_A) {
            return ~kbd_scan_lines(&board.kbd);
        }
        else {
            // read PSG register
            uint64_t ay_pins = AY38912_BC1;
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
        if (cpc.video.vsync_bit()) {
            val |= (1<<0);
        }
        return val;
    }
    else {
        // shouldn't happen
        return 0xFF;
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
cpc_t::update_memory_mapping() {
    // index into RAM config array
    int ram_table_index;
    uint8_t* rom0_ptr,*rom1_ptr;
    if (system::kccompact == this->cur_model) {
        ram_table_index = 0;
        rom0_ptr = roms.ptr(rom_images::kcc_os);
        rom1_ptr = roms.ptr(rom_images::kcc_basic);
    }
    else if (system::cpc6128 == this->cur_model) {
        ram_table_index = this->ram_config & 0x07;
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
    // 0x0000..0x3FFF
    if (this->ga_config & (1<<2)) {
        // read/write from and to RAM bank
        mem_map_ram(&board.mem, 0, 0x0000, 0x4000, board.ram[i0]);
    }
    else {
        // read from ROM, write to RAM
        mem_map_rw(&board.mem, 0, 0x0000, 0x4000, rom0_ptr, board.ram[i0]);
    }
    // 0x4000..0x7FFF
    mem_map_ram(&board.mem, 0, 0x4000, 0x4000, board.ram[i1]);
    // 0x8000..0xBFFF
    mem_map_ram(&board.mem, 0, 0x8000, 0x4000, board.ram[i2]);
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

//------------------------------------------------------------------------------
void
cpc_t::put_input(uint8_t ascii, uint8_t joy0_mask) {
    if (ascii) {
        kbd_key_down(&board.kbd, ascii);
        kbd_key_up(&board.kbd, ascii);
    }
    // FIXME: joystick
}

//------------------------------------------------------------------------------
void
cpc_t::decode_audio(float* buffer, int num_samples) {
    board.audiobuffer.read(buffer, num_samples);
}

//------------------------------------------------------------------------------
const void*
cpc_t::framebuffer(int& out_width, int& out_height) {
    if (this->video.debug_video) {
        out_width = cpc_video::dbg_max_display_width;
        out_height = cpc_video::dbg_max_display_height;
    }
    else {
        out_width = cpc_video::max_display_width;
        out_height = cpc_video::max_display_height;
    }
    return board.rgba8_buffer;
}

//------------------------------------------------------------------------------
/*
bool
cpc::load_sna(filesystem* fs, const char* name, filetype type, bool start) {
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
            YAKC_ASSERT(sizeof(this->board->ram) >= 0x10000);
            fs->read(fp, this->board->ram, 0x10000);
        }
        else {
            YAKC_ASSERT(sizeof(this->board->ram) >= 0x20000);
            fs->read(fp, this->board->ram, 0x20000);
        }
    }
    // CPU state
    auto& cpu = this->board->z80;
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
    for (int i = 0; i < 17; i++) {
        this->cpu_out(0x7FFF, i);
        this->cpu_out(0x7FFF, (hdr.pens[i] & 0x1F) | 0x40);
    }
    this->cpu_out(0x7FFF, hdr.selected_pen & 0x1F);
    this->cpu_out(0x7FFF, (hdr.gate_array_config & 0x3F) | 0x80);
    this->cpu_out(0x7FFF, (hdr.ram_config & 0x3F) | 0xC0);
    auto& vdg = this->board->mc6845;
    for (int i = 0; i < 18; i++) {
        vdg.select(i);
        vdg.write(hdr.crtc_regs[i]);
    }
    vdg.select(hdr.crtc_selected);
    // FIXME: rom_config
    auto& ppi = this->board->i8255;
    ppi.output[i8255::PORT_A] = hdr.ppi_a;
    ppi.output[i8255::PORT_B] = hdr.ppi_b;
    ppi.output[i8255::PORT_C] = hdr.ppi_c;
    this->board->ay8910.select(hdr.psg_selected);
    for (int i = 0; i < 16; i++) {
        this->board->ay8910.regs[i] = hdr.psg_regs[i];
    }
    fs->close(fp);
    fs->rm(name);
    if (!hdr_valid) {
        return false;
    }
    return true;
}
*/

//------------------------------------------------------------------------------
/*
bool
cpc::load_bin(filesystem* fs, const char* name, filetype type, bool start) {
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
        auto& cpu = this->board->z80;
        uint8_t val;
        for (uint16_t i=0; i < len; i++) {
            fs->read(fp, &val, sizeof(val));
            cpu.mem.w8(load_addr + i, val);
        }
        cpu.PC = start_addr;
        cpu.IFF1 = cpu.IFF2 = true;
        cpu.HALT = false;
    }
    fs->close(fp);
    fs->rm(name);
    return true;
}
*/

//------------------------------------------------------------------------------
bool
cpc_t::quickload(filesystem* fs, const char* name, filetype type, bool start) {
return false;
/*
    if (filetype::cpc_sna == type) {
        return this->load_sna(fs, name, type, start);
    }
    else if (filetype::cpc_bin == type) {
        return this->load_bin(fs, name, type, start);
    }
    else {
        return false;
    }
*/
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
