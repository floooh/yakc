//------------------------------------------------------------------------------
//  z80_test.cc
//------------------------------------------------------------------------------
#include "UnitTest++/src/UnitTest++.h"
#include "yakc/z80.h"
#include "yakc/system_bus.h"

using namespace YAKC;

static ubyte ram0[0x4000];

class cpuTestBus : public system_bus {
public:
    uword out_port = 0;
    ubyte out_byte = 0xFF;
    void cpu_out(uword port, ubyte val) {
        this->out_port = port;
        this->out_byte  = val;
    }
    ubyte cpu_in(uword port) {
        return (port & 0xFF) * 2;
    }
};
static cpuTestBus bus;

static z80 init_z80() {
    z80 cpu;
    memset(ram0, 0, sizeof(ram0));
    cpu.mem.map(0, 0x0000, sizeof(ram0), ram0, true);
    cpu.init();
    return cpu;
}

// LD A,R
// LD A,I
TEST(LD_A_RI) {
    z80 cpu = init_z80();
    cpu.IFF1 = true;
    cpu.IFF2 = true;
    cpu.R = 0x34;
    cpu.I = 0x1;
    cpu.F = z80::CF;
    ubyte prog[] {
        0xED, 0x57,         // LD A,I
        0x97,               // SUB A
        0xED, 0x5F,         // LD A,R
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    CHECK(9 == cpu.step(&bus)); CHECK(0x01 == cpu.A); CHECK(cpu.test_flags(z80::PF|z80::CF));
    CHECK(4 == cpu.step(&bus)); CHECK(0x00 == cpu.A); CHECK(cpu.test_flags(z80::ZF|z80::NF));
    CHECK(9 == cpu.step(&bus)); CHECK(0x39 == cpu.A); CHECK(cpu.test_flags(z80::PF));
}

// LD r,s
// LD r,n
TEST(LD_r_sn) {
    z80 cpu = init_z80();
    ubyte prog[] = {
        0x3E, 0x12,     // LD A,0x12
        0x47,           // LD B,A
        0x4F,           // LD C,A
        0x57,           // LD D,A
        0x5F,           // LD E,A
        0x67,           // LD H,A
        0x6F,           // LD L,A
        0x7F,           // LD A,A

        0x06, 0x13,     // LD B,0x13
        0x40,           // LD B,B
        0x48,           // LD C,B
        0x50,           // LD D,B
        0x58,           // LD E,B
        0x60,           // LD H,B
        0x68,           // LD L,B
        0x78,           // LD A,B

        0x0E, 0x14,     // LD C,0x14
        0x41,           // LD B,C
        0x49,           // LD C,C
        0x51,           // LD D,C
        0x59,           // LD E,C
        0x61,           // LD H,C
        0x69,           // LD L,C
        0x79,           // LD A,C

        0x16, 0x15,     // LD D,0x15
        0x42,           // LD B,D
        0x4A,           // LD C,D
        0x52,           // LD D,D
        0x5A,           // LD E,D
        0x62,           // LD H,D
        0x6A,           // LD L,D
        0x7A,           // LD A,D

        0x1E, 0x16,     // LD E,0x16
        0x43,           // LD B,E
        0x4B,           // LD C,E
        0x53,           // LD D,E
        0x5B,           // LD E,E
        0x63,           // LD H,E
        0x6B,           // LD L,E
        0x7B,           // LD A,E

        0x26, 0x17,     // LD H,0x17
        0x44,           // LD B,H
        0x4C,           // LD C,H
        0x54,           // LD D,H
        0x5C,           // LD E,H
        0x64,           // LD H,H
        0x6C,           // LD L,H
        0x7C,           // LD A,H

        0x2E, 0x18,     // LD L,0x18
        0x45,           // LD B,L
        0x4D,           // LD C,L
        0x55,           // LD D,L
        0x5D,           // LD E,L
        0x65,           // LD H,L
        0x6D,           // LD L,L
        0x7D,           // LD A,L
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));
    CHECK(7==cpu.step(&bus)); CHECK(0x12 == cpu.A);
    CHECK(4==cpu.step(&bus)); CHECK(0x12 == cpu.B);
    CHECK(4==cpu.step(&bus)); CHECK(0x12 == cpu.C);
    CHECK(4==cpu.step(&bus)); CHECK(0x12 == cpu.D);
    CHECK(4==cpu.step(&bus)); CHECK(0x12 == cpu.E);
    CHECK(4==cpu.step(&bus)); CHECK(0x12 == cpu.H);
    CHECK(4==cpu.step(&bus)); CHECK(0x12 == cpu.L);
    CHECK(4==cpu.step(&bus)); CHECK(0x12 == cpu.A);
    CHECK(7==cpu.step(&bus)); CHECK(0x13 == cpu.B);
    CHECK(4==cpu.step(&bus)); CHECK(0x13 == cpu.B);
    CHECK(4==cpu.step(&bus)); CHECK(0x13 == cpu.C);
    CHECK(4==cpu.step(&bus)); CHECK(0x13 == cpu.D);
    CHECK(4==cpu.step(&bus)); CHECK(0x13 == cpu.E);
    CHECK(4==cpu.step(&bus)); CHECK(0x13 == cpu.H);
    CHECK(4==cpu.step(&bus)); CHECK(0x13 == cpu.L);
    CHECK(4==cpu.step(&bus)); CHECK(0x13 == cpu.A);
    CHECK(7==cpu.step(&bus)); CHECK(0x14 == cpu.C);
    CHECK(4==cpu.step(&bus)); CHECK(0x14 == cpu.B);
    CHECK(4==cpu.step(&bus)); CHECK(0x14 == cpu.C);
    CHECK(4==cpu.step(&bus)); CHECK(0x14 == cpu.D);
    CHECK(4==cpu.step(&bus)); CHECK(0x14 == cpu.E);
    CHECK(4==cpu.step(&bus)); CHECK(0x14 == cpu.H);
    CHECK(4==cpu.step(&bus)); CHECK(0x14 == cpu.L);
    CHECK(4==cpu.step(&bus)); CHECK(0x14 == cpu.A);
    CHECK(7==cpu.step(&bus)); CHECK(0x15 == cpu.D);
    CHECK(4==cpu.step(&bus)); CHECK(0x15 == cpu.B);
    CHECK(4==cpu.step(&bus)); CHECK(0x15 == cpu.C);
    CHECK(4==cpu.step(&bus)); CHECK(0x15 == cpu.D);
    CHECK(4==cpu.step(&bus)); CHECK(0x15 == cpu.E);
    CHECK(4==cpu.step(&bus)); CHECK(0x15 == cpu.H);
    CHECK(4==cpu.step(&bus)); CHECK(0x15 == cpu.L);
    CHECK(4==cpu.step(&bus)); CHECK(0x15 == cpu.A);
    CHECK(7==cpu.step(&bus)); CHECK(0x16 == cpu.E);
    CHECK(4==cpu.step(&bus)); CHECK(0x16 == cpu.B);
    CHECK(4==cpu.step(&bus)); CHECK(0x16 == cpu.C);
    CHECK(4==cpu.step(&bus)); CHECK(0x16 == cpu.D);
    CHECK(4==cpu.step(&bus)); CHECK(0x16 == cpu.E);
    CHECK(4==cpu.step(&bus)); CHECK(0x16 == cpu.H);
    CHECK(4==cpu.step(&bus)); CHECK(0x16 == cpu.L);
    CHECK(4==cpu.step(&bus)); CHECK(0x16 == cpu.A);
    CHECK(7==cpu.step(&bus)); CHECK(0x17 == cpu.H);
    CHECK(4==cpu.step(&bus)); CHECK(0x17 == cpu.B);
    CHECK(4==cpu.step(&bus)); CHECK(0x17 == cpu.C);
    CHECK(4==cpu.step(&bus)); CHECK(0x17 == cpu.D);
    CHECK(4==cpu.step(&bus)); CHECK(0x17 == cpu.E);
    CHECK(4==cpu.step(&bus)); CHECK(0x17 == cpu.H);
    CHECK(4==cpu.step(&bus)); CHECK(0x17 == cpu.L);
    CHECK(4==cpu.step(&bus)); CHECK(0x17 == cpu.A);
    CHECK(7==cpu.step(&bus)); CHECK(0x18 == cpu.L);
    CHECK(4==cpu.step(&bus)); CHECK(0x18 == cpu.B);
    CHECK(4==cpu.step(&bus)); CHECK(0x18 == cpu.C);
    CHECK(4==cpu.step(&bus)); CHECK(0x18 == cpu.D);
    CHECK(4==cpu.step(&bus)); CHECK(0x18 == cpu.E);
    CHECK(4==cpu.step(&bus)); CHECK(0x18 == cpu.H);
    CHECK(4==cpu.step(&bus)); CHECK(0x18 == cpu.L);
    CHECK(4==cpu.step(&bus)); CHECK(0x18 == cpu.A);
}

