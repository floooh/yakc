//------------------------------------------------------------------------------
//  mos6502.cc
//------------------------------------------------------------------------------
#include "mos6502.h"
#include "yakc/core/system_bus.h"

namespace YAKC {

//------------------------------------------------------------------------------
//  address mode table [cc][bbb][aaa]
//
enum {
    A____,      // no addressing mode
    A_IMM,      // #
    A_ZER,      // zp
    A_ZPX,      // zp,X
    A_ZPY,      // zp,Y
    A_ABS,      // abs
    A_ABX,      // abs,X
    A_ABY,      // abs,Y
    A_IDX,      // (zp,X)
    A_IDY,      // (zp),Y

    A_INV,      // this is an invalid instruction
};
uint8_t mos6502::addr_modes[4][8][8] = {
    // cc = 00
    {
        //---    BIT    JMP    JMP()  STY    LDY    CPY    CPX
        { A____, A____, A____, A____, A_INV, A_IMM, A_IMM, A_IMM },
        { A_INV, A_ZER, A_INV, A_INV, A_ZER, A_ZER, A_ZER, A_ZER },
        { A____, A____, A____, A____, A____, A____, A____, A____ },
        { A_INV, A_ABS, A_ABS, A_ABS, A_ABS, A_ABS, A_ABS, A_ABS },
        { A____, A____, A____, A____, A____, A____, A____, A____ },
        { A_INV, A_INV, A_INV, A_INV, A_ZPX, A_ZPX, A_INV, A_INV },
        { A_INV, A_INV, A_INV, A_INV, A____, A_INV, A_INV, A_INV },
        { A_INV, A_INV, A_INV, A_INV, A_INV, A_ABX, A_INV, A_INV },
    },
    // cc = 01
    {
        //ORA    AND    EOR    ADC    STA    LDA    CMP    SBC
        { A_IDX, A_IDX, A_IDX, A_IDX, A_IDX, A_IDX, A_IDX, A_IDX },
        { A_ZER, A_ZER, A_ZER, A_ZER, A_ZER, A_ZER, A_ZER, A_ZER },
        { A_IMM, A_IMM, A_IMM, A_IMM, A_INV, A_IMM, A_IMM, A_IMM },
        { A_ABS, A_ABS, A_ABS, A_ABS, A_ABS, A_ABS, A_ABS, A_ABS },
        { A_IDY, A_IDY, A_IDY, A_IDY, A_IDY, A_IDY, A_IDY, A_IDY },
        { A_ZPX, A_ZPX, A_ZPX, A_ZPX, A_ZPX, A_ZPX, A_ZPX, A_ZPX },
        { A_ABY, A_ABY, A_ABY, A_ABY, A_ABY, A_ABY, A_ABY, A_ABY },
        { A_ABX, A_ABX, A_ABX, A_ABX, A_ABX, A_ABX, A_ABX, A_ABX },
    },
    // cc = 02
    {
        //ASL    ROL    LSR    ROR    STX    LDX    DEC    INC
        { A_INV, A_INV, A_INV, A_INV, A_INV, A_IMM, A_INV, A_INV },
        { A_ZER, A_ZER, A_ZER, A_ZER, A_ZER, A_ZER, A_ZER, A_ZER },
        { A____, A____, A____, A____, A____, A____, A____, A____ },
        { A_ABS, A_ABS, A_ABS, A_ABS, A_ABS, A_ABS, A_ABS, A_ABS },
        { A_INV, A_INV, A_INV, A_INV, A_INV, A_INV, A_INV, A_INV },
        { A_ZPX, A_ZPX, A_ZPX, A_ZPX, A_ZPY, A_ZPY, A_ZPX, A_ZPX },
        { A_INV, A_INV, A_INV, A_INV, A____, A____, A_INV, A_INV },
        { A_ABX, A_ABX, A_ABX, A_ABX, A_INV, A_ABY, A_ABX, A_ABX },
    },
    // cc = 03
    {
        { A_INV, A_INV, A_INV, A_INV, A_INV, A_INV, A_INV, A_INV },
        { A_INV, A_INV, A_INV, A_INV, A_INV, A_INV, A_INV, A_INV },
        { A_INV, A_INV, A_INV, A_INV, A_INV, A_INV, A_INV, A_INV },
        { A_INV, A_INV, A_INV, A_INV, A_INV, A_INV, A_INV, A_INV },
        { A_INV, A_INV, A_INV, A_INV, A_INV, A_INV, A_INV, A_INV },
        { A_INV, A_INV, A_INV, A_INV, A_INV, A_INV, A_INV, A_INV },
        { A_INV, A_INV, A_INV, A_INV, A_INV, A_INV, A_INV, A_INV },
        { A_INV, A_INV, A_INV, A_INV, A_INV, A_INV, A_INV, A_INV },
    },
};

//------------------------------------------------------------------------------
// memory r/w modes tables [cc][aaa]
//  M_R:  read
//  M_W:  read
//  M_RW: read/modify/write
//
enum {
    M___,       // no memory access
    M_R,        // read
    M_W,        // write
    M_RW,       // read-modify-write
};
uint8_t mos6502::rw_table[4][8] = {
    //---    BIT    JMP    JMP()  STY    LDY    CPY    CPX
    { M___,  M_R,   M_R,   M_R,   M_W,   M_R,   M_R,   M_R },
    //ORA    AND    EOR    ADC    STA    LDA    CMP    SBC
    { M_R,   M_R,   M_R,   M_R,   M_W,   M_R,   M_R,   M_R },
    //ASL    ROL    LSR    ROR    STX    LDX    DEC    INC
    { M_RW,  M_RW,  M_RW,  M_RW,  M_W,   M_R,   M_RW,  M_RW},
    //
    { M___,  M___,  M___,  M___,  M___,  M___,  M___,  M___},
};


//------------------------------------------------------------------------------
mos6502::mos6502() {
    this->init();
}

//------------------------------------------------------------------------------
void
mos6502::init() {
    A = 0; X = 0; Y = 0; S = 0; P = XF; PC = 0;
    OP = 0;
    RW = true;
    Cycle = 0;
    State = Fetch;
    AddrMode = A____;
    MemAccess = M___;
}

//------------------------------------------------------------------------------
void
mos6502::reset() {
    P = (IF | XF);
    S = 0xFD;
    PC = mem.r16(0xFFFC);
    RW = true;
    Cycle = 0;
    State = Fetch;
    AddrMode = A____;
    MemAccess = M___;
}

//------------------------------------------------------------------------------
uint32_t
mos6502::step_op(system_bus* bus) {
    do {
        this->step(bus);
    }
    while (State != Fetch);
    return Cycle;
}

//------------------------------------------------------------------------------
void
mos6502::step(system_bus* bus) {
    if (State == Fetch) {
        this->step_fetch();
    }
    if (RW) {
        DATA = mem.r8(ADDR);
    }
    if (State == Decode) {
        step_decode();
    }
    if (State == Addr) {
        step_addr();
    }
    if (State == Exec) {
        step_exec();
    }
    if (!RW) {
        mem.w8(ADDR, DATA);
    }
    Cycle++;
}

//------------------------------------------------------------------------------
void
mos6502::step_fetch() {
    this->ADDR = PC++;
    RW = true;
    State = Decode;
    Cycle = 0;
}

//------------------------------------------------------------------------------
void
mos6502::step_decode() {
    //  - determine addressing mode
    //  - determine memory acces (read, write, read/write)
    OP = DATA;
    uint8_t cc  = OP & 0x03;
    uint8_t bbb = (OP >> 2) & 0x07;
    uint8_t aaa = (OP >> 5) & 0x07;
    State = Addr;
    AddrMode  = addr_modes[cc][bbb][aaa];
    MemAccess = rw_table[cc][aaa];
}

//------------------------------------------------------------------------------
void
mos6502::step_addr() {
    switch (AddrMode) {
        case A_IMM: this->step_addr_imm(); break;
        case A_ZER: this->step_addr_zer(); break;
        case A_ZPX: this->step_addr_zpx(); break;
        case A_ZPY: this->step_addr_zpy(); break;
        case A_ABS: this->step_addr_abs(); break;
        case A_ABX: this->step_addr_abx(); break;
        case A_ABY: this->step_addr_aby(); break;
        case A_IDX: this->step_addr_idx(); break;
        case A_IDY: this->step_addr_idy(); break;
        default: State = Exec; break;
    }
}

//------------------------------------------------------------------------------
void
mos6502::step_addr_imm() {
    switch (Cycle) {
        case 0: ADDR = PC++; break;
        default: State = Exec; break;
    }
}

//------------------------------------------------------------------------------
void
mos6502::step_addr_zer() {
    switch (Cycle) {
        case 0: ADDR = PC++; break;
        case 1: ADDR = DATA; break;
        default: State = Exec; break;
    }
}

//------------------------------------------------------------------------------
void
mos6502::step_addr_zpx() {
    switch (Cycle) {
        case 0: ADDR = PC++; break;
        case 1: ADDR = DATA; break;
        case 2: ADDR = (ADDR + X) & 0x00FF; break;
        default: State = Exec; break;
    }
}

//------------------------------------------------------------------------------
void
mos6502::step_addr_zpy() {
    switch (Cycle) {
        case 0: ADDR = PC++; break;
        case 1: ADDR = DATA; break;
        case 2: ADDR = (ADDR + Y) & 0x00FF; break;
        default: State = Exec; break;
    }
}

//------------------------------------------------------------------------------
void
mos6502::step_addr_abs() {
    switch (Cycle) {
        case 0: ADDR = PC++; break;
        case 1: tmp = DATA; ADDR = PC++; break;
        case 2: ADDR = DATA<<8 | tmp; break;
        default: State = Exec; break;
    }
}

//------------------------------------------------------------------------------
void
mos6502::step_addr_abx() {
    switch (Cycle) {
        case 0: ADDR = PC++; break;
        case 1: tmp = DATA + X; ADDR = PC++; break;
        case 2: ADDR = (DATA<<8) | (tmp&0xFF); break;
        case 3:
            // if page boundary was not crossed, and not store, can exit early
            if (((tmp & 0xFF00) == 0x0000) && (MemAccess == M_R)) {
                State = Exec;
            }
            else {
                // add carry and read again
                ADDR = (ADDR & 0xFF00) + tmp;
            }
            break;
        default: State = Exec; break;
    }
}

//------------------------------------------------------------------------------
void
mos6502::step_addr_aby() {
    switch (Cycle) {
        case 0: ADDR = PC++; break;
        case 1: tmp = DATA + Y; ADDR = PC++; break;
        case 2: ADDR = (DATA<<8) | (tmp&0xFF); break;
        case 3:
            // if page boundary was not crossed, and not store, can exit early
            if (((tmp & 0xFF00) == 0x0000) && (MemAccess == M_R)) {
                State = Exec;
            }
            else {
                // add carry and read again
                ADDR = (ADDR & 0xFF00) + tmp;
            }
            break;
        default: State = Exec; break;
    }
}

//------------------------------------------------------------------------------
void
mos6502::step_addr_idx() {
    switch (Cycle) {
        case 0: ADDR = PC++; break;
        case 1: ADDR = DATA; break;
        case 2: ADDR = (ADDR + X) & 0x00FF; break;
        case 3: tmp = DATA; ADDR = (ADDR + 1) & 0x00FF; break;
        case 4: ADDR = (DATA<<8) | tmp; break;
        default: State = Exec; break;
    }
}

//------------------------------------------------------------------------------
void
mos6502::step_addr_idy() {
    switch (Cycle) {
        case 0: ADDR = PC++; break;
        case 1: ADDR = DATA; break;
        case 2: tmp = DATA + Y; ADDR = (ADDR + 1) & 0x00FF; break;
        case 3: ADDR = (DATA<<8) | (tmp&0xFF); break;
        case 4:
            // if page boundary was not crossed, and not store, can exit early
            if (((tmp & 0xFF00) == 0x0000) && (MemAccess == M_R)) {
                // page boundary not crossed, can exit early
                State = Exec;
            }
            else {
                // add carry and read again
                ADDR = (ADDR & 0xFF00) + tmp;
            }
            break;
        default: State = Exec; break;
    }
}

//------------------------------------------------------------------------------
void
mos6502::step_exec() {
    uint8_t cc  = OP & 0x03;
    uint8_t aaa = (OP >> 5) & 0x07;
    switch (cc) {
        case 0:
            switch (aaa) {
                case 1: this->bit(); break;
                case 2: this->jmp(); break;
                case 3: this->jmpi(); break;
                case 4: this->sty(); break;
                case 5: this->ldy(); break;
                case 6: this->cpy(); break;
                case 7: this->cpx(); break;
            }
            break;

        case 1:
            switch (aaa) {
                case 0: this->ora(); break;
                case 1: this->anda(); break;
                case 2: this->eor(); break;
                case 3: this->adc(); break;
                case 4: this->sta(); break;
                case 5: this->lda(); break;
                case 6: this->cmp(); break;
                case 7: this->sbc(); break;
            }
            break;

        case 2:
            switch (aaa) {
                case 0: this->asl(); break;
                case 1: this->rol(); break;
                case 2: this->lsr(); break;
                case 3: this->ror(); break;
                case 4: this->stx(); break;
                case 5: this->ldx(); break;
                case 6: this->dec(); break;
                case 7: this->inc(); break;
            }
            break;
    }
}

} // namespace YAKC