// LD r,(HL)
TEST(LD_r_iHL) {
    z80 cpu = init_z80();
    ubyte prog[] = {
        0x21, 0x00, 0x10,   // LD HL,0x1000
        0x3E, 0x33,         // LD A,0x33
        0x77,               // LD (HL),A
        0x3E, 0x22,         // LD A,0x22
        0x46,               // LD B,(HL)
        0x4E,               // LD C,(HL)
        0x56,               // LD D,(HL)
        0x5E,               // LD E,(HL)
        0x66,               // LD H,(HL)
        0x26, 0x10,         // LD H,0x10
        0x6E,               // LD L,(HL)
        0x2E, 0x00,         // LD L,0x00
        0x7E,               // LD A,(HL)
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    CHECK(10==cpu.step(&bus)); CHECK(0x1000 == cpu.HL);   
    CHECK(7==cpu.step(&bus)); CHECK(0x33 == cpu.A);       
    CHECK(7==cpu.step(&bus)); CHECK(0x33 == cpu.mem.r8(0x1000));
    CHECK(7==cpu.step(&bus)); CHECK(0x22 == cpu.A);       
    CHECK(7==cpu.step(&bus)); CHECK(0x33 == cpu.B);       
    CHECK(7==cpu.step(&bus)); CHECK(0x33 == cpu.C);       
    CHECK(7==cpu.step(&bus)); CHECK(0x33 == cpu.D);       
    CHECK(7==cpu.step(&bus)); CHECK(0x33 == cpu.E);       
    CHECK(7==cpu.step(&bus)); CHECK(0x33 == cpu.H);       
    CHECK(7==cpu.step(&bus)); CHECK(0x10 == cpu.H);       
    CHECK(7==cpu.step(&bus)); CHECK(0x33 == cpu.L);       
    CHECK(7==cpu.step(&bus)); CHECK(0x00 == cpu.L);       
    CHECK(7==cpu.step(&bus)); CHECK(0x33 == cpu.A);       
}

// LD r,([IX|IY] + d)
TEST(LD_r_IXY_d) {
    z80 cpu = init_z80();
    ubyte data[] = {
        1, 2, 3, 4, 5, 6, 7, 8
    };
    cpu.mem.write(0x1000, data, sizeof(data));

    ubyte prog[] = {
        0xDD, 0x21, 0x03, 0x10,     // LD IX,0x1003
        0xDD, 0x7E, 0x00,           // LD A,(IX+0)
        0xDD, 0x46, 0x01,           // LD B,(IX+1)
        0xDD, 0x4E, 0x02,           // LD C,(IX+2)
        0xDD, 0x56, 0xFF,           // LD D,(IX-1)
        0xDD, 0x5E, 0xFE,           // LD E,(IX-2)
        0xDD, 0x66, 0x03,           // LD H,(IX+3)
        0xDD, 0x6E, 0xFD,           // LD L,(IX-3)

        0xFD, 0x21, 0x04, 0x10,     // LD IY,0x1003
        0xFD, 0x7E, 0x00,           // LD A,(IY+0)
        0xFD, 0x46, 0x01,           // LD B,(IY+1)
        0xFD, 0x4E, 0x02,           // LD C,(IY+2)
        0xFD, 0x56, 0xFF,           // LD D,(IY-1)
        0xFD, 0x5E, 0xFE,           // LD E,(IY-2)
        0xFD, 0x66, 0x03,           // LD H,(IY+3)
        0xFD, 0x6E, 0xFD,           // LD L,(IY-3)
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    CHECK(14==cpu.step(&bus)); CHECK(0x1003 == cpu.IX);
    CHECK(19==cpu.step(&bus)); CHECK(4 == cpu.A);      
    CHECK(19==cpu.step(&bus)); CHECK(5 == cpu.B);      
    CHECK(19==cpu.step(&bus)); CHECK(6 == cpu.C);      
    CHECK(19==cpu.step(&bus)); CHECK(3 == cpu.D);      
    CHECK(19==cpu.step(&bus)); CHECK(2 == cpu.E);      
    CHECK(19==cpu.step(&bus)); CHECK(7 == cpu.H);      
    CHECK(19==cpu.step(&bus)); CHECK(1 == cpu.L);      
    CHECK(14==cpu.step(&bus)); CHECK(0x1004 == cpu.IY);
    CHECK(19==cpu.step(&bus)); CHECK(5 == cpu.A);      
    CHECK(19==cpu.step(&bus)); CHECK(6 == cpu.B);      
    CHECK(19==cpu.step(&bus)); CHECK(7 == cpu.C);      
    CHECK(19==cpu.step(&bus)); CHECK(4 == cpu.D);      
    CHECK(19==cpu.step(&bus)); CHECK(3 == cpu.E);      
    CHECK(19==cpu.step(&bus)); CHECK(8 == cpu.H);      
    CHECK(19==cpu.step(&bus)); CHECK(2 == cpu.L);      
}

// LD (HL),r
TEST(LD_iHL_r) {
    z80 cpu = init_z80();
    ubyte prog[] = {
        0x21, 0x00, 0x10,   // LD HL,0x1000
        0x3E, 0x12,         // LD A,0x12
        0x77,               // LD (HL),A
        0x06, 0x13,         // LD B,0x13
        0x70,               // LD (HL),B
        0x0E, 0x14,         // LD C,0x14
        0x71,               // LD (HL),C
        0x16, 0x15,         // LD D,0x15
        0x72,               // LD (HL),D
        0x1E, 0x16,         // LD E,0x16
        0x73,               // LD (HL),E
        0x74,               // LD (HL),H
        0x75,               // LD (HL),L
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    CHECK(10==cpu.step(&bus)); CHECK(0x1000 == cpu.HL);   
    CHECK(7==cpu.step(&bus)); CHECK(0x12 == cpu.A);       
    CHECK(7==cpu.step(&bus)); CHECK(0x12 == cpu.mem.r8(0x1000));
    CHECK(7==cpu.step(&bus)); CHECK(0x13 == cpu.B);       
    CHECK(7==cpu.step(&bus)); CHECK(0x13 == cpu.mem.r8(0x1000));
    CHECK(7==cpu.step(&bus)); CHECK(0x14 == cpu.C);       
    CHECK(7==cpu.step(&bus)); CHECK(0x14 == cpu.mem.r8(0x1000));
    CHECK(7==cpu.step(&bus)); CHECK(0x15 == cpu.D);       
    CHECK(7==cpu.step(&bus)); CHECK(0x15 == cpu.mem.r8(0x1000));
    CHECK(7==cpu.step(&bus)); CHECK(0x16 == cpu.E);       
    CHECK(7==cpu.step(&bus)); CHECK(0x16 == cpu.mem.r8(0x1000));
    CHECK(7==cpu.step(&bus)); CHECK(0x10 == cpu.mem.r8(0x1000));
    CHECK(7==cpu.step(&bus)); CHECK(0x00 == cpu.mem.r8(0x1000));
}

// LD (IX|IY + d),r
TEST(rIXY_d_r) {
    z80 cpu = init_z80();
    ubyte prog[] = {
        0xDD, 0x21, 0x03, 0x10,     // LD IX,0x1003
        0x3E, 0x12,                 // LD A,0x12
        0xDD, 0x77, 0x00,           // LD (IX+0),A
        0x06, 0x13,                 // LD B,0x13
        0xDD, 0x70, 0x01,           // LD (IX+1),B
        0x0E, 0x14,                 // LD C,0x14
        0xDD, 0x71, 0x02,           // LD (IX+2),C
        0x16, 0x15,                 // LD D,0x15
        0xDD, 0x72, 0xFF,           // LD (IX-1),D
        0x1E, 0x16,                 // LD E,0x16
        0xDD, 0x73, 0xFE,           // LD (IX-2),E
        0x26, 0x17,                 // LD H,0x17
        0xDD, 0x74, 0x03,           // LD (IX+3),H
        0x2E, 0x18,                 // LD L,0x18
        0xDD, 0x75, 0xFD,           // LD (IX-3),L
        0xFD, 0x21, 0x03, 0x10,     // LD IY,0x1003
        0x3E, 0x12,                 // LD A,0x12
        0xFD, 0x77, 0x00,           // LD (IY+0),A
        0x06, 0x13,                 // LD B,0x13
        0xFD, 0x70, 0x01,           // LD (IY+1),B
        0x0E, 0x14,                 // LD C,0x14
        0xFD, 0x71, 0x02,           // LD (IY+2),C
        0x16, 0x15,                 // LD D,0x15
        0xFD, 0x72, 0xFF,           // LD (IY-1),D
        0x1E, 0x16,                 // LD E,0x16
        0xFD, 0x73, 0xFE,           // LD (IY-2),E
        0x26, 0x17,                 // LD H,0x17
        0xFD, 0x74, 0x03,           // LD (IY+3),H
        0x2E, 0x18,                 // LD L,0x18
        0xFD, 0x75, 0xFD,           // LD (IY-3),L
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    CHECK(14==cpu.step(&bus)); CHECK(0x1003 == cpu.IX);      
    CHECK(7 ==cpu.step(&bus)); CHECK(0x12 == cpu.A);         
    CHECK(19==cpu.step(&bus)); CHECK(0x12 == cpu.mem.r8(0x1003));  
    CHECK(7 ==cpu.step(&bus)); CHECK(0x13 == cpu.B);         
    CHECK(19==cpu.step(&bus)); CHECK(0x13 == cpu.mem.r8(0x1004));  
    CHECK(7 ==cpu.step(&bus)); CHECK(0x14 == cpu.C);         
    CHECK(19==cpu.step(&bus)); CHECK(0x14 == cpu.mem.r8(0x1005));  
    CHECK(7 ==cpu.step(&bus)); CHECK(0x15 == cpu.D);         
    CHECK(19==cpu.step(&bus)); CHECK(0x15 == cpu.mem.r8(0x1002));  
    CHECK(7 ==cpu.step(&bus)); CHECK(0x16 == cpu.E);         
    CHECK(19==cpu.step(&bus)); CHECK(0x16 == cpu.mem.r8(0x1001));  
    CHECK(7 ==cpu.step(&bus)); CHECK(0x17 == cpu.H);         
    CHECK(19==cpu.step(&bus)); CHECK(0x17 == cpu.mem.r8(0x1006));  
    CHECK(7 ==cpu.step(&bus)); CHECK(0x18 == cpu.L);         
    CHECK(19==cpu.step(&bus)); CHECK(0x18 == cpu.mem.r8(0x1000));  
    CHECK(14==cpu.step(&bus)); CHECK(0x1003 == cpu.IY);
    CHECK(7 ==cpu.step(&bus)); CHECK(0x12 == cpu.A);        
    CHECK(19==cpu.step(&bus)); CHECK(0x12 == cpu.mem.r8(0x1003)); 
    CHECK(7 ==cpu.step(&bus)); CHECK(0x13 == cpu.B);        
    CHECK(19==cpu.step(&bus)); CHECK(0x13 == cpu.mem.r8(0x1004)); 
    CHECK(7 ==cpu.step(&bus)); CHECK(0x14 == cpu.C);        
    CHECK(19==cpu.step(&bus)); CHECK(0x14 == cpu.mem.r8(0x1005)); 
    CHECK(7 ==cpu.step(&bus)); CHECK(0x15 == cpu.D);        
    CHECK(19==cpu.step(&bus)); CHECK(0x15 == cpu.mem.r8(0x1002)); 
    CHECK(7 ==cpu.step(&bus)); CHECK(0x16 == cpu.E);        
    CHECK(19==cpu.step(&bus)); CHECK(0x16 == cpu.mem.r8(0x1001)); 
    CHECK(7 ==cpu.step(&bus)); CHECK(0x17 == cpu.H);        
    CHECK(19==cpu.step(&bus)); CHECK(0x17 == cpu.mem.r8(0x1006)); 
    CHECK(7 ==cpu.step(&bus)); CHECK(0x18 == cpu.L);        
    CHECK(19==cpu.step(&bus)); CHECK(0x18 == cpu.mem.r8(0x1000)); 
}

// LD (HL),n
TEST(LD_iHL_n) {
    z80 cpu = init_z80();
    ubyte prog[] = {
        0x21, 0x00, 0x20,   // LD HL,0x2000
        0x36, 0x33,         // LD (HL),0x33
        0x21, 0x00, 0x10,   // LD HL,0x1000
        0x36, 0x65,         // LD (HL),0x65
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    CHECK(10==cpu.step(&bus)); CHECK(0x2000 == cpu.HL);    
    CHECK(10==cpu.step(&bus)); CHECK(0x33 == cpu.mem.r8(0x2000));
    CHECK(10==cpu.step(&bus)); CHECK(0x1000 == cpu.HL);    
    CHECK(10==cpu.step(&bus)); CHECK(0x65 == cpu.mem.r8(0x1000));
}

// LD ([IX|IY] + d),n
TEST(LD_iIXY_d_n) {
    z80 cpu = init_z80();
    ubyte prog[] = {
        0xDD, 0x21, 0x00, 0x20,     // LD IX,0x2000
        0xDD, 0x36, 0x02, 0x33,     // LD (IX+2),0x33
        0xDD, 0x36, 0xFE, 0x11,     // LD (IX-2),0x11
        0xFD, 0x21, 0x00, 0x10,     // LD IY,0x1000
        0xFD, 0x36, 0x01, 0x22,     // LD (IY+1),0x22
        0xFD, 0x36, 0xFF, 0x44,     // LD (IY-1),0x44
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    CHECK(14==cpu.step(&bus)); CHECK(0x2000 == cpu.IX);    
    CHECK(19==cpu.step(&bus)); CHECK(0x33 == cpu.mem.r8(0x2002));
    CHECK(19==cpu.step(&bus)); CHECK(0x11 == cpu.mem.r8(0x1FFE));
    CHECK(14==cpu.step(&bus)); CHECK(0x1000 == cpu.IY);    
    CHECK(19==cpu.step(&bus)); CHECK(0x22 == cpu.mem.r8(0x1001));
    CHECK(19==cpu.step(&bus)); CHECK(0x44 == cpu.mem.r8(0x0FFF));
}

// LD A,(BC)
// LD A,(DE)
// LD A,(nn)
TEST(LD_A_iBCDEnn) {
    z80 cpu = init_z80();
    ubyte data[] = {
        0x11, 0x22, 0x33
    };
    cpu.mem.write(0x1000, data, sizeof(data));

    ubyte prog[] = {
        0x01, 0x00, 0x10,   // LD BC,0x1000
        0x11, 0x01, 0x10,   // LD DE,0x1001
        0x0A,               // LD A,(BC)
        0x1A,               // LD A,(DE)
        0x3A, 0x02, 0x10,   // LD A,(0x1002)
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    CHECK(10==cpu.step(&bus)); CHECK(0x1000 == cpu.BC);
    CHECK(10==cpu.step(&bus)); CHECK(0x1001 == cpu.DE);
    CHECK(7 ==cpu.step(&bus)); CHECK(0x11 == cpu.A);   
    CHECK(7 ==cpu.step(&bus)); CHECK(0x22 == cpu.A);   
    CHECK(13==cpu.step(&bus)); CHECK(0x33 == cpu.A);   
}

// LD (BC),A
// LD (DE),A
// LD (nn),A
TEST(LD_iBCDEnn_A) {
    z80 cpu = init_z80();
    ubyte prog[] = {
        0x01, 0x00, 0x10,   // LD BC,0x1000
        0x11, 0x01, 0x10,   // LD DE,0x1001
        0x3E, 0x77,         // LD A,0x77
        0x02,               // LD (BC),A
        0x12,               // LD (DE),A
        0x32, 0x02, 0x10,   // LD (0x1002),A
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    CHECK(10==cpu.step(&bus)); CHECK(0x1000 == cpu.BC);    
    CHECK(10==cpu.step(&bus)); CHECK(0x1001 == cpu.DE);    
    CHECK(7 ==cpu.step(&bus)); CHECK(0x77 == cpu.A);       
    CHECK(7 ==cpu.step(&bus)); CHECK(0x77 == cpu.mem.r8(0x1000));
    CHECK(7 ==cpu.step(&bus)); CHECK(0x77 == cpu.mem.r8(0x1001));
    CHECK(13==cpu.step(&bus)); CHECK(0x77 == cpu.mem.r8(0x1002));
}

// LD I,A
// LD R,A
TEST(LD_IR_A) {
    z80 cpu = init_z80();
    ubyte prog[] = {
        0x3E, 0x45,     // LD A,0x45
        0xED, 0x47,     // LD I,A
        0xED, 0x4F,     // LD R,A
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    CHECK(7==cpu.step(&bus)); CHECK(0x45 == cpu.A);
    CHECK(9==cpu.step(&bus)); CHECK(0x45 == cpu.I);
    CHECK(9==cpu.step(&bus)); CHECK(0x45 == cpu.R);
}

// LD dd,nn
// LD IX,nn
// LD IY,nn
TEST(LD_ddIXY_nn) {
    z80 cpu = init_z80();
    ubyte prog[] = {
        0x01, 0x34, 0x12,       // LD BC,0x1234
        0x11, 0x78, 0x56,       // LD DE,0x5678
        0x21, 0xBC, 0x9A,       // LD HL,0x9ABC
        0x31, 0x68, 0x13,       // LD SP,0x1368
        0xDD, 0x21, 0x21, 0x43, // LD IX,0x4321
        0xFD, 0x21, 0x65, 0x87, // LD IY,0x8765
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    CHECK(10==cpu.step(&bus)); CHECK(0x1234 == cpu.BC);
    CHECK(10==cpu.step(&bus)); CHECK(0x5678 == cpu.DE);
    CHECK(10==cpu.step(&bus)); CHECK(0x9ABC == cpu.HL);
    CHECK(10==cpu.step(&bus)); CHECK(0x1368 == cpu.SP);
    CHECK(14==cpu.step(&bus)); CHECK(0x4321 == cpu.IX);
    CHECK(14==cpu.step(&bus)); CHECK(0x8765 == cpu.IY);
}

// LD HL,(nn)
// LD dd,(nn)
// LD IX,(nn)
// LD IY,(nn)
TEST(LD_HLddIXY_inn) {
    z80 cpu = init_z80();
    ubyte data[] = {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
    };
    cpu.mem.write(0x1000, data, sizeof(data));

    ubyte prog[] = {
        0x2A, 0x00, 0x10,           // LD HL,(0x1000)
        0xED, 0x4B, 0x01, 0x10,     // LD BC,(0x1001)
        0xED, 0x5B, 0x02, 0x10,     // LD DE,(0x1002)
        0xED, 0x6B, 0x03, 0x10,     // LD HL,(0x1003) undocumented 'long' version
        0xED, 0x7B, 0x04, 0x10,     // LD SP,(0x1004)
        0xDD, 0x2A, 0x05, 0x10,     // LD IX,(0x1004)
        0xFD, 0x2A, 0x06, 0x10,     // LD IY,(0x1005)
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    CHECK(16==cpu.step(&bus)); CHECK(0x0201 == cpu.HL);
    CHECK(20==cpu.step(&bus)); CHECK(0x0302 == cpu.BC);
    CHECK(20==cpu.step(&bus)); CHECK(0x0403 == cpu.DE);
    CHECK(20==cpu.step(&bus)); CHECK(0x0504 == cpu.HL);
    CHECK(20==cpu.step(&bus)); CHECK(0x0605 == cpu.SP);
    CHECK(20==cpu.step(&bus)); CHECK(0x0706 == cpu.IX);
    CHECK(20==cpu.step(&bus)); CHECK(0x0807 == cpu.IY);
}

// LD (nn),HL
// LD (nn),dd
// LD (nn),IX
// LD (nn),IY
TEST(LD_inn_HLDDIXY) {
    z80 cpu = init_z80();
    ubyte prog[] = {
        0x21, 0x01, 0x02,           // LD HL,0x0201
        0x22, 0x00, 0x10,           // LD (0x1000),HL
        0x01, 0x34, 0x12,           // LD BC,0x1234
        0xED, 0x43, 0x02, 0x10,     // LD (0x1002),BC
        0x11, 0x78, 0x56,           // LD DE,0x5678
        0xED, 0x53, 0x04, 0x10,     // LD (0x1004),DE
        0x21, 0xBC, 0x9A,           // LD HL,0x9ABC
        0xED, 0x63, 0x06, 0x10,     // LD (0x1006),HL undocumented 'long' version
        0x31, 0x68, 0x13,           // LD SP,0x1368
        0xED, 0x73, 0x08, 0x10,     // LD (0x1008),SP
        0xDD, 0x21, 0x21, 0x43,     // LD IX,0x4321
        0xDD, 0x22, 0x0A, 0x10,     // LD (0x100A),IX
        0xFD, 0x21, 0x65, 0x87,     // LD IY,0x8765
        0xFD, 0x22, 0x0C, 0x10,     // LD (0x100C),IY
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    CHECK(10==cpu.step(&bus)); CHECK(0x0201 == cpu.HL);       
    CHECK(16==cpu.step(&bus)); CHECK(0x0201 == cpu.mem.r16(0x1000));
    CHECK(10==cpu.step(&bus)); CHECK(0x1234 == cpu.BC);       
    CHECK(20==cpu.step(&bus)); CHECK(0x1234 == cpu.mem.r16(0x1002));
    CHECK(10==cpu.step(&bus)); CHECK(0x5678 == cpu.DE);       
    CHECK(20==cpu.step(&bus)); CHECK(0x5678 == cpu.mem.r16(0x1004));
    CHECK(10==cpu.step(&bus)); CHECK(0x9ABC == cpu.HL);       
    CHECK(20==cpu.step(&bus)); CHECK(0x9ABC == cpu.mem.r16(0x1006));
    CHECK(10==cpu.step(&bus)); CHECK(0x1368 == cpu.SP);       
    CHECK(20==cpu.step(&bus)); CHECK(0x1368 == cpu.mem.r16(0x1008));
    CHECK(14==cpu.step(&bus)); CHECK(0x4321 == cpu.IX);       
    CHECK(20==cpu.step(&bus)); CHECK(0x4321 == cpu.mem.r16(0x100A));
    CHECK(14==cpu.step(&bus)); CHECK(0x8765 == cpu.IY);       
    CHECK(20==cpu.step(&bus)); CHECK(0x8765 == cpu.mem.r16(0x100C));
}

// LD SP,HL
// LD SP,IX
// LD SP,IY
TEST(LD_SP_HLIXY) {
    z80 cpu = init_z80();
    ubyte prog[] = {
        0x21, 0x34, 0x12,           // LD HL,0x1234
        0xDD, 0x21, 0x78, 0x56,     // LD IX,0x5678
        0xFD, 0x21, 0xBC, 0x9A,     // LD IY,0x9ABC
        0xF9,                       // LD SP,HL
        0xDD, 0xF9,                 // LD SP,IX
        0xFD, 0xF9,                 // LD SP,IY
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    CHECK(10==cpu.step(&bus)); CHECK(0x1234 == cpu.HL);
    CHECK(14==cpu.step(&bus)); CHECK(0x5678 == cpu.IX);
    CHECK(14==cpu.step(&bus)); CHECK(0x9ABC == cpu.IY);
    CHECK(6 ==cpu.step(&bus)); CHECK(0x1234 == cpu.SP);
    CHECK(10==cpu.step(&bus)); CHECK(0x5678 == cpu.SP);
    CHECK(10==cpu.step(&bus)); CHECK(0x9ABC == cpu.SP);
}

// PUSH qq
// PUSH IX
// PUSH IY
// POP qq
// POP IX
// POP IY
TEST(PUSH_POP_qqIXY) {
    z80 cpu = init_z80();
    ubyte prog[] = {
        0x01, 0x34, 0x12,       // LD BC,0x1234
        0x11, 0x78, 0x56,       // LD DE,0x5678
        0x21, 0xBC, 0x9A,       // LD HL,0x9ABC
        0x3E, 0xEF,             // LD A,0xEF
        0xDD, 0x21, 0x45, 0x23, // LD IX,0x2345
        0xFD, 0x21, 0x89, 0x67, // LD IY,0x6789
        0x31, 0x00, 0x01,       // LD SP,0x0100
        0xF5,                   // PUSH AF
        0xC5,                   // PUSH BC
        0xD5,                   // PUSH DE
        0xE5,                   // PUSH HL
        0xDD, 0xE5,             // PUSH IX
        0xFD, 0xE5,             // PUSH IY
        0xF1,                   // POP AF
        0xC1,                   // POP BC
        0xD1,                   // POP DE
        0xE1,                   // POP HL
        0xDD, 0xE1,             // POP IX
        0xFD, 0xE1,             // POP IY
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    CHECK(10==cpu.step(&bus)); CHECK(0x1234 == cpu.BC);
    CHECK(10==cpu.step(&bus)); CHECK(0x5678 == cpu.DE);
    CHECK(10==cpu.step(&bus)); CHECK(0x9ABC == cpu.HL);
    CHECK(7 ==cpu.step(&bus)); CHECK(0xEF00 == cpu.AF);
    CHECK(14==cpu.step(&bus)); CHECK(0x2345 == cpu.IX);
    CHECK(14==cpu.step(&bus)); CHECK(0x6789 == cpu.IY);
    CHECK(10==cpu.step(&bus)); CHECK(0x0100 == cpu.SP);
    CHECK(11==cpu.step(&bus)); CHECK(0xEF00 == cpu.mem.r16(0x00FE)); CHECK(0x00FE == cpu.SP);
    CHECK(11==cpu.step(&bus)); CHECK(0x1234 == cpu.mem.r16(0x00FC)); CHECK(0x00FC == cpu.SP);
    CHECK(11==cpu.step(&bus)); CHECK(0x5678 == cpu.mem.r16(0x00FA)); CHECK(0x00FA == cpu.SP);
    CHECK(11==cpu.step(&bus)); CHECK(0x9ABC == cpu.mem.r16(0x00F8)); CHECK(0x00F8 == cpu.SP);
    CHECK(15==cpu.step(&bus)); CHECK(0x2345 == cpu.mem.r16(0x00F6)); CHECK(0x00F6 == cpu.SP);
    CHECK(15==cpu.step(&bus)); CHECK(0x6789 == cpu.mem.r16(0x00F4)); CHECK(0x00F4 == cpu.SP);
    CHECK(10==cpu.step(&bus)); CHECK(0x6789 == cpu.AF); CHECK(0x00F6 == cpu.SP);
    CHECK(10==cpu.step(&bus)); CHECK(0x2345 == cpu.BC); CHECK(0x00F8 == cpu.SP);
    CHECK(10==cpu.step(&bus)); CHECK(0x9ABC == cpu.DE); CHECK(0x00FA == cpu.SP);
    CHECK(10==cpu.step(&bus)); CHECK(0x5678 == cpu.HL); CHECK(0x00FC == cpu.SP);
    CHECK(14==cpu.step(&bus)); CHECK(0x1234 == cpu.IX); CHECK(0x00FE == cpu.SP);
    CHECK(14==cpu.step(&bus)); CHECK(0xEF00 == cpu.IY); CHECK(0x0100 == cpu.SP);
}

// EX DE,HL
// EX AF,AF'
// EXX
// EX (SP),HL
// EX (SP),IX
// EX (SP),IY
TEST(EX) {
    z80 cpu = init_z80();
    ubyte prog[] = {
        0x21, 0x34, 0x12,       // LD HL,0x1234
        0x11, 0x78, 0x56,       // LD DE,0x5678
        0xEB,                   // EX DE,HL
        0x3E, 0x11,             // LD A,0x11
        0x08,                   // EX AF,AF'
        0x3E, 0x22,             // LD A,0x22
        0x08,                   // EX AF,AF'
        0x01, 0xBC, 0x9A,       // LD BC,0x9ABC
        0xD9,                   // EXX
        0x21, 0x11, 0x11,       // LD HL,0x1111
        0x11, 0x22, 0x22,       // LD DE,0x2222
        0x01, 0x33, 0x33,       // LD BC,0x3333
        0xD9,                   // EXX
        0x31, 0x00, 0x01,       // LD SP,0x0100
        0xD5,                   // PUSH DE
        0xE3,                   // EX (SP),HL
        0xDD, 0x21, 0x99, 0x88, // LD IX,0x8899
        0xDD, 0xE3,             // EX (SP),IX
        0xFD, 0x21, 0x77, 0x66, // LD IY,0x6677
        0xFD, 0xE3,             // EX (SP),IY
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    CHECK(10==cpu.step(&bus)); CHECK(0x1234 == cpu.HL);
    CHECK(10==cpu.step(&bus)); CHECK(0x5678 == cpu.DE);
    CHECK(4 ==cpu.step(&bus)); CHECK(0x1234 == cpu.DE); CHECK(0x5678 == cpu.HL); 
    CHECK(7 ==cpu.step(&bus)); CHECK(0x1100 == cpu.AF); CHECK(0x0000 == cpu.AF_);
    CHECK(4 ==cpu.step(&bus)); CHECK(0x0000 == cpu.AF); CHECK(0x1100 == cpu.AF_);
    CHECK(7 ==cpu.step(&bus)); CHECK(0x2200 == cpu.AF); CHECK(0x1100 == cpu.AF_);
    CHECK(4 ==cpu.step(&bus)); CHECK(0x1100 == cpu.AF); CHECK(0x2200 == cpu.AF_);
    CHECK(10==cpu.step(&bus)); CHECK(0x9ABC == cpu.BC);
    CHECK(4 ==cpu.step(&bus));
    CHECK(0x0000 == cpu.HL); CHECK(0x5678 == cpu.HL_);
    CHECK(0x0000 == cpu.DE); CHECK(0x1234 == cpu.DE_);
    CHECK(0x0000 == cpu.BC); CHECK(0x9ABC == cpu.BC_);
    CHECK(10==cpu.step(&bus)); CHECK(0x1111 == cpu.HL);
    CHECK(10==cpu.step(&bus)); CHECK(0x2222 == cpu.DE);
    CHECK(10==cpu.step(&bus)); CHECK(0x3333 == cpu.BC);
    CHECK(4 ==cpu.step(&bus));
    CHECK(0x5678 == cpu.HL); CHECK(0x1111 == cpu.HL_);
    CHECK(0x1234 == cpu.DE); CHECK(0x2222 == cpu.DE_);
    CHECK(0x9ABC == cpu.BC); CHECK(0x3333 == cpu.BC_);
    CHECK(10==cpu.step(&bus)); CHECK(0x0100 == cpu.SP);
    CHECK(11==cpu.step(&bus)); CHECK(0x1234 == cpu.mem.r16(0x00FE));
    CHECK(19==cpu.step(&bus)); CHECK(0x1234 == cpu.HL); CHECK(0x5678 == cpu.mem.r16(0x00FE));
    CHECK(14==cpu.step(&bus)); CHECK(0x8899 == cpu.IX);
    CHECK(23==cpu.step(&bus)); CHECK(0x5678 == cpu.IX); CHECK(0x8899 == cpu.mem.r16(0x00FE));
    CHECK(14==cpu.step(&bus)); CHECK(0x6677 == cpu.IY);
    CHECK(23==cpu.step(&bus)); CHECK(0x8899 == cpu.IY); CHECK(0x6677 == cpu.mem.r16(0x00FE));
}

// ADD A,r
// ADD A,n
TEST(ADD_A_r) {
    z80 cpu = init_z80();
    ubyte prog[] = {
        0x3E, 0x0F,     // LD A,0x0F
        0x87,           // ADD A,A
        0x06, 0xE0,     // LD B,0xE0
        0x80,           // ADD A,B
        0x3E, 0x81,     // LD A,0x81
        0x0E, 0x80,     // LD C,0x80
        0x81,           // ADD A,C
        0x16, 0xFF,     // LD D,0xFF
        0x82,           // ADD A,D
        0x1E, 0x40,     // LD E,0x40
        0x83,           // ADD A,E
        0x26, 0x80,     // LD H,0x80
        0x84,           // ADD A,H
        0x2E, 0x33,     // LD L,0x33
        0x85,           // ADD A,L
        0xC6, 0x44,     // ADD A,0x44

    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    CHECK(7==cpu.step(&bus)); CHECK(0x0F == cpu.A); CHECK(cpu.test_flags(0));      
    CHECK(4==cpu.step(&bus)); CHECK(0x1E == cpu.A); CHECK(cpu.test_flags(z80::HF));
    CHECK(7==cpu.step(&bus)); CHECK(0xE0 == cpu.B);                                
    CHECK(4==cpu.step(&bus)); CHECK(0xFE == cpu.A); CHECK(cpu.test_flags(z80::SF));
    CHECK(7==cpu.step(&bus)); CHECK(0x81 == cpu.A);                                
    CHECK(7==cpu.step(&bus)); CHECK(0x80 == cpu.C);                                
    CHECK(4==cpu.step(&bus)); CHECK(0x01 == cpu.A); CHECK(cpu.test_flags(z80::VF|z80::CF));
    CHECK(7==cpu.step(&bus)); CHECK(0xFF == cpu.D);
    CHECK(4==cpu.step(&bus)); CHECK(0x00 == cpu.A); CHECK(cpu.test_flags(z80::ZF|z80::HF|z80::CF));
    CHECK(7==cpu.step(&bus)); CHECK(0x40 == cpu.E);                                
    CHECK(4==cpu.step(&bus)); CHECK(0x40 == cpu.A); CHECK(cpu.test_flags(0));      
    CHECK(7==cpu.step(&bus)); CHECK(0x80 == cpu.H);                                
    CHECK(4==cpu.step(&bus)); CHECK(0xC0 == cpu.A); CHECK(cpu.test_flags(z80::SF));
    CHECK(7==cpu.step(&bus)); CHECK(0x33 == cpu.L);                                
    CHECK(4==cpu.step(&bus)); CHECK(0xF3 == cpu.A); CHECK(cpu.test_flags(z80::SF));
    CHECK(7==cpu.step(&bus)); CHECK(0x37 == cpu.A); CHECK(cpu.test_flags(z80::CF));
}

// ADD A,(HL)
// ADD A,(IX+d)
// ADD A,(IY+d)
TEST(ADD_a_iHLIXY_d) {
    z80 cpu = init_z80();
    ubyte data[] = { 0x41, 0x61, 0x81 };
    cpu.mem.write(0x1000, data, sizeof(data));

    ubyte prog[] = {
        0x21, 0x00, 0x10,       // LD HL,0x1000
        0xDD, 0x21, 0x00, 0x10, // LD IX,0x1000
        0xFD, 0x21, 0x03, 0x10, // LD IY,0x1003
        0x3E, 0x00,             // LD A,0x00
        0x86,                   // ADD A,(HL)
        0xDD, 0x86, 0x01,       // ADD A,(IX+1)
        0xFD, 0x86, 0xFF,       // ADD A,(IY-1)
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    CHECK(10==cpu.step(&bus)); CHECK(0x1000 == cpu.HL);
    CHECK(14==cpu.step(&bus)); CHECK(0x1000 == cpu.IX);
    CHECK(14==cpu.step(&bus)); CHECK(0x1003 == cpu.IY);
    CHECK(7 ==cpu.step(&bus)); CHECK(0x00 == cpu.A);
    CHECK(7 ==cpu.step(&bus)); CHECK(0x41 == cpu.A); CHECK(cpu.test_flags(0));
    CHECK(19==cpu.step(&bus)); CHECK(0xA2 == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::VF));
    CHECK(19==cpu.step(&bus)); CHECK(0x23 == cpu.A); CHECK(cpu.test_flags(z80::VF|z80::CF));
}

// ADC A,r
// ADC A,n
TEST(ADC_a_r) {
    z80 cpu = init_z80();
    ubyte prog[] = {
        0x3E, 0x00,         // LD A,0x00
        0x06, 0x41,         // LD B,0x41
        0x0E, 0x61,         // LD C,0x61
        0x16, 0x81,         // LD D,0x81
        0x1E, 0x41,         // LD E,0x41
        0x26, 0x61,         // LD H,0x61
        0x2E, 0x81,         // LD L,0x81
        0x8F,               // ADC A,A
        0x88,               // ADC A,B
        0x89,               // ADC A,C
        0x8A,               // ADC A,D
        0x8B,               // ADC A,E
        0x8C,               // ADC A,H
        0x8D,               // ADC A,L
        0xCE, 0x01,         // ADC A,0x01
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    CHECK(7==cpu.step(&bus)); CHECK(0x00 == cpu.A);
    CHECK(7==cpu.step(&bus)); CHECK(0x41 == cpu.B);
    CHECK(7==cpu.step(&bus)); CHECK(0x61 == cpu.C);
    CHECK(7==cpu.step(&bus)); CHECK(0x81 == cpu.D);
    CHECK(7==cpu.step(&bus)); CHECK(0x41 == cpu.E);
    CHECK(7==cpu.step(&bus)); CHECK(0x61 == cpu.H);
    CHECK(7==cpu.step(&bus)); CHECK(0x81 == cpu.L);
    CHECK(4==cpu.step(&bus)); CHECK(0x00 == cpu.A); CHECK(cpu.test_flags(z80::ZF));
    CHECK(4==cpu.step(&bus)); CHECK(0x41 == cpu.A); CHECK(cpu.test_flags(0));
    CHECK(4==cpu.step(&bus)); CHECK(0xA2 == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::VF));
    CHECK(4==cpu.step(&bus)); CHECK(0x23 == cpu.A); CHECK(cpu.test_flags(z80::VF|z80::CF));
    CHECK(4==cpu.step(&bus)); CHECK(0x65 == cpu.A); CHECK(cpu.test_flags(0));
    CHECK(4==cpu.step(&bus)); CHECK(0xC6 == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::VF));
    CHECK(4==cpu.step(&bus)); CHECK(0x47 == cpu.A); CHECK(cpu.test_flags(z80::VF|z80::CF));
    CHECK(7==cpu.step(&bus)); CHECK(0x49 == cpu.A); CHECK(cpu.test_flags(0));
}

// ADC A,(HL)
// ADC A,(IX+d)
// ADC A,(IY+d)
TEST(ADC_a_iHLIXY_d) {
    z80 cpu = init_z80();
    ubyte data[] = { 0x41, 0x61, 0x81, 0x2 };
    cpu.mem.write(0x1000, data, sizeof(data));

    ubyte prog[] = {
        0x21, 0x00, 0x10,       // LD HL,0x1000
        0xDD, 0x21, 0x00, 0x10, // LD IX,0x1000
        0xFD, 0x21, 0x03, 0x10, // LD IY,0x1003
        0x3E, 0x00,             // LD A,0x00
        0x86,                   // ADD A,(HL)
        0xDD, 0x8E, 0x01,       // ADC A,(IX+1)
        0xFD, 0x8E, 0xFF,       // ADC A,(IY-1)
        0xDD, 0x8E, 0x03,       // ADC A,(IX+3)
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    CHECK(10==cpu.step(&bus)); CHECK(0x1000 == cpu.HL);
    CHECK(14==cpu.step(&bus)); CHECK(0x1000 == cpu.IX);
    CHECK(14==cpu.step(&bus)); CHECK(0x1003 == cpu.IY);
    CHECK(7 ==cpu.step(&bus)); CHECK(0x00 == cpu.A);
    CHECK(7 ==cpu.step(&bus)); CHECK(0x41 == cpu.A); CHECK(cpu.test_flags(0));
    CHECK(19==cpu.step(&bus)); CHECK(0xA2 == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::VF));
    CHECK(19==cpu.step(&bus)); CHECK(0x23 == cpu.A); CHECK(cpu.test_flags(z80::VF|z80::CF));
    CHECK(19==cpu.step(&bus)); CHECK(0x26 == cpu.A); CHECK(cpu.test_flags(0));
}

// SUB A,r
// SUB A,n
TEST(SUB_A_r) {
    z80 cpu = init_z80();
    ubyte prog[] = {
        0x3E, 0x04,     // LD A,0x04
        0x06, 0x01,     // LD B,0x01
        0x0E, 0xF8,     // LD C,0xF8
        0x16, 0x0F,     // LD D,0x0F
        0x1E, 0x79,     // LD E,0x79
        0x26, 0xC0,     // LD H,0xC0
        0x2E, 0xBF,     // LD L,0xBF
        0x97,           // SUB A,A
        0x90,           // SUB A,B
        0x91,           // SUB A,C
        0x92,           // SUB A,D
        0x93,           // SUB A,E
        0x94,           // SUB A,H
        0x95,           // SUB A,L
        0xD6, 0x01,     // SUB A,0x01
        0xD6, 0xFE,     // SUB A,0xFE
    };
    cpu.mem.write(0x000, prog, sizeof(prog));

    CHECK(7==cpu.step(&bus)); CHECK(0x04 == cpu.A);
    CHECK(7==cpu.step(&bus)); CHECK(0x01 == cpu.B);
    CHECK(7==cpu.step(&bus)); CHECK(0xF8 == cpu.C);
    CHECK(7==cpu.step(&bus)); CHECK(0x0F == cpu.D);
    CHECK(7==cpu.step(&bus)); CHECK(0x79 == cpu.E);
    CHECK(7==cpu.step(&bus)); CHECK(0xC0 == cpu.H);
    CHECK(7==cpu.step(&bus)); CHECK(0xBF == cpu.L);
    CHECK(4==cpu.step(&bus)); CHECK(0x0 == cpu.A); CHECK(cpu.test_flags(z80::ZF|z80::NF));
    CHECK(4==cpu.step(&bus)); CHECK(0xFF == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::HF|z80::NF|z80::CF));
    CHECK(4==cpu.step(&bus)); CHECK(0x07 == cpu.A); CHECK(cpu.test_flags(z80::NF));
    CHECK(4==cpu.step(&bus)); CHECK(0xF8 == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::HF|z80::NF|z80::CF));
    CHECK(4==cpu.step(&bus)); CHECK(0x7F == cpu.A); CHECK(cpu.test_flags(z80::HF|z80::VF|z80::NF));
    CHECK(4==cpu.step(&bus)); CHECK(0xBF == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::VF|z80::NF|z80::CF));
    CHECK(4==cpu.step(&bus)); CHECK(0x00 == cpu.A); CHECK(cpu.test_flags(z80::ZF|z80::NF));
    CHECK(7==cpu.step(&bus)); CHECK(0xFF == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::HF|z80::NF|z80::CF));
    CHECK(7==cpu.step(&bus)); CHECK(0x01 == cpu.A); CHECK(cpu.test_flags(z80::NF));
}

// CP A,r
// CP A,n
TEST(CP_A_r) {
    z80 cpu = init_z80();
    ubyte prog[] = {
        0x3E, 0x04,     // LD A,0x04
        0x06, 0x05,     // LD B,0x05
        0x0E, 0x03,     // LD C,0x03
        0x16, 0xff,     // LD D,0xff
        0x1E, 0xaa,     // LD E,0xaa
        0x26, 0x80,     // LD H,0x80
        0x2E, 0x7f,     // LD L,0x7f
        0xBF,           // CP A
        0xB8,           // CP B
        0xB9,           // CP C
        0xBA,           // CP D
        0xBB,           // CP E
        0xBC,           // CP H
        0xBD,           // CP L
        0xFE, 0x04,     // CP 0x04
    };
    cpu.mem.write(0x000, prog, sizeof(prog));

    CHECK(7==cpu.step(&bus)); CHECK(0x04 == cpu.A);
    CHECK(7==cpu.step(&bus)); CHECK(0x05 == cpu.B);
    CHECK(7==cpu.step(&bus)); CHECK(0x03 == cpu.C);
    CHECK(7==cpu.step(&bus)); CHECK(0xff == cpu.D);
    CHECK(7==cpu.step(&bus)); CHECK(0xaa == cpu.E);
    CHECK(7==cpu.step(&bus)); CHECK(0x80 == cpu.H);
    CHECK(7==cpu.step(&bus)); CHECK(0x7f == cpu.L);
    CHECK(4==cpu.step(&bus)); CHECK(0x04 == cpu.A); CHECK(cpu.test_flags(z80::ZF|z80::NF));
    CHECK(4==cpu.step(&bus)); CHECK(0x04 == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::HF|z80::NF|z80::CF)); 
    CHECK(4==cpu.step(&bus)); CHECK(0x04 == cpu.A); CHECK(cpu.test_flags(z80::NF));
    CHECK(4==cpu.step(&bus)); CHECK(0x04 == cpu.A); CHECK(cpu.test_flags(z80::HF|z80::NF|z80::CF));
    CHECK(4==cpu.step(&bus)); CHECK(0x04 == cpu.A); CHECK(cpu.test_flags(z80::HF|z80::NF|z80::CF));
    CHECK(4==cpu.step(&bus)); CHECK(0x04 == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::VF|z80::NF|z80::CF));
    CHECK(4==cpu.step(&bus)); CHECK(0x04 == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::HF|z80::NF|z80::CF));
    CHECK(7==cpu.step(&bus)); CHECK(0x04 == cpu.A); CHECK(cpu.test_flags(z80::ZF|z80::NF));
}

// SUB A,(HL)
// SUB A,(IX+d)
// SUB A,(IY+d)
TEST(SUB_a_iHLIXY_d) {
    z80 cpu = init_z80();
    ubyte data[] = { 0x41, 0x61, 0x81 };
    cpu.mem.write(0x1000, data, sizeof(data));

    ubyte prog[] = {
        0x21, 0x00, 0x10,       // LD HL,0x1000
        0xDD, 0x21, 0x00, 0x10, // LD IX,0x1000
        0xFD, 0x21, 0x03, 0x10, // LD IY,0x1003
        0x3E, 0x00,             // LD A,0x00
        0x96,                   // SUB A,(HL)
        0xDD, 0x96, 0x01,       // SUB A,(IX+1)
        0xFD, 0x96, 0xFE,       // SUB A,(IY-2)
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    CHECK(10==cpu.step(&bus)); CHECK(0x1000 == cpu.HL);
    CHECK(14==cpu.step(&bus)); CHECK(0x1000 == cpu.IX);
    CHECK(14==cpu.step(&bus)); CHECK(0x1003 == cpu.IY);
    CHECK(7 ==cpu.step(&bus)); CHECK(0x00 == cpu.A);
    CHECK(7 ==cpu.step(&bus)); CHECK(0xBF == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::HF|z80::NF|z80::CF));
    CHECK(19==cpu.step(&bus)); CHECK(0x5E == cpu.A); CHECK(cpu.test_flags(z80::VF|z80::NF));
    CHECK(19==cpu.step(&bus)); CHECK(0xFD == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::NF|z80::CF));
}

// SUB A,(HL)
// SUB A,(IX+d)
// SUB A,(IY+d)
TEST(CP_a_iHLIXY_d) {
    z80 cpu = init_z80();
    ubyte data[] = { 0x41, 0x61, 0x22 };
    cpu.mem.write(0x1000, data, sizeof(data));

    ubyte prog[] = {
        0x21, 0x00, 0x10,       // LD HL,0x1000
        0xDD, 0x21, 0x00, 0x10, // LD IX,0x1000
        0xFD, 0x21, 0x03, 0x10, // LD IY,0x1003
        0x3E, 0x41,             // LD A,0x41
        0xBE,                   // CP (HL)
        0xDD, 0xBE, 0x01,       // CP (IX+1)
        0xFD, 0xBE, 0xFF,       // CP (IY-1)
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    CHECK(10==cpu.step(&bus)); CHECK(0x1000 == cpu.HL);
    CHECK(14==cpu.step(&bus)); CHECK(0x1000 == cpu.IX);
    CHECK(14==cpu.step(&bus)); CHECK(0x1003 == cpu.IY);
    CHECK(7 ==cpu.step(&bus)); CHECK(0x41 == cpu.A);
    CHECK(7 ==cpu.step(&bus)); CHECK(0x41 == cpu.A); CHECK(cpu.test_flags(z80::ZF|z80::NF));
    CHECK(19==cpu.step(&bus)); CHECK(0x41 == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::NF|z80::CF));
    CHECK(19==cpu.step(&bus)); CHECK(0x41 == cpu.A); CHECK(cpu.test_flags(z80::HF|z80::NF));
}

// SBC A,r
// SBC A,n
TEST(SBC_A_r) {
    z80 cpu = init_z80();
    ubyte prog[] = {
        0x3E, 0x04,     // LD A,0x04
        0x06, 0x01,     // LD B,0x01
        0x0E, 0xF8,     // LD C,0xF8
        0x16, 0x0F,     // LD D,0x0F
        0x1E, 0x79,     // LD E,0x79
        0x26, 0xC0,     // LD H,0xC0
        0x2E, 0xBF,     // LD L,0xBF
        0x97,           // SUB A,A
        0x98,           // SBC A,B
        0x99,           // SBC A,C
        0x9A,           // SBC A,D
        0x9B,           // SBC A,E
        0x9C,           // SBC A,H
        0x9D,           // SBC A,L
        0xDE, 0x01,     // SBC A,0x01
        0xDE, 0xFE,     // SBC A,0xFE
    };
    cpu.mem.write(0x000, prog, sizeof(prog));

    // skip over LDs
    for (int i = 0; i < 7; i++) {
        cpu.step(&bus);
    }
    CHECK(4==cpu.step(&bus)); CHECK(0x0 == cpu.A); CHECK(cpu.test_flags(z80::ZF|z80::NF));
    CHECK(4==cpu.step(&bus)); CHECK(0xFF == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::HF|z80::NF|z80::CF));
    CHECK(4==cpu.step(&bus)); CHECK(0x06 == cpu.A); CHECK(cpu.test_flags(z80::NF));
    CHECK(4==cpu.step(&bus)); CHECK(0xF7 == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::HF|z80::NF|z80::CF));
    CHECK(4==cpu.step(&bus)); CHECK(0x7D == cpu.A); CHECK(cpu.test_flags(z80::HF|z80::VF|z80::NF));
    CHECK(4==cpu.step(&bus)); CHECK(0xBD == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::VF|z80::NF|z80::CF));
    CHECK(4==cpu.step(&bus)); CHECK(0xFD == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::HF|z80::NF|z80::CF));
    CHECK(7==cpu.step(&bus)); CHECK(0xFB == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::NF));
    CHECK(7==cpu.step(&bus)); CHECK(0xFD == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::HF|z80::NF|z80::CF));
}

// SBC A,(HL)
// SBC A,(IX+d)
// SBC A,(IY+d)
TEST(SBC_a_iHLIXY_d) {
    z80 cpu = init_z80();

    ubyte data[] = { 0x41, 0x61, 0x81 };
    cpu.mem.write(0x1000, data, sizeof(data));

    ubyte prog[] = {
        0x21, 0x00, 0x10,       // LD HL,0x1000
        0xDD, 0x21, 0x00, 0x10, // LD IX,0x1000
        0xFD, 0x21, 0x03, 0x10, // LD IY,0x1003
        0x3E, 0x00,             // LD A,0x00
        0x9E,                   // SBC A,(HL)
        0xDD, 0x9E, 0x01,       // SBC A,(IX+1)
        0xFD, 0x9E, 0xFE,       // SBC A,(IY-2)
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    CHECK(10==cpu.step(&bus)); CHECK(0x1000 == cpu.HL);
    CHECK(14==cpu.step(&bus)); CHECK(0x1000 == cpu.IX);
    CHECK(14==cpu.step(&bus)); CHECK(0x1003 == cpu.IY);
    CHECK(7 ==cpu.step(&bus)); CHECK(0x00 == cpu.A);
    CHECK(7 ==cpu.step(&bus)); CHECK(0xBF == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::HF|z80::NF|z80::CF));
    CHECK(19==cpu.step(&bus)); CHECK(0x5D == cpu.A); CHECK(cpu.test_flags(z80::VF|z80::NF));
    CHECK(19==cpu.step(&bus)); CHECK(0xFC == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::NF|z80::CF));
}

// OR r
// OR n
TEST(OR_r) {
    z80 cpu = init_z80();

    ubyte prog[] = {
        0x97,           // SUB A
        0x06, 0x01,     // LD B,0x01
        0x0E, 0x02,     // LD C,0x02
        0x16, 0x04,     // LD D,0x04
        0x1E, 0x08,     // LD E,0x08
        0x26, 0x10,     // LD H,0x10
        0x2E, 0x20,     // LD L,0x20
        0xB7,           // OR A
        0xB0,           // OR B
        0xB1,           // OR C
        0xB2,           // OR D
        0xB3,           // OR E
        0xB4,           // OR H
        0xB5,           // OR L
        0xF6, 0x40,     // OR 0x40
        0xF6, 0x80,     // OR 0x80
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    // skip loads
    for (int i = 0; i < 7; i++) {
        cpu.step(&bus);
    }
    CHECK(4==cpu.step(&bus)); CHECK(0x00 == cpu.A); CHECK(cpu.test_flags(z80::ZF|z80::PF));
    CHECK(4==cpu.step(&bus)); CHECK(0x01 == cpu.A); CHECK(cpu.test_flags(0));
    CHECK(4==cpu.step(&bus)); CHECK(0x03 == cpu.A); CHECK(cpu.test_flags(z80::PF));
    CHECK(4==cpu.step(&bus)); CHECK(0x07 == cpu.A); CHECK(cpu.test_flags(0));
    CHECK(4==cpu.step(&bus)); CHECK(0x0F == cpu.A); CHECK(cpu.test_flags(z80::PF));
    CHECK(4==cpu.step(&bus)); CHECK(0x1F == cpu.A); CHECK(cpu.test_flags(0));
    CHECK(4==cpu.step(&bus)); CHECK(0x3F == cpu.A); CHECK(cpu.test_flags(z80::PF));
    CHECK(7==cpu.step(&bus)); CHECK(0x7F == cpu.A); CHECK(cpu.test_flags(0));
    CHECK(7==cpu.step(&bus)); CHECK(0xFF == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::PF));
}

// XOR r
// XOR n
TEST(XOR_r) {
    z80 cpu = init_z80();

    ubyte prog[] = {
        0x97,           // SUB A
        0x06, 0x01,     // LD B,0x01
        0x0E, 0x03,     // LD C,0x03
        0x16, 0x07,     // LD D,0x07
        0x1E, 0x0F,     // LD E,0x0F
        0x26, 0x1F,     // LD H,0x1F
        0x2E, 0x3F,     // LD L,0x3F
        0xAF,           // XOR A
        0xA8,           // XOR B
        0xA9,           // XOR C
        0xAA,           // XOR D
        0xAB,           // XOR E
        0xAC,           // XOR H
        0xAD,           // XOR L
        0xEE, 0x7F,     // XOR 0x7F
        0xEE, 0xFF,     // XOR 0xFF
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    // skip loads
    for (int i = 0; i < 7; i++) {
        cpu.step(&bus);
    }
    CHECK(4==cpu.step(&bus)); CHECK(0x00 == cpu.A); CHECK(cpu.test_flags(z80::ZF|z80::PF));
    CHECK(4==cpu.step(&bus)); CHECK(0x01 == cpu.A); CHECK(cpu.test_flags(0));
    CHECK(4==cpu.step(&bus)); CHECK(0x02 == cpu.A); CHECK(cpu.test_flags(0));
    CHECK(4==cpu.step(&bus)); CHECK(0x05 == cpu.A); CHECK(cpu.test_flags(z80::PF));
    CHECK(4==cpu.step(&bus)); CHECK(0x0A == cpu.A); CHECK(cpu.test_flags(z80::PF));
    CHECK(4==cpu.step(&bus)); CHECK(0x15 == cpu.A); CHECK(cpu.test_flags(0));
    CHECK(4==cpu.step(&bus)); CHECK(0x2A == cpu.A); CHECK(cpu.test_flags(0));
    CHECK(7==cpu.step(&bus)); CHECK(0x55 == cpu.A); CHECK(cpu.test_flags(z80::PF));
    CHECK(7==cpu.step(&bus)); CHECK(0xAA == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::PF));
}

// OR (HL)
// OR (IX+d)
// OR (IY+d)
// XOR (HL)
// XOR (IX+d)
// XOR (IY+d)
TEST(OR_XOR_iHLIXY_d) {
    z80 cpu = init_z80();

    ubyte data[] = { 0x41, 0x62, 0x84 };
    cpu.mem.write(0x1000, data, sizeof(data));
    ubyte prog[] = {
        0x21, 0x00, 0x10,           // LD HL,0x1000
        0xDD, 0x21, 0x00, 0x10,     // LD IX,0x1000
        0xFD, 0x21, 0x03, 0x10,     // LD IY,0x1003
        0xB6,                       // OR (HL)
        0xDD, 0xB6, 0x01,           // OR (IX+1)
        0xFD, 0xB6, 0xFF,           // OR (IY-1)
        0xAE,                       // XOR (HL)
        0xDD, 0xAE, 0x01,           // XOR (IX+1)
        0xFD, 0xAE, 0xFF,           // XOR (IY-1)
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    // skip loads
    cpu.step(&bus); cpu.step(&bus); cpu.step(&bus);
    CHECK(7 ==cpu.step(&bus)); CHECK(0x41 == cpu.A); CHECK(cpu.test_flags(z80::PF));
    CHECK(19==cpu.step(&bus)); CHECK(0x63 == cpu.A); CHECK(cpu.test_flags(z80::PF));
    CHECK(19==cpu.step(&bus)); CHECK(0xE7 == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::PF));
    CHECK(7 ==cpu.step(&bus)); CHECK(0xA6 == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::PF));
    CHECK(19==cpu.step(&bus)); CHECK(0xC4 == cpu.A); CHECK(cpu.test_flags(z80::SF));
    CHECK(19==cpu.step(&bus)); CHECK(0x40 == cpu.A); CHECK(cpu.test_flags(0));
}

// AND r
// AND n
TEST(AND_rn) {
    z80 cpu = init_z80();

    ubyte prog[] = {
        0x3E, 0xFF,             // LD A,0xFF
        0x06, 0x01,             // LD B,0x01
        0x0E, 0x03,             // LD C,0x02
        0x16, 0x04,             // LD D,0x04
        0x1E, 0x08,             // LD E,0x08
        0x26, 0x10,             // LD H,0x10
        0x2E, 0x20,             // LD L,0x20
        0xA0,                   // AND B
        0xF6, 0xFF,             // OR 0xFF
        0xA1,                   // AND C
        0xF6, 0xFF,             // OR 0xFF
        0xA2,                   // AND D
        0xF6, 0xFF,             // OR 0xFF
        0xA3,                   // AND E
        0xF6, 0xFF,             // OR 0xFF
        0xA4,                   // AND H
        0xF6, 0xFF,             // OR 0xFF
        0xA5,                   // AND L
        0xF6, 0xFF,             // OR 0xFF
        0xE6, 0x40,             // AND 0x40
        0xF6, 0xFF,             // OR 0xFF
        0xE6, 0xAA,             // AND 0xAA
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    // skip loads
    for (int i = 0; i < 7; i++) {
        cpu.step(&bus);
    }
    CHECK(4==cpu.step(&bus)); CHECK(0x01 == cpu.A); CHECK(cpu.test_flags(z80::HF));
    CHECK(7==cpu.step(&bus)); CHECK(0xFF == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::PF));
    CHECK(4==cpu.step(&bus)); CHECK(0x03 == cpu.A); CHECK(cpu.test_flags(z80::HF|z80::PF));
    CHECK(7==cpu.step(&bus)); CHECK(0xFF == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::PF));
    CHECK(4==cpu.step(&bus)); CHECK(0x04 == cpu.A); CHECK(cpu.test_flags(z80::HF));
    CHECK(7==cpu.step(&bus)); CHECK(0xFF == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::PF));
    CHECK(4==cpu.step(&bus)); CHECK(0x08 == cpu.A); CHECK(cpu.test_flags(z80::HF));
    CHECK(7==cpu.step(&bus)); CHECK(0xFF == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::PF));
    CHECK(4==cpu.step(&bus)); CHECK(0x10 == cpu.A); CHECK(cpu.test_flags(z80::HF));
    CHECK(7==cpu.step(&bus)); CHECK(0xFF == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::PF));
    CHECK(4==cpu.step(&bus)); CHECK(0x20 == cpu.A); CHECK(cpu.test_flags(z80::HF));
    CHECK(7==cpu.step(&bus)); CHECK(0xFF == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::PF));
    CHECK(7==cpu.step(&bus)); CHECK(0x40 == cpu.A); CHECK(cpu.test_flags(z80::HF));
    CHECK(7==cpu.step(&bus)); CHECK(0xFF == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::PF));
    CHECK(7==cpu.step(&bus)); CHECK(0xAA == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::HF|z80::PF));
}

// AND (HL)
// AND (IX+d)
// AND (IY+d)
TEST(AND_iHLIXY_d) {
    z80 cpu = init_z80();

    ubyte data[] = { 0xFE, 0xAA, 0x99 };
    cpu.mem.write(0x1000, data, sizeof(data));
    ubyte prog[] = {
        0x21, 0x00, 0x10,           // LD HL,0x1000
        0xDD, 0x21, 0x00, 0x10,     // LD IX,0x1000
        0xFD, 0x21, 0x03, 0x10,     // LD IY,0x1003
        0x3E, 0xFF,                 // LD A,0xFF
        0xA6,                       // AND (HL)
        0xDD, 0xA6, 0x01,           // AND (IX+1)
        0xFD, 0xA6, 0xFF,           // AND (IX-1)
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    // skip loads
    cpu.step(&bus); cpu.step(&bus); cpu.step(&bus); cpu.step(&bus);
    CHECK(7 ==cpu.step(&bus)); CHECK(0xFE == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::HF));
    CHECK(19==cpu.step(&bus)); CHECK(0xAA == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::HF|z80::PF));
    CHECK(19==cpu.step(&bus)); CHECK(0x88 == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::HF|z80::PF));
}

// INC r
// DEC r
TEST(INC_DEC_r) {
    z80 cpu = init_z80();

    ubyte prog[] = {
        0x3e, 0x00,         // LD A,0x00
        0x06, 0xFF,         // LD B,0xFF
        0x0e, 0x0F,         // LD C,0x0F
        0x16, 0x0E,         // LD D,0x0E
        0x1E, 0x7F,         // LD E,0x7F
        0x26, 0x3E,         // LD H,0x3E
        0x2E, 0x23,         // LD L,0x23
        0x3C,               // INC A
        0x3D,               // DEC A
        0x04,               // INC B
        0x05,               // DEC B
        0x0C,               // INC C
        0x0D,               // DEC C
        0x14,               // INC D
        0x15,               // DEC D
        0xFE, 0x01,         // CP 0x01  // set carry flag (should be preserved)
        0x1C,               // INC E
        0x1D,               // DEC E
        0x24,               // INC H
        0x25,               // DEC H
        0x2C,               // INC L
        0x2D,               // DEC L
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    // skip loads
    for (int i = 0; i < 7; i++) {
        cpu.step(&bus);
    }
    CHECK(4==cpu.step(&bus)); CHECK(0x01 == cpu.A); CHECK(cpu.test_flags(0));
    CHECK(4==cpu.step(&bus)); CHECK(0x00 == cpu.A); CHECK(cpu.test_flags(z80::ZF|z80::NF));
    CHECK(4==cpu.step(&bus)); CHECK(0x00 == cpu.B); CHECK(cpu.test_flags(z80::ZF|z80::HF));
    CHECK(4==cpu.step(&bus)); CHECK(0xFF == cpu.B); CHECK(cpu.test_flags(z80::SF|z80::HF|z80::NF));
    CHECK(4==cpu.step(&bus)); CHECK(0x10 == cpu.C); CHECK(cpu.test_flags(z80::HF));
    CHECK(4==cpu.step(&bus)); CHECK(0x0F == cpu.C); CHECK(cpu.test_flags(z80::HF|z80::NF));
    CHECK(4==cpu.step(&bus)); CHECK(0x0F == cpu.D); CHECK(cpu.test_flags(0));
    CHECK(4==cpu.step(&bus)); CHECK(0x0E == cpu.D); CHECK(cpu.test_flags(z80::NF));
    CHECK(7==cpu.step(&bus)); CHECK(0x00 == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::HF|z80::NF|z80::CF));
    CHECK(4==cpu.step(&bus)); CHECK(0x80 == cpu.E); CHECK(cpu.test_flags(z80::SF|z80::HF|z80::VF|z80::CF));
    CHECK(4==cpu.step(&bus)); CHECK(0x7F == cpu.E); CHECK(cpu.test_flags(z80::HF|z80::VF|z80::NF|z80::CF));
    CHECK(4==cpu.step(&bus)); CHECK(0x3F == cpu.H); CHECK(cpu.test_flags(z80::CF));
    CHECK(4==cpu.step(&bus)); CHECK(0x3E == cpu.H); CHECK(cpu.test_flags(z80::NF|z80::CF));
    CHECK(4==cpu.step(&bus)); CHECK(0x24 == cpu.L); CHECK(cpu.test_flags(z80::CF));
    CHECK(4==cpu.step(&bus)); CHECK(0x23 == cpu.L); CHECK(cpu.test_flags(z80::NF|z80::CF));
}

// INC (HL)
// INC (IX+d)
// INC (IY+d)
// DEC (HL)
// DEC (IX+d)
// DEC (IY+d)
TEST(INC_DEC_iHLIXIY_d) {
    z80 cpu = init_z80();

    ubyte data[] = {
        0x00, 0x3F, 0x7F
    };
    cpu.mem.write(0x1000, data, sizeof(data));

    ubyte prog[] = {
        0x21, 0x00, 0x10,           // LD HL,0x1000
        0xDD, 0x21, 0x00, 0x10,     // LD IX,0x1000
        0xFD, 0x21, 0x03, 0x10,     // LD IY,0x1003
        0x35,                       // DEC (HL)
        0x34,                       // INC (HL)
        0xDD, 0x34, 0x01,           // INC (IX+1)
        0xDD, 0x35, 0x01,           // DEC (IX+1)
        0xFD, 0x34, 0xFF,           // INC (IY-1)
        0xFD, 0x35, 0xFF,           // DEC (IY-1)
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    // skip loads
    for (int i = 0; i < 3; i++) {
        cpu.step(&bus);
    }
    CHECK(11==cpu.step(&bus)); CHECK(0xFF == cpu.mem.r8(0x1000)); CHECK(cpu.test_flags(z80::SF|z80::HF|z80::NF));
    CHECK(11==cpu.step(&bus)); CHECK(0x00 == cpu.mem.r8(0x1000)); CHECK(cpu.test_flags(z80::ZF|z80::HF));
    CHECK(23==cpu.step(&bus)); CHECK(0x40 == cpu.mem.r8(0x1001)); CHECK(cpu.test_flags(z80::HF));
    CHECK(23==cpu.step(&bus)); CHECK(0x3F == cpu.mem.r8(0x1001)); CHECK(cpu.test_flags(z80::HF|z80::NF));
    CHECK(23==cpu.step(&bus)); CHECK(0x80 == cpu.mem.r8(0x1002)); CHECK(cpu.test_flags(z80::SF|z80::HF|z80::VF));
    CHECK(23==cpu.step(&bus)); CHECK(0x7F == cpu.mem.r8(0x1002)); CHECK(cpu.test_flags(z80::HF|z80::PF|z80::NF));
}

// INC ss
// DEC ss
// INC IX
// DEC IX
// INC IY
// DEC IY
TEST(INC_DEC_ss_IX_IY) {
    z80 cpu = init_z80();

    ubyte prog[] = {
        0x01, 0x00, 0x00,       // LD BC,0x0000
        0x11, 0xFF, 0xFF,       // LD DE,0xffff
        0x21, 0xFF, 0x00,       // LD HL,0x00ff
        0x31, 0x11, 0x11,       // LD SP,0x1111
        0xDD, 0x21, 0xFF, 0x0F, // LD IX,0x0fff
        0xFD, 0x21, 0x34, 0x12, // LD IY,0x1234
        0x0B,                   // DEC BC
        0x03,                   // INC BC
        0x13,                   // INC DE
        0x1B,                   // DEC DE
        0x23,                   // INC HL
        0x2B,                   // DEC HL
        0x33,                   // INC SP
        0x3B,                   // DEC SP
        0xDD, 0x23,             // INC IX
        0xDD, 0x2B,             // DEC IX
        0xFD, 0x23,             // INC IX
        0xFD, 0x2B,             // DEC IX
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    for (int i = 0; i < 6; i++) {
        cpu.step(&bus);
    }

    CHECK(6==cpu.step(&bus)); CHECK(0xFFFF == cpu.BC);
    CHECK(6==cpu.step(&bus)); CHECK(0x0000 == cpu.BC);
    CHECK(6==cpu.step(&bus)); CHECK(0x0000 == cpu.DE);
    CHECK(6==cpu.step(&bus)); CHECK(0xFFFF == cpu.DE);
    CHECK(6==cpu.step(&bus)); CHECK(0x0100 == cpu.HL);
    CHECK(6==cpu.step(&bus)); CHECK(0x00FF == cpu.HL);
    CHECK(6==cpu.step(&bus)); CHECK(0x1112 == cpu.SP);
    CHECK(6==cpu.step(&bus)); CHECK(0x1111 == cpu.SP);
    CHECK(10==cpu.step(&bus)); CHECK(0x1000 == cpu.IX);
    CHECK(10==cpu.step(&bus)); CHECK(0x0FFF == cpu.IX);
    CHECK(10==cpu.step(&bus)); CHECK(0x1235 == cpu.IY);
    CHECK(10==cpu.step(&bus)); CHECK(0x1234 == cpu.IY);
}

TEST(RLCA_RLA_RRCA_RRA) {
    z80 cpu = init_z80();

    ubyte prog[] = {
        0x3E, 0xA0,     // LD A,0xA0
        0x07,           // RLCA
        0x07,           // RLCA
        0x0F,           // RRCA
        0x0F,           // RRCA
        0x17,           // RLA
        0x17,           // RLA
        0x1F,           // RRA
        0x1F,           // RRA
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    cpu.F = 0xff;
    CHECK(7==cpu.step(&bus)); CHECK(0xA0 == cpu.A);
    CHECK(4==cpu.step(&bus)); CHECK(0x41 == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::ZF|z80::VF|z80::CF));
    CHECK(4==cpu.step(&bus)); CHECK(0x82 == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::ZF|z80::VF));
    CHECK(4==cpu.step(&bus)); CHECK(0x41 == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::ZF|z80::VF));
    CHECK(4==cpu.step(&bus)); CHECK(0xA0 == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::ZF|z80::VF|z80::CF));
    CHECK(4==cpu.step(&bus)); CHECK(0x41 == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::ZF|z80::VF|z80::CF));
    CHECK(4==cpu.step(&bus)); CHECK(0x83 == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::ZF|z80::VF));
    CHECK(4==cpu.step(&bus)); CHECK(0x41 == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::ZF|z80::VF|z80::CF));
    CHECK(4==cpu.step(&bus)); CHECK(0xA0 == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::ZF|z80::VF|z80::CF));
}

TEST(RLC_RL_RRC_RR_r) {
    z80 cpu = init_z80();
    ubyte prog[] = {
        0x3E, 0x01,     // LD A,0x01
        0x06, 0xFF,     // LD B,0xFF
        0x0E, 0x03,     // LD C,0x03
        0x16, 0xFE,     // LD D,0xFE
        0x1E, 0x11,     // LD E,0x11
        0x26, 0x3F,     // LD H,0x3F
        0x2E, 0x70,     // LD L,0x70

        0xCB, 0x0F,     // RRC A
        0xCB, 0x07,     // RLC A
        0xCB, 0x08,     // RRC B
        0xCB, 0x00,     // RLC B
        0xCB, 0x01,     // RLC C
        0xCB, 0x09,     // RRC C
        0xCB, 0x02,     // RLC D
        0xCB, 0x0A,     // RRC D
        0xCB, 0x0B,     // RRC E
        0xCB, 0x03,     // RLC E
        0xCB, 0x04,     // RLC H
        0xCB, 0x0C,     // RCC H
        0xCB, 0x05,     // RLC L
        0xCB, 0x0D,     // RRC L

        0xCB, 0x1F,     // RR A
        0xCB, 0x17,     // RL A
        0xCB, 0x18,     // RR B
        0xCB, 0x10,     // RL B
        0xCB, 0x11,     // RL C
        0xCB, 0x19,     // RR C
        0xCB, 0x12,     // RL D
        0xCB, 0x1A,     // RR D
        0xCB, 0x1B,     // RR E
        0xCB, 0x13,     // RL E
        0xCB, 0x14,     // RL H
        0xCB, 0x1C,     // RR H
        0xCB, 0x15,     // RL L
        0xCB, 0x1D,     // RR L
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    // skip loads
    for (int i = 0; i < 7; i++) {
        cpu.step(&bus);
    }
    CHECK(8==cpu.step(&bus)); CHECK(0x80 == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::CF));
    CHECK(8==cpu.step(&bus)); CHECK(0x01 == cpu.A); CHECK(cpu.test_flags(z80::CF));
    CHECK(8==cpu.step(&bus)); CHECK(0xFF == cpu.B); CHECK(cpu.test_flags(z80::SF|z80::PF|z80::CF));
    CHECK(8==cpu.step(&bus)); CHECK(0xFF == cpu.B); CHECK(cpu.test_flags(z80::SF|z80::PF|z80::CF));
    CHECK(8==cpu.step(&bus)); CHECK(0x06 == cpu.C); CHECK(cpu.test_flags(z80::PF));
    CHECK(8==cpu.step(&bus)); CHECK(0x03 == cpu.C); CHECK(cpu.test_flags(z80::PF));
    CHECK(8==cpu.step(&bus)); CHECK(0xFD == cpu.D); CHECK(cpu.test_flags(z80::SF|z80::CF));
    CHECK(8==cpu.step(&bus)); CHECK(0xFE == cpu.D); CHECK(cpu.test_flags(z80::SF|z80::CF));
    CHECK(8==cpu.step(&bus)); CHECK(0x88 == cpu.E); CHECK(cpu.test_flags(z80::SF|z80::PF|z80::CF));
    CHECK(8==cpu.step(&bus)); CHECK(0x11 == cpu.E); CHECK(cpu.test_flags(z80::PF|z80::CF));
    CHECK(8==cpu.step(&bus)); CHECK(0x7E == cpu.H); CHECK(cpu.test_flags(z80::PF));
    CHECK(8==cpu.step(&bus)); CHECK(0x3F == cpu.H); CHECK(cpu.test_flags(z80::PF));
    CHECK(8==cpu.step(&bus)); CHECK(0xE0 == cpu.L); CHECK(cpu.test_flags(z80::SF));
    CHECK(8==cpu.step(&bus)); CHECK(0x70 == cpu.L); CHECK(cpu.test_flags(0));
    CHECK(8==cpu.step(&bus)); CHECK(0x00 == cpu.A); CHECK(cpu.test_flags(z80::ZF|z80::PF|z80::CF));
    CHECK(8==cpu.step(&bus)); CHECK(0x01 == cpu.A); CHECK(cpu.test_flags(0));
    CHECK(8==cpu.step(&bus)); CHECK(0x7F == cpu.B); CHECK(cpu.test_flags(z80::CF));
    CHECK(8==cpu.step(&bus)); CHECK(0xFF == cpu.B); CHECK(cpu.test_flags(z80::SF|z80::PF));
    CHECK(8==cpu.step(&bus)); CHECK(0x06 == cpu.C); CHECK(cpu.test_flags(z80::PF));
    CHECK(8==cpu.step(&bus)); CHECK(0x03 == cpu.C); CHECK(cpu.test_flags(z80::PF));
    CHECK(8==cpu.step(&bus)); CHECK(0xFC == cpu.D); CHECK(cpu.test_flags(z80::SF|z80::PF|z80::CF));
    CHECK(8==cpu.step(&bus)); CHECK(0xFE == cpu.D); CHECK(cpu.test_flags(z80::SF));
    CHECK(8==cpu.step(&bus)); CHECK(0x08 == cpu.E); CHECK(cpu.test_flags(z80::CF));
    CHECK(8==cpu.step(&bus)); CHECK(0x11 == cpu.E); CHECK(cpu.test_flags(z80::PF));
    CHECK(8==cpu.step(&bus)); CHECK(0x7E == cpu.H); CHECK(cpu.test_flags(z80::PF));
    CHECK(8==cpu.step(&bus)); CHECK(0x3F == cpu.H); CHECK(cpu.test_flags(z80::PF));
    CHECK(8==cpu.step(&bus)); CHECK(0xE0 == cpu.L); CHECK(cpu.test_flags(z80::SF));
    CHECK(8==cpu.step(&bus)); CHECK(0x70 == cpu.L); CHECK(cpu.test_flags(0));
}

TEST(RRC_RLC_RR_RL_iHLIX_d) {
    z80 cpu = init_z80();

    ubyte data[] = { 0x01, 0xFF, 0x11 };
    cpu.mem.write(0x1000, data, sizeof(data));

    ubyte prog[] = {
        0x21, 0x00, 0x10,           // LD HL,0x1000
        0xDD, 0x21, 0x00, 0x10,     // LD IX,0x1001
        0xFD, 0x21, 0x03, 0x10,     // LD IY,0x1003
        0xCB, 0x0E,                 // RRC (HL)
        0x7E,                       // LD A,(HL)
        0xCB, 0x06,                 // RLC (HL)
        0x7E,                       // LD A,(HL)
        0xDD, 0xCB, 0x01, 0x0E,     // RRC (IX+1)
        0xDD, 0x7E, 0x01,           // LD A,(IX+1)
        0xDD, 0xCB, 0x01, 0x06,     // RLC (IX+1)
        0xDD, 0x7E, 0x01,           // LD A,(IX+1)
        0xFD, 0xCB, 0xFF, 0x0E,     // RRC (IY-1)
        0xFD, 0x7E, 0xFF,           // LD A,(IY-1)
        0xFD, 0xCB, 0xFF, 0x06,     // RLC (IY-1)
        0xFD, 0x7E, 0xFF,           // LD A,(IY-1)
        0xCB, 0x1E,                 // RR (HL)
        0x7E,                       // LD A,(HL)
        0xCB, 0x16,                 // RL (HL)
        0x7E,                       // LD A,(HL)
        0xDD, 0xCB, 0x01, 0x1E,     // RR (IX+1)
        0xDD, 0x7E, 0x01,           // LD A,(IX+1)
        0xDD, 0xCB, 0x01, 0x16,     // RL (IX+1)
        0xDD, 0x7E, 0x01,           // LD A,(IX+1)
        0xFD, 0xCB, 0xFF, 0x16,     // RL (IY-1)
        0xFD, 0x7E, 0xFF,           // LD A,(IY-1)
        0xFD, 0xCB, 0xFF, 0x1E,     // RR (IY-1)
        0xFD, 0x7E, 0xFF,           // LD A,(IY-1)
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    // skip loads
    for (int i = 0; i < 3; i++) {
        cpu.step(&bus);
    }
    CHECK(15==cpu.step(&bus)); CHECK(0x80 == cpu.mem.r8(0x1000)); CHECK(cpu.test_flags(z80::SF|z80::CF));
    CHECK(7 ==cpu.step(&bus)); CHECK(0x80 == cpu.A);
    CHECK(15==cpu.step(&bus)); CHECK(0x01 == cpu.mem.r8(0x1000)); CHECK(cpu.test_flags(z80::CF));
    CHECK(7 ==cpu.step(&bus)); CHECK(0x01 == cpu.A);
    CHECK(23==cpu.step(&bus)); CHECK(0xFF == cpu.mem.r8(0x1001)); CHECK(cpu.test_flags(z80::SF|z80::PF|z80::CF));
    CHECK(19==cpu.step(&bus)); CHECK(0xFF == cpu.A);
    CHECK(23==cpu.step(&bus)); CHECK(0xFF == cpu.mem.r8(0x1001)); CHECK(cpu.test_flags(z80::SF|z80::PF|z80::CF));
    CHECK(19==cpu.step(&bus)); CHECK(0xFF == cpu.A);
    CHECK(23==cpu.step(&bus)); CHECK(0x88 == cpu.mem.r8(0x1002)); CHECK(cpu.test_flags(z80::SF|z80::PF|z80::CF));
    CHECK(19==cpu.step(&bus)); CHECK(0x88 == cpu.A);
    CHECK(23==cpu.step(&bus)); CHECK(0x11 == cpu.mem.r8(0x1002)); CHECK(cpu.test_flags(z80::PF|z80::CF)); 
    CHECK(19==cpu.step(&bus)); CHECK(0x11 == cpu.A);
    CHECK(15==cpu.step(&bus)); CHECK(0x80 == cpu.mem.r8(0x1000)); CHECK(cpu.test_flags(z80::SF|z80::CF));
    CHECK(7 ==cpu.step(&bus)); CHECK(0x80 == cpu.A);
    CHECK(15==cpu.step(&bus)); CHECK(0x01 == cpu.mem.r8(0x1000)); CHECK(cpu.test_flags(z80::CF));
    CHECK(7 ==cpu.step(&bus)); CHECK(0x01 == cpu.A);
    CHECK(23==cpu.step(&bus)); CHECK(0xFF == cpu.mem.r8(0x1001)); CHECK(cpu.test_flags(z80::SF|z80::PF|z80::CF));
    CHECK(19==cpu.step(&bus)); CHECK(0xFF == cpu.A);
    CHECK(23==cpu.step(&bus)); CHECK(0xFF == cpu.mem.r8(0x1001)); CHECK(cpu.test_flags(z80::SF|z80::PF|z80::CF));
    CHECK(19==cpu.step(&bus)); CHECK(0xFF == cpu.A);
    CHECK(23==cpu.step(&bus)); CHECK(0x23 == cpu.mem.r8(0x1002)); CHECK(cpu.test_flags(0));
    CHECK(19==cpu.step(&bus)); CHECK(0x23 == cpu.A);
    CHECK(23==cpu.step(&bus)); CHECK(0x11 == cpu.mem.r8(0x1002)); CHECK(cpu.test_flags(z80::PF|z80::CF));
    CHECK(19==cpu.step(&bus)); CHECK(0x11 == cpu.A);
}

TEST(SLA_r) {
    z80 cpu = init_z80();

    ubyte prog[] = {
        0x3E, 0x01,         // LD A,0x01
        0x06, 0x80,         // LD B,0x80
        0x0E, 0xAA,         // LD C,0xAA
        0x16, 0xFE,         // LD D,0xFE
        0x1E, 0x7F,         // LD E,0x7F
        0x26, 0x11,         // LD H,0x11
        0x2E, 0x00,         // LD L,0x00
        0xCB, 0x27,         // SLA A
        0xCB, 0x20,         // SLA B
        0xCB, 0x21,         // SLA C
        0xCB, 0x22,         // SLA D
        0xCB, 0x23,         // SLA E
        0xCB, 0x24,         // SLA H
        0xCB, 0x25,         // SLA L
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    // skip loads
    for (int i = 0; i < 7; i++) {
        cpu.step(&bus);
    }
    CHECK(8==cpu.step(&bus)); CHECK(0x02 == cpu.A); CHECK(cpu.test_flags(0));
    CHECK(8==cpu.step(&bus)); CHECK(0x00 == cpu.B); CHECK(cpu.test_flags(z80::ZF|z80::PF|z80::CF));
    CHECK(8==cpu.step(&bus)); CHECK(0x54 == cpu.C); CHECK(cpu.test_flags(z80::CF));
    CHECK(8==cpu.step(&bus)); CHECK(0xFC == cpu.D); CHECK(cpu.test_flags(z80::SF|z80::PF|z80::CF));
    CHECK(8==cpu.step(&bus)); CHECK(0xFE == cpu.E); CHECK(cpu.test_flags(z80::SF));
    CHECK(8==cpu.step(&bus)); CHECK(0x22 == cpu.H); CHECK(cpu.test_flags(z80::PF));
    CHECK(8==cpu.step(&bus)); CHECK(0x00 == cpu.L); CHECK(cpu.test_flags(z80::ZF|z80::PF));
}

TEST(SRA_r) {
    z80 cpu = init_z80();

    ubyte prog[] = {
        0x3E, 0x01,         // LD A,0x01
        0x06, 0x80,         // LD B,0x80
        0x0E, 0xAA,         // LD C,0xAA
        0x16, 0xFE,         // LD D,0xFE
        0x1E, 0x7F,         // LD E,0x7F
        0x26, 0x11,         // LD H,0x11
        0x2E, 0x00,         // LD L,0x00
        0xCB, 0x2F,         // SRA A
        0xCB, 0x28,         // SRA B
        0xCB, 0x29,         // SRA C
        0xCB, 0x2A,         // SRA D
        0xCB, 0x2B,         // SRA E
        0xCB, 0x2C,         // SRA H
        0xCB, 0x2D,         // SRA L
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    // skip loads
    for (int i = 0; i < 7; i++) {
        cpu.step(&bus);
    }
    CHECK(8==cpu.step(&bus)); CHECK(0x00 == cpu.A); CHECK(cpu.test_flags(z80::ZF|z80::PF|z80::CF));
    CHECK(8==cpu.step(&bus)); CHECK(0xC0 == cpu.B); CHECK(cpu.test_flags(z80::SF|z80::PF));
    CHECK(8==cpu.step(&bus)); CHECK(0xD5 == cpu.C); CHECK(cpu.test_flags(z80::SF));
    CHECK(8==cpu.step(&bus)); CHECK(0xFF == cpu.D); CHECK(cpu.test_flags(z80::SF|z80::PF));
    CHECK(8==cpu.step(&bus)); CHECK(0x3F == cpu.E); CHECK(cpu.test_flags(z80::PF|z80::CF));
    CHECK(8==cpu.step(&bus)); CHECK(0x08 == cpu.H); CHECK(cpu.test_flags(z80::CF));
    CHECK(8==cpu.step(&bus)); CHECK(0x00 == cpu.L); CHECK(cpu.test_flags(z80::ZF|z80::PF));
}

TEST(SRL_r) {
    z80 cpu = init_z80();

    ubyte prog[] = {
        0x3E, 0x01,         // LD A,0x01
        0x06, 0x80,         // LD B,0x80
        0x0E, 0xAA,         // LD C,0xAA
        0x16, 0xFE,         // LD D,0xFE
        0x1E, 0x7F,         // LD E,0x7F
        0x26, 0x11,         // LD H,0x11
        0x2E, 0x00,         // LD L,0x00
        0xCB, 0x3F,         // SRL A
        0xCB, 0x38,         // SRL B
        0xCB, 0x39,         // SRL C
        0xCB, 0x3A,         // SRL D
        0xCB, 0x3B,         // SRL E
        0xCB, 0x3C,         // SRL H
        0xCB, 0x3D,         // SRL L
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    // skip loads
    for (int i = 0; i < 7; i++) {
        cpu.step(&bus);
    }
    CHECK(8==cpu.step(&bus)); CHECK(0x00 == cpu.A); CHECK(cpu.test_flags(z80::ZF|z80::PF|z80::CF));
    CHECK(8==cpu.step(&bus)); CHECK(0x40 == cpu.B); CHECK(cpu.test_flags(0));
    CHECK(8==cpu.step(&bus)); CHECK(0x55 == cpu.C); CHECK(cpu.test_flags(z80::PF));
    CHECK(8==cpu.step(&bus)); CHECK(0x7F == cpu.D); CHECK(cpu.test_flags(0));
    CHECK(8==cpu.step(&bus)); CHECK(0x3F == cpu.E); CHECK(cpu.test_flags(z80::PF|z80::CF));
    CHECK(8==cpu.step(&bus)); CHECK(0x08 == cpu.H); CHECK(cpu.test_flags(z80::CF));
    CHECK(8==cpu.step(&bus)); CHECK(0x00 == cpu.L); CHECK(cpu.test_flags(z80::ZF|z80::PF));
}

TEST(SLA_iHLIXY_d) {
    z80 cpu = init_z80();

    ubyte data[] = { 0x01, 0x80, 0xAA };
    cpu.mem.write(0x1000, data, sizeof(data));
    ubyte prog[] = {
        0x21, 0x00, 0x10,           // LD HL,0x1000
        0xDD, 0x21, 0x00, 0x10,     // LD IX,0x1001
        0xFD, 0x21, 0x03, 0x10,     // LD IY,0x1003
        0xCB, 0x26,                 // SLA (HL)
        0x7E,                       // LD A,(HL)
        0xDD, 0xCB, 0x01, 0x26,     // SLA (IX+1)
        0xDD, 0x7E, 0x01,           // LD A,(IX+1)
        0xFD, 0xCB, 0xFF, 0x26,     // SLA (IY-1)
        0xFD, 0x7E, 0xFF,           // LD A,(IY-1)
    };
    cpu.mem.write(0x000, prog, sizeof(prog));

    // skip loads
    for (int i = 0; i < 3; i++) {
        cpu.step(&bus);
    }
    CHECK(15==cpu.step(&bus)); CHECK(0x02 == cpu.mem.r8(0x1000)); CHECK(cpu.test_flags(0));
    CHECK(7 ==cpu.step(&bus)); CHECK(0x02 == cpu.A);
    CHECK(23==cpu.step(&bus)); CHECK(0x00 == cpu.mem.r8(0x1001)); CHECK(cpu.test_flags(z80::ZF|z80::PF|z80::CF));
    CHECK(19==cpu.step(&bus)); CHECK(0x00 == cpu.A);
    CHECK(23==cpu.step(&bus)); CHECK(0x54 == cpu.mem.r8(0x1002)); CHECK(cpu.test_flags(z80::CF));
    CHECK(19==cpu.step(&bus)); CHECK(0x54 == cpu.A);
}

TEST(SRA_iHLIXY_d) {
    z80 cpu = init_z80();

    ubyte data[] = { 0x01, 0x80, 0xAA };
    cpu.mem.write(0x1000, data, sizeof(data));
    ubyte prog[] = {
        0x21, 0x00, 0x10,           // LD HL,0x1000
        0xDD, 0x21, 0x00, 0x10,     // LD IX,0x1001
        0xFD, 0x21, 0x03, 0x10,     // LD IY,0x1003
        0xCB, 0x2E,                 // SRA (HL)
        0x7E,                       // LD A,(HL)
        0xDD, 0xCB, 0x01, 0x2E,     // SRA (IX+1)
        0xDD, 0x7E, 0x01,           // LD A,(IX+1)
        0xFD, 0xCB, 0xFF, 0x2E,     // SRA (IY-1)
        0xFD, 0x7E, 0xFF,           // LD A,(IY-1)
    };
    cpu.mem.write(0x000, prog, sizeof(prog));

    // skip loads
    for (int i = 0; i < 3; i++) {
        cpu.step(&bus);
    }
    CHECK(15==cpu.step(&bus)); CHECK(0x00 == cpu.mem.r8(0x1000)); CHECK(cpu.test_flags(z80::ZF|z80::PF|z80::CF));
    CHECK(7 ==cpu.step(&bus)); CHECK(0x00 == cpu.A);
    CHECK(23==cpu.step(&bus)); CHECK(0xC0 == cpu.mem.r8(0x1001)); CHECK(cpu.test_flags(z80::SF|z80::PF));
    CHECK(19==cpu.step(&bus)); CHECK(0xC0 == cpu.A);
    CHECK(23==cpu.step(&bus)); CHECK(0xD5 == cpu.mem.r8(0x1002)); CHECK(cpu.test_flags(z80::SF));
    CHECK(19==cpu.step(&bus)); CHECK(0xD5 == cpu.A);
}

TEST(SRL_iHLIXY_d) {
    z80 cpu = init_z80();

    ubyte data[] = { 0x01, 0x80, 0xAA };
    cpu.mem.write(0x1000, data, sizeof(data));
    ubyte prog[] = {
        0x21, 0x00, 0x10,           // LD HL,0x1000
        0xDD, 0x21, 0x00, 0x10,     // LD IX,0x1001
        0xFD, 0x21, 0x03, 0x10,     // LD IY,0x1003
        0xCB, 0x3E,                 // SRL (HL)
        0x7E,                       // LD A,(HL)
        0xDD, 0xCB, 0x01, 0x3E,     // SRL (IX+1)
        0xDD, 0x7E, 0x01,           // LD A,(IX+1)
        0xFD, 0xCB, 0xFF, 0x3E,     // SRL (IY-1)
        0xFD, 0x7E, 0xFF,           // LD A,(IY-1)
    };
    cpu.mem.write(0x000, prog, sizeof(prog));

    // skip loads
    for (int i = 0; i < 3; i++) {
        cpu.step(&bus);
    }
    CHECK(15==cpu.step(&bus)); CHECK(0x00 == cpu.mem.r8(0x1000)); CHECK(cpu.test_flags(z80::ZF|z80::PF|z80::CF));
    CHECK(7 ==cpu.step(&bus)); CHECK(0x00 == cpu.A);
    CHECK(23==cpu.step(&bus)); CHECK(0x40 == cpu.mem.r8(0x1001)); CHECK(cpu.test_flags(0));
    CHECK(19==cpu.step(&bus)); CHECK(0x40 == cpu.A);
    CHECK(23==cpu.step(&bus)); CHECK(0x55 == cpu.mem.r8(0x1002)); CHECK(cpu.test_flags(z80::PF));
    CHECK(19==cpu.step(&bus)); CHECK(0x55 == cpu.A);
}

TEST(RLD_RRD) {
    z80 cpu = init_z80();

    ubyte prog[] = {
        0x3E, 0x12,         // LD A,0x12
        0x21, 0x00, 0x10,   // LD HL,0x1000
        0x36, 0x34,         // LD (HL),0x34
        0xED, 0x67,         // RRD
        0xED, 0x6F,         // RLD
        0x7E,               // LD A,(HL)
        0x3E, 0xFE,         // LD A,0xFE
        0x36, 0x00,         // LD (HL),0x00
        0xED, 0x6F,         // RLD
        0xED, 0x67,         // RRD
        0x7E,               // LD A,(HL)
        0x3E, 0x01,         // LD A,0x01
        0x36, 0x00,         // LD (HL),0x00
        0xED, 0x6F,         // RLD
        0xED, 0x67,         // RRD
        0x7E
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    CHECK(7 ==cpu.step(&bus)); CHECK(0x12 == cpu.A);
    CHECK(10==cpu.step(&bus)); CHECK(0x1000 == cpu.HL);
    CHECK(10==cpu.step(&bus)); CHECK(0x34 == cpu.mem.r8(0x1000));
    CHECK(18==cpu.step(&bus)); CHECK(0x14 == cpu.A); CHECK(0x23 == cpu.mem.r8(0x1000));
    CHECK(18==cpu.step(&bus)); CHECK(0x12 == cpu.A); CHECK(0x34 == cpu.mem.r8(0x1000));
    CHECK(7 ==cpu.step(&bus)); CHECK(0x34 == cpu.A);
    CHECK(7 ==cpu.step(&bus)); CHECK(0xFE == cpu.A);
    CHECK(10==cpu.step(&bus)); CHECK(0x00 == cpu.mem.r8(0x1000));
    CHECK(18==cpu.step(&bus)); CHECK(0xF0 == cpu.A); CHECK(0x0E == cpu.mem.r8(0x1000)); CHECK(cpu.test_flags(z80::SF|z80::PF));
    CHECK(18==cpu.step(&bus)); CHECK(0xFE == cpu.A); CHECK(0x00 == cpu.mem.r8(0x1000)); CHECK(cpu.test_flags(z80::SF));
    CHECK(7 ==cpu.step(&bus)); CHECK(0x00 == cpu.A);
    CHECK(7 ==cpu.step(&bus)); CHECK(0x01 == cpu.A);
    CHECK(10 ==cpu.step(&bus)); CHECK(0x00 == cpu.mem.r8(0x1000));
    cpu.F |= z80::CF;
    CHECK(18==cpu.step(&bus)); CHECK(0x00 == cpu.A); CHECK(0x01 == cpu.mem.r8(0x1000)); CHECK(cpu.test_flags(z80::ZF|z80::PF|z80::CF));
    CHECK(18==cpu.step(&bus)); CHECK(0x01 == cpu.A); CHECK(0x00 == cpu.mem.r8(0x1000)); CHECK(cpu.test_flags(z80::CF));
    CHECK(7 ==cpu.step(&bus)); CHECK(0x00 == cpu.A);
}

TEST(HALT) {
    z80 cpu = init_z80();

    ubyte prog[] = {
        0x76,           // HALT
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    CHECK(4==cpu.step(&bus)); CHECK(0x0000 == cpu.PC);
    CHECK(4==cpu.step(&bus)); CHECK(0x0000 == cpu.PC);
    CHECK(4==cpu.step(&bus)); CHECK(0x0000 == cpu.PC);
}

TEST(LDI) {
    z80 cpu = init_z80();

    ubyte data[] = {
        0x01, 0x02, 0x03,
    };
    cpu.mem.write(0x1000, data, sizeof(data));

    ubyte prog[] = {
        0x21, 0x00, 0x10,       // LD HL,0x1000
        0x11, 0x00, 0x20,       // LD DE,0x2000
        0x01, 0x03, 0x00,       // LD BC,0x0003
        0xED, 0xA0,             // LDI
        0xED, 0xA0,             // LDI
        0xED, 0xA0,             // LDI
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    // skip loads
    for (int i = 0; i < 3; i++) {
        cpu.step(&bus);
    }
    CHECK(16==cpu.step(&bus));
    CHECK(0x1001 == cpu.HL);
    CHECK(0x2001 == cpu.DE);
    CHECK(0x0002 == cpu.BC);
    CHECK(0x01 == cpu.mem.r8(0x2000));
    CHECK(cpu.test_flags(z80::PF));
    CHECK(16==cpu.step(&bus));
    CHECK(0x1002 == cpu.HL);
    CHECK(0x2002 == cpu.DE);
    CHECK(0x0001 == cpu.BC);
    CHECK(0x02 == cpu.mem.r8(0x2001));
    CHECK(cpu.test_flags(z80::PF));
    CHECK(16==cpu.step(&bus));
    CHECK(0x1003 == cpu.HL);
    CHECK(0x2003 == cpu.DE);
    CHECK(0x0000 == cpu.BC);
    CHECK(0x03 == cpu.mem.r8(0x2002));
    CHECK(cpu.test_flags(0));
}

TEST(LDIR) {
    z80 cpu = init_z80();

    ubyte data[] = {
        0x01, 0x02, 0x03,
    };
    cpu.mem.write(0x1000, data, sizeof(data));

    ubyte prog[] = {
        0x21, 0x00, 0x10,       // LD HL,0x1000
        0x11, 0x00, 0x20,       // LD DE,0x2000
        0x01, 0x03, 0x00,       // LD BC,0x0003
        0xED, 0xB0,             // LDIR
        0x3E, 0x33,             // LD A,0x33
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    // skip loads
    for (int i = 0; i < 3; i++) {
        cpu.step(&bus);
    }
    CHECK(21==cpu.step(&bus));
    CHECK(0x1001 == cpu.HL);
    CHECK(0x2001 == cpu.DE);
    CHECK(0x0002 == cpu.BC);
    CHECK(0x01 == cpu.mem.r8(0x2000));
    CHECK(cpu.test_flags(z80::PF));
    CHECK(21==cpu.step(&bus));
    CHECK(0x1002 == cpu.HL);
    CHECK(0x2002 == cpu.DE);
    CHECK(0x0001 == cpu.BC);
    CHECK(0x02 == cpu.mem.r8(0x2001));
    CHECK(cpu.test_flags(z80::PF));
    CHECK(16==cpu.step(&bus));
    CHECK(0x1003 == cpu.HL);
    CHECK(0x2003 == cpu.DE);
    CHECK(0x0000 == cpu.BC);
    CHECK(0x03 == cpu.mem.r8(0x2002));
    CHECK(cpu.test_flags(0));
    cpu.step(&bus); CHECK(0x33 == cpu.A);
}

TEST(LDD) {
    z80 cpu = init_z80();

    ubyte data[] = {
        0x01, 0x02, 0x03,
    };
    cpu.mem.write(0x1000, data, sizeof(data));

    ubyte prog[] = {
        0x21, 0x02, 0x10,       // LD HL,0x1002
        0x11, 0x02, 0x20,       // LD DE,0x2002
        0x01, 0x03, 0x00,       // LD BC,0x0003
        0xED, 0xA8,             // LDD
        0xED, 0xA8,             // LDD
        0xED, 0xA8,             // LDD
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    // skip loads
    for (int i = 0; i < 3; i++) {
        cpu.step(&bus);
    }
    CHECK(16==cpu.step(&bus));
    CHECK(0x1001 == cpu.HL);
    CHECK(0x2001 == cpu.DE);
    CHECK(0x0002 == cpu.BC);
    CHECK(0x03 == cpu.mem.r8(0x2002));
    CHECK(cpu.test_flags(z80::PF));
    CHECK(16==cpu.step(&bus));
    CHECK(0x1000 == cpu.HL);
    CHECK(0x2000 == cpu.DE);
    CHECK(0x0001 == cpu.BC);
    CHECK(0x02 == cpu.mem.r8(0x2001));
    CHECK(cpu.test_flags(z80::PF));
    CHECK(16==cpu.step(&bus));
    CHECK(0x0FFF == cpu.HL);
    CHECK(0x1FFF == cpu.DE);
    CHECK(0x0000 == cpu.BC);
    CHECK(0x01 == cpu.mem.r8(0x2000));
    CHECK(cpu.test_flags(0));
}

TEST(LDDR) {
    z80 cpu = init_z80();

    ubyte data[] = {
        0x01, 0x02, 0x03,
    };
    cpu.mem.write(0x1000, data, sizeof(data));

    ubyte prog[] = {
        0x21, 0x02, 0x10,       // LD HL,0x1002
        0x11, 0x02, 0x20,       // LD DE,0x2002
        0x01, 0x03, 0x00,       // LD BC,0x0003
        0xED, 0xB8,             // LDDR
        0x3E, 0x33,             // LD A,0x33
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    // skip loads
    for (int i = 0; i < 3; i++) {
        cpu.step(&bus);
    }
    CHECK(21==cpu.step(&bus));
    CHECK(0x1001 == cpu.HL);
    CHECK(0x2001 == cpu.DE);
    CHECK(0x0002 == cpu.BC);
    CHECK(0x03 == cpu.mem.r8(0x2002));
    CHECK(cpu.test_flags(z80::PF));
    CHECK(21==cpu.step(&bus));
    CHECK(0x1000 == cpu.HL);
    CHECK(0x2000 == cpu.DE);
    CHECK(0x0001 == cpu.BC);
    CHECK(0x02 == cpu.mem.r8(0x2001));
    CHECK(cpu.test_flags(z80::PF));
    CHECK(16==cpu.step(&bus));
    CHECK(0x0FFF == cpu.HL);
    CHECK(0x1FFF == cpu.DE);
    CHECK(0x0000 == cpu.BC);
    CHECK(0x01 == cpu.mem.r8(0x2000));
    CHECK(cpu.test_flags(0));
    cpu.step(&bus); CHECK(0x33 == cpu.A);
}

TEST(CPI) {
    z80 cpu = init_z80();

    ubyte data[] = {
        0x01, 0x02, 0x03, 0x04
    };
    cpu.mem.write(0x1000, data, sizeof(data));

    ubyte prog[] = {
        0x21, 0x00, 0x10,       // ld hl,0x1000
        0x01, 0x04, 0x00,       // ld bc,0x0004
        0x3e, 0x03,             // ld a,0x03
        0xed, 0xa1,             // cpi
        0xed, 0xa1,             // cpi
        0xed, 0xa1,             // cpi
        0xed, 0xa1,             // cpi
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    // skip loads
    for (int i = 0; i < 3; i++) {
        cpu.step(&bus);
    }
    CHECK(16 == cpu.step(&bus));
    CHECK(0x1001 == cpu.HL);
    CHECK(0x0003 == cpu.BC);
    CHECK(cpu.test_flags(z80::PF|z80::NF));
    cpu.F |= z80::CF;
    CHECK(16 == cpu.step(&bus));
    CHECK(0x1002 == cpu.HL);
    CHECK(0x0002 == cpu.BC);
    CHECK(cpu.test_flags(z80::PF|z80::NF|z80::CF));
    CHECK(16 == cpu.step(&bus));
    CHECK(0x1003 == cpu.HL);
    CHECK(0x0001 == cpu.BC);
    CHECK(cpu.test_flags(z80::ZF|z80::PF|z80::NF|z80::CF));
    CHECK(16 == cpu.step(&bus));
    CHECK(0x1004 == cpu.HL);
    CHECK(0x0000 == cpu.BC);
    CHECK(cpu.test_flags(z80::SF|z80::HF|z80::NF|z80::CF));
}

TEST(CPIR) {
    z80 cpu = init_z80();

    ubyte data[] = {
        0x01, 0x02, 0x03, 0x04
    };
    cpu.mem.write(0x1000, data, sizeof(data));

    ubyte prog[] = {
        0x21, 0x00, 0x10,       // ld hl,0x1000
        0x01, 0x04, 0x00,       // ld bc,0x0004
        0x3e, 0x03,             // ld a,0x03
        0xed, 0xb1,             // cpir
        0xed, 0xb1,             // cpir
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    // skip loads
    for (int i = 0; i < 3; i++) {
        cpu.step(&bus);
    }
    CHECK(21 == cpu.step(&bus));
    CHECK(0x1001 == cpu.HL);
    CHECK(0x0003 == cpu.BC);
    CHECK(cpu.test_flags(z80::PF|z80::NF));
    cpu.F |= z80::CF;
    CHECK(21 == cpu.step(&bus));
    CHECK(0x1002 == cpu.HL);
    CHECK(0x0002 == cpu.BC);
    CHECK(cpu.test_flags(z80::PF|z80::NF|z80::CF));
    CHECK(16 == cpu.step(&bus));
    CHECK(0x1003 == cpu.HL);
    CHECK(0x0001 == cpu.BC);
    CHECK(cpu.test_flags(z80::ZF|z80::PF|z80::NF|z80::CF));
    CHECK(16 == cpu.step(&bus));
    CHECK(0x1004 == cpu.HL);
    CHECK(0x0000 == cpu.BC);
    CHECK(cpu.test_flags(z80::SF|z80::HF|z80::NF|z80::CF));
}

TEST(CPD) {
    z80 cpu = init_z80();

    ubyte data[] = {
        0x01, 0x02, 0x03, 0x04
    };
    cpu.mem.write(0x1000, data, sizeof(data));

    ubyte prog[] = {
        0x21, 0x03, 0x10,       // ld hl,0x1004
        0x01, 0x04, 0x00,       // ld bc,0x0004
        0x3e, 0x02,             // ld a,0x03
        0xed, 0xa9,             // cpi
        0xed, 0xa9,             // cpi
        0xed, 0xa9,             // cpi
        0xed, 0xa9,             // cpi
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    // skip loads
    for (int i = 0; i < 3; i++) {
        cpu.step(&bus);
    }
    CHECK(16 == cpu.step(&bus));
    CHECK(0x1002 == cpu.HL);
    CHECK(0x0003 == cpu.BC);
    CHECK(cpu.test_flags(z80::SF|z80::HF|z80::PF|z80::NF));
    cpu.F |= z80::CF;
    CHECK(16 == cpu.step(&bus));
    CHECK(0x1001 == cpu.HL);
    CHECK(0x0002 == cpu.BC);
    CHECK(cpu.test_flags(z80::SF|z80::HF|z80::PF|z80::NF|z80::CF));
    CHECK(16 == cpu.step(&bus));
    CHECK(0x1000 == cpu.HL);
    CHECK(0x0001 == cpu.BC);
    CHECK(cpu.test_flags(z80::ZF|z80::PF|z80::NF|z80::CF));
    CHECK(16 == cpu.step(&bus));
    CHECK(0x0FFF == cpu.HL);
    CHECK(0x0000 == cpu.BC);
    CHECK(cpu.test_flags(z80::NF|z80::CF));
}

TEST(CPDR) {
    z80 cpu = init_z80();

    ubyte data[] = {
        0x01, 0x02, 0x03, 0x04
    };
    cpu.mem.write(0x1000, data, sizeof(data));

    ubyte prog[] = {
        0x21, 0x03, 0x10,       // ld hl,0x1004
        0x01, 0x04, 0x00,       // ld bc,0x0004
        0x3e, 0x02,             // ld a,0x03
        0xed, 0xb9,             // cpdr
        0xed, 0xb9,             // cpdr
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    // skip loads
    for (int i = 0; i < 3; i++) {
        cpu.step(&bus);
    }
    CHECK(21 == cpu.step(&bus));
    CHECK(0x1002 == cpu.HL);
    CHECK(0x0003 == cpu.BC);
    CHECK(cpu.test_flags(z80::SF|z80::HF|z80::PF|z80::NF));
    cpu.F |= z80::CF;
    CHECK(21 == cpu.step(&bus));
    CHECK(0x1001 == cpu.HL);
    CHECK(0x0002 == cpu.BC);
    CHECK(cpu.test_flags(z80::SF|z80::HF|z80::PF|z80::NF|z80::CF));
    CHECK(16 == cpu.step(&bus));
    CHECK(0x1000 == cpu.HL);
    CHECK(0x0001 == cpu.BC);
    CHECK(cpu.test_flags(z80::ZF|z80::PF|z80::NF|z80::CF));
    CHECK(16 == cpu.step(&bus));
    CHECK(0x0FFF == cpu.HL);
    CHECK(0x0000 == cpu.BC);
    CHECK(cpu.test_flags(z80::NF|z80::CF));
}

TEST(DAA) {
    z80 cpu = init_z80();

    ubyte prog[] = {
        0x3e, 0x15,         // ld a,0x15
        0x06, 0x27,         // ld b,0x27
        0x80,               // add a,b
        0x27,               // daa
        0x90,               // sub b
        0x27,               // daa
        0x3e, 0x90,         // ld a,0x90
        0x06, 0x15,         // ld b,0x15
        0x80,               // add a,b
        0x27,               // daa
        0x90,               // sub b
        0x27                // daa
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    CHECK(7==cpu.step(&bus)); CHECK(0x15 == cpu.A);
    CHECK(7==cpu.step(&bus)); CHECK(0x27 == cpu.B);
    CHECK(4==cpu.step(&bus)); CHECK(0x3C == cpu.A); CHECK(cpu.test_flags(0));
    CHECK(4==cpu.step(&bus)); CHECK(0x42 == cpu.A); CHECK(cpu.test_flags(z80::HF|z80::PF));
    CHECK(4==cpu.step(&bus)); CHECK(0x1B == cpu.A); CHECK(cpu.test_flags(z80::HF|z80::NF));
    CHECK(4==cpu.step(&bus)); CHECK(0x15 == cpu.A); CHECK(cpu.test_flags(z80::NF));
    CHECK(7==cpu.step(&bus)); CHECK(0x90 == cpu.A); CHECK(cpu.test_flags(z80::NF));
    CHECK(7==cpu.step(&bus)); CHECK(0x15 == cpu.B); CHECK(cpu.test_flags(z80::NF));
    CHECK(4==cpu.step(&bus)); CHECK(0xA5 == cpu.A); CHECK(cpu.test_flags(z80::SF));
    CHECK(4==cpu.step(&bus)); CHECK(0x05 == cpu.A); CHECK(cpu.test_flags(z80::PF|z80::CF));
    CHECK(4==cpu.step(&bus)); CHECK(0xF0 == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::NF|z80::CF));
    CHECK(4==cpu.step(&bus)); CHECK(0x90 == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::PF|z80::NF|z80::CF));
}

TEST(CPL) {
    z80 cpu = init_z80();

    ubyte prog[] = {
        0x97,               // SUB A
        0x2F,               // CPL
        0x2F,               // CPL
        0xC6, 0xAA,         // ADD A,0xAA
        0x2F,               // CPL
        0x2F,               // CPL
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    CHECK(4==cpu.step(&bus)); CHECK(0x00 == cpu.A); CHECK(cpu.test_flags(z80::ZF|z80::NF));
    CHECK(4==cpu.step(&bus)); CHECK(0xFF == cpu.A); CHECK(cpu.test_flags(z80::ZF|z80::HF|z80::NF));
    CHECK(4==cpu.step(&bus)); CHECK(0x00 == cpu.A); CHECK(cpu.test_flags(z80::ZF|z80::HF|z80::NF));
    CHECK(7==cpu.step(&bus)); CHECK(0xAA == cpu.A); CHECK(cpu.test_flags(z80::SF));
    CHECK(4==cpu.step(&bus)); CHECK(0x55 == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::HF|z80::NF));
    CHECK(4==cpu.step(&bus)); CHECK(0xAA == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::HF|z80::NF));
}

TEST(NEG) {
    z80 cpu = init_z80();

    ubyte prog[] = {
        0x3E, 0x01,         // LD A,0x01
        0xED, 0x44,         // NEG
        0xC6, 0x01,         // ADD A,0x01
        0xED, 0x44,         // NEG
        0xD6, 0x80,         // SUB A,0x80
        0xED, 0x44,         // NEG
        0xC6, 0x40,         // ADD A,0x40
        0xED, 0x44,         // NEG
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    CHECK(7==cpu.step(&bus)); CHECK(0x01 == cpu.A);
    CHECK(8==cpu.step(&bus)); CHECK(0xFF == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::HF|z80::NF|z80::CF));
    CHECK(7==cpu.step(&bus)); CHECK(0x00 == cpu.A); CHECK(cpu.test_flags(z80::ZF|z80::HF|z80::CF));
    CHECK(8==cpu.step(&bus)); CHECK(0x00 == cpu.A); CHECK(cpu.test_flags(z80::ZF|z80::NF));
    CHECK(7==cpu.step(&bus)); CHECK(0x80 == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::PF|z80::NF|z80::CF));
    CHECK(8==cpu.step(&bus)); CHECK(0x80 == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::PF|z80::NF|z80::CF));
    CHECK(7==cpu.step(&bus)); CHECK(0xC0 == cpu.A); CHECK(cpu.test_flags(z80::SF));
    CHECK(8==cpu.step(&bus)); CHECK(0x40 == cpu.A); CHECK(cpu.test_flags(z80::NF|z80::CF));
}

TEST(CCF_SCF) {
    z80 cpu = init_z80();

    ubyte prog[] = {
        0x97,           // SUB A
        0x37,           // SCF
        0x3F,           // CCF
        0xD6, 0xCC,     // SUB 0xCC
        0x3F,           // CCF
        0x37,           // SCF
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    CHECK(4==cpu.step(&bus)); CHECK(0x00 == cpu.A); CHECK(cpu.test_flags(z80::ZF|z80::NF));
    CHECK(4==cpu.step(&bus)); CHECK(0x00 == cpu.A); CHECK(cpu.test_flags(z80::ZF|z80::CF));
    CHECK(4==cpu.step(&bus)); CHECK(0x00 == cpu.A); CHECK(cpu.test_flags(z80::ZF|z80::HF));
    CHECK(7==cpu.step(&bus)); CHECK(0x34 == cpu.A); CHECK(cpu.test_flags(z80::HF|z80::NF|z80::CF)); 
    CHECK(4==cpu.step(&bus)); CHECK(0x34 == cpu.A); CHECK(cpu.test_flags(z80::HF));
    CHECK(4==cpu.step(&bus)); CHECK(0x34 == cpu.A); CHECK(cpu.test_flags(z80::CF));
}

TEST(DI_EI_IM) {
    z80 cpu = init_z80();

    ubyte prog[] = {
        0xF3,           // DI
        0xFB,           // EI
        0x00,           // NOP
        0xF3,           // DI
        0xFB,           // EI
        0x00,           // NOP
        0xED, 0x46,     // IM 0
        0xED, 0x56,     // IM 1
        0xED, 0x5E,     // IM 2
        0xED, 0x46,     // IM 0
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    CHECK(4==cpu.step(&bus)); CHECK(!cpu.IFF1); CHECK(!cpu.IFF2);
    CHECK(4==cpu.step(&bus)); CHECK(!cpu.IFF1); CHECK(!cpu.IFF2);
    CHECK(4==cpu.step(&bus)); CHECK(cpu.IFF1); CHECK(cpu.IFF2);
    CHECK(4==cpu.step(&bus)); CHECK(!cpu.IFF1); CHECK(!cpu.IFF2);
    CHECK(4==cpu.step(&bus)); CHECK(!cpu.IFF1); CHECK(!cpu.IFF2);
    CHECK(4==cpu.step(&bus)); CHECK(cpu.IFF1); CHECK(cpu.IFF2);
    CHECK(8==cpu.step(&bus)); CHECK(0 == cpu.IM);
    CHECK(8==cpu.step(&bus)); CHECK(1 == cpu.IM);
    CHECK(8==cpu.step(&bus)); CHECK(2 == cpu.IM);
    CHECK(8==cpu.step(&bus)); CHECK(0 == cpu.IM);
}

TEST(JP_cc_nn) {
    z80 cpu = init_z80();

    ubyte prog[] = {
        0x97,               //          SUB A
        0xC2, 0x0C, 0x02,   //          JP NZ,label0
        0xCA, 0x0C, 0x02,   //          JP Z,label0
        0x00,               //          NOP
        0xC6, 0x01,         // label0:  ADD A,0x01
        0xCA, 0x15, 0x02,   //          JP Z,label1
        0xC2, 0x15, 0x02,   //          JP NZ,label1
        0x00,               //          NOP
        0x07,               // label1:  RLCA
        0xEA, 0x1D, 0x02,   //          JP PE,label2
        0xE2, 0x1D, 0x02,   //          JP PO,label2
        0x00,               //          NOP
        0xC6, 0xFD,         // label2:  ADD A,0xFD
        0xF2, 0x26, 0x02,   //          JP P,label3
        0xFA, 0x26, 0x02,   //          JP M,label3
        0x00,               //          NOP
        0xD2, 0x2D, 0x02,   // label3:  JP NC,label4
        0xDA, 0x2D, 0x02,   //          JP C,label4
        0x00,               //          NOP
        0x00,               //          NOP
    };
    cpu.mem.write(0x0204, prog, sizeof(prog));
    cpu.PC = 0x0204;

    CHECK(4 ==cpu.step(&bus)); CHECK(0x00 == cpu.A); CHECK(cpu.test_flags(z80::ZF|z80::NF));
    CHECK(10==cpu.step(&bus)); CHECK(0x0208 == cpu.PC);
    CHECK(10==cpu.step(&bus)); CHECK(0x020C == cpu.PC);
    CHECK(7 ==cpu.step(&bus)); CHECK(0x01 == cpu.A); CHECK(cpu.test_flags(0));
    CHECK(10==cpu.step(&bus)); CHECK(0x0211 == cpu.PC);
    CHECK(10==cpu.step(&bus)); CHECK(0x0215 == cpu.PC);
    CHECK(4 ==cpu.step(&bus)); CHECK(0x02 == cpu.A); CHECK(cpu.test_flags(0));
    CHECK(10==cpu.step(&bus)); CHECK(0x0219 == cpu.PC);
    CHECK(10==cpu.step(&bus)); CHECK(0x021D == cpu.PC);
    CHECK(7 ==cpu.step(&bus)); CHECK(0xFF == cpu.A); CHECK(cpu.test_flags(z80::SF));
    CHECK(10==cpu.step(&bus)); CHECK(0x0222 == cpu.PC);
    CHECK(10==cpu.step(&bus)); CHECK(0x0226 == cpu.PC);
    CHECK(10==cpu.step(&bus)); CHECK(0x022D == cpu.PC);
}

TEST(JP_JR) {
    z80 cpu = init_z80();

    ubyte prog[] = {
        0x21, 0x16, 0x02,           //      LD HL,l3
        0xDD, 0x21, 0x19, 0x02,     //      LD IX,l4
        0xFD, 0x21, 0x21, 0x02,     //      LD IY,l5
        0xC3, 0x14, 0x02,           //      JP l0
        0x18, 0x04,                 // l1:  JR l2
        0x18, 0xFC,                 // l0:  JR l1
        0xDD, 0xE9,                 // l3:  JP (IX)
        0xE9,                       // l2:  JP (HL)
        0xFD, 0xE9,                 // l4:  JP (IY)
        0x18, 0x06,                 // l6:  JR l7
        0x00, 0x00, 0x00, 0x00,     //      4x NOP
        0x18, 0xF8,                 // l5:  JR l6
        0x00                        // l7:  NOP
    };
    cpu.mem.write(0x0204, prog, sizeof(prog));
    cpu.PC = 0x0204;

    CHECK(10==cpu.step(&bus)); CHECK(0x0216 == cpu.HL);
    CHECK(14==cpu.step(&bus)); CHECK(0x0219 == cpu.IX);
    CHECK(14==cpu.step(&bus)); CHECK(0x0221 == cpu.IY);
    CHECK(10==cpu.step(&bus)); CHECK(0x0214 == cpu.PC);
    CHECK(12==cpu.step(&bus)); CHECK(0x0212 == cpu.PC);
    CHECK(12==cpu.step(&bus)); CHECK(0x0218 == cpu.PC);
    CHECK(4 ==cpu.step(&bus)); CHECK(0x0216 == cpu.PC);
    CHECK(8 ==cpu.step(&bus)); CHECK(0x0219 == cpu.PC);
    CHECK(8 ==cpu.step(&bus)); CHECK(0x0221 == cpu.PC);
    CHECK(12==cpu.step(&bus)); CHECK(0x021B == cpu.PC);
    CHECK(12==cpu.step(&bus)); CHECK(0x0223 == cpu.PC);
}

TEST(JR_cc_e) {
    z80 cpu = init_z80();

    ubyte prog[] = {
        0x97,           //      SUB A
        0x20, 0x03,     //      JR NZ,l0
        0x28, 0x01,     //      JR Z,l0
        0x00,           //      NOP
        0xC6, 0x01,     // l0:  ADD A,0x01
        0x28, 0x03,     //      JR Z,l1
        0x20, 0x01,     //      JR NZ,l1
        0x00,           //      NOP
        0xD6, 0x03,     // l1:  SUB 0x03
        0x30, 0x03,     //      JR NC,l2
        0x38, 0x01,     //      JR C,l2
        0x00,           //      NOP
        0x00,           // l2:  NOP
    };
    cpu.mem.write(0x0204, prog, sizeof(prog));
    cpu.PC = 0x0204;

    CHECK(4 ==cpu.step(&bus)); CHECK(0x00 == cpu.A); CHECK(cpu.test_flags(z80::ZF|z80::NF));
    CHECK(7 ==cpu.step(&bus)); CHECK(0x0207 == cpu.PC);
    CHECK(12==cpu.step(&bus)); CHECK(0x020A == cpu.PC);
    CHECK(7 ==cpu.step(&bus)); CHECK(0x01 == cpu.A); CHECK(cpu.test_flags(0));
    CHECK(7 ==cpu.step(&bus)); CHECK(0x020E == cpu.PC);
    CHECK(12==cpu.step(&bus)); CHECK(0x0211 == cpu.PC);
    CHECK(7 ==cpu.step(&bus)); CHECK(0xFE == cpu.A); CHECK(cpu.test_flags(z80::SF|z80::HF|z80::NF|z80::CF));
    CHECK(7 ==cpu.step(&bus)); CHECK(0x0215 == cpu.PC);
    CHECK(12==cpu.step(&bus)); CHECK(0x0218 == cpu.PC);
}

TEST(DJNZ) {
    z80 cpu = init_z80();

    ubyte prog[] = {
        0x06, 0x03,         //      LD B,0x03
        0x97,               //      SUB A
        0x3C,               // l0:  INC A
        0x10, 0xFD,         //      DJNZ l0
        0x00,               //      NOP
    };
    cpu.mem.write(0x0204, prog, sizeof(prog));
    cpu.PC = 0x0204;

    CHECK(7 ==cpu.step(&bus)); CHECK(0x03 == cpu.B);
    CHECK(4 ==cpu.step(&bus)); CHECK(0x00 == cpu.A);
    CHECK(4 ==cpu.step(&bus)); CHECK(0x01 == cpu.A);
    CHECK(13==cpu.step(&bus)); CHECK(0x02 == cpu.B); CHECK(0x0207 == cpu.PC);
    CHECK(4 ==cpu.step(&bus)); CHECK(0x02 == cpu.A);
    CHECK(13==cpu.step(&bus)); CHECK(0x01 == cpu.B); CHECK(0x0207 == cpu.PC);
    CHECK(4 ==cpu.step(&bus)); CHECK(0x03 == cpu.A);
    CHECK(8 ==cpu.step(&bus)); CHECK(0x00 == cpu.B); CHECK(0x020A == cpu.PC);
}

TEST(CALL_RET) {
    z80 cpu = init_z80();

    ubyte prog[] = {
        0xCD, 0x0A, 0x02,       //      CALL l0
        0xCD, 0x0A, 0x02,       //      CALL l0
        0xC9,                   // l0:  RET
    };
    cpu.mem.write(0x0204, prog, sizeof(prog));
    cpu.SP = 0x0100;
    cpu.PC = 0x0204;

    CHECK(17 == cpu.step(&bus));
    CHECK(0x020A == cpu.PC);
    CHECK(0x00FE == cpu.SP);
    CHECK(0x0207 == cpu.mem.r16(0x00FE));

    CHECK(10 == cpu.step(&bus));
    CHECK(0x0207 == cpu.PC);
    CHECK(0x0100 == cpu.SP);

    CHECK(17 == cpu.step(&bus));
    CHECK(0x020A == cpu.PC);
    CHECK(0x00FE == cpu.SP);
    CHECK(0x020A == cpu.mem.r16(0x00FE));

    CHECK(10 == cpu.step(&bus));
    CHECK(0x020A == cpu.PC);
    CHECK(0x0100 == cpu.SP);
}

TEST(CALL_RET_cc) {
    z80 cpu = init_z80();

    ubyte prog[] = {
        0x97,               //      SUB A
        0xC4, 0x29, 0x02,   //      CALL NZ,l0
        0xCC, 0x29, 0x02,   //      CALL Z,l0
        0xC6, 0x01,         //      ADD A,0x01
        0xCC, 0x2B, 0x02,   //      CALL Z,l1
        0xC4, 0x2B, 0x02,   //      CALL NZ,l1
        0x07,               //      RLCA
        0xEC, 0x2D, 0x02,   //      CALL PE,l2
        0xE4, 0x2D, 0x02,   //      CALL PO,l2
        0xD6, 0x03,         //      SUB 0x03
        0xF4, 0x2F, 0x02,   //      CALL P,l3
        0xFC, 0x2F, 0x02,   //      CALL M,l3
        0xD4, 0x31, 0x02,   //      CALL NC,l4
        0xDC, 0x31, 0x02,   //      CALL C,l4
        0xC9,               //      RET
        0xC0,               // l0:  RET NZ
        0xC8,               //      RET Z
        0xC8,               // l1:  RET Z
        0xC0,               //      RET NZ
        0xE8,               // l2:  RET PE
        0xE0,               //      RET PO
        0xF0,               // l3:  RET P
        0xF8,               //      RET M
        0xD0,               // l4:  RET NC
        0xD8,               //      RET C
    };
    cpu.mem.write(0x0204, prog, sizeof(prog));
    cpu.PC = 0x0204;
    cpu.SP = 0x0100;

    CHECK(4 ==cpu.step(&bus)); CHECK(0x00 == cpu.A);
    CHECK(10==cpu.step(&bus)); CHECK(0x0208 == cpu.PC);
    CHECK(17==cpu.step(&bus)); CHECK(0x0229 == cpu.PC);
    CHECK(5 ==cpu.step(&bus)); CHECK(0x022A == cpu.PC);
    CHECK(11==cpu.step(&bus)); CHECK(0x020B == cpu.PC);
    CHECK(7 ==cpu.step(&bus)); CHECK(0x01 == cpu.A);
    CHECK(10==cpu.step(&bus)); CHECK(0x0210 == cpu.PC);
    CHECK(17==cpu.step(&bus)); CHECK(0x022B == cpu.PC);
    CHECK(5 ==cpu.step(&bus)); CHECK(0x022C == cpu.PC);
    CHECK(11==cpu.step(&bus)); CHECK(0x0213 == cpu.PC);
    CHECK(4 ==cpu.step(&bus)); CHECK(0x02 == cpu.A);
    CHECK(10==cpu.step(&bus)); CHECK(0x0217 == cpu.PC);
    CHECK(17==cpu.step(&bus)); CHECK(0x022D == cpu.PC);
    CHECK(5 ==cpu.step(&bus)); CHECK(0x022E == cpu.PC);
    CHECK(11==cpu.step(&bus)); CHECK(0x021A == cpu.PC);
    CHECK(7 ==cpu.step(&bus)); CHECK(0xFF == cpu.A);
    CHECK(10==cpu.step(&bus)); CHECK(0x021F == cpu.PC);
    CHECK(17==cpu.step(&bus)); CHECK(0x022F == cpu.PC);
    CHECK(5 ==cpu.step(&bus)); CHECK(0x0230 == cpu.PC);
    CHECK(11==cpu.step(&bus)); CHECK(0x0222 == cpu.PC);
    CHECK(10==cpu.step(&bus)); CHECK(0x0225 == cpu.PC);
    CHECK(17==cpu.step(&bus)); CHECK(0x0231 == cpu.PC);
    CHECK(5 ==cpu.step(&bus)); CHECK(0x0232 == cpu.PC);
    CHECK(11==cpu.step(&bus)); CHECK(0x0228 == cpu.PC);
}

TEST(IN) {
    z80 cpu = init_z80();

    ubyte prog[] = {
        0x3E, 0x01,         // LD A,0x01
        0xDB, 0x03,         // IN A,(0x03)
        0xDB, 0x04,         // IN A,(0x04)
        0x01, 0x02, 0x02,   // LD BC,0x0202
        0xED, 0x78,         // IN A,(C)
        0x01, 0xFF, 0x05,   // LD BC,0x05FF
        0xED, 0x50,         // IN D,(C)
        0x01, 0x05, 0x05,   // LD BC,0x0505
        0xED, 0x58,         // IN E,(C)
        0x01, 0x06, 0x01,   // LD BC,0x0106
        0xED, 0x60,         // IN H,(C)
        0x01, 0x00, 0x10,   // LD BC,0x0000
        0xED, 0x68,         // IN L,(C)
        0xED, 0x40,         // IN B,(C)
        0xED, 0x48,         // IN C,(B)
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));
    cpu.F = z80::HF|z80::CF;

    CHECK(7 ==cpu.step(&bus)); CHECK(0x01 == cpu.A); CHECK(cpu.test_flags(z80::HF|z80::CF));
    CHECK(11==cpu.step(&bus)); CHECK(0x06 == cpu.A); CHECK(cpu.test_flags(z80::HF|z80::CF));
    CHECK(11==cpu.step(&bus)); CHECK(0x08 == cpu.A); CHECK(cpu.test_flags(z80::HF|z80::CF));
    CHECK(10==cpu.step(&bus)); CHECK(0x0202 == cpu.BC);
    CHECK(12==cpu.step(&bus)); CHECK(0x04 == cpu.A); CHECK(cpu.test_flags(z80::CF));
    CHECK(10==cpu.step(&bus)); CHECK(0x05FF == cpu.BC);
    CHECK(12==cpu.step(&bus)); CHECK(0xFE == cpu.D); CHECK(cpu.test_flags(z80::SF|z80::CF));
    CHECK(10==cpu.step(&bus)); CHECK(0x0505 == cpu.BC);
    CHECK(12==cpu.step(&bus)); CHECK(0x0A == cpu.E);CHECK(cpu.test_flags(z80::PF|z80::CF));
    CHECK(10==cpu.step(&bus)); CHECK(0x0106 == cpu.BC);
    CHECK(12==cpu.step(&bus)); CHECK(0x0C == cpu.H); CHECK(cpu.test_flags(z80::PF|z80::CF));
    CHECK(10==cpu.step(&bus)); CHECK(0x1000 == cpu.BC);
    CHECK(12==cpu.step(&bus)); CHECK(0x00 == cpu.L); CHECK(cpu.test_flags(z80::ZF|z80::PF|z80::CF));
    CHECK(12==cpu.step(&bus)); CHECK(0x00 == cpu.B); CHECK(cpu.test_flags(z80::ZF|z80::PF|z80::CF));
    CHECK(12==cpu.step(&bus)); CHECK(0x00 == cpu.C); CHECK(cpu.test_flags(z80::ZF|z80::PF|z80::CF));
}

TEST(INIR_INDR) {
    z80 cpu = init_z80();

    ubyte prog[] = {
        0x21, 0x00, 0x10,       // LD HL,0x1000
        0x01, 0x02, 0x03,       // LD BC,0x0302
        0xED, 0xB2,             // INIR
        0x01, 0x03, 0x03,       // LD BC,0x0303
        0xED, 0xBA
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    CHECK(10 == cpu.step(&bus)); CHECK(0x1000 == cpu.HL);
    CHECK(10 == cpu.step(&bus)); CHECK(0x0302 == cpu.BC);

    CHECK(21==cpu.step(&bus));
    CHECK(0x1001 == cpu.HL);
    CHECK(0x0202 == cpu.BC);
    CHECK(0x04 == cpu.mem.r8(0x1000));
    CHECK(!(cpu.F & z80::ZF));
    CHECK(21==cpu.step(&bus));
    CHECK(0x1002 == cpu.HL);
    CHECK(0x0102 == cpu.BC);
    CHECK(0x04 == cpu.mem.r8(0x1001));
    CHECK(!(cpu.F & z80::ZF));
    CHECK(16==cpu.step(&bus));
    CHECK(0x1003 == cpu.HL);
    CHECK(0x0002 == cpu.BC);
    CHECK(0x04 == cpu.mem.r8(0x1002));
    CHECK(cpu.F & z80::ZF);

    CHECK(10==cpu.step(&bus)); CHECK(0x0303 == cpu.BC);
    CHECK(21==cpu.step(&bus));
    CHECK(0x1002 == cpu.HL);
    CHECK(0x0203 == cpu.BC);
    CHECK(0x06 == cpu.mem.r8(0x1003));
    CHECK(!(cpu.F & z80::ZF));
    CHECK(21==cpu.step(&bus));
    CHECK(0x1001 == cpu.HL);
    CHECK(0x0103 == cpu.BC);
    CHECK(0x06 == cpu.mem.r8(0x1002));
    CHECK(!(cpu.F & z80::ZF));
    CHECK(16==cpu.step(&bus));
    CHECK(0x1000 == cpu.HL);
    CHECK(0x0003 == cpu.BC);
    CHECK(0x06 == cpu.mem.r8(0x1001));
    CHECK(cpu.F & z80::ZF);
}

TEST(OUT) {
    z80 cpu = init_z80();

    ubyte prog[] = {
        0x3E, 0x01,         // LD A,0x01
        0xD3, 0x01,         // OUT (0x01),A
        0xD3, 0x02,         // OUT (0x02),A
        0x01, 0x34, 0x12,   // LD BC,0x1234
        0x11, 0x78, 0x56,   // LD DE,0x5678
        0x21, 0xCD, 0xAB,   // LD HL,0xABCD
        0xED, 0x79,         // OUT (C),A
        0xED, 0x41,         // OUT (C),B
        0xED, 0x49,         // OUT (C),C
        0xED, 0x51,         // OUT (C),D
        0xED, 0x59,         // OUT (C),E
        0xED, 0x61,         // OUT (C),H
        0xED, 0x69,         // OUT (C),L
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    CHECK(7 ==cpu.step(&bus)); CHECK(0x01 == cpu.A);
    CHECK(11==cpu.step(&bus)); CHECK(0x0101 == bus.out_port); CHECK(0x01 == bus.out_byte);
    CHECK(11==cpu.step(&bus)); CHECK(0x0102 == bus.out_port); CHECK(0x01 == bus.out_byte);
    CHECK(10==cpu.step(&bus)); CHECK(0x1234 == cpu.BC);
    CHECK(10==cpu.step(&bus)); CHECK(0x5678 == cpu.DE);
    CHECK(10==cpu.step(&bus)); CHECK(0xABCD == cpu.HL);
    CHECK(12==cpu.step(&bus)); CHECK(0x1234 == bus.out_port); CHECK(0x01 == bus.out_byte);
    CHECK(12==cpu.step(&bus)); CHECK(0x1234 == bus.out_port); CHECK(0x12 == bus.out_byte);
    CHECK(12==cpu.step(&bus)); CHECK(0x1234 == bus.out_port); CHECK(0x34 == bus.out_byte);
    CHECK(12==cpu.step(&bus)); CHECK(0x1234 == bus.out_port); CHECK(0x56 == bus.out_byte);
    CHECK(12==cpu.step(&bus)); CHECK(0x1234 == bus.out_port); CHECK(0x78 == bus.out_byte);
    CHECK(12==cpu.step(&bus)); CHECK(0x1234 == bus.out_port); CHECK(0xAB == bus.out_byte);
    CHECK(12==cpu.step(&bus)); CHECK(0x1234 == bus.out_port); CHECK(0xCD == bus.out_byte);
}

TEST(OTIR_OTDR) {
    z80 cpu = init_z80();

    ubyte data[] = {
        0x01, 0x02, 0x03, 0x04
    };
    cpu.mem.write(0x1000, data, sizeof(data));

    ubyte prog[] = {
        0x21, 0x00, 0x10,       // LD HL,0x1000
        0x01, 0x02, 0x03,       // LD BC,0x0302
        0xED, 0xB3,             // OTIR
        0x01, 0x03, 0x03,       // LD BC,0x0303
        0xED, 0xBB,             // OTDR
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    CHECK(10==cpu.step(&bus)); CHECK(0x1000 == cpu.HL);
    CHECK(10==cpu.step(&bus)); CHECK(0x0302 == cpu.BC);
    CHECK(21==cpu.step(&bus));
    CHECK(0x1001 == cpu.HL);
    CHECK(0x0202 == cpu.BC);
    CHECK(0x0202 == bus.out_port); CHECK(0x01 == bus.out_byte);
    CHECK(!(cpu.F & z80::ZF));
    CHECK(21==cpu.step(&bus));
    CHECK(0x1002 == cpu.HL);
    CHECK(0x0102 == cpu.BC);
    CHECK(0x0102 == bus.out_port); CHECK(0x02 == bus.out_byte);
    CHECK(!(cpu.F & z80::ZF));
    CHECK(16==cpu.step(&bus));
    CHECK(0x1003 == cpu.HL);
    CHECK(0x0002 == cpu.BC);
    CHECK(0x0002 == bus.out_port); CHECK(0x03 == bus.out_byte);
    CHECK(cpu.F & z80::ZF);

    CHECK(10 == cpu.step(&bus)); CHECK(0x0303 == cpu.BC);
    CHECK(21==cpu.step(&bus));
    CHECK(0x1002 == cpu.HL);
    CHECK(0x0203 == cpu.BC);
    CHECK(0x0203 == bus.out_port); CHECK(0x04 == bus.out_byte);
    CHECK(!(cpu.F & z80::ZF));
    CHECK(21==cpu.step(&bus));
    CHECK(0x1001 == cpu.HL);
    CHECK(0x0103 == cpu.BC);
    CHECK(0x0103 == bus.out_port); CHECK(0x03 == bus.out_byte);
    CHECK(!(cpu.F & z80::ZF));
    CHECK(16==cpu.step(&bus));
    CHECK(0x1000 == cpu.HL);
    CHECK(0x0003 == cpu.BC);
    CHECK(0x0003 == bus.out_port); CHECK(0x02 == bus.out_byte);
    CHECK(cpu.F & z80::ZF);
}

TEST(ADD_ADC_SBC_16) {
    z80 cpu = init_z80();

    ubyte prog[] = {
        0x21, 0xFC, 0x00,       // LD HL,0x00FC
        0x01, 0x08, 0x00,       // LD BC,0x0008
        0x11, 0xFF, 0xFF,       // LD DE,0xFFFF
        0x09,                   // ADD HL,BC
        0x19,                   // ADD HL,DE
        0xED, 0x4A,             // ADC HL,BC
        0x29,                   // ADD HL,HL
        0x19,                   // ADD HL,DE
        0xED, 0x42,             // SBD HL,BC
        0xDD, 0x21, 0xFC, 0x00, // LD IX,0x00FC
        0x31, 0x00, 0x10,       // LD SP,0x1000
        0xDD, 0x09,             // ADD IX, BC
        0xDD, 0x19,             // ADD IX, DE
        0xDD, 0x29,             // ADD IX, IX
        0xDD, 0x39,             // ADD IX, SP
        0xFD, 0x21, 0xFF, 0xFF, // LD IY,0xFFFF
        0xFD, 0x09,             // ADD IY,BC
        0xFD, 0x19,             // ADD IY,DE
        0xFD, 0x29,             // ADD IY,IY
        0xFD, 0x39,             // ADD IY,SP
    };
    cpu.mem.write(0x0000, prog, sizeof(prog));

    CHECK(10==cpu.step(&bus)); CHECK(0x00FC == cpu.HL);
    CHECK(10==cpu.step(&bus)); CHECK(0x0008 == cpu.BC);
    CHECK(10==cpu.step(&bus)); CHECK(0xFFFF == cpu.DE);
    CHECK(11==cpu.step(&bus)); CHECK(0x0104 == cpu.HL); CHECK(cpu.test_flags(0));
    CHECK(11==cpu.step(&bus)); CHECK(0x0103 == cpu.HL); CHECK(cpu.test_flags(z80::HF|z80::CF));
    CHECK(15==cpu.step(&bus)); CHECK(0x010C == cpu.HL); CHECK(cpu.test_flags(0));
    CHECK(11==cpu.step(&bus)); CHECK(0x0218 == cpu.HL); CHECK(cpu.test_flags(0));
    CHECK(11==cpu.step(&bus)); CHECK(0x0217 == cpu.HL); CHECK(cpu.test_flags(z80::HF|z80::CF));
    CHECK(15==cpu.step(&bus)); CHECK(0x020E == cpu.HL); CHECK(cpu.test_flags(z80::NF));
    CHECK(14==cpu.step(&bus)); CHECK(0x00FC == cpu.IX);
    CHECK(10==cpu.step(&bus)); CHECK(0x1000 == cpu.SP);
    CHECK(15==cpu.step(&bus)); CHECK(0x0104 == cpu.IX); CHECK(cpu.test_flags(0));
    CHECK(15==cpu.step(&bus)); CHECK(0x0103 == cpu.IX); CHECK(cpu.test_flags(z80::HF|z80::CF));
    CHECK(15==cpu.step(&bus)); CHECK(0x0206 == cpu.IX); CHECK(cpu.test_flags(0));
    CHECK(15==cpu.step(&bus)); CHECK(0x1206 == cpu.IX); CHECK(cpu.test_flags(0));
    CHECK(14==cpu.step(&bus)); CHECK(0xFFFF == cpu.IY);
    CHECK(15==cpu.step(&bus)); CHECK(0x0007 == cpu.IY); CHECK(cpu.test_flags(z80::HF|z80::CF));
    CHECK(15==cpu.step(&bus)); CHECK(0x0006 == cpu.IY); CHECK(cpu.test_flags(z80::HF|z80::CF));
    CHECK(15==cpu.step(&bus)); CHECK(0x000C == cpu.IY); CHECK(cpu.test_flags(0));
    CHECK(15==cpu.step(&bus)); CHECK(0x100C == cpu.IY); CHECK(cpu.test_flags(0));
}

TEST(cpu) {

    // setup CPU with a 16 kByte RAM bank at 0x0000
    z80 cpu = init_z80();

    // check initial reset
    cpu.reset();
    CHECK(0 == cpu.PC);
    CHECK(0 == cpu.IM);
    CHECK(!cpu.IFF1);
    CHECK(!cpu.IFF2);
    CHECK(0 == cpu.I);
    CHECK(0 == cpu.R);

    /// run a few nops and check PC and T state counter
    CHECK(4 == cpu.step(&bus));
    CHECK(1 == cpu.PC);
    CHECK(4 == cpu.step(&bus));
    CHECK(2 == cpu.PC);

    /// LD BC,0x1234
    const ubyte f = cpu.F;
    ram0[2] = 0x01; ram0[3] = 0x34; ram0[4] = 0x12;
    CHECK(10 == cpu.step(&bus));
    CHECK(5 == cpu.PC);
    CHECK(0x1234 == cpu.BC);
    CHECK(0x12 == cpu.B);
    CHECK(0x34 == cpu.C);
    CHECK(f == cpu.F);        // does not modify flags
}

