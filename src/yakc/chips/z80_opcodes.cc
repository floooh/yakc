// #version:8#
// machine generated, do not edit!
#include "z80.h"
namespace YAKC {
uint32_t z80::do_op(system_bus* bus) {
  switch (fetch_op()) {
    case 0x0: return cc_op[0x0]; // NOP
    case 0x1: BC=mem.r16(PC); PC+=2; return cc_op[0x1]; // LD BC,nn
    case 0x2: mem.w8(BC,A); Z=C+1; W=A; return cc_op[0x2]; // LD (BC),A
    case 0x3: BC++; return cc_op[0x3]; // INC BC
    case 0x4: B=inc8(B); return cc_op[0x4]; // INC B
    case 0x5: B=dec8(B); return cc_op[0x5]; // DEC B
    case 0x6: B=mem.r8(PC++); return cc_op[0x6]; // LD B,n
    case 0x7: rlca8(); return cc_op[0x7]; // RLCA
    case 0x8: swap16(AF,AF_); return cc_op[0x8]; // EX AF,AF'
    case 0x9: HL=add16(HL,BC); return cc_op[0x9]; // ADD HL,BC
    case 0xa: A=mem.r8(BC); WZ=BC+1; return cc_op[0xa]; // LD A,(BC)
    case 0xb: BC--; return cc_op[0xb]; // DEC BC
    case 0xc: C=inc8(C); return cc_op[0xc]; // INC C
    case 0xd: C=dec8(C); return cc_op[0xd]; // DEC C
    case 0xe: C=mem.r8(PC++); return cc_op[0xe]; // LD C,n
    case 0xf: rrca8(); return cc_op[0xf]; // RRCA
    case 0x10: if (--B>0) { WZ=PC=PC+mem.rs8(PC)+1; return cc_ex[0x10]+cc_op[0x10]; } else { PC++; return cc_op[0x10]; } // DJNZ
    case 0x11: DE=mem.r16(PC); PC+=2; return cc_op[0x11]; // LD DE,nn
    case 0x12: mem.w8(DE,A); Z=E+1; W=A; return cc_op[0x12]; // LD (DE),A
    case 0x13: DE++; return cc_op[0x13]; // INC DE
    case 0x14: D=inc8(D); return cc_op[0x14]; // INC D
    case 0x15: D=dec8(D); return cc_op[0x15]; // DEC D
    case 0x16: D=mem.r8(PC++); return cc_op[0x16]; // LD D,n
    case 0x17: rla8(); return cc_op[0x17]; // RLA
    case 0x18: WZ=PC=PC+mem.rs8(PC)+1; return cc_op[0x18]; // JR d
    case 0x19: HL=add16(HL,DE); return cc_op[0x19]; // ADD HL,DE
    case 0x1a: A=mem.r8(DE); WZ=DE+1; return cc_op[0x1a]; // LD A,(DE)
    case 0x1b: DE--; return cc_op[0x1b]; // DEC DE
    case 0x1c: E=inc8(E); return cc_op[0x1c]; // INC E
    case 0x1d: E=dec8(E); return cc_op[0x1d]; // DEC E
    case 0x1e: E=mem.r8(PC++); return cc_op[0x1e]; // LD E,n
    case 0x1f: rra8(); return cc_op[0x1f]; // RRA
    case 0x20: if (!(F&ZF)) { WZ=PC=PC+mem.rs8(PC)+1; return cc_ex[0x20]+cc_op[0x20]; } else { PC++; return cc_op[0x20]; } // JR NZ,d
    case 0x21: HL=mem.r16(PC); PC+=2; return cc_op[0x21]; // LD HL,nn
    case 0x22: WZ=mem.r16(PC); mem.w16(WZ++,HL); PC+=2; return cc_op[0x22]; // LD (nn),HL
    case 0x23: HL++; return cc_op[0x23]; // INC HL
    case 0x24: H=inc8(H); return cc_op[0x24]; // INC H
    case 0x25: H=dec8(H); return cc_op[0x25]; // DEC H
    case 0x26: H=mem.r8(PC++); return cc_op[0x26]; // LD H,n
    case 0x27: daa(); return cc_op[0x27]; // DAA
    case 0x28: if ((F&ZF)) { WZ=PC=PC+mem.rs8(PC)+1; return cc_ex[0x28]+cc_op[0x28]; } else { PC++; return cc_op[0x28]; } // JR Z,d
    case 0x29: HL=add16(HL,HL); return cc_op[0x29]; // ADD HL,HL
    case 0x2a: WZ=mem.r16(PC); HL=mem.r16(WZ++); PC+=2; return cc_op[0x2a]; // LD HL,(nn)
    case 0x2b: HL--; return cc_op[0x2b]; // DEC HL
    case 0x2c: L=inc8(L); return cc_op[0x2c]; // INC L
    case 0x2d: L=dec8(L); return cc_op[0x2d]; // DEC L
    case 0x2e: L=mem.r8(PC++); return cc_op[0x2e]; // LD L,n
    case 0x2f: A^=0xFF; F=(F&(SF|ZF|PF|CF))|HF|NF|(A&(YF|XF)); return cc_op[0x2f]; // CPL
    case 0x30: if (!(F&CF)) { WZ=PC=PC+mem.rs8(PC)+1; return cc_ex[0x30]+cc_op[0x30]; } else { PC++; return cc_op[0x30]; } // JR NC,d
    case 0x31: SP=mem.r16(PC); PC+=2; return cc_op[0x31]; // LD SP,nn
    case 0x32: WZ=mem.r16(PC); mem.w8(WZ++,A); W=A; PC+=2; return cc_op[0x32]; // LD (nn),A
    case 0x33: SP++; return cc_op[0x33]; // INC SP
    case 0x34: { uint16_t a=HL; mem.w8(a,inc8(mem.r8(a))); } return cc_op[0x34]; // INC (HL)
    case 0x35: { uint16_t a=HL; mem.w8(a,dec8(mem.r8(a))); } return cc_op[0x35]; // DEC (HL)
    case 0x36: { uint16_t a=HL; mem.w8(a,mem.r8(PC++)); } return cc_op[0x36]; // LD (HL),n
    case 0x37: F=(F&(SF|ZF|YF|XF|PF))|CF|(A&(YF|XF)); return cc_op[0x37]; // SCF
    case 0x38: if ((F&CF)) { WZ=PC=PC+mem.rs8(PC)+1; return cc_ex[0x38]+cc_op[0x38]; } else { PC++; return cc_op[0x38]; } // JR C,d
    case 0x39: HL=add16(HL,SP); return cc_op[0x39]; // ADD HL,SP
    case 0x3a: WZ=mem.r16(PC); A=mem.r8(WZ++); PC+=2; return cc_op[0x3a]; // LD A,(nn)
    case 0x3b: SP--; return cc_op[0x3b]; // DEC SP
    case 0x3c: A=inc8(A); return cc_op[0x3c]; // INC A
    case 0x3d: A=dec8(A); return cc_op[0x3d]; // DEC A
    case 0x3e: A=mem.r8(PC++); return cc_op[0x3e]; // LD A,n
    case 0x3f: F=((F&(SF|ZF|YF|XF|PF|CF))|((F&CF)<<4)|(A&(YF|XF)))^CF; return cc_op[0x3f]; // CCF
    case 0x40: B=B; return cc_op[0x40]; // LD B,B
    case 0x41: B=C; return cc_op[0x41]; // LD B,C
    case 0x42: B=D; return cc_op[0x42]; // LD B,D
    case 0x43: B=E; return cc_op[0x43]; // LD B,E
    case 0x44: B=H; return cc_op[0x44]; // LD B,H
    case 0x45: B=L; return cc_op[0x45]; // LD B,L
    case 0x46: { uint16_t a=HL; B=mem.r8(a); } return cc_op[0x46]; // LD B,(HL)
    case 0x47: B=A; return cc_op[0x47]; // LD B,A
    case 0x48: C=B; return cc_op[0x48]; // LD C,B
    case 0x49: C=C; return cc_op[0x49]; // LD C,C
    case 0x4a: C=D; return cc_op[0x4a]; // LD C,D
    case 0x4b: C=E; return cc_op[0x4b]; // LD C,E
    case 0x4c: C=H; return cc_op[0x4c]; // LD C,H
    case 0x4d: C=L; return cc_op[0x4d]; // LD C,L
    case 0x4e: { uint16_t a=HL; C=mem.r8(a); } return cc_op[0x4e]; // LD C,(HL)
    case 0x4f: C=A; return cc_op[0x4f]; // LD C,A
    case 0x50: D=B; return cc_op[0x50]; // LD D,B
    case 0x51: D=C; return cc_op[0x51]; // LD D,C
    case 0x52: D=D; return cc_op[0x52]; // LD D,D
    case 0x53: D=E; return cc_op[0x53]; // LD D,E
    case 0x54: D=H; return cc_op[0x54]; // LD D,H
    case 0x55: D=L; return cc_op[0x55]; // LD D,L
    case 0x56: { uint16_t a=HL; D=mem.r8(a); } return cc_op[0x56]; // LD D,(HL)
    case 0x57: D=A; return cc_op[0x57]; // LD D,A
    case 0x58: E=B; return cc_op[0x58]; // LD E,B
    case 0x59: E=C; return cc_op[0x59]; // LD E,C
    case 0x5a: E=D; return cc_op[0x5a]; // LD E,D
    case 0x5b: E=E; return cc_op[0x5b]; // LD E,E
    case 0x5c: E=H; return cc_op[0x5c]; // LD E,H
    case 0x5d: E=L; return cc_op[0x5d]; // LD E,L
    case 0x5e: { uint16_t a=HL; E=mem.r8(a); } return cc_op[0x5e]; // LD E,(HL)
    case 0x5f: E=A; return cc_op[0x5f]; // LD E,A
    case 0x60: H=B; return cc_op[0x60]; // LD H,B
    case 0x61: H=C; return cc_op[0x61]; // LD H,C
    case 0x62: H=D; return cc_op[0x62]; // LD H,D
    case 0x63: H=E; return cc_op[0x63]; // LD H,E
    case 0x64: H=H; return cc_op[0x64]; // LD H,H
    case 0x65: H=L; return cc_op[0x65]; // LD H,L
    case 0x66: { uint16_t a=HL; H=mem.r8(a); } return cc_op[0x66]; // LD H,(HL)
    case 0x67: H=A; return cc_op[0x67]; // LD H,A
    case 0x68: L=B; return cc_op[0x68]; // LD L,B
    case 0x69: L=C; return cc_op[0x69]; // LD L,C
    case 0x6a: L=D; return cc_op[0x6a]; // LD L,D
    case 0x6b: L=E; return cc_op[0x6b]; // LD L,E
    case 0x6c: L=H; return cc_op[0x6c]; // LD L,H
    case 0x6d: L=L; return cc_op[0x6d]; // LD L,L
    case 0x6e: { uint16_t a=HL; L=mem.r8(a); } return cc_op[0x6e]; // LD L,(HL)
    case 0x6f: L=A; return cc_op[0x6f]; // LD L,A
    case 0x70: { uint16_t a=HL; mem.w8(a,B); } return cc_op[0x70]; // LD (HL),B
    case 0x71: { uint16_t a=HL; mem.w8(a,C); } return cc_op[0x71]; // LD (HL),C
    case 0x72: { uint16_t a=HL; mem.w8(a,D); } return cc_op[0x72]; // LD (HL),D
    case 0x73: { uint16_t a=HL; mem.w8(a,E); } return cc_op[0x73]; // LD (HL),E
    case 0x74: { uint16_t a=HL; mem.w8(a,H); } return cc_op[0x74]; // LD (HL),H
    case 0x75: { uint16_t a=HL; mem.w8(a,L); } return cc_op[0x75]; // LD (HL),L
    case 0x76: halt(); return cc_op[0x76]; // HALT
    case 0x77: { uint16_t a=HL; mem.w8(a,A); } return cc_op[0x77]; // LD (HL),A
    case 0x78: A=B; return cc_op[0x78]; // LD A,B
    case 0x79: A=C; return cc_op[0x79]; // LD A,C
    case 0x7a: A=D; return cc_op[0x7a]; // LD A,D
    case 0x7b: A=E; return cc_op[0x7b]; // LD A,E
    case 0x7c: A=H; return cc_op[0x7c]; // LD A,H
    case 0x7d: A=L; return cc_op[0x7d]; // LD A,L
    case 0x7e: { uint16_t a=HL; A=mem.r8(a); } return cc_op[0x7e]; // LD A,(HL)
    case 0x7f: A=A; return cc_op[0x7f]; // LD A,A
    case 0x80: add8(B); return cc_op[0x80]; // ADD B
    case 0x81: add8(C); return cc_op[0x81]; // ADD C
    case 0x82: add8(D); return cc_op[0x82]; // ADD D
    case 0x83: add8(E); return cc_op[0x83]; // ADD E
    case 0x84: add8(H); return cc_op[0x84]; // ADD H
    case 0x85: add8(L); return cc_op[0x85]; // ADD L
    case 0x86: { uint16_t a=HL; add8(mem.r8(a)); } return cc_op[0x86]; // ADD (HL)
    case 0x87: add8(A); return cc_op[0x87]; // ADD A
    case 0x88: adc8(B); return cc_op[0x88]; // ADC B
    case 0x89: adc8(C); return cc_op[0x89]; // ADC C
    case 0x8a: adc8(D); return cc_op[0x8a]; // ADC D
    case 0x8b: adc8(E); return cc_op[0x8b]; // ADC E
    case 0x8c: adc8(H); return cc_op[0x8c]; // ADC H
    case 0x8d: adc8(L); return cc_op[0x8d]; // ADC L
    case 0x8e: { uint16_t a=HL; adc8(mem.r8(a)); } return cc_op[0x8e]; // ADC (HL)
    case 0x8f: adc8(A); return cc_op[0x8f]; // ADC A
    case 0x90: sub8(B); return cc_op[0x90]; // SUB B
    case 0x91: sub8(C); return cc_op[0x91]; // SUB C
    case 0x92: sub8(D); return cc_op[0x92]; // SUB D
    case 0x93: sub8(E); return cc_op[0x93]; // SUB E
    case 0x94: sub8(H); return cc_op[0x94]; // SUB H
    case 0x95: sub8(L); return cc_op[0x95]; // SUB L
    case 0x96: { uint16_t a=HL; sub8(mem.r8(a)); } return cc_op[0x96]; // SUB (HL)
    case 0x97: sub8(A); return cc_op[0x97]; // SUB A
    case 0x98: sbc8(B); return cc_op[0x98]; // SBC B
    case 0x99: sbc8(C); return cc_op[0x99]; // SBC C
    case 0x9a: sbc8(D); return cc_op[0x9a]; // SBC D
    case 0x9b: sbc8(E); return cc_op[0x9b]; // SBC E
    case 0x9c: sbc8(H); return cc_op[0x9c]; // SBC H
    case 0x9d: sbc8(L); return cc_op[0x9d]; // SBC L
    case 0x9e: { uint16_t a=HL; sbc8(mem.r8(a)); } return cc_op[0x9e]; // SBC (HL)
    case 0x9f: sbc8(A); return cc_op[0x9f]; // SBC A
    case 0xa0: and8(B); return cc_op[0xa0]; // AND B
    case 0xa1: and8(C); return cc_op[0xa1]; // AND C
    case 0xa2: and8(D); return cc_op[0xa2]; // AND D
    case 0xa3: and8(E); return cc_op[0xa3]; // AND E
    case 0xa4: and8(H); return cc_op[0xa4]; // AND H
    case 0xa5: and8(L); return cc_op[0xa5]; // AND L
    case 0xa6: { uint16_t a=HL; and8(mem.r8(a)); } return cc_op[0xa6]; // AND (HL)
    case 0xa7: and8(A); return cc_op[0xa7]; // AND A
    case 0xa8: xor8(B); return cc_op[0xa8]; // XOR B
    case 0xa9: xor8(C); return cc_op[0xa9]; // XOR C
    case 0xaa: xor8(D); return cc_op[0xaa]; // XOR D
    case 0xab: xor8(E); return cc_op[0xab]; // XOR E
    case 0xac: xor8(H); return cc_op[0xac]; // XOR H
    case 0xad: xor8(L); return cc_op[0xad]; // XOR L
    case 0xae: { uint16_t a=HL; xor8(mem.r8(a)); } return cc_op[0xae]; // XOR (HL)
    case 0xaf: xor8(A); return cc_op[0xaf]; // XOR A
    case 0xb0: or8(B); return cc_op[0xb0]; // OR B
    case 0xb1: or8(C); return cc_op[0xb1]; // OR C
    case 0xb2: or8(D); return cc_op[0xb2]; // OR D
    case 0xb3: or8(E); return cc_op[0xb3]; // OR E
    case 0xb4: or8(H); return cc_op[0xb4]; // OR H
    case 0xb5: or8(L); return cc_op[0xb5]; // OR L
    case 0xb6: { uint16_t a=HL; or8(mem.r8(a)); } return cc_op[0xb6]; // OR (HL)
    case 0xb7: or8(A); return cc_op[0xb7]; // OR A
    case 0xb8: cp8(B); return cc_op[0xb8]; // CP B
    case 0xb9: cp8(C); return cc_op[0xb9]; // CP C
    case 0xba: cp8(D); return cc_op[0xba]; // CP D
    case 0xbb: cp8(E); return cc_op[0xbb]; // CP E
    case 0xbc: cp8(H); return cc_op[0xbc]; // CP H
    case 0xbd: cp8(L); return cc_op[0xbd]; // CP L
    case 0xbe: { uint16_t a=HL; cp8(mem.r8(a)); } return cc_op[0xbe]; // CP (HL)
    case 0xbf: cp8(A); return cc_op[0xbf]; // CP A
    case 0xc0: if (!(F&ZF)) { WZ=PC=mem.r16(SP); SP+=2; return cc_ex[0xc0]+cc_op[0xc0]; } else return cc_op[0xc0]; // RET NZ
    case 0xc1: BC=mem.r16(SP); SP+=2; return cc_op[0xc1]; // POP BC
    case 0xc2: WZ=mem.r16(PC); if (!(F&ZF)) { PC=WZ; } else { PC+=2; }; return cc_op[0xc2]; // JP NZ,nn
    case 0xc3: WZ=PC=mem.r16(PC); return cc_op[0xc3]; // JP nn
    case 0xc4: WZ=mem.r16(PC); PC+=2; if (!(F&ZF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return cc_ex[0xc4]+cc_op[0xc4]; } else { return cc_op[0xc4]; } // CALL NZ,nn
    case 0xc5: SP-=2; mem.w16(SP,BC); return cc_op[0xc5]; // PUSH BC
    case 0xc6: add8(mem.r8(PC++)); return cc_op[0xc6]; // ADD n
    case 0xc7: rst(0x0); return cc_op[0xc7]; // RST 0x0
    case 0xc8: if ((F&ZF)) { WZ=PC=mem.r16(SP); SP+=2; return cc_ex[0xc8]+cc_op[0xc8]; } else return cc_op[0xc8]; // RET Z
    case 0xc9: WZ=PC=mem.r16(SP); SP+=2; return cc_op[0xc9]; // RET
    case 0xca: WZ=mem.r16(PC); if ((F&ZF)) { PC=WZ; } else { PC+=2; }; return cc_op[0xca]; // JP Z,nn
    case 0xcb:
      switch (fetch_op()) {
        case 0x0: B=rlc8(B); return cc_cb[0x0]; // RLC B
        case 0x1: C=rlc8(C); return cc_cb[0x1]; // RLC C
        case 0x2: D=rlc8(D); return cc_cb[0x2]; // RLC D
        case 0x3: E=rlc8(E); return cc_cb[0x3]; // RLC E
        case 0x4: H=rlc8(H); return cc_cb[0x4]; // RLC H
        case 0x5: L=rlc8(L); return cc_cb[0x5]; // RLC L
        case 0x6: { uint16_t a=HL; mem.w8(a,rlc8(mem.r8(a))); } return cc_cb[0x6]; // RLC (HL)
        case 0x7: A=rlc8(A); return cc_cb[0x7]; // RLC A
        case 0x8: B=rrc8(B); return cc_cb[0x8]; // RRC B
        case 0x9: C=rrc8(C); return cc_cb[0x9]; // RRC C
        case 0xa: D=rrc8(D); return cc_cb[0xa]; // RRC D
        case 0xb: E=rrc8(E); return cc_cb[0xb]; // RRC E
        case 0xc: H=rrc8(H); return cc_cb[0xc]; // RRC H
        case 0xd: L=rrc8(L); return cc_cb[0xd]; // RRC L
        case 0xe: { uint16_t a=HL; mem.w8(a,rrc8(mem.r8(a))); } return cc_cb[0xe]; // RRC (HL)
        case 0xf: A=rrc8(A); return cc_cb[0xf]; // RRC A
        case 0x10: B=rl8(B); return cc_cb[0x10]; // RL B
        case 0x11: C=rl8(C); return cc_cb[0x11]; // RL C
        case 0x12: D=rl8(D); return cc_cb[0x12]; // RL D
        case 0x13: E=rl8(E); return cc_cb[0x13]; // RL E
        case 0x14: H=rl8(H); return cc_cb[0x14]; // RL H
        case 0x15: L=rl8(L); return cc_cb[0x15]; // RL L
        case 0x16: { uint16_t a=HL; mem.w8(a,rl8(mem.r8(a))); } return cc_cb[0x16]; // RL (HL)
        case 0x17: A=rl8(A); return cc_cb[0x17]; // RL A
        case 0x18: B=rr8(B); return cc_cb[0x18]; // RR B
        case 0x19: C=rr8(C); return cc_cb[0x19]; // RR C
        case 0x1a: D=rr8(D); return cc_cb[0x1a]; // RR D
        case 0x1b: E=rr8(E); return cc_cb[0x1b]; // RR E
        case 0x1c: H=rr8(H); return cc_cb[0x1c]; // RR H
        case 0x1d: L=rr8(L); return cc_cb[0x1d]; // RR L
        case 0x1e: { uint16_t a=HL; mem.w8(a,rr8(mem.r8(a))); } return cc_cb[0x1e]; // RR (HL)
        case 0x1f: A=rr8(A); return cc_cb[0x1f]; // RR A
        case 0x20: B=sla8(B); return cc_cb[0x20]; // SLA B
        case 0x21: C=sla8(C); return cc_cb[0x21]; // SLA C
        case 0x22: D=sla8(D); return cc_cb[0x22]; // SLA D
        case 0x23: E=sla8(E); return cc_cb[0x23]; // SLA E
        case 0x24: H=sla8(H); return cc_cb[0x24]; // SLA H
        case 0x25: L=sla8(L); return cc_cb[0x25]; // SLA L
        case 0x26: { uint16_t a=HL; mem.w8(a,sla8(mem.r8(a))); } return cc_cb[0x26]; // SLA (HL)
        case 0x27: A=sla8(A); return cc_cb[0x27]; // SLA A
        case 0x28: B=sra8(B); return cc_cb[0x28]; // SRA B
        case 0x29: C=sra8(C); return cc_cb[0x29]; // SRA C
        case 0x2a: D=sra8(D); return cc_cb[0x2a]; // SRA D
        case 0x2b: E=sra8(E); return cc_cb[0x2b]; // SRA E
        case 0x2c: H=sra8(H); return cc_cb[0x2c]; // SRA H
        case 0x2d: L=sra8(L); return cc_cb[0x2d]; // SRA L
        case 0x2e: { uint16_t a=HL; mem.w8(a,sra8(mem.r8(a))); } return cc_cb[0x2e]; // SRA (HL)
        case 0x2f: A=sra8(A); return cc_cb[0x2f]; // SRA A
        case 0x30: B=sll8(B); return cc_cb[0x30]; // SLL B
        case 0x31: C=sll8(C); return cc_cb[0x31]; // SLL C
        case 0x32: D=sll8(D); return cc_cb[0x32]; // SLL D
        case 0x33: E=sll8(E); return cc_cb[0x33]; // SLL E
        case 0x34: H=sll8(H); return cc_cb[0x34]; // SLL H
        case 0x35: L=sll8(L); return cc_cb[0x35]; // SLL L
        case 0x36: { uint16_t a=HL; mem.w8(a,sll8(mem.r8(a))); } return cc_cb[0x36]; // SLL (HL)
        case 0x37: A=sll8(A); return cc_cb[0x37]; // SLL A
        case 0x38: B=srl8(B); return cc_cb[0x38]; // SRL B
        case 0x39: C=srl8(C); return cc_cb[0x39]; // SRL C
        case 0x3a: D=srl8(D); return cc_cb[0x3a]; // SRL D
        case 0x3b: E=srl8(E); return cc_cb[0x3b]; // SRL E
        case 0x3c: H=srl8(H); return cc_cb[0x3c]; // SRL H
        case 0x3d: L=srl8(L); return cc_cb[0x3d]; // SRL L
        case 0x3e: { uint16_t a=HL; mem.w8(a,srl8(mem.r8(a))); } return cc_cb[0x3e]; // SRL (HL)
        case 0x3f: A=srl8(A); return cc_cb[0x3f]; // SRL A
        case 0x40: bit(B,0x1); return cc_cb[0x40]; // BIT 0,B
        case 0x41: bit(C,0x1); return cc_cb[0x41]; // BIT 0,C
        case 0x42: bit(D,0x1); return cc_cb[0x42]; // BIT 0,D
        case 0x43: bit(E,0x1); return cc_cb[0x43]; // BIT 0,E
        case 0x44: bit(H,0x1); return cc_cb[0x44]; // BIT 0,H
        case 0x45: bit(L,0x1); return cc_cb[0x45]; // BIT 0,L
        case 0x46: { uint16_t a=HL; ibit(mem.r8(a),0x1); } return cc_cb[0x46]; // BIT 0,(HL)
        case 0x47: bit(A,0x1); return cc_cb[0x47]; // BIT 0,A
        case 0x48: bit(B,0x2); return cc_cb[0x48]; // BIT 1,B
        case 0x49: bit(C,0x2); return cc_cb[0x49]; // BIT 1,C
        case 0x4a: bit(D,0x2); return cc_cb[0x4a]; // BIT 1,D
        case 0x4b: bit(E,0x2); return cc_cb[0x4b]; // BIT 1,E
        case 0x4c: bit(H,0x2); return cc_cb[0x4c]; // BIT 1,H
        case 0x4d: bit(L,0x2); return cc_cb[0x4d]; // BIT 1,L
        case 0x4e: { uint16_t a=HL; ibit(mem.r8(a),0x2); } return cc_cb[0x4e]; // BIT 1,(HL)
        case 0x4f: bit(A,0x2); return cc_cb[0x4f]; // BIT 1,A
        case 0x50: bit(B,0x4); return cc_cb[0x50]; // BIT 2,B
        case 0x51: bit(C,0x4); return cc_cb[0x51]; // BIT 2,C
        case 0x52: bit(D,0x4); return cc_cb[0x52]; // BIT 2,D
        case 0x53: bit(E,0x4); return cc_cb[0x53]; // BIT 2,E
        case 0x54: bit(H,0x4); return cc_cb[0x54]; // BIT 2,H
        case 0x55: bit(L,0x4); return cc_cb[0x55]; // BIT 2,L
        case 0x56: { uint16_t a=HL; ibit(mem.r8(a),0x4); } return cc_cb[0x56]; // BIT 2,(HL)
        case 0x57: bit(A,0x4); return cc_cb[0x57]; // BIT 2,A
        case 0x58: bit(B,0x8); return cc_cb[0x58]; // BIT 3,B
        case 0x59: bit(C,0x8); return cc_cb[0x59]; // BIT 3,C
        case 0x5a: bit(D,0x8); return cc_cb[0x5a]; // BIT 3,D
        case 0x5b: bit(E,0x8); return cc_cb[0x5b]; // BIT 3,E
        case 0x5c: bit(H,0x8); return cc_cb[0x5c]; // BIT 3,H
        case 0x5d: bit(L,0x8); return cc_cb[0x5d]; // BIT 3,L
        case 0x5e: { uint16_t a=HL; ibit(mem.r8(a),0x8); } return cc_cb[0x5e]; // BIT 3,(HL)
        case 0x5f: bit(A,0x8); return cc_cb[0x5f]; // BIT 3,A
        case 0x60: bit(B,0x10); return cc_cb[0x60]; // BIT 4,B
        case 0x61: bit(C,0x10); return cc_cb[0x61]; // BIT 4,C
        case 0x62: bit(D,0x10); return cc_cb[0x62]; // BIT 4,D
        case 0x63: bit(E,0x10); return cc_cb[0x63]; // BIT 4,E
        case 0x64: bit(H,0x10); return cc_cb[0x64]; // BIT 4,H
        case 0x65: bit(L,0x10); return cc_cb[0x65]; // BIT 4,L
        case 0x66: { uint16_t a=HL; ibit(mem.r8(a),0x10); } return cc_cb[0x66]; // BIT 4,(HL)
        case 0x67: bit(A,0x10); return cc_cb[0x67]; // BIT 4,A
        case 0x68: bit(B,0x20); return cc_cb[0x68]; // BIT 5,B
        case 0x69: bit(C,0x20); return cc_cb[0x69]; // BIT 5,C
        case 0x6a: bit(D,0x20); return cc_cb[0x6a]; // BIT 5,D
        case 0x6b: bit(E,0x20); return cc_cb[0x6b]; // BIT 5,E
        case 0x6c: bit(H,0x20); return cc_cb[0x6c]; // BIT 5,H
        case 0x6d: bit(L,0x20); return cc_cb[0x6d]; // BIT 5,L
        case 0x6e: { uint16_t a=HL; ibit(mem.r8(a),0x20); } return cc_cb[0x6e]; // BIT 5,(HL)
        case 0x6f: bit(A,0x20); return cc_cb[0x6f]; // BIT 5,A
        case 0x70: bit(B,0x40); return cc_cb[0x70]; // BIT 6,B
        case 0x71: bit(C,0x40); return cc_cb[0x71]; // BIT 6,C
        case 0x72: bit(D,0x40); return cc_cb[0x72]; // BIT 6,D
        case 0x73: bit(E,0x40); return cc_cb[0x73]; // BIT 6,E
        case 0x74: bit(H,0x40); return cc_cb[0x74]; // BIT 6,H
        case 0x75: bit(L,0x40); return cc_cb[0x75]; // BIT 6,L
        case 0x76: { uint16_t a=HL; ibit(mem.r8(a),0x40); } return cc_cb[0x76]; // BIT 6,(HL)
        case 0x77: bit(A,0x40); return cc_cb[0x77]; // BIT 6,A
        case 0x78: bit(B,0x80); return cc_cb[0x78]; // BIT 7,B
        case 0x79: bit(C,0x80); return cc_cb[0x79]; // BIT 7,C
        case 0x7a: bit(D,0x80); return cc_cb[0x7a]; // BIT 7,D
        case 0x7b: bit(E,0x80); return cc_cb[0x7b]; // BIT 7,E
        case 0x7c: bit(H,0x80); return cc_cb[0x7c]; // BIT 7,H
        case 0x7d: bit(L,0x80); return cc_cb[0x7d]; // BIT 7,L
        case 0x7e: { uint16_t a=HL; ibit(mem.r8(a),0x80); } return cc_cb[0x7e]; // BIT 7,(HL)
        case 0x7f: bit(A,0x80); return cc_cb[0x7f]; // BIT 7,A
        case 0x80: B&=~0x1; return cc_cb[0x80]; // RES 0,B
        case 0x81: C&=~0x1; return cc_cb[0x81]; // RES 0,C
        case 0x82: D&=~0x1; return cc_cb[0x82]; // RES 0,D
        case 0x83: E&=~0x1; return cc_cb[0x83]; // RES 0,E
        case 0x84: H&=~0x1; return cc_cb[0x84]; // RES 0,H
        case 0x85: L&=~0x1; return cc_cb[0x85]; // RES 0,L
        case 0x86: { uint16_t a=HL; mem.w8(a,mem.r8(a)&~0x1); } return cc_cb[0x86]; // RES 0,(HL)
        case 0x87: A&=~0x1; return cc_cb[0x87]; // RES 0,A
        case 0x88: B&=~0x2; return cc_cb[0x88]; // RES 1,B
        case 0x89: C&=~0x2; return cc_cb[0x89]; // RES 1,C
        case 0x8a: D&=~0x2; return cc_cb[0x8a]; // RES 1,D
        case 0x8b: E&=~0x2; return cc_cb[0x8b]; // RES 1,E
        case 0x8c: H&=~0x2; return cc_cb[0x8c]; // RES 1,H
        case 0x8d: L&=~0x2; return cc_cb[0x8d]; // RES 1,L
        case 0x8e: { uint16_t a=HL; mem.w8(a,mem.r8(a)&~0x2); } return cc_cb[0x8e]; // RES 1,(HL)
        case 0x8f: A&=~0x2; return cc_cb[0x8f]; // RES 1,A
        case 0x90: B&=~0x4; return cc_cb[0x90]; // RES 2,B
        case 0x91: C&=~0x4; return cc_cb[0x91]; // RES 2,C
        case 0x92: D&=~0x4; return cc_cb[0x92]; // RES 2,D
        case 0x93: E&=~0x4; return cc_cb[0x93]; // RES 2,E
        case 0x94: H&=~0x4; return cc_cb[0x94]; // RES 2,H
        case 0x95: L&=~0x4; return cc_cb[0x95]; // RES 2,L
        case 0x96: { uint16_t a=HL; mem.w8(a,mem.r8(a)&~0x4); } return cc_cb[0x96]; // RES 2,(HL)
        case 0x97: A&=~0x4; return cc_cb[0x97]; // RES 2,A
        case 0x98: B&=~0x8; return cc_cb[0x98]; // RES 3,B
        case 0x99: C&=~0x8; return cc_cb[0x99]; // RES 3,C
        case 0x9a: D&=~0x8; return cc_cb[0x9a]; // RES 3,D
        case 0x9b: E&=~0x8; return cc_cb[0x9b]; // RES 3,E
        case 0x9c: H&=~0x8; return cc_cb[0x9c]; // RES 3,H
        case 0x9d: L&=~0x8; return cc_cb[0x9d]; // RES 3,L
        case 0x9e: { uint16_t a=HL; mem.w8(a,mem.r8(a)&~0x8); } return cc_cb[0x9e]; // RES 3,(HL)
        case 0x9f: A&=~0x8; return cc_cb[0x9f]; // RES 3,A
        case 0xa0: B&=~0x10; return cc_cb[0xa0]; // RES 4,B
        case 0xa1: C&=~0x10; return cc_cb[0xa1]; // RES 4,C
        case 0xa2: D&=~0x10; return cc_cb[0xa2]; // RES 4,D
        case 0xa3: E&=~0x10; return cc_cb[0xa3]; // RES 4,E
        case 0xa4: H&=~0x10; return cc_cb[0xa4]; // RES 4,H
        case 0xa5: L&=~0x10; return cc_cb[0xa5]; // RES 4,L
        case 0xa6: { uint16_t a=HL; mem.w8(a,mem.r8(a)&~0x10); } return cc_cb[0xa6]; // RES 4,(HL)
        case 0xa7: A&=~0x10; return cc_cb[0xa7]; // RES 4,A
        case 0xa8: B&=~0x20; return cc_cb[0xa8]; // RES 5,B
        case 0xa9: C&=~0x20; return cc_cb[0xa9]; // RES 5,C
        case 0xaa: D&=~0x20; return cc_cb[0xaa]; // RES 5,D
        case 0xab: E&=~0x20; return cc_cb[0xab]; // RES 5,E
        case 0xac: H&=~0x20; return cc_cb[0xac]; // RES 5,H
        case 0xad: L&=~0x20; return cc_cb[0xad]; // RES 5,L
        case 0xae: { uint16_t a=HL; mem.w8(a,mem.r8(a)&~0x20); } return cc_cb[0xae]; // RES 5,(HL)
        case 0xaf: A&=~0x20; return cc_cb[0xaf]; // RES 5,A
        case 0xb0: B&=~0x40; return cc_cb[0xb0]; // RES 6,B
        case 0xb1: C&=~0x40; return cc_cb[0xb1]; // RES 6,C
        case 0xb2: D&=~0x40; return cc_cb[0xb2]; // RES 6,D
        case 0xb3: E&=~0x40; return cc_cb[0xb3]; // RES 6,E
        case 0xb4: H&=~0x40; return cc_cb[0xb4]; // RES 6,H
        case 0xb5: L&=~0x40; return cc_cb[0xb5]; // RES 6,L
        case 0xb6: { uint16_t a=HL; mem.w8(a,mem.r8(a)&~0x40); } return cc_cb[0xb6]; // RES 6,(HL)
        case 0xb7: A&=~0x40; return cc_cb[0xb7]; // RES 6,A
        case 0xb8: B&=~0x80; return cc_cb[0xb8]; // RES 7,B
        case 0xb9: C&=~0x80; return cc_cb[0xb9]; // RES 7,C
        case 0xba: D&=~0x80; return cc_cb[0xba]; // RES 7,D
        case 0xbb: E&=~0x80; return cc_cb[0xbb]; // RES 7,E
        case 0xbc: H&=~0x80; return cc_cb[0xbc]; // RES 7,H
        case 0xbd: L&=~0x80; return cc_cb[0xbd]; // RES 7,L
        case 0xbe: { uint16_t a=HL; mem.w8(a,mem.r8(a)&~0x80); } return cc_cb[0xbe]; // RES 7,(HL)
        case 0xbf: A&=~0x80; return cc_cb[0xbf]; // RES 7,A
        case 0xc0: B|=0x1; return cc_cb[0xc0]; // SET 0,B
        case 0xc1: C|=0x1; return cc_cb[0xc1]; // SET 0,C
        case 0xc2: D|=0x1; return cc_cb[0xc2]; // SET 0,D
        case 0xc3: E|=0x1; return cc_cb[0xc3]; // SET 0,E
        case 0xc4: H|=0x1; return cc_cb[0xc4]; // SET 0,H
        case 0xc5: L|=0x1; return cc_cb[0xc5]; // SET 0,L
        case 0xc6: { uint16_t a=HL; mem.w8(a,mem.r8(a)|0x1);} return cc_cb[0xc6]; // SET 0,(HL)
        case 0xc7: A|=0x1; return cc_cb[0xc7]; // SET 0,A
        case 0xc8: B|=0x2; return cc_cb[0xc8]; // SET 1,B
        case 0xc9: C|=0x2; return cc_cb[0xc9]; // SET 1,C
        case 0xca: D|=0x2; return cc_cb[0xca]; // SET 1,D
        case 0xcb: E|=0x2; return cc_cb[0xcb]; // SET 1,E
        case 0xcc: H|=0x2; return cc_cb[0xcc]; // SET 1,H
        case 0xcd: L|=0x2; return cc_cb[0xcd]; // SET 1,L
        case 0xce: { uint16_t a=HL; mem.w8(a,mem.r8(a)|0x2);} return cc_cb[0xce]; // SET 1,(HL)
        case 0xcf: A|=0x2; return cc_cb[0xcf]; // SET 1,A
        case 0xd0: B|=0x4; return cc_cb[0xd0]; // SET 2,B
        case 0xd1: C|=0x4; return cc_cb[0xd1]; // SET 2,C
        case 0xd2: D|=0x4; return cc_cb[0xd2]; // SET 2,D
        case 0xd3: E|=0x4; return cc_cb[0xd3]; // SET 2,E
        case 0xd4: H|=0x4; return cc_cb[0xd4]; // SET 2,H
        case 0xd5: L|=0x4; return cc_cb[0xd5]; // SET 2,L
        case 0xd6: { uint16_t a=HL; mem.w8(a,mem.r8(a)|0x4);} return cc_cb[0xd6]; // SET 2,(HL)
        case 0xd7: A|=0x4; return cc_cb[0xd7]; // SET 2,A
        case 0xd8: B|=0x8; return cc_cb[0xd8]; // SET 3,B
        case 0xd9: C|=0x8; return cc_cb[0xd9]; // SET 3,C
        case 0xda: D|=0x8; return cc_cb[0xda]; // SET 3,D
        case 0xdb: E|=0x8; return cc_cb[0xdb]; // SET 3,E
        case 0xdc: H|=0x8; return cc_cb[0xdc]; // SET 3,H
        case 0xdd: L|=0x8; return cc_cb[0xdd]; // SET 3,L
        case 0xde: { uint16_t a=HL; mem.w8(a,mem.r8(a)|0x8);} return cc_cb[0xde]; // SET 3,(HL)
        case 0xdf: A|=0x8; return cc_cb[0xdf]; // SET 3,A
        case 0xe0: B|=0x10; return cc_cb[0xe0]; // SET 4,B
        case 0xe1: C|=0x10; return cc_cb[0xe1]; // SET 4,C
        case 0xe2: D|=0x10; return cc_cb[0xe2]; // SET 4,D
        case 0xe3: E|=0x10; return cc_cb[0xe3]; // SET 4,E
        case 0xe4: H|=0x10; return cc_cb[0xe4]; // SET 4,H
        case 0xe5: L|=0x10; return cc_cb[0xe5]; // SET 4,L
        case 0xe6: { uint16_t a=HL; mem.w8(a,mem.r8(a)|0x10);} return cc_cb[0xe6]; // SET 4,(HL)
        case 0xe7: A|=0x10; return cc_cb[0xe7]; // SET 4,A
        case 0xe8: B|=0x20; return cc_cb[0xe8]; // SET 5,B
        case 0xe9: C|=0x20; return cc_cb[0xe9]; // SET 5,C
        case 0xea: D|=0x20; return cc_cb[0xea]; // SET 5,D
        case 0xeb: E|=0x20; return cc_cb[0xeb]; // SET 5,E
        case 0xec: H|=0x20; return cc_cb[0xec]; // SET 5,H
        case 0xed: L|=0x20; return cc_cb[0xed]; // SET 5,L
        case 0xee: { uint16_t a=HL; mem.w8(a,mem.r8(a)|0x20);} return cc_cb[0xee]; // SET 5,(HL)
        case 0xef: A|=0x20; return cc_cb[0xef]; // SET 5,A
        case 0xf0: B|=0x40; return cc_cb[0xf0]; // SET 6,B
        case 0xf1: C|=0x40; return cc_cb[0xf1]; // SET 6,C
        case 0xf2: D|=0x40; return cc_cb[0xf2]; // SET 6,D
        case 0xf3: E|=0x40; return cc_cb[0xf3]; // SET 6,E
        case 0xf4: H|=0x40; return cc_cb[0xf4]; // SET 6,H
        case 0xf5: L|=0x40; return cc_cb[0xf5]; // SET 6,L
        case 0xf6: { uint16_t a=HL; mem.w8(a,mem.r8(a)|0x40);} return cc_cb[0xf6]; // SET 6,(HL)
        case 0xf7: A|=0x40; return cc_cb[0xf7]; // SET 6,A
        case 0xf8: B|=0x80; return cc_cb[0xf8]; // SET 7,B
        case 0xf9: C|=0x80; return cc_cb[0xf9]; // SET 7,C
        case 0xfa: D|=0x80; return cc_cb[0xfa]; // SET 7,D
        case 0xfb: E|=0x80; return cc_cb[0xfb]; // SET 7,E
        case 0xfc: H|=0x80; return cc_cb[0xfc]; // SET 7,H
        case 0xfd: L|=0x80; return cc_cb[0xfd]; // SET 7,L
        case 0xfe: { uint16_t a=HL; mem.w8(a,mem.r8(a)|0x80);} return cc_cb[0xfe]; // SET 7,(HL)
        case 0xff: A|=0x80; return cc_cb[0xff]; // SET 7,A
        default: return invalid_opcode(2);
      }
      break;
    case 0xcc: WZ=mem.r16(PC); PC+=2; if ((F&ZF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return cc_ex[0xcc]+cc_op[0xcc]; } else { return cc_op[0xcc]; } // CALL Z,nn
    case 0xcd: SP-=2; mem.w16(SP,PC+2); WZ=PC=mem.r16(PC); return cc_op[0xcd]; // CALL nn
    case 0xce: adc8(mem.r8(PC++)); return cc_op[0xce]; // ADC n
    case 0xcf: rst(0x8); return cc_op[0xcf]; // RST 0x8
    case 0xd0: if (!(F&CF)) { WZ=PC=mem.r16(SP); SP+=2; return cc_ex[0xd0]+cc_op[0xd0]; } else return cc_op[0xd0]; // RET NC
    case 0xd1: DE=mem.r16(SP); SP+=2; return cc_op[0xd1]; // POP DE
    case 0xd2: WZ=mem.r16(PC); if (!(F&CF)) { PC=WZ; } else { PC+=2; }; return cc_op[0xd2]; // JP NC,nn
    case 0xd3: out(bus, (A<<8)|mem.r8(PC++),A); return cc_op[0xd3]; // OUT (n),A
    case 0xd4: WZ=mem.r16(PC); PC+=2; if (!(F&CF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return cc_ex[0xd4]+cc_op[0xd4]; } else { return cc_op[0xd4]; } // CALL NC,nn
    case 0xd5: SP-=2; mem.w16(SP,DE); return cc_op[0xd5]; // PUSH DE
    case 0xd6: sub8(mem.r8(PC++)); return cc_op[0xd6]; // SUB n
    case 0xd7: rst(0x10); return cc_op[0xd7]; // RST 0x10
    case 0xd8: if ((F&CF)) { WZ=PC=mem.r16(SP); SP+=2; return cc_ex[0xd8]+cc_op[0xd8]; } else return cc_op[0xd8]; // RET C
    case 0xd9: swap16(BC,BC_); swap16(DE,DE_); swap16(HL,HL_); swap16(WZ,WZ_); return cc_op[0xd9]; // EXX
    case 0xda: WZ=mem.r16(PC); if ((F&CF)) { PC=WZ; } else { PC+=2; }; return cc_op[0xda]; // JP C,nn
    case 0xdb: A=in(bus, (A<<8)|mem.r8(PC++)); return cc_op[0xdb]; // IN A,(n)
    case 0xdc: WZ=mem.r16(PC); PC+=2; if ((F&CF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return cc_ex[0xdc]+cc_op[0xdc]; } else { return cc_op[0xdc]; } // CALL C,nn
    case 0xdd:
      switch (fetch_op()) {
        case 0x0: return cc_dd[0x0]; // NOP
        case 0x1: BC=mem.r16(PC); PC+=2; return cc_dd[0x1]; // LD BC,nn
        case 0x2: mem.w8(BC,A); Z=C+1; W=A; return cc_dd[0x2]; // LD (BC),A
        case 0x3: BC++; return cc_dd[0x3]; // INC BC
        case 0x4: B=inc8(B); return cc_dd[0x4]; // INC B
        case 0x5: B=dec8(B); return cc_dd[0x5]; // DEC B
        case 0x6: B=mem.r8(PC++); return cc_dd[0x6]; // LD B,n
        case 0x7: rlca8(); return cc_dd[0x7]; // RLCA
        case 0x8: swap16(AF,AF_); return cc_dd[0x8]; // EX AF,AF'
        case 0x9: IX=add16(IX,BC); return cc_dd[0x9]; // ADD IX,BC
        case 0xa: A=mem.r8(BC); WZ=BC+1; return cc_dd[0xa]; // LD A,(BC)
        case 0xb: BC--; return cc_dd[0xb]; // DEC BC
        case 0xc: C=inc8(C); return cc_dd[0xc]; // INC C
        case 0xd: C=dec8(C); return cc_dd[0xd]; // DEC C
        case 0xe: C=mem.r8(PC++); return cc_dd[0xe]; // LD C,n
        case 0xf: rrca8(); return cc_dd[0xf]; // RRCA
        case 0x10: if (--B>0) { WZ=PC=PC+mem.rs8(PC)+1; return cc_ex[0x10]+cc_dd[0x10]; } else { PC++; return cc_dd[0x10]; } // DJNZ
        case 0x11: DE=mem.r16(PC); PC+=2; return cc_dd[0x11]; // LD DE,nn
        case 0x12: mem.w8(DE,A); Z=E+1; W=A; return cc_dd[0x12]; // LD (DE),A
        case 0x13: DE++; return cc_dd[0x13]; // INC DE
        case 0x14: D=inc8(D); return cc_dd[0x14]; // INC D
        case 0x15: D=dec8(D); return cc_dd[0x15]; // DEC D
        case 0x16: D=mem.r8(PC++); return cc_dd[0x16]; // LD D,n
        case 0x17: rla8(); return cc_dd[0x17]; // RLA
        case 0x18: WZ=PC=PC+mem.rs8(PC)+1; return cc_dd[0x18]; // JR d
        case 0x19: IX=add16(IX,DE); return cc_dd[0x19]; // ADD IX,DE
        case 0x1a: A=mem.r8(DE); WZ=DE+1; return cc_dd[0x1a]; // LD A,(DE)
        case 0x1b: DE--; return cc_dd[0x1b]; // DEC DE
        case 0x1c: E=inc8(E); return cc_dd[0x1c]; // INC E
        case 0x1d: E=dec8(E); return cc_dd[0x1d]; // DEC E
        case 0x1e: E=mem.r8(PC++); return cc_dd[0x1e]; // LD E,n
        case 0x1f: rra8(); return cc_dd[0x1f]; // RRA
        case 0x20: if (!(F&ZF)) { WZ=PC=PC+mem.rs8(PC)+1; return cc_ex[0x20]+cc_dd[0x20]; } else { PC++; return cc_dd[0x20]; } // JR NZ,d
        case 0x21: IX=mem.r16(PC); PC+=2; return cc_dd[0x21]; // LD IX,nn
        case 0x22: WZ=mem.r16(PC); mem.w16(WZ++,IX); PC+=2; return cc_dd[0x22]; // LD (nn),IX
        case 0x23: IX++; return cc_dd[0x23]; // INC IX
        case 0x24: IXH=inc8(IXH); return cc_dd[0x24]; // INC IXH
        case 0x25: IXH=dec8(IXH); return cc_dd[0x25]; // DEC IXH
        case 0x26: IXH=mem.r8(PC++); return cc_dd[0x26]; // LD IXH,n
        case 0x27: daa(); return cc_dd[0x27]; // DAA
        case 0x28: if ((F&ZF)) { WZ=PC=PC+mem.rs8(PC)+1; return cc_ex[0x28]+cc_dd[0x28]; } else { PC++; return cc_dd[0x28]; } // JR Z,d
        case 0x29: IX=add16(IX,IX); return cc_dd[0x29]; // ADD IX,IX
        case 0x2a: WZ=mem.r16(PC); IX=mem.r16(WZ++); PC+=2; return cc_dd[0x2a]; // LD IX,(nn)
        case 0x2b: IX--; return cc_dd[0x2b]; // DEC IX
        case 0x2c: IXL=inc8(IXL); return cc_dd[0x2c]; // INC IXL
        case 0x2d: IXL=dec8(IXL); return cc_dd[0x2d]; // DEC IXL
        case 0x2e: IXL=mem.r8(PC++); return cc_dd[0x2e]; // LD IXL,n
        case 0x2f: A^=0xFF; F=(F&(SF|ZF|PF|CF))|HF|NF|(A&(YF|XF)); return cc_dd[0x2f]; // CPL
        case 0x30: if (!(F&CF)) { WZ=PC=PC+mem.rs8(PC)+1; return cc_ex[0x30]+cc_dd[0x30]; } else { PC++; return cc_dd[0x30]; } // JR NC,d
        case 0x31: SP=mem.r16(PC); PC+=2; return cc_dd[0x31]; // LD SP,nn
        case 0x32: WZ=mem.r16(PC); mem.w8(WZ++,A); W=A; PC+=2; return cc_dd[0x32]; // LD (nn),A
        case 0x33: SP++; return cc_dd[0x33]; // INC SP
        case 0x34: { uint16_t a=WZ=IX+mem.rs8(PC++); mem.w8(a,inc8(mem.r8(a))); } return cc_dd[0x34]; // INC (IX+d)
        case 0x35: { uint16_t a=WZ=IX+mem.rs8(PC++); mem.w8(a,dec8(mem.r8(a))); } return cc_dd[0x35]; // DEC (IX+d)
        case 0x36: { uint16_t a=WZ=IX+mem.rs8(PC++); mem.w8(a,mem.r8(PC++)); } return cc_dd[0x36]; // LD (IX+d),n
        case 0x37: F=(F&(SF|ZF|YF|XF|PF))|CF|(A&(YF|XF)); return cc_dd[0x37]; // SCF
        case 0x38: if ((F&CF)) { WZ=PC=PC+mem.rs8(PC)+1; return cc_ex[0x38]+cc_dd[0x38]; } else { PC++; return cc_dd[0x38]; } // JR C,d
        case 0x39: IX=add16(IX,SP); return cc_dd[0x39]; // ADD IX,SP
        case 0x3a: WZ=mem.r16(PC); A=mem.r8(WZ++); PC+=2; return cc_dd[0x3a]; // LD A,(nn)
        case 0x3b: SP--; return cc_dd[0x3b]; // DEC SP
        case 0x3c: A=inc8(A); return cc_dd[0x3c]; // INC A
        case 0x3d: A=dec8(A); return cc_dd[0x3d]; // DEC A
        case 0x3e: A=mem.r8(PC++); return cc_dd[0x3e]; // LD A,n
        case 0x3f: F=((F&(SF|ZF|YF|XF|PF|CF))|((F&CF)<<4)|(A&(YF|XF)))^CF; return cc_dd[0x3f]; // CCF
        case 0x40: B=B; return cc_dd[0x40]; // LD B,B
        case 0x41: B=C; return cc_dd[0x41]; // LD B,C
        case 0x42: B=D; return cc_dd[0x42]; // LD B,D
        case 0x43: B=E; return cc_dd[0x43]; // LD B,E
        case 0x44: B=IXH; return cc_dd[0x44]; // LD B,IXH
        case 0x45: B=IXL; return cc_dd[0x45]; // LD B,IXL
        case 0x46: { uint16_t a=WZ=IX+mem.rs8(PC++); B=mem.r8(a); } return cc_dd[0x46]; // LD B,(IX+d)
        case 0x47: B=A; return cc_dd[0x47]; // LD B,A
        case 0x48: C=B; return cc_dd[0x48]; // LD C,B
        case 0x49: C=C; return cc_dd[0x49]; // LD C,C
        case 0x4a: C=D; return cc_dd[0x4a]; // LD C,D
        case 0x4b: C=E; return cc_dd[0x4b]; // LD C,E
        case 0x4c: C=IXH; return cc_dd[0x4c]; // LD C,IXH
        case 0x4d: C=IXL; return cc_dd[0x4d]; // LD C,IXL
        case 0x4e: { uint16_t a=WZ=IX+mem.rs8(PC++); C=mem.r8(a); } return cc_dd[0x4e]; // LD C,(IX+d)
        case 0x4f: C=A; return cc_dd[0x4f]; // LD C,A
        case 0x50: D=B; return cc_dd[0x50]; // LD D,B
        case 0x51: D=C; return cc_dd[0x51]; // LD D,C
        case 0x52: D=D; return cc_dd[0x52]; // LD D,D
        case 0x53: D=E; return cc_dd[0x53]; // LD D,E
        case 0x54: D=IXH; return cc_dd[0x54]; // LD D,IXH
        case 0x55: D=IXL; return cc_dd[0x55]; // LD D,IXL
        case 0x56: { uint16_t a=WZ=IX+mem.rs8(PC++); D=mem.r8(a); } return cc_dd[0x56]; // LD D,(IX+d)
        case 0x57: D=A; return cc_dd[0x57]; // LD D,A
        case 0x58: E=B; return cc_dd[0x58]; // LD E,B
        case 0x59: E=C; return cc_dd[0x59]; // LD E,C
        case 0x5a: E=D; return cc_dd[0x5a]; // LD E,D
        case 0x5b: E=E; return cc_dd[0x5b]; // LD E,E
        case 0x5c: E=IXH; return cc_dd[0x5c]; // LD E,IXH
        case 0x5d: E=IXL; return cc_dd[0x5d]; // LD E,IXL
        case 0x5e: { uint16_t a=WZ=IX+mem.rs8(PC++); E=mem.r8(a); } return cc_dd[0x5e]; // LD E,(IX+d)
        case 0x5f: E=A; return cc_dd[0x5f]; // LD E,A
        case 0x60: IXH=B; return cc_dd[0x60]; // LD IXH,B
        case 0x61: IXH=C; return cc_dd[0x61]; // LD IXH,C
        case 0x62: IXH=D; return cc_dd[0x62]; // LD IXH,D
        case 0x63: IXH=E; return cc_dd[0x63]; // LD IXH,E
        case 0x64: IXH=IXH; return cc_dd[0x64]; // LD IXH,IXH
        case 0x65: IXH=IXL; return cc_dd[0x65]; // LD IXH,IXL
        case 0x66: { uint16_t a=WZ=IX+mem.rs8(PC++); H=mem.r8(a); } return cc_dd[0x66]; // LD H,(IX+d)
        case 0x67: IXH=A; return cc_dd[0x67]; // LD IXH,A
        case 0x68: IXL=B; return cc_dd[0x68]; // LD IXL,B
        case 0x69: IXL=C; return cc_dd[0x69]; // LD IXL,C
        case 0x6a: IXL=D; return cc_dd[0x6a]; // LD IXL,D
        case 0x6b: IXL=E; return cc_dd[0x6b]; // LD IXL,E
        case 0x6c: IXL=IXH; return cc_dd[0x6c]; // LD IXL,IXH
        case 0x6d: IXL=IXL; return cc_dd[0x6d]; // LD IXL,IXL
        case 0x6e: { uint16_t a=WZ=IX+mem.rs8(PC++); L=mem.r8(a); } return cc_dd[0x6e]; // LD L,(IX+d)
        case 0x6f: IXL=A; return cc_dd[0x6f]; // LD IXL,A
        case 0x70: { uint16_t a=WZ=IX+mem.rs8(PC++); mem.w8(a,B); } return cc_dd[0x70]; // LD (IX+d),B
        case 0x71: { uint16_t a=WZ=IX+mem.rs8(PC++); mem.w8(a,C); } return cc_dd[0x71]; // LD (IX+d),C
        case 0x72: { uint16_t a=WZ=IX+mem.rs8(PC++); mem.w8(a,D); } return cc_dd[0x72]; // LD (IX+d),D
        case 0x73: { uint16_t a=WZ=IX+mem.rs8(PC++); mem.w8(a,E); } return cc_dd[0x73]; // LD (IX+d),E
        case 0x74: { uint16_t a=WZ=IX+mem.rs8(PC++); mem.w8(a,H); } return cc_dd[0x74]; // LD (IX+d),H
        case 0x75: { uint16_t a=WZ=IX+mem.rs8(PC++); mem.w8(a,L); } return cc_dd[0x75]; // LD (IX+d),L
        case 0x76: halt(); return cc_dd[0x76]; // HALT
        case 0x77: { uint16_t a=WZ=IX+mem.rs8(PC++); mem.w8(a,A); } return cc_dd[0x77]; // LD (IX+d),A
        case 0x78: A=B; return cc_dd[0x78]; // LD A,B
        case 0x79: A=C; return cc_dd[0x79]; // LD A,C
        case 0x7a: A=D; return cc_dd[0x7a]; // LD A,D
        case 0x7b: A=E; return cc_dd[0x7b]; // LD A,E
        case 0x7c: A=IXH; return cc_dd[0x7c]; // LD A,IXH
        case 0x7d: A=IXL; return cc_dd[0x7d]; // LD A,IXL
        case 0x7e: { uint16_t a=WZ=IX+mem.rs8(PC++); A=mem.r8(a); } return cc_dd[0x7e]; // LD A,(IX+d)
        case 0x7f: A=A; return cc_dd[0x7f]; // LD A,A
        case 0x80: add8(B); return cc_dd[0x80]; // ADD B
        case 0x81: add8(C); return cc_dd[0x81]; // ADD C
        case 0x82: add8(D); return cc_dd[0x82]; // ADD D
        case 0x83: add8(E); return cc_dd[0x83]; // ADD E
        case 0x84: add8(IXH); return cc_dd[0x84]; // ADD IXH
        case 0x85: add8(IXL); return cc_dd[0x85]; // ADD IXL
        case 0x86: { uint16_t a=WZ=IX+mem.rs8(PC++); add8(mem.r8(a)); } return cc_dd[0x86]; // ADD (IX+d)
        case 0x87: add8(A); return cc_dd[0x87]; // ADD A
        case 0x88: adc8(B); return cc_dd[0x88]; // ADC B
        case 0x89: adc8(C); return cc_dd[0x89]; // ADC C
        case 0x8a: adc8(D); return cc_dd[0x8a]; // ADC D
        case 0x8b: adc8(E); return cc_dd[0x8b]; // ADC E
        case 0x8c: adc8(IXH); return cc_dd[0x8c]; // ADC IXH
        case 0x8d: adc8(IXL); return cc_dd[0x8d]; // ADC IXL
        case 0x8e: { uint16_t a=WZ=IX+mem.rs8(PC++); adc8(mem.r8(a)); } return cc_dd[0x8e]; // ADC (IX+d)
        case 0x8f: adc8(A); return cc_dd[0x8f]; // ADC A
        case 0x90: sub8(B); return cc_dd[0x90]; // SUB B
        case 0x91: sub8(C); return cc_dd[0x91]; // SUB C
        case 0x92: sub8(D); return cc_dd[0x92]; // SUB D
        case 0x93: sub8(E); return cc_dd[0x93]; // SUB E
        case 0x94: sub8(IXH); return cc_dd[0x94]; // SUB IXH
        case 0x95: sub8(IXL); return cc_dd[0x95]; // SUB IXL
        case 0x96: { uint16_t a=WZ=IX+mem.rs8(PC++); sub8(mem.r8(a)); } return cc_dd[0x96]; // SUB (IX+d)
        case 0x97: sub8(A); return cc_dd[0x97]; // SUB A
        case 0x98: sbc8(B); return cc_dd[0x98]; // SBC B
        case 0x99: sbc8(C); return cc_dd[0x99]; // SBC C
        case 0x9a: sbc8(D); return cc_dd[0x9a]; // SBC D
        case 0x9b: sbc8(E); return cc_dd[0x9b]; // SBC E
        case 0x9c: sbc8(IXH); return cc_dd[0x9c]; // SBC IXH
        case 0x9d: sbc8(IXL); return cc_dd[0x9d]; // SBC IXL
        case 0x9e: { uint16_t a=WZ=IX+mem.rs8(PC++); sbc8(mem.r8(a)); } return cc_dd[0x9e]; // SBC (IX+d)
        case 0x9f: sbc8(A); return cc_dd[0x9f]; // SBC A
        case 0xa0: and8(B); return cc_dd[0xa0]; // AND B
        case 0xa1: and8(C); return cc_dd[0xa1]; // AND C
        case 0xa2: and8(D); return cc_dd[0xa2]; // AND D
        case 0xa3: and8(E); return cc_dd[0xa3]; // AND E
        case 0xa4: and8(IXH); return cc_dd[0xa4]; // AND IXH
        case 0xa5: and8(IXL); return cc_dd[0xa5]; // AND IXL
        case 0xa6: { uint16_t a=WZ=IX+mem.rs8(PC++); and8(mem.r8(a)); } return cc_dd[0xa6]; // AND (IX+d)
        case 0xa7: and8(A); return cc_dd[0xa7]; // AND A
        case 0xa8: xor8(B); return cc_dd[0xa8]; // XOR B
        case 0xa9: xor8(C); return cc_dd[0xa9]; // XOR C
        case 0xaa: xor8(D); return cc_dd[0xaa]; // XOR D
        case 0xab: xor8(E); return cc_dd[0xab]; // XOR E
        case 0xac: xor8(IXH); return cc_dd[0xac]; // XOR IXH
        case 0xad: xor8(IXL); return cc_dd[0xad]; // XOR IXL
        case 0xae: { uint16_t a=WZ=IX+mem.rs8(PC++); xor8(mem.r8(a)); } return cc_dd[0xae]; // XOR (IX+d)
        case 0xaf: xor8(A); return cc_dd[0xaf]; // XOR A
        case 0xb0: or8(B); return cc_dd[0xb0]; // OR B
        case 0xb1: or8(C); return cc_dd[0xb1]; // OR C
        case 0xb2: or8(D); return cc_dd[0xb2]; // OR D
        case 0xb3: or8(E); return cc_dd[0xb3]; // OR E
        case 0xb4: or8(IXH); return cc_dd[0xb4]; // OR IXH
        case 0xb5: or8(IXL); return cc_dd[0xb5]; // OR IXL
        case 0xb6: { uint16_t a=WZ=IX+mem.rs8(PC++); or8(mem.r8(a)); } return cc_dd[0xb6]; // OR (IX+d)
        case 0xb7: or8(A); return cc_dd[0xb7]; // OR A
        case 0xb8: cp8(B); return cc_dd[0xb8]; // CP B
        case 0xb9: cp8(C); return cc_dd[0xb9]; // CP C
        case 0xba: cp8(D); return cc_dd[0xba]; // CP D
        case 0xbb: cp8(E); return cc_dd[0xbb]; // CP E
        case 0xbc: cp8(IXH); return cc_dd[0xbc]; // CP IXH
        case 0xbd: cp8(IXL); return cc_dd[0xbd]; // CP IXL
        case 0xbe: { uint16_t a=WZ=IX+mem.rs8(PC++); cp8(mem.r8(a)); } return cc_dd[0xbe]; // CP (IX+d)
        case 0xbf: cp8(A); return cc_dd[0xbf]; // CP A
        case 0xc0: if (!(F&ZF)) { WZ=PC=mem.r16(SP); SP+=2; return cc_ex[0xc0]+cc_dd[0xc0]; } else return cc_dd[0xc0]; // RET NZ
        case 0xc1: BC=mem.r16(SP); SP+=2; return cc_dd[0xc1]; // POP BC
        case 0xc2: WZ=mem.r16(PC); if (!(F&ZF)) { PC=WZ; } else { PC+=2; }; return cc_dd[0xc2]; // JP NZ,nn
        case 0xc3: WZ=PC=mem.r16(PC); return cc_dd[0xc3]; // JP nn
        case 0xc4: WZ=mem.r16(PC); PC+=2; if (!(F&ZF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return cc_ex[0xc4]+cc_dd[0xc4]; } else { return cc_dd[0xc4]; } // CALL NZ,nn
        case 0xc5: SP-=2; mem.w16(SP,BC); return cc_dd[0xc5]; // PUSH BC
        case 0xc6: add8(mem.r8(PC++)); return cc_dd[0xc6]; // ADD n
        case 0xc7: rst(0x0); return cc_dd[0xc7]; // RST 0x0
        case 0xc8: if ((F&ZF)) { WZ=PC=mem.r16(SP); SP+=2; return cc_ex[0xc8]+cc_dd[0xc8]; } else return cc_dd[0xc8]; // RET Z
        case 0xc9: WZ=PC=mem.r16(SP); SP+=2; return cc_dd[0xc9]; // RET
        case 0xca: WZ=mem.r16(PC); if ((F&ZF)) { PC=WZ; } else { PC+=2; }; return cc_dd[0xca]; // JP Z,nn
        case 0xcb:
          { const int d = mem.rs8(PC++);
          switch (fetch_op()) {
            case 0x0: { uint16_t a=WZ=IX+d;; B=rlc8(mem.r8(a)); mem.w8(a,B); } return cc_ddcb[0x0]; // RLC (IX+d),B
            case 0x1: { uint16_t a=WZ=IX+d;; C=rlc8(mem.r8(a)); mem.w8(a,C); } return cc_ddcb[0x1]; // RLC (IX+d),C
            case 0x2: { uint16_t a=WZ=IX+d;; D=rlc8(mem.r8(a)); mem.w8(a,D); } return cc_ddcb[0x2]; // RLC (IX+d),D
            case 0x3: { uint16_t a=WZ=IX+d;; E=rlc8(mem.r8(a)); mem.w8(a,E); } return cc_ddcb[0x3]; // RLC (IX+d),E
            case 0x4: { uint16_t a=WZ=IX+d;; H=rlc8(mem.r8(a)); mem.w8(a,H); } return cc_ddcb[0x4]; // RLC (IX+d),H
            case 0x5: { uint16_t a=WZ=IX+d;; L=rlc8(mem.r8(a)); mem.w8(a,L); } return cc_ddcb[0x5]; // RLC (IX+d),L
            case 0x6: { uint16_t a=WZ=IX+d;; mem.w8(a,rlc8(mem.r8(a))); } return cc_ddcb[0x6]; // RLC (IX+d)
            case 0x7: { uint16_t a=WZ=IX+d;; A=rlc8(mem.r8(a)); mem.w8(a,A); } return cc_ddcb[0x7]; // RLC (IX+d),A
            case 0x8: { uint16_t a=WZ=IX+d;; B=rrc8(mem.r8(a)); mem.w8(a,B); } return cc_ddcb[0x8]; // RRC (IX+d),B
            case 0x9: { uint16_t a=WZ=IX+d;; C=rrc8(mem.r8(a)); mem.w8(a,C); } return cc_ddcb[0x9]; // RRC (IX+d),C
            case 0xa: { uint16_t a=WZ=IX+d;; D=rrc8(mem.r8(a)); mem.w8(a,D); } return cc_ddcb[0xa]; // RRC (IX+d),D
            case 0xb: { uint16_t a=WZ=IX+d;; E=rrc8(mem.r8(a)); mem.w8(a,E); } return cc_ddcb[0xb]; // RRC (IX+d),E
            case 0xc: { uint16_t a=WZ=IX+d;; H=rrc8(mem.r8(a)); mem.w8(a,H); } return cc_ddcb[0xc]; // RRC (IX+d),H
            case 0xd: { uint16_t a=WZ=IX+d;; L=rrc8(mem.r8(a)); mem.w8(a,L); } return cc_ddcb[0xd]; // RRC (IX+d),L
            case 0xe: { uint16_t a=WZ=IX+d;; mem.w8(a,rrc8(mem.r8(a))); } return cc_ddcb[0xe]; // RRC (IX+d)
            case 0xf: { uint16_t a=WZ=IX+d;; A=rrc8(mem.r8(a)); mem.w8(a,A); } return cc_ddcb[0xf]; // RRC (IX+d),A
            case 0x10: { uint16_t a=WZ=IX+d;; B=rl8(mem.r8(a)); mem.w8(a,B); } return cc_ddcb[0x10]; // RL (IX+d),B
            case 0x11: { uint16_t a=WZ=IX+d;; C=rl8(mem.r8(a)); mem.w8(a,C); } return cc_ddcb[0x11]; // RL (IX+d),C
            case 0x12: { uint16_t a=WZ=IX+d;; D=rl8(mem.r8(a)); mem.w8(a,D); } return cc_ddcb[0x12]; // RL (IX+d),D
            case 0x13: { uint16_t a=WZ=IX+d;; E=rl8(mem.r8(a)); mem.w8(a,E); } return cc_ddcb[0x13]; // RL (IX+d),E
            case 0x14: { uint16_t a=WZ=IX+d;; H=rl8(mem.r8(a)); mem.w8(a,H); } return cc_ddcb[0x14]; // RL (IX+d),H
            case 0x15: { uint16_t a=WZ=IX+d;; L=rl8(mem.r8(a)); mem.w8(a,L); } return cc_ddcb[0x15]; // RL (IX+d),L
            case 0x16: { uint16_t a=WZ=IX+d;; mem.w8(a,rl8(mem.r8(a))); } return cc_ddcb[0x16]; // RL (IX+d)
            case 0x17: { uint16_t a=WZ=IX+d;; A=rl8(mem.r8(a)); mem.w8(a,A); } return cc_ddcb[0x17]; // RL (IX+d),A
            case 0x18: { uint16_t a=WZ=IX+d;; B=rr8(mem.r8(a)); mem.w8(a,B); } return cc_ddcb[0x18]; // RR (IX+d),B
            case 0x19: { uint16_t a=WZ=IX+d;; C=rr8(mem.r8(a)); mem.w8(a,C); } return cc_ddcb[0x19]; // RR (IX+d),C
            case 0x1a: { uint16_t a=WZ=IX+d;; D=rr8(mem.r8(a)); mem.w8(a,D); } return cc_ddcb[0x1a]; // RR (IX+d),D
            case 0x1b: { uint16_t a=WZ=IX+d;; E=rr8(mem.r8(a)); mem.w8(a,E); } return cc_ddcb[0x1b]; // RR (IX+d),E
            case 0x1c: { uint16_t a=WZ=IX+d;; H=rr8(mem.r8(a)); mem.w8(a,H); } return cc_ddcb[0x1c]; // RR (IX+d),H
            case 0x1d: { uint16_t a=WZ=IX+d;; L=rr8(mem.r8(a)); mem.w8(a,L); } return cc_ddcb[0x1d]; // RR (IX+d),L
            case 0x1e: { uint16_t a=WZ=IX+d;; mem.w8(a,rr8(mem.r8(a))); } return cc_ddcb[0x1e]; // RR (IX+d)
            case 0x1f: { uint16_t a=WZ=IX+d;; A=rr8(mem.r8(a)); mem.w8(a,A); } return cc_ddcb[0x1f]; // RR (IX+d),A
            case 0x20: { uint16_t a=WZ=IX+d;; B=sla8(mem.r8(a)); mem.w8(a,B); } return cc_ddcb[0x20]; // SLA (IX+d),B
            case 0x21: { uint16_t a=WZ=IX+d;; C=sla8(mem.r8(a)); mem.w8(a,C); } return cc_ddcb[0x21]; // SLA (IX+d),C
            case 0x22: { uint16_t a=WZ=IX+d;; D=sla8(mem.r8(a)); mem.w8(a,D); } return cc_ddcb[0x22]; // SLA (IX+d),D
            case 0x23: { uint16_t a=WZ=IX+d;; E=sla8(mem.r8(a)); mem.w8(a,E); } return cc_ddcb[0x23]; // SLA (IX+d),E
            case 0x24: { uint16_t a=WZ=IX+d;; H=sla8(mem.r8(a)); mem.w8(a,H); } return cc_ddcb[0x24]; // SLA (IX+d),H
            case 0x25: { uint16_t a=WZ=IX+d;; L=sla8(mem.r8(a)); mem.w8(a,L); } return cc_ddcb[0x25]; // SLA (IX+d),L
            case 0x26: { uint16_t a=WZ=IX+d;; mem.w8(a,sla8(mem.r8(a))); } return cc_ddcb[0x26]; // SLA (IX+d)
            case 0x27: { uint16_t a=WZ=IX+d;; A=sla8(mem.r8(a)); mem.w8(a,A); } return cc_ddcb[0x27]; // SLA (IX+d),A
            case 0x28: { uint16_t a=WZ=IX+d;; B=sra8(mem.r8(a)); mem.w8(a,B); } return cc_ddcb[0x28]; // SRA (IX+d),B
            case 0x29: { uint16_t a=WZ=IX+d;; C=sra8(mem.r8(a)); mem.w8(a,C); } return cc_ddcb[0x29]; // SRA (IX+d),C
            case 0x2a: { uint16_t a=WZ=IX+d;; D=sra8(mem.r8(a)); mem.w8(a,D); } return cc_ddcb[0x2a]; // SRA (IX+d),D
            case 0x2b: { uint16_t a=WZ=IX+d;; E=sra8(mem.r8(a)); mem.w8(a,E); } return cc_ddcb[0x2b]; // SRA (IX+d),E
            case 0x2c: { uint16_t a=WZ=IX+d;; H=sra8(mem.r8(a)); mem.w8(a,H); } return cc_ddcb[0x2c]; // SRA (IX+d),H
            case 0x2d: { uint16_t a=WZ=IX+d;; L=sra8(mem.r8(a)); mem.w8(a,L); } return cc_ddcb[0x2d]; // SRA (IX+d),L
            case 0x2e: { uint16_t a=WZ=IX+d;; mem.w8(a,sra8(mem.r8(a))); } return cc_ddcb[0x2e]; // SRA (IX+d)
            case 0x2f: { uint16_t a=WZ=IX+d;; A=sra8(mem.r8(a)); mem.w8(a,A); } return cc_ddcb[0x2f]; // SRA (IX+d),A
            case 0x30: { uint16_t a=WZ=IX+d;; B=sll8(mem.r8(a)); mem.w8(a,B); } return cc_ddcb[0x30]; // SLL (IX+d),B
            case 0x31: { uint16_t a=WZ=IX+d;; C=sll8(mem.r8(a)); mem.w8(a,C); } return cc_ddcb[0x31]; // SLL (IX+d),C
            case 0x32: { uint16_t a=WZ=IX+d;; D=sll8(mem.r8(a)); mem.w8(a,D); } return cc_ddcb[0x32]; // SLL (IX+d),D
            case 0x33: { uint16_t a=WZ=IX+d;; E=sll8(mem.r8(a)); mem.w8(a,E); } return cc_ddcb[0x33]; // SLL (IX+d),E
            case 0x34: { uint16_t a=WZ=IX+d;; H=sll8(mem.r8(a)); mem.w8(a,H); } return cc_ddcb[0x34]; // SLL (IX+d),H
            case 0x35: { uint16_t a=WZ=IX+d;; L=sll8(mem.r8(a)); mem.w8(a,L); } return cc_ddcb[0x35]; // SLL (IX+d),L
            case 0x36: { uint16_t a=WZ=IX+d;; mem.w8(a,sll8(mem.r8(a))); } return cc_ddcb[0x36]; // SLL (IX+d)
            case 0x37: { uint16_t a=WZ=IX+d;; A=sll8(mem.r8(a)); mem.w8(a,A); } return cc_ddcb[0x37]; // SLL (IX+d),A
            case 0x38: { uint16_t a=WZ=IX+d;; B=srl8(mem.r8(a)); mem.w8(a,B); } return cc_ddcb[0x38]; // SRL (IX+d),B
            case 0x39: { uint16_t a=WZ=IX+d;; C=srl8(mem.r8(a)); mem.w8(a,C); } return cc_ddcb[0x39]; // SRL (IX+d),C
            case 0x3a: { uint16_t a=WZ=IX+d;; D=srl8(mem.r8(a)); mem.w8(a,D); } return cc_ddcb[0x3a]; // SRL (IX+d),D
            case 0x3b: { uint16_t a=WZ=IX+d;; E=srl8(mem.r8(a)); mem.w8(a,E); } return cc_ddcb[0x3b]; // SRL (IX+d),E
            case 0x3c: { uint16_t a=WZ=IX+d;; H=srl8(mem.r8(a)); mem.w8(a,H); } return cc_ddcb[0x3c]; // SRL (IX+d),H
            case 0x3d: { uint16_t a=WZ=IX+d;; L=srl8(mem.r8(a)); mem.w8(a,L); } return cc_ddcb[0x3d]; // SRL (IX+d),L
            case 0x3e: { uint16_t a=WZ=IX+d;; mem.w8(a,srl8(mem.r8(a))); } return cc_ddcb[0x3e]; // SRL (IX+d)
            case 0x3f: { uint16_t a=WZ=IX+d;; A=srl8(mem.r8(a)); mem.w8(a,A); } return cc_ddcb[0x3f]; // SRL (IX+d),A
            case 0x40: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x1); } return cc_ddcb[0x40]; // BIT 0,(IX+d)
            case 0x41: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x1); } return cc_ddcb[0x41]; // BIT 0,(IX+d)
            case 0x42: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x1); } return cc_ddcb[0x42]; // BIT 0,(IX+d)
            case 0x43: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x1); } return cc_ddcb[0x43]; // BIT 0,(IX+d)
            case 0x44: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x1); } return cc_ddcb[0x44]; // BIT 0,(IX+d)
            case 0x45: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x1); } return cc_ddcb[0x45]; // BIT 0,(IX+d)
            case 0x46: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x1); } return cc_ddcb[0x46]; // BIT 0,(IX+d)
            case 0x47: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x1); } return cc_ddcb[0x47]; // BIT 0,(IX+d)
            case 0x48: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x2); } return cc_ddcb[0x48]; // BIT 1,(IX+d)
            case 0x49: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x2); } return cc_ddcb[0x49]; // BIT 1,(IX+d)
            case 0x4a: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x2); } return cc_ddcb[0x4a]; // BIT 1,(IX+d)
            case 0x4b: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x2); } return cc_ddcb[0x4b]; // BIT 1,(IX+d)
            case 0x4c: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x2); } return cc_ddcb[0x4c]; // BIT 1,(IX+d)
            case 0x4d: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x2); } return cc_ddcb[0x4d]; // BIT 1,(IX+d)
            case 0x4e: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x2); } return cc_ddcb[0x4e]; // BIT 1,(IX+d)
            case 0x4f: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x2); } return cc_ddcb[0x4f]; // BIT 1,(IX+d)
            case 0x50: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x4); } return cc_ddcb[0x50]; // BIT 2,(IX+d)
            case 0x51: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x4); } return cc_ddcb[0x51]; // BIT 2,(IX+d)
            case 0x52: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x4); } return cc_ddcb[0x52]; // BIT 2,(IX+d)
            case 0x53: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x4); } return cc_ddcb[0x53]; // BIT 2,(IX+d)
            case 0x54: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x4); } return cc_ddcb[0x54]; // BIT 2,(IX+d)
            case 0x55: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x4); } return cc_ddcb[0x55]; // BIT 2,(IX+d)
            case 0x56: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x4); } return cc_ddcb[0x56]; // BIT 2,(IX+d)
            case 0x57: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x4); } return cc_ddcb[0x57]; // BIT 2,(IX+d)
            case 0x58: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x8); } return cc_ddcb[0x58]; // BIT 3,(IX+d)
            case 0x59: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x8); } return cc_ddcb[0x59]; // BIT 3,(IX+d)
            case 0x5a: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x8); } return cc_ddcb[0x5a]; // BIT 3,(IX+d)
            case 0x5b: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x8); } return cc_ddcb[0x5b]; // BIT 3,(IX+d)
            case 0x5c: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x8); } return cc_ddcb[0x5c]; // BIT 3,(IX+d)
            case 0x5d: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x8); } return cc_ddcb[0x5d]; // BIT 3,(IX+d)
            case 0x5e: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x8); } return cc_ddcb[0x5e]; // BIT 3,(IX+d)
            case 0x5f: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x8); } return cc_ddcb[0x5f]; // BIT 3,(IX+d)
            case 0x60: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x10); } return cc_ddcb[0x60]; // BIT 4,(IX+d)
            case 0x61: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x10); } return cc_ddcb[0x61]; // BIT 4,(IX+d)
            case 0x62: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x10); } return cc_ddcb[0x62]; // BIT 4,(IX+d)
            case 0x63: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x10); } return cc_ddcb[0x63]; // BIT 4,(IX+d)
            case 0x64: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x10); } return cc_ddcb[0x64]; // BIT 4,(IX+d)
            case 0x65: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x10); } return cc_ddcb[0x65]; // BIT 4,(IX+d)
            case 0x66: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x10); } return cc_ddcb[0x66]; // BIT 4,(IX+d)
            case 0x67: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x10); } return cc_ddcb[0x67]; // BIT 4,(IX+d)
            case 0x68: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x20); } return cc_ddcb[0x68]; // BIT 5,(IX+d)
            case 0x69: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x20); } return cc_ddcb[0x69]; // BIT 5,(IX+d)
            case 0x6a: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x20); } return cc_ddcb[0x6a]; // BIT 5,(IX+d)
            case 0x6b: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x20); } return cc_ddcb[0x6b]; // BIT 5,(IX+d)
            case 0x6c: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x20); } return cc_ddcb[0x6c]; // BIT 5,(IX+d)
            case 0x6d: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x20); } return cc_ddcb[0x6d]; // BIT 5,(IX+d)
            case 0x6e: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x20); } return cc_ddcb[0x6e]; // BIT 5,(IX+d)
            case 0x6f: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x20); } return cc_ddcb[0x6f]; // BIT 5,(IX+d)
            case 0x70: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x40); } return cc_ddcb[0x70]; // BIT 6,(IX+d)
            case 0x71: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x40); } return cc_ddcb[0x71]; // BIT 6,(IX+d)
            case 0x72: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x40); } return cc_ddcb[0x72]; // BIT 6,(IX+d)
            case 0x73: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x40); } return cc_ddcb[0x73]; // BIT 6,(IX+d)
            case 0x74: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x40); } return cc_ddcb[0x74]; // BIT 6,(IX+d)
            case 0x75: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x40); } return cc_ddcb[0x75]; // BIT 6,(IX+d)
            case 0x76: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x40); } return cc_ddcb[0x76]; // BIT 6,(IX+d)
            case 0x77: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x40); } return cc_ddcb[0x77]; // BIT 6,(IX+d)
            case 0x78: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x80); } return cc_ddcb[0x78]; // BIT 7,(IX+d)
            case 0x79: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x80); } return cc_ddcb[0x79]; // BIT 7,(IX+d)
            case 0x7a: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x80); } return cc_ddcb[0x7a]; // BIT 7,(IX+d)
            case 0x7b: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x80); } return cc_ddcb[0x7b]; // BIT 7,(IX+d)
            case 0x7c: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x80); } return cc_ddcb[0x7c]; // BIT 7,(IX+d)
            case 0x7d: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x80); } return cc_ddcb[0x7d]; // BIT 7,(IX+d)
            case 0x7e: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x80); } return cc_ddcb[0x7e]; // BIT 7,(IX+d)
            case 0x7f: { uint16_t a=WZ=IX+d;; ibit(mem.r8(a),0x80); } return cc_ddcb[0x7f]; // BIT 7,(IX+d)
            case 0x80: { uint16_t a=WZ=IX+d;; B=mem.r8(a)&~0x1; mem.w8(a,B); } return cc_ddcb[0x80]; // RES 0,(IX+d),B
            case 0x81: { uint16_t a=WZ=IX+d;; C=mem.r8(a)&~0x1; mem.w8(a,C); } return cc_ddcb[0x81]; // RES 0,(IX+d),C
            case 0x82: { uint16_t a=WZ=IX+d;; D=mem.r8(a)&~0x1; mem.w8(a,D); } return cc_ddcb[0x82]; // RES 0,(IX+d),D
            case 0x83: { uint16_t a=WZ=IX+d;; E=mem.r8(a)&~0x1; mem.w8(a,E); } return cc_ddcb[0x83]; // RES 0,(IX+d),E
            case 0x84: { uint16_t a=WZ=IX+d;; H=mem.r8(a)&~0x1; mem.w8(a,H); } return cc_ddcb[0x84]; // RES 0,(IX+d),H
            case 0x85: { uint16_t a=WZ=IX+d;; L=mem.r8(a)&~0x1; mem.w8(a,L); } return cc_ddcb[0x85]; // RES 0,(IX+d),L
            case 0x86: { uint16_t a=WZ=IX+d;; mem.w8(a,mem.r8(a)&~0x1); } return cc_ddcb[0x86]; // RES 0,(IX+d)
            case 0x87: { uint16_t a=WZ=IX+d;; A=mem.r8(a)&~0x1; mem.w8(a,A); } return cc_ddcb[0x87]; // RES 0,(IX+d),A
            case 0x88: { uint16_t a=WZ=IX+d;; B=mem.r8(a)&~0x2; mem.w8(a,B); } return cc_ddcb[0x88]; // RES 1,(IX+d),B
            case 0x89: { uint16_t a=WZ=IX+d;; C=mem.r8(a)&~0x2; mem.w8(a,C); } return cc_ddcb[0x89]; // RES 1,(IX+d),C
            case 0x8a: { uint16_t a=WZ=IX+d;; D=mem.r8(a)&~0x2; mem.w8(a,D); } return cc_ddcb[0x8a]; // RES 1,(IX+d),D
            case 0x8b: { uint16_t a=WZ=IX+d;; E=mem.r8(a)&~0x2; mem.w8(a,E); } return cc_ddcb[0x8b]; // RES 1,(IX+d),E
            case 0x8c: { uint16_t a=WZ=IX+d;; H=mem.r8(a)&~0x2; mem.w8(a,H); } return cc_ddcb[0x8c]; // RES 1,(IX+d),H
            case 0x8d: { uint16_t a=WZ=IX+d;; L=mem.r8(a)&~0x2; mem.w8(a,L); } return cc_ddcb[0x8d]; // RES 1,(IX+d),L
            case 0x8e: { uint16_t a=WZ=IX+d;; mem.w8(a,mem.r8(a)&~0x2); } return cc_ddcb[0x8e]; // RES 1,(IX+d)
            case 0x8f: { uint16_t a=WZ=IX+d;; A=mem.r8(a)&~0x2; mem.w8(a,A); } return cc_ddcb[0x8f]; // RES 1,(IX+d),A
            case 0x90: { uint16_t a=WZ=IX+d;; B=mem.r8(a)&~0x4; mem.w8(a,B); } return cc_ddcb[0x90]; // RES 2,(IX+d),B
            case 0x91: { uint16_t a=WZ=IX+d;; C=mem.r8(a)&~0x4; mem.w8(a,C); } return cc_ddcb[0x91]; // RES 2,(IX+d),C
            case 0x92: { uint16_t a=WZ=IX+d;; D=mem.r8(a)&~0x4; mem.w8(a,D); } return cc_ddcb[0x92]; // RES 2,(IX+d),D
            case 0x93: { uint16_t a=WZ=IX+d;; E=mem.r8(a)&~0x4; mem.w8(a,E); } return cc_ddcb[0x93]; // RES 2,(IX+d),E
            case 0x94: { uint16_t a=WZ=IX+d;; H=mem.r8(a)&~0x4; mem.w8(a,H); } return cc_ddcb[0x94]; // RES 2,(IX+d),H
            case 0x95: { uint16_t a=WZ=IX+d;; L=mem.r8(a)&~0x4; mem.w8(a,L); } return cc_ddcb[0x95]; // RES 2,(IX+d),L
            case 0x96: { uint16_t a=WZ=IX+d;; mem.w8(a,mem.r8(a)&~0x4); } return cc_ddcb[0x96]; // RES 2,(IX+d)
            case 0x97: { uint16_t a=WZ=IX+d;; A=mem.r8(a)&~0x4; mem.w8(a,A); } return cc_ddcb[0x97]; // RES 2,(IX+d),A
            case 0x98: { uint16_t a=WZ=IX+d;; B=mem.r8(a)&~0x8; mem.w8(a,B); } return cc_ddcb[0x98]; // RES 3,(IX+d),B
            case 0x99: { uint16_t a=WZ=IX+d;; C=mem.r8(a)&~0x8; mem.w8(a,C); } return cc_ddcb[0x99]; // RES 3,(IX+d),C
            case 0x9a: { uint16_t a=WZ=IX+d;; D=mem.r8(a)&~0x8; mem.w8(a,D); } return cc_ddcb[0x9a]; // RES 3,(IX+d),D
            case 0x9b: { uint16_t a=WZ=IX+d;; E=mem.r8(a)&~0x8; mem.w8(a,E); } return cc_ddcb[0x9b]; // RES 3,(IX+d),E
            case 0x9c: { uint16_t a=WZ=IX+d;; H=mem.r8(a)&~0x8; mem.w8(a,H); } return cc_ddcb[0x9c]; // RES 3,(IX+d),H
            case 0x9d: { uint16_t a=WZ=IX+d;; L=mem.r8(a)&~0x8; mem.w8(a,L); } return cc_ddcb[0x9d]; // RES 3,(IX+d),L
            case 0x9e: { uint16_t a=WZ=IX+d;; mem.w8(a,mem.r8(a)&~0x8); } return cc_ddcb[0x9e]; // RES 3,(IX+d)
            case 0x9f: { uint16_t a=WZ=IX+d;; A=mem.r8(a)&~0x8; mem.w8(a,A); } return cc_ddcb[0x9f]; // RES 3,(IX+d),A
            case 0xa0: { uint16_t a=WZ=IX+d;; B=mem.r8(a)&~0x10; mem.w8(a,B); } return cc_ddcb[0xa0]; // RES 4,(IX+d),B
            case 0xa1: { uint16_t a=WZ=IX+d;; C=mem.r8(a)&~0x10; mem.w8(a,C); } return cc_ddcb[0xa1]; // RES 4,(IX+d),C
            case 0xa2: { uint16_t a=WZ=IX+d;; D=mem.r8(a)&~0x10; mem.w8(a,D); } return cc_ddcb[0xa2]; // RES 4,(IX+d),D
            case 0xa3: { uint16_t a=WZ=IX+d;; E=mem.r8(a)&~0x10; mem.w8(a,E); } return cc_ddcb[0xa3]; // RES 4,(IX+d),E
            case 0xa4: { uint16_t a=WZ=IX+d;; H=mem.r8(a)&~0x10; mem.w8(a,H); } return cc_ddcb[0xa4]; // RES 4,(IX+d),H
            case 0xa5: { uint16_t a=WZ=IX+d;; L=mem.r8(a)&~0x10; mem.w8(a,L); } return cc_ddcb[0xa5]; // RES 4,(IX+d),L
            case 0xa6: { uint16_t a=WZ=IX+d;; mem.w8(a,mem.r8(a)&~0x10); } return cc_ddcb[0xa6]; // RES 4,(IX+d)
            case 0xa7: { uint16_t a=WZ=IX+d;; A=mem.r8(a)&~0x10; mem.w8(a,A); } return cc_ddcb[0xa7]; // RES 4,(IX+d),A
            case 0xa8: { uint16_t a=WZ=IX+d;; B=mem.r8(a)&~0x20; mem.w8(a,B); } return cc_ddcb[0xa8]; // RES 5,(IX+d),B
            case 0xa9: { uint16_t a=WZ=IX+d;; C=mem.r8(a)&~0x20; mem.w8(a,C); } return cc_ddcb[0xa9]; // RES 5,(IX+d),C
            case 0xaa: { uint16_t a=WZ=IX+d;; D=mem.r8(a)&~0x20; mem.w8(a,D); } return cc_ddcb[0xaa]; // RES 5,(IX+d),D
            case 0xab: { uint16_t a=WZ=IX+d;; E=mem.r8(a)&~0x20; mem.w8(a,E); } return cc_ddcb[0xab]; // RES 5,(IX+d),E
            case 0xac: { uint16_t a=WZ=IX+d;; H=mem.r8(a)&~0x20; mem.w8(a,H); } return cc_ddcb[0xac]; // RES 5,(IX+d),H
            case 0xad: { uint16_t a=WZ=IX+d;; L=mem.r8(a)&~0x20; mem.w8(a,L); } return cc_ddcb[0xad]; // RES 5,(IX+d),L
            case 0xae: { uint16_t a=WZ=IX+d;; mem.w8(a,mem.r8(a)&~0x20); } return cc_ddcb[0xae]; // RES 5,(IX+d)
            case 0xaf: { uint16_t a=WZ=IX+d;; A=mem.r8(a)&~0x20; mem.w8(a,A); } return cc_ddcb[0xaf]; // RES 5,(IX+d),A
            case 0xb0: { uint16_t a=WZ=IX+d;; B=mem.r8(a)&~0x40; mem.w8(a,B); } return cc_ddcb[0xb0]; // RES 6,(IX+d),B
            case 0xb1: { uint16_t a=WZ=IX+d;; C=mem.r8(a)&~0x40; mem.w8(a,C); } return cc_ddcb[0xb1]; // RES 6,(IX+d),C
            case 0xb2: { uint16_t a=WZ=IX+d;; D=mem.r8(a)&~0x40; mem.w8(a,D); } return cc_ddcb[0xb2]; // RES 6,(IX+d),D
            case 0xb3: { uint16_t a=WZ=IX+d;; E=mem.r8(a)&~0x40; mem.w8(a,E); } return cc_ddcb[0xb3]; // RES 6,(IX+d),E
            case 0xb4: { uint16_t a=WZ=IX+d;; H=mem.r8(a)&~0x40; mem.w8(a,H); } return cc_ddcb[0xb4]; // RES 6,(IX+d),H
            case 0xb5: { uint16_t a=WZ=IX+d;; L=mem.r8(a)&~0x40; mem.w8(a,L); } return cc_ddcb[0xb5]; // RES 6,(IX+d),L
            case 0xb6: { uint16_t a=WZ=IX+d;; mem.w8(a,mem.r8(a)&~0x40); } return cc_ddcb[0xb6]; // RES 6,(IX+d)
            case 0xb7: { uint16_t a=WZ=IX+d;; A=mem.r8(a)&~0x40; mem.w8(a,A); } return cc_ddcb[0xb7]; // RES 6,(IX+d),A
            case 0xb8: { uint16_t a=WZ=IX+d;; B=mem.r8(a)&~0x80; mem.w8(a,B); } return cc_ddcb[0xb8]; // RES 7,(IX+d),B
            case 0xb9: { uint16_t a=WZ=IX+d;; C=mem.r8(a)&~0x80; mem.w8(a,C); } return cc_ddcb[0xb9]; // RES 7,(IX+d),C
            case 0xba: { uint16_t a=WZ=IX+d;; D=mem.r8(a)&~0x80; mem.w8(a,D); } return cc_ddcb[0xba]; // RES 7,(IX+d),D
            case 0xbb: { uint16_t a=WZ=IX+d;; E=mem.r8(a)&~0x80; mem.w8(a,E); } return cc_ddcb[0xbb]; // RES 7,(IX+d),E
            case 0xbc: { uint16_t a=WZ=IX+d;; H=mem.r8(a)&~0x80; mem.w8(a,H); } return cc_ddcb[0xbc]; // RES 7,(IX+d),H
            case 0xbd: { uint16_t a=WZ=IX+d;; L=mem.r8(a)&~0x80; mem.w8(a,L); } return cc_ddcb[0xbd]; // RES 7,(IX+d),L
            case 0xbe: { uint16_t a=WZ=IX+d;; mem.w8(a,mem.r8(a)&~0x80); } return cc_ddcb[0xbe]; // RES 7,(IX+d)
            case 0xbf: { uint16_t a=WZ=IX+d;; A=mem.r8(a)&~0x80; mem.w8(a,A); } return cc_ddcb[0xbf]; // RES 7,(IX+d),A
            case 0xc0: { uint16_t a=WZ=IX+d;; B=mem.r8(a)|0x1; mem.w8(a,B);} return cc_ddcb[0xc0]; // SET 0,(IX+d),B
            case 0xc1: { uint16_t a=WZ=IX+d;; C=mem.r8(a)|0x1; mem.w8(a,C);} return cc_ddcb[0xc1]; // SET 0,(IX+d),C
            case 0xc2: { uint16_t a=WZ=IX+d;; D=mem.r8(a)|0x1; mem.w8(a,D);} return cc_ddcb[0xc2]; // SET 0,(IX+d),D
            case 0xc3: { uint16_t a=WZ=IX+d;; E=mem.r8(a)|0x1; mem.w8(a,E);} return cc_ddcb[0xc3]; // SET 0,(IX+d),E
            case 0xc4: { uint16_t a=WZ=IX+d;; H=mem.r8(a)|0x1; mem.w8(a,IXH);} return cc_ddcb[0xc4]; // SET 0,(IX+d),H
            case 0xc5: { uint16_t a=WZ=IX+d;; L=mem.r8(a)|0x1; mem.w8(a,IXL);} return cc_ddcb[0xc5]; // SET 0,(IX+d),L
            case 0xc6: { uint16_t a=WZ=IX+d;; mem.w8(a,mem.r8(a)|0x1);} return cc_ddcb[0xc6]; // SET 0,(IX+d)
            case 0xc7: { uint16_t a=WZ=IX+d;; A=mem.r8(a)|0x1; mem.w8(a,A);} return cc_ddcb[0xc7]; // SET 0,(IX+d),A
            case 0xc8: { uint16_t a=WZ=IX+d;; B=mem.r8(a)|0x2; mem.w8(a,B);} return cc_ddcb[0xc8]; // SET 1,(IX+d),B
            case 0xc9: { uint16_t a=WZ=IX+d;; C=mem.r8(a)|0x2; mem.w8(a,C);} return cc_ddcb[0xc9]; // SET 1,(IX+d),C
            case 0xca: { uint16_t a=WZ=IX+d;; D=mem.r8(a)|0x2; mem.w8(a,D);} return cc_ddcb[0xca]; // SET 1,(IX+d),D
            case 0xcb: { uint16_t a=WZ=IX+d;; E=mem.r8(a)|0x2; mem.w8(a,E);} return cc_ddcb[0xcb]; // SET 1,(IX+d),E
            case 0xcc: { uint16_t a=WZ=IX+d;; H=mem.r8(a)|0x2; mem.w8(a,IXH);} return cc_ddcb[0xcc]; // SET 1,(IX+d),H
            case 0xcd: { uint16_t a=WZ=IX+d;; L=mem.r8(a)|0x2; mem.w8(a,IXL);} return cc_ddcb[0xcd]; // SET 1,(IX+d),L
            case 0xce: { uint16_t a=WZ=IX+d;; mem.w8(a,mem.r8(a)|0x2);} return cc_ddcb[0xce]; // SET 1,(IX+d)
            case 0xcf: { uint16_t a=WZ=IX+d;; A=mem.r8(a)|0x2; mem.w8(a,A);} return cc_ddcb[0xcf]; // SET 1,(IX+d),A
            case 0xd0: { uint16_t a=WZ=IX+d;; B=mem.r8(a)|0x4; mem.w8(a,B);} return cc_ddcb[0xd0]; // SET 2,(IX+d),B
            case 0xd1: { uint16_t a=WZ=IX+d;; C=mem.r8(a)|0x4; mem.w8(a,C);} return cc_ddcb[0xd1]; // SET 2,(IX+d),C
            case 0xd2: { uint16_t a=WZ=IX+d;; D=mem.r8(a)|0x4; mem.w8(a,D);} return cc_ddcb[0xd2]; // SET 2,(IX+d),D
            case 0xd3: { uint16_t a=WZ=IX+d;; E=mem.r8(a)|0x4; mem.w8(a,E);} return cc_ddcb[0xd3]; // SET 2,(IX+d),E
            case 0xd4: { uint16_t a=WZ=IX+d;; H=mem.r8(a)|0x4; mem.w8(a,IXH);} return cc_ddcb[0xd4]; // SET 2,(IX+d),H
            case 0xd5: { uint16_t a=WZ=IX+d;; L=mem.r8(a)|0x4; mem.w8(a,IXL);} return cc_ddcb[0xd5]; // SET 2,(IX+d),L
            case 0xd6: { uint16_t a=WZ=IX+d;; mem.w8(a,mem.r8(a)|0x4);} return cc_ddcb[0xd6]; // SET 2,(IX+d)
            case 0xd7: { uint16_t a=WZ=IX+d;; A=mem.r8(a)|0x4; mem.w8(a,A);} return cc_ddcb[0xd7]; // SET 2,(IX+d),A
            case 0xd8: { uint16_t a=WZ=IX+d;; B=mem.r8(a)|0x8; mem.w8(a,B);} return cc_ddcb[0xd8]; // SET 3,(IX+d),B
            case 0xd9: { uint16_t a=WZ=IX+d;; C=mem.r8(a)|0x8; mem.w8(a,C);} return cc_ddcb[0xd9]; // SET 3,(IX+d),C
            case 0xda: { uint16_t a=WZ=IX+d;; D=mem.r8(a)|0x8; mem.w8(a,D);} return cc_ddcb[0xda]; // SET 3,(IX+d),D
            case 0xdb: { uint16_t a=WZ=IX+d;; E=mem.r8(a)|0x8; mem.w8(a,E);} return cc_ddcb[0xdb]; // SET 3,(IX+d),E
            case 0xdc: { uint16_t a=WZ=IX+d;; H=mem.r8(a)|0x8; mem.w8(a,IXH);} return cc_ddcb[0xdc]; // SET 3,(IX+d),H
            case 0xdd: { uint16_t a=WZ=IX+d;; L=mem.r8(a)|0x8; mem.w8(a,IXL);} return cc_ddcb[0xdd]; // SET 3,(IX+d),L
            case 0xde: { uint16_t a=WZ=IX+d;; mem.w8(a,mem.r8(a)|0x8);} return cc_ddcb[0xde]; // SET 3,(IX+d)
            case 0xdf: { uint16_t a=WZ=IX+d;; A=mem.r8(a)|0x8; mem.w8(a,A);} return cc_ddcb[0xdf]; // SET 3,(IX+d),A
            case 0xe0: { uint16_t a=WZ=IX+d;; B=mem.r8(a)|0x10; mem.w8(a,B);} return cc_ddcb[0xe0]; // SET 4,(IX+d),B
            case 0xe1: { uint16_t a=WZ=IX+d;; C=mem.r8(a)|0x10; mem.w8(a,C);} return cc_ddcb[0xe1]; // SET 4,(IX+d),C
            case 0xe2: { uint16_t a=WZ=IX+d;; D=mem.r8(a)|0x10; mem.w8(a,D);} return cc_ddcb[0xe2]; // SET 4,(IX+d),D
            case 0xe3: { uint16_t a=WZ=IX+d;; E=mem.r8(a)|0x10; mem.w8(a,E);} return cc_ddcb[0xe3]; // SET 4,(IX+d),E
            case 0xe4: { uint16_t a=WZ=IX+d;; H=mem.r8(a)|0x10; mem.w8(a,IXH);} return cc_ddcb[0xe4]; // SET 4,(IX+d),H
            case 0xe5: { uint16_t a=WZ=IX+d;; L=mem.r8(a)|0x10; mem.w8(a,IXL);} return cc_ddcb[0xe5]; // SET 4,(IX+d),L
            case 0xe6: { uint16_t a=WZ=IX+d;; mem.w8(a,mem.r8(a)|0x10);} return cc_ddcb[0xe6]; // SET 4,(IX+d)
            case 0xe7: { uint16_t a=WZ=IX+d;; A=mem.r8(a)|0x10; mem.w8(a,A);} return cc_ddcb[0xe7]; // SET 4,(IX+d),A
            case 0xe8: { uint16_t a=WZ=IX+d;; B=mem.r8(a)|0x20; mem.w8(a,B);} return cc_ddcb[0xe8]; // SET 5,(IX+d),B
            case 0xe9: { uint16_t a=WZ=IX+d;; C=mem.r8(a)|0x20; mem.w8(a,C);} return cc_ddcb[0xe9]; // SET 5,(IX+d),C
            case 0xea: { uint16_t a=WZ=IX+d;; D=mem.r8(a)|0x20; mem.w8(a,D);} return cc_ddcb[0xea]; // SET 5,(IX+d),D
            case 0xeb: { uint16_t a=WZ=IX+d;; E=mem.r8(a)|0x20; mem.w8(a,E);} return cc_ddcb[0xeb]; // SET 5,(IX+d),E
            case 0xec: { uint16_t a=WZ=IX+d;; H=mem.r8(a)|0x20; mem.w8(a,IXH);} return cc_ddcb[0xec]; // SET 5,(IX+d),H
            case 0xed: { uint16_t a=WZ=IX+d;; L=mem.r8(a)|0x20; mem.w8(a,IXL);} return cc_ddcb[0xed]; // SET 5,(IX+d),L
            case 0xee: { uint16_t a=WZ=IX+d;; mem.w8(a,mem.r8(a)|0x20);} return cc_ddcb[0xee]; // SET 5,(IX+d)
            case 0xef: { uint16_t a=WZ=IX+d;; A=mem.r8(a)|0x20; mem.w8(a,A);} return cc_ddcb[0xef]; // SET 5,(IX+d),A
            case 0xf0: { uint16_t a=WZ=IX+d;; B=mem.r8(a)|0x40; mem.w8(a,B);} return cc_ddcb[0xf0]; // SET 6,(IX+d),B
            case 0xf1: { uint16_t a=WZ=IX+d;; C=mem.r8(a)|0x40; mem.w8(a,C);} return cc_ddcb[0xf1]; // SET 6,(IX+d),C
            case 0xf2: { uint16_t a=WZ=IX+d;; D=mem.r8(a)|0x40; mem.w8(a,D);} return cc_ddcb[0xf2]; // SET 6,(IX+d),D
            case 0xf3: { uint16_t a=WZ=IX+d;; E=mem.r8(a)|0x40; mem.w8(a,E);} return cc_ddcb[0xf3]; // SET 6,(IX+d),E
            case 0xf4: { uint16_t a=WZ=IX+d;; H=mem.r8(a)|0x40; mem.w8(a,IXH);} return cc_ddcb[0xf4]; // SET 6,(IX+d),H
            case 0xf5: { uint16_t a=WZ=IX+d;; L=mem.r8(a)|0x40; mem.w8(a,IXL);} return cc_ddcb[0xf5]; // SET 6,(IX+d),L
            case 0xf6: { uint16_t a=WZ=IX+d;; mem.w8(a,mem.r8(a)|0x40);} return cc_ddcb[0xf6]; // SET 6,(IX+d)
            case 0xf7: { uint16_t a=WZ=IX+d;; A=mem.r8(a)|0x40; mem.w8(a,A);} return cc_ddcb[0xf7]; // SET 6,(IX+d),A
            case 0xf8: { uint16_t a=WZ=IX+d;; B=mem.r8(a)|0x80; mem.w8(a,B);} return cc_ddcb[0xf8]; // SET 7,(IX+d),B
            case 0xf9: { uint16_t a=WZ=IX+d;; C=mem.r8(a)|0x80; mem.w8(a,C);} return cc_ddcb[0xf9]; // SET 7,(IX+d),C
            case 0xfa: { uint16_t a=WZ=IX+d;; D=mem.r8(a)|0x80; mem.w8(a,D);} return cc_ddcb[0xfa]; // SET 7,(IX+d),D
            case 0xfb: { uint16_t a=WZ=IX+d;; E=mem.r8(a)|0x80; mem.w8(a,E);} return cc_ddcb[0xfb]; // SET 7,(IX+d),E
            case 0xfc: { uint16_t a=WZ=IX+d;; H=mem.r8(a)|0x80; mem.w8(a,IXH);} return cc_ddcb[0xfc]; // SET 7,(IX+d),H
            case 0xfd: { uint16_t a=WZ=IX+d;; L=mem.r8(a)|0x80; mem.w8(a,IXL);} return cc_ddcb[0xfd]; // SET 7,(IX+d),L
            case 0xfe: { uint16_t a=WZ=IX+d;; mem.w8(a,mem.r8(a)|0x80);} return cc_ddcb[0xfe]; // SET 7,(IX+d)
            case 0xff: { uint16_t a=WZ=IX+d;; A=mem.r8(a)|0x80; mem.w8(a,A);} return cc_ddcb[0xff]; // SET 7,(IX+d),A
            default: return invalid_opcode(4);
          }
          break;
          }
        case 0xcc: WZ=mem.r16(PC); PC+=2; if ((F&ZF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return cc_ex[0xcc]+cc_dd[0xcc]; } else { return cc_dd[0xcc]; } // CALL Z,nn
        case 0xcd: SP-=2; mem.w16(SP,PC+2); WZ=PC=mem.r16(PC); return cc_dd[0xcd]; // CALL nn
        case 0xce: adc8(mem.r8(PC++)); return cc_dd[0xce]; // ADC n
        case 0xcf: rst(0x8); return cc_dd[0xcf]; // RST 0x8
        case 0xd0: if (!(F&CF)) { WZ=PC=mem.r16(SP); SP+=2; return cc_ex[0xd0]+cc_dd[0xd0]; } else return cc_dd[0xd0]; // RET NC
        case 0xd1: DE=mem.r16(SP); SP+=2; return cc_dd[0xd1]; // POP DE
        case 0xd2: WZ=mem.r16(PC); if (!(F&CF)) { PC=WZ; } else { PC+=2; }; return cc_dd[0xd2]; // JP NC,nn
        case 0xd3: out(bus, (A<<8)|mem.r8(PC++),A); return cc_dd[0xd3]; // OUT (n),A
        case 0xd4: WZ=mem.r16(PC); PC+=2; if (!(F&CF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return cc_ex[0xd4]+cc_dd[0xd4]; } else { return cc_dd[0xd4]; } // CALL NC,nn
        case 0xd5: SP-=2; mem.w16(SP,DE); return cc_dd[0xd5]; // PUSH DE
        case 0xd6: sub8(mem.r8(PC++)); return cc_dd[0xd6]; // SUB n
        case 0xd7: rst(0x10); return cc_dd[0xd7]; // RST 0x10
        case 0xd8: if ((F&CF)) { WZ=PC=mem.r16(SP); SP+=2; return cc_ex[0xd8]+cc_dd[0xd8]; } else return cc_dd[0xd8]; // RET C
        case 0xd9: swap16(BC,BC_); swap16(DE,DE_); swap16(HL,HL_); swap16(WZ,WZ_); return cc_dd[0xd9]; // EXX
        case 0xda: WZ=mem.r16(PC); if ((F&CF)) { PC=WZ; } else { PC+=2; }; return cc_dd[0xda]; // JP C,nn
        case 0xdb: A=in(bus, (A<<8)|mem.r8(PC++)); return cc_dd[0xdb]; // IN A,(n)
        case 0xdc: WZ=mem.r16(PC); PC+=2; if ((F&CF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return cc_ex[0xdc]+cc_dd[0xdc]; } else { return cc_dd[0xdc]; } // CALL C,nn
        case 0xde: sbc8(mem.r8(PC++)); return cc_dd[0xde]; // SBC n
        case 0xdf: rst(0x18); return cc_dd[0xdf]; // RST 0x18
        case 0xe0: if (!(F&PF)) { WZ=PC=mem.r16(SP); SP+=2; return cc_ex[0xe0]+cc_dd[0xe0]; } else return cc_dd[0xe0]; // RET PO
        case 0xe1: IX=mem.r16(SP); SP+=2; return cc_dd[0xe1]; // POP IX
        case 0xe2: WZ=mem.r16(PC); if (!(F&PF)) { PC=WZ; } else { PC+=2; }; return cc_dd[0xe2]; // JP PO,nn
        case 0xe3: {uint16_t swp=mem.r16(SP); mem.w16(SP,IX); IX=WZ=swp;} return cc_dd[0xe3]; // EX (SP),IX
        case 0xe4: WZ=mem.r16(PC); PC+=2; if (!(F&PF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return cc_ex[0xe4]+cc_dd[0xe4]; } else { return cc_dd[0xe4]; } // CALL PO,nn
        case 0xe5: SP-=2; mem.w16(SP,IX); return cc_dd[0xe5]; // PUSH IX
        case 0xe6: and8(mem.r8(PC++)); return cc_dd[0xe6]; // AND n
        case 0xe7: rst(0x20); return cc_dd[0xe7]; // RST 0x20
        case 0xe8: if ((F&PF)) { WZ=PC=mem.r16(SP); SP+=2; return cc_ex[0xe8]+cc_dd[0xe8]; } else return cc_dd[0xe8]; // RET PE
        case 0xe9: PC=IX; return cc_dd[0xe9]; // JP IX
        case 0xea: WZ=mem.r16(PC); if ((F&PF)) { PC=WZ; } else { PC+=2; }; return cc_dd[0xea]; // JP PE,nn
        case 0xeb: swap16(DE,HL); return cc_dd[0xeb]; // EX DE,HL
        case 0xec: WZ=mem.r16(PC); PC+=2; if ((F&PF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return cc_ex[0xec]+cc_dd[0xec]; } else { return cc_dd[0xec]; } // CALL PE,nn
        case 0xee: xor8(mem.r8(PC++)); return cc_dd[0xee]; // XOR n
        case 0xef: rst(0x28); return cc_dd[0xef]; // RST 0x28
        case 0xf0: if (!(F&SF)) { WZ=PC=mem.r16(SP); SP+=2; return cc_ex[0xf0]+cc_dd[0xf0]; } else return cc_dd[0xf0]; // RET P
        case 0xf1: AF=mem.r16(SP); SP+=2; return cc_dd[0xf1]; // POP AF
        case 0xf2: WZ=mem.r16(PC); if (!(F&SF)) { PC=WZ; } else { PC+=2; }; return cc_dd[0xf2]; // JP P,nn
        case 0xf3: di(); return cc_dd[0xf3]; // DI
        case 0xf4: WZ=mem.r16(PC); PC+=2; if (!(F&SF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return cc_ex[0xf4]+cc_dd[0xf4]; } else { return cc_dd[0xf4]; } // CALL P,nn
        case 0xf5: SP-=2; mem.w16(SP,AF); return cc_dd[0xf5]; // PUSH AF
        case 0xf6: or8(mem.r8(PC++)); return cc_dd[0xf6]; // OR n
        case 0xf7: rst(0x30); return cc_dd[0xf7]; // RST 0x30
        case 0xf8: if ((F&SF)) { WZ=PC=mem.r16(SP); SP+=2; return cc_ex[0xf8]+cc_dd[0xf8]; } else return cc_dd[0xf8]; // RET M
        case 0xf9: SP=IX; return cc_dd[0xf9]; // LD SP,IX
        case 0xfa: WZ=mem.r16(PC); if ((F&SF)) { PC=WZ; } else { PC+=2; }; return cc_dd[0xfa]; // JP M,nn
        case 0xfb: ei(); return cc_dd[0xfb]; // EI
        case 0xfc: WZ=mem.r16(PC); PC+=2; if ((F&SF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return cc_ex[0xfc]+cc_dd[0xfc]; } else { return cc_dd[0xfc]; } // CALL M,nn
        case 0xfe: cp8(mem.r8(PC++)); return cc_dd[0xfe]; // CP n
        case 0xff: rst(0x38); return cc_dd[0xff]; // RST 0x38
        default: return invalid_opcode(2);
      }
      break;
    case 0xde: sbc8(mem.r8(PC++)); return cc_op[0xde]; // SBC n
    case 0xdf: rst(0x18); return cc_op[0xdf]; // RST 0x18
    case 0xe0: if (!(F&PF)) { WZ=PC=mem.r16(SP); SP+=2; return cc_ex[0xe0]+cc_op[0xe0]; } else return cc_op[0xe0]; // RET PO
    case 0xe1: HL=mem.r16(SP); SP+=2; return cc_op[0xe1]; // POP HL
    case 0xe2: WZ=mem.r16(PC); if (!(F&PF)) { PC=WZ; } else { PC+=2; }; return cc_op[0xe2]; // JP PO,nn
    case 0xe3: {uint16_t swp=mem.r16(SP); mem.w16(SP,HL); HL=WZ=swp;} return cc_op[0xe3]; // EX (SP),HL
    case 0xe4: WZ=mem.r16(PC); PC+=2; if (!(F&PF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return cc_ex[0xe4]+cc_op[0xe4]; } else { return cc_op[0xe4]; } // CALL PO,nn
    case 0xe5: SP-=2; mem.w16(SP,HL); return cc_op[0xe5]; // PUSH HL
    case 0xe6: and8(mem.r8(PC++)); return cc_op[0xe6]; // AND n
    case 0xe7: rst(0x20); return cc_op[0xe7]; // RST 0x20
    case 0xe8: if ((F&PF)) { WZ=PC=mem.r16(SP); SP+=2; return cc_ex[0xe8]+cc_op[0xe8]; } else return cc_op[0xe8]; // RET PE
    case 0xe9: PC=HL; return cc_op[0xe9]; // JP HL
    case 0xea: WZ=mem.r16(PC); if ((F&PF)) { PC=WZ; } else { PC+=2; }; return cc_op[0xea]; // JP PE,nn
    case 0xeb: swap16(DE,HL); return cc_op[0xeb]; // EX DE,HL
    case 0xec: WZ=mem.r16(PC); PC+=2; if ((F&PF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return cc_ex[0xec]+cc_op[0xec]; } else { return cc_op[0xec]; } // CALL PE,nn
    case 0xed:
      switch (fetch_op()) {
        case 0x40: B=in(bus, BC); F=szp[B]|(F&CF); return cc_ed[0x40]; // IN B,(C)
        case 0x41: out(bus, BC,B); return cc_ed[0x41]; // OUT (C),B
        case 0x42: HL=sbc16(HL,BC); return cc_ed[0x42]; // SBC HL,BC
        case 0x43: WZ=mem.r16(PC); mem.w16(WZ++,BC); PC+=2; return cc_ed[0x43]; // LD (nn),BC
        case 0x44: neg8(); return cc_ed[0x44]; // NEG
        case 0x46: IM=0; return cc_ed[0x46]; // IM 0
        case 0x47: I=A; return cc_ed[0x47]; // LD I,A
        case 0x48: C=in(bus, BC); F=szp[C]|(F&CF); return cc_ed[0x48]; // IN C,(C)
        case 0x49: out(bus, BC,C); return cc_ed[0x49]; // OUT (C),C
        case 0x4a: HL=adc16(HL,BC); return cc_ed[0x4a]; // ADC HL,BC
        case 0x4b: WZ=mem.r16(PC); BC=mem.r16(WZ++); PC+=2; return cc_ed[0x4b]; // LD BC,(nn)
        case 0x4c: neg8(); return cc_ed[0x4c]; // NEG
        case 0x4d: reti(); return cc_ed[0x4d]; // RETI
        case 0x4e: IM=0; return cc_ed[0x4e]; // IM 0
        case 0x4f: R=A; return cc_ed[0x4f]; // LD R,A
        case 0x50: D=in(bus, BC); F=szp[D]|(F&CF); return cc_ed[0x50]; // IN D,(C)
        case 0x51: out(bus, BC,D); return cc_ed[0x51]; // OUT (C),D
        case 0x52: HL=sbc16(HL,DE); return cc_ed[0x52]; // SBC HL,DE
        case 0x53: WZ=mem.r16(PC); mem.w16(WZ++,DE); PC+=2; return cc_ed[0x53]; // LD (nn),DE
        case 0x54: neg8(); return cc_ed[0x54]; // NEG
        case 0x56: IM=1; return cc_ed[0x56]; // IM 1
        case 0x57: A=I; F=sziff2(I,IFF2)|(F&CF); return cc_ed[0x57]; // LD A,I
        case 0x58: E=in(bus, BC); F=szp[E]|(F&CF); return cc_ed[0x58]; // IN E,(C)
        case 0x59: out(bus, BC,E); return cc_ed[0x59]; // OUT (C),E
        case 0x5a: HL=adc16(HL,DE); return cc_ed[0x5a]; // ADC HL,DE
        case 0x5b: WZ=mem.r16(PC); DE=mem.r16(WZ++); PC+=2; return cc_ed[0x5b]; // LD DE,(nn)
        case 0x5c: neg8(); return cc_ed[0x5c]; // NEG
        case 0x5e: IM=2; return cc_ed[0x5e]; // IM 2
        case 0x5f: A=R; F=sziff2(R,IFF2)|(F&CF); return cc_ed[0x5f]; // LD A,R
        case 0x60: H=in(bus, BC); F=szp[H]|(F&CF); return cc_ed[0x60]; // IN H,(C)
        case 0x61: out(bus, BC,H); return cc_ed[0x61]; // OUT (C),H
        case 0x62: HL=sbc16(HL,HL); return cc_ed[0x62]; // SBC HL,HL
        case 0x63: WZ=mem.r16(PC); mem.w16(WZ++,HL); PC+=2; return cc_ed[0x63]; // LD (nn),HL
        case 0x64: neg8(); return cc_ed[0x64]; // NEG
        case 0x66: IM=0; return cc_ed[0x66]; // IM 0
        case 0x67: rrd(); return cc_ed[0x67]; // RRD
        case 0x68: L=in(bus, BC); F=szp[L]|(F&CF); return cc_ed[0x68]; // IN L,(C)
        case 0x69: out(bus, BC,L); return cc_ed[0x69]; // OUT (C),L
        case 0x6a: HL=adc16(HL,HL); return cc_ed[0x6a]; // ADC HL,HL
        case 0x6b: WZ=mem.r16(PC); HL=mem.r16(WZ++); PC+=2; return cc_ed[0x6b]; // LD HL,(nn)
        case 0x6c: neg8(); return cc_ed[0x6c]; // NEG
        case 0x6e: IM=0; return cc_ed[0x6e]; // IM 0
        case 0x6f: rld(); return cc_ed[0x6f]; // RLD
        case 0x70: F=szp[in(bus, BC)]|(F&CF); return cc_ed[0x70]; // IN (C)
        case 0x71: out(bus, BC,0); return cc_ed[0x71]; // None
        case 0x72: HL=sbc16(HL,SP); return cc_ed[0x72]; // SBC HL,SP
        case 0x73: WZ=mem.r16(PC); mem.w16(WZ++,SP); PC+=2; return cc_ed[0x73]; // LD (nn),SP
        case 0x74: neg8(); return cc_ed[0x74]; // NEG
        case 0x76: IM=1; return cc_ed[0x76]; // IM 1
        case 0x77: return cc_ed[0x77]; // NOP (ED)
        case 0x78: A=in(bus, BC); F=szp[A]|(F&CF); return cc_ed[0x78]; // IN A,(C)
        case 0x79: out(bus, BC,A); return cc_ed[0x79]; // OUT (C),A
        case 0x7a: HL=adc16(HL,SP); return cc_ed[0x7a]; // ADC HL,SP
        case 0x7b: WZ=mem.r16(PC); SP=mem.r16(WZ++); PC+=2; return cc_ed[0x7b]; // LD SP,(nn)
        case 0x7c: neg8(); return cc_ed[0x7c]; // NEG
        case 0x7e: IM=2; return cc_ed[0x7e]; // IM 2
        case 0x7f: return cc_ed[0x7f]; // NOP (ED)
        case 0xa0: ldi(); return cc_ed[0xa0]; // LDI
        case 0xa1: cpi(); return cc_ed[0xa1]; // CPI
        case 0xa2: ini(bus); return cc_ed[0xa2]; // INI
        case 0xa3: outi(bus); return cc_ed[0xa3]; // OUTI
        case 0xa8: ldd(); return cc_ed[0xa8]; // LDD
        case 0xa9: cpd(); return cc_ed[0xa9]; // CPD
        case 0xaa: ind(bus); return cc_ed[0xaa]; // IND
        case 0xab: outd(bus); return cc_ed[0xab]; // OUTD
        case 0xb0: return ldir(); // LDIR
        case 0xb1: return cpir(); // CPIR
        case 0xb2: return inir(bus); // INIR
        case 0xb3: return otir(bus); // OTIR
        case 0xb8: return lddr(); // LDDR
        case 0xb9: return cpdr(); // CPDR
        case 0xba: return indr(bus); // INDR
        case 0xbb: return otdr(bus); // OTDR
        default: return invalid_opcode(2);
      }
      break;
    case 0xee: xor8(mem.r8(PC++)); return cc_op[0xee]; // XOR n
    case 0xef: rst(0x28); return cc_op[0xef]; // RST 0x28
    case 0xf0: if (!(F&SF)) { WZ=PC=mem.r16(SP); SP+=2; return cc_ex[0xf0]+cc_op[0xf0]; } else return cc_op[0xf0]; // RET P
    case 0xf1: AF=mem.r16(SP); SP+=2; return cc_op[0xf1]; // POP AF
    case 0xf2: WZ=mem.r16(PC); if (!(F&SF)) { PC=WZ; } else { PC+=2; }; return cc_op[0xf2]; // JP P,nn
    case 0xf3: di(); return cc_op[0xf3]; // DI
    case 0xf4: WZ=mem.r16(PC); PC+=2; if (!(F&SF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return cc_ex[0xf4]+cc_op[0xf4]; } else { return cc_op[0xf4]; } // CALL P,nn
    case 0xf5: SP-=2; mem.w16(SP,AF); return cc_op[0xf5]; // PUSH AF
    case 0xf6: or8(mem.r8(PC++)); return cc_op[0xf6]; // OR n
    case 0xf7: rst(0x30); return cc_op[0xf7]; // RST 0x30
    case 0xf8: if ((F&SF)) { WZ=PC=mem.r16(SP); SP+=2; return cc_ex[0xf8]+cc_op[0xf8]; } else return cc_op[0xf8]; // RET M
    case 0xf9: SP=HL; return cc_op[0xf9]; // LD SP,HL
    case 0xfa: WZ=mem.r16(PC); if ((F&SF)) { PC=WZ; } else { PC+=2; }; return cc_op[0xfa]; // JP M,nn
    case 0xfb: ei(); return cc_op[0xfb]; // EI
    case 0xfc: WZ=mem.r16(PC); PC+=2; if ((F&SF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return cc_ex[0xfc]+cc_op[0xfc]; } else { return cc_op[0xfc]; } // CALL M,nn
    case 0xfd:
      switch (fetch_op()) {
        case 0x0: return cc_fd[0x0]; // NOP
        case 0x1: BC=mem.r16(PC); PC+=2; return cc_fd[0x1]; // LD BC,nn
        case 0x2: mem.w8(BC,A); Z=C+1; W=A; return cc_fd[0x2]; // LD (BC),A
        case 0x3: BC++; return cc_fd[0x3]; // INC BC
        case 0x4: B=inc8(B); return cc_fd[0x4]; // INC B
        case 0x5: B=dec8(B); return cc_fd[0x5]; // DEC B
        case 0x6: B=mem.r8(PC++); return cc_fd[0x6]; // LD B,n
        case 0x7: rlca8(); return cc_fd[0x7]; // RLCA
        case 0x8: swap16(AF,AF_); return cc_fd[0x8]; // EX AF,AF'
        case 0x9: IY=add16(IY,BC); return cc_fd[0x9]; // ADD IY,BC
        case 0xa: A=mem.r8(BC); WZ=BC+1; return cc_fd[0xa]; // LD A,(BC)
        case 0xb: BC--; return cc_fd[0xb]; // DEC BC
        case 0xc: C=inc8(C); return cc_fd[0xc]; // INC C
        case 0xd: C=dec8(C); return cc_fd[0xd]; // DEC C
        case 0xe: C=mem.r8(PC++); return cc_fd[0xe]; // LD C,n
        case 0xf: rrca8(); return cc_fd[0xf]; // RRCA
        case 0x10: if (--B>0) { WZ=PC=PC+mem.rs8(PC)+1; return cc_ex[0x10]+cc_fd[0x10]; } else { PC++; return cc_fd[0x10]; } // DJNZ
        case 0x11: DE=mem.r16(PC); PC+=2; return cc_fd[0x11]; // LD DE,nn
        case 0x12: mem.w8(DE,A); Z=E+1; W=A; return cc_fd[0x12]; // LD (DE),A
        case 0x13: DE++; return cc_fd[0x13]; // INC DE
        case 0x14: D=inc8(D); return cc_fd[0x14]; // INC D
        case 0x15: D=dec8(D); return cc_fd[0x15]; // DEC D
        case 0x16: D=mem.r8(PC++); return cc_fd[0x16]; // LD D,n
        case 0x17: rla8(); return cc_fd[0x17]; // RLA
        case 0x18: WZ=PC=PC+mem.rs8(PC)+1; return cc_fd[0x18]; // JR d
        case 0x19: IY=add16(IY,DE); return cc_fd[0x19]; // ADD IY,DE
        case 0x1a: A=mem.r8(DE); WZ=DE+1; return cc_fd[0x1a]; // LD A,(DE)
        case 0x1b: DE--; return cc_fd[0x1b]; // DEC DE
        case 0x1c: E=inc8(E); return cc_fd[0x1c]; // INC E
        case 0x1d: E=dec8(E); return cc_fd[0x1d]; // DEC E
        case 0x1e: E=mem.r8(PC++); return cc_fd[0x1e]; // LD E,n
        case 0x1f: rra8(); return cc_fd[0x1f]; // RRA
        case 0x20: if (!(F&ZF)) { WZ=PC=PC+mem.rs8(PC)+1; return cc_ex[0x20]+cc_fd[0x20]; } else { PC++; return cc_fd[0x20]; } // JR NZ,d
        case 0x21: IY=mem.r16(PC); PC+=2; return cc_fd[0x21]; // LD IY,nn
        case 0x22: WZ=mem.r16(PC); mem.w16(WZ++,IY); PC+=2; return cc_fd[0x22]; // LD (nn),IY
        case 0x23: IY++; return cc_fd[0x23]; // INC IY
        case 0x24: IYH=inc8(IYH); return cc_fd[0x24]; // INC IYH
        case 0x25: IYH=dec8(IYH); return cc_fd[0x25]; // DEC IYH
        case 0x26: IYH=mem.r8(PC++); return cc_fd[0x26]; // LD IYH,n
        case 0x27: daa(); return cc_fd[0x27]; // DAA
        case 0x28: if ((F&ZF)) { WZ=PC=PC+mem.rs8(PC)+1; return cc_ex[0x28]+cc_fd[0x28]; } else { PC++; return cc_fd[0x28]; } // JR Z,d
        case 0x29: IY=add16(IY,IY); return cc_fd[0x29]; // ADD IY,IY
        case 0x2a: WZ=mem.r16(PC); IY=mem.r16(WZ++); PC+=2; return cc_fd[0x2a]; // LD IY,(nn)
        case 0x2b: IY--; return cc_fd[0x2b]; // DEC IY
        case 0x2c: IYL=inc8(IYL); return cc_fd[0x2c]; // INC IYL
        case 0x2d: IYL=dec8(IYL); return cc_fd[0x2d]; // DEC IYL
        case 0x2e: IYL=mem.r8(PC++); return cc_fd[0x2e]; // LD IYL,n
        case 0x2f: A^=0xFF; F=(F&(SF|ZF|PF|CF))|HF|NF|(A&(YF|XF)); return cc_fd[0x2f]; // CPL
        case 0x30: if (!(F&CF)) { WZ=PC=PC+mem.rs8(PC)+1; return cc_ex[0x30]+cc_fd[0x30]; } else { PC++; return cc_fd[0x30]; } // JR NC,d
        case 0x31: SP=mem.r16(PC); PC+=2; return cc_fd[0x31]; // LD SP,nn
        case 0x32: WZ=mem.r16(PC); mem.w8(WZ++,A); W=A; PC+=2; return cc_fd[0x32]; // LD (nn),A
        case 0x33: SP++; return cc_fd[0x33]; // INC SP
        case 0x34: { uint16_t a=WZ=IY+mem.rs8(PC++); mem.w8(a,inc8(mem.r8(a))); } return cc_fd[0x34]; // INC (IY+d)
        case 0x35: { uint16_t a=WZ=IY+mem.rs8(PC++); mem.w8(a,dec8(mem.r8(a))); } return cc_fd[0x35]; // DEC (IY+d)
        case 0x36: { uint16_t a=WZ=IY+mem.rs8(PC++); mem.w8(a,mem.r8(PC++)); } return cc_fd[0x36]; // LD (IY+d),n
        case 0x37: F=(F&(SF|ZF|YF|XF|PF))|CF|(A&(YF|XF)); return cc_fd[0x37]; // SCF
        case 0x38: if ((F&CF)) { WZ=PC=PC+mem.rs8(PC)+1; return cc_ex[0x38]+cc_fd[0x38]; } else { PC++; return cc_fd[0x38]; } // JR C,d
        case 0x39: IY=add16(IY,SP); return cc_fd[0x39]; // ADD IY,SP
        case 0x3a: WZ=mem.r16(PC); A=mem.r8(WZ++); PC+=2; return cc_fd[0x3a]; // LD A,(nn)
        case 0x3b: SP--; return cc_fd[0x3b]; // DEC SP
        case 0x3c: A=inc8(A); return cc_fd[0x3c]; // INC A
        case 0x3d: A=dec8(A); return cc_fd[0x3d]; // DEC A
        case 0x3e: A=mem.r8(PC++); return cc_fd[0x3e]; // LD A,n
        case 0x3f: F=((F&(SF|ZF|YF|XF|PF|CF))|((F&CF)<<4)|(A&(YF|XF)))^CF; return cc_fd[0x3f]; // CCF
        case 0x40: B=B; return cc_fd[0x40]; // LD B,B
        case 0x41: B=C; return cc_fd[0x41]; // LD B,C
        case 0x42: B=D; return cc_fd[0x42]; // LD B,D
        case 0x43: B=E; return cc_fd[0x43]; // LD B,E
        case 0x44: B=IYH; return cc_fd[0x44]; // LD B,IYH
        case 0x45: B=IYL; return cc_fd[0x45]; // LD B,IYL
        case 0x46: { uint16_t a=WZ=IY+mem.rs8(PC++); B=mem.r8(a); } return cc_fd[0x46]; // LD B,(IY+d)
        case 0x47: B=A; return cc_fd[0x47]; // LD B,A
        case 0x48: C=B; return cc_fd[0x48]; // LD C,B
        case 0x49: C=C; return cc_fd[0x49]; // LD C,C
        case 0x4a: C=D; return cc_fd[0x4a]; // LD C,D
        case 0x4b: C=E; return cc_fd[0x4b]; // LD C,E
        case 0x4c: C=IYH; return cc_fd[0x4c]; // LD C,IYH
        case 0x4d: C=IYL; return cc_fd[0x4d]; // LD C,IYL
        case 0x4e: { uint16_t a=WZ=IY+mem.rs8(PC++); C=mem.r8(a); } return cc_fd[0x4e]; // LD C,(IY+d)
        case 0x4f: C=A; return cc_fd[0x4f]; // LD C,A
        case 0x50: D=B; return cc_fd[0x50]; // LD D,B
        case 0x51: D=C; return cc_fd[0x51]; // LD D,C
        case 0x52: D=D; return cc_fd[0x52]; // LD D,D
        case 0x53: D=E; return cc_fd[0x53]; // LD D,E
        case 0x54: D=IYH; return cc_fd[0x54]; // LD D,IYH
        case 0x55: D=IYL; return cc_fd[0x55]; // LD D,IYL
        case 0x56: { uint16_t a=WZ=IY+mem.rs8(PC++); D=mem.r8(a); } return cc_fd[0x56]; // LD D,(IY+d)
        case 0x57: D=A; return cc_fd[0x57]; // LD D,A
        case 0x58: E=B; return cc_fd[0x58]; // LD E,B
        case 0x59: E=C; return cc_fd[0x59]; // LD E,C
        case 0x5a: E=D; return cc_fd[0x5a]; // LD E,D
        case 0x5b: E=E; return cc_fd[0x5b]; // LD E,E
        case 0x5c: E=IYH; return cc_fd[0x5c]; // LD E,IYH
        case 0x5d: E=IYL; return cc_fd[0x5d]; // LD E,IYL
        case 0x5e: { uint16_t a=WZ=IY+mem.rs8(PC++); E=mem.r8(a); } return cc_fd[0x5e]; // LD E,(IY+d)
        case 0x5f: E=A; return cc_fd[0x5f]; // LD E,A
        case 0x60: IYH=B; return cc_fd[0x60]; // LD IYH,B
        case 0x61: IYH=C; return cc_fd[0x61]; // LD IYH,C
        case 0x62: IYH=D; return cc_fd[0x62]; // LD IYH,D
        case 0x63: IYH=E; return cc_fd[0x63]; // LD IYH,E
        case 0x64: IYH=IYH; return cc_fd[0x64]; // LD IYH,IYH
        case 0x65: IYH=IYL; return cc_fd[0x65]; // LD IYH,IYL
        case 0x66: { uint16_t a=WZ=IY+mem.rs8(PC++); H=mem.r8(a); } return cc_fd[0x66]; // LD H,(IY+d)
        case 0x67: IYH=A; return cc_fd[0x67]; // LD IYH,A
        case 0x68: IYL=B; return cc_fd[0x68]; // LD IYL,B
        case 0x69: IYL=C; return cc_fd[0x69]; // LD IYL,C
        case 0x6a: IYL=D; return cc_fd[0x6a]; // LD IYL,D
        case 0x6b: IYL=E; return cc_fd[0x6b]; // LD IYL,E
        case 0x6c: IYL=IYH; return cc_fd[0x6c]; // LD IYL,IYH
        case 0x6d: IYL=IYL; return cc_fd[0x6d]; // LD IYL,IYL
        case 0x6e: { uint16_t a=WZ=IY+mem.rs8(PC++); L=mem.r8(a); } return cc_fd[0x6e]; // LD L,(IY+d)
        case 0x6f: IYL=A; return cc_fd[0x6f]; // LD IYL,A
        case 0x70: { uint16_t a=WZ=IY+mem.rs8(PC++); mem.w8(a,B); } return cc_fd[0x70]; // LD (IY+d),B
        case 0x71: { uint16_t a=WZ=IY+mem.rs8(PC++); mem.w8(a,C); } return cc_fd[0x71]; // LD (IY+d),C
        case 0x72: { uint16_t a=WZ=IY+mem.rs8(PC++); mem.w8(a,D); } return cc_fd[0x72]; // LD (IY+d),D
        case 0x73: { uint16_t a=WZ=IY+mem.rs8(PC++); mem.w8(a,E); } return cc_fd[0x73]; // LD (IY+d),E
        case 0x74: { uint16_t a=WZ=IY+mem.rs8(PC++); mem.w8(a,H); } return cc_fd[0x74]; // LD (IY+d),H
        case 0x75: { uint16_t a=WZ=IY+mem.rs8(PC++); mem.w8(a,L); } return cc_fd[0x75]; // LD (IY+d),L
        case 0x76: halt(); return cc_fd[0x76]; // HALT
        case 0x77: { uint16_t a=WZ=IY+mem.rs8(PC++); mem.w8(a,A); } return cc_fd[0x77]; // LD (IY+d),A
        case 0x78: A=B; return cc_fd[0x78]; // LD A,B
        case 0x79: A=C; return cc_fd[0x79]; // LD A,C
        case 0x7a: A=D; return cc_fd[0x7a]; // LD A,D
        case 0x7b: A=E; return cc_fd[0x7b]; // LD A,E
        case 0x7c: A=IYH; return cc_fd[0x7c]; // LD A,IYH
        case 0x7d: A=IYL; return cc_fd[0x7d]; // LD A,IYL
        case 0x7e: { uint16_t a=WZ=IY+mem.rs8(PC++); A=mem.r8(a); } return cc_fd[0x7e]; // LD A,(IY+d)
        case 0x7f: A=A; return cc_fd[0x7f]; // LD A,A
        case 0x80: add8(B); return cc_fd[0x80]; // ADD B
        case 0x81: add8(C); return cc_fd[0x81]; // ADD C
        case 0x82: add8(D); return cc_fd[0x82]; // ADD D
        case 0x83: add8(E); return cc_fd[0x83]; // ADD E
        case 0x84: add8(IYH); return cc_fd[0x84]; // ADD IYH
        case 0x85: add8(IYL); return cc_fd[0x85]; // ADD IYL
        case 0x86: { uint16_t a=WZ=IY+mem.rs8(PC++); add8(mem.r8(a)); } return cc_fd[0x86]; // ADD (IY+d)
        case 0x87: add8(A); return cc_fd[0x87]; // ADD A
        case 0x88: adc8(B); return cc_fd[0x88]; // ADC B
        case 0x89: adc8(C); return cc_fd[0x89]; // ADC C
        case 0x8a: adc8(D); return cc_fd[0x8a]; // ADC D
        case 0x8b: adc8(E); return cc_fd[0x8b]; // ADC E
        case 0x8c: adc8(IYH); return cc_fd[0x8c]; // ADC IYH
        case 0x8d: adc8(IYL); return cc_fd[0x8d]; // ADC IYL
        case 0x8e: { uint16_t a=WZ=IY+mem.rs8(PC++); adc8(mem.r8(a)); } return cc_fd[0x8e]; // ADC (IY+d)
        case 0x8f: adc8(A); return cc_fd[0x8f]; // ADC A
        case 0x90: sub8(B); return cc_fd[0x90]; // SUB B
        case 0x91: sub8(C); return cc_fd[0x91]; // SUB C
        case 0x92: sub8(D); return cc_fd[0x92]; // SUB D
        case 0x93: sub8(E); return cc_fd[0x93]; // SUB E
        case 0x94: sub8(IYH); return cc_fd[0x94]; // SUB IYH
        case 0x95: sub8(IYL); return cc_fd[0x95]; // SUB IYL
        case 0x96: { uint16_t a=WZ=IY+mem.rs8(PC++); sub8(mem.r8(a)); } return cc_fd[0x96]; // SUB (IY+d)
        case 0x97: sub8(A); return cc_fd[0x97]; // SUB A
        case 0x98: sbc8(B); return cc_fd[0x98]; // SBC B
        case 0x99: sbc8(C); return cc_fd[0x99]; // SBC C
        case 0x9a: sbc8(D); return cc_fd[0x9a]; // SBC D
        case 0x9b: sbc8(E); return cc_fd[0x9b]; // SBC E
        case 0x9c: sbc8(IYH); return cc_fd[0x9c]; // SBC IYH
        case 0x9d: sbc8(IYL); return cc_fd[0x9d]; // SBC IYL
        case 0x9e: { uint16_t a=WZ=IY+mem.rs8(PC++); sbc8(mem.r8(a)); } return cc_fd[0x9e]; // SBC (IY+d)
        case 0x9f: sbc8(A); return cc_fd[0x9f]; // SBC A
        case 0xa0: and8(B); return cc_fd[0xa0]; // AND B
        case 0xa1: and8(C); return cc_fd[0xa1]; // AND C
        case 0xa2: and8(D); return cc_fd[0xa2]; // AND D
        case 0xa3: and8(E); return cc_fd[0xa3]; // AND E
        case 0xa4: and8(IYH); return cc_fd[0xa4]; // AND IYH
        case 0xa5: and8(IYL); return cc_fd[0xa5]; // AND IYL
        case 0xa6: { uint16_t a=WZ=IY+mem.rs8(PC++); and8(mem.r8(a)); } return cc_fd[0xa6]; // AND (IY+d)
        case 0xa7: and8(A); return cc_fd[0xa7]; // AND A
        case 0xa8: xor8(B); return cc_fd[0xa8]; // XOR B
        case 0xa9: xor8(C); return cc_fd[0xa9]; // XOR C
        case 0xaa: xor8(D); return cc_fd[0xaa]; // XOR D
        case 0xab: xor8(E); return cc_fd[0xab]; // XOR E
        case 0xac: xor8(IYH); return cc_fd[0xac]; // XOR IYH
        case 0xad: xor8(IYL); return cc_fd[0xad]; // XOR IYL
        case 0xae: { uint16_t a=WZ=IY+mem.rs8(PC++); xor8(mem.r8(a)); } return cc_fd[0xae]; // XOR (IY+d)
        case 0xaf: xor8(A); return cc_fd[0xaf]; // XOR A
        case 0xb0: or8(B); return cc_fd[0xb0]; // OR B
        case 0xb1: or8(C); return cc_fd[0xb1]; // OR C
        case 0xb2: or8(D); return cc_fd[0xb2]; // OR D
        case 0xb3: or8(E); return cc_fd[0xb3]; // OR E
        case 0xb4: or8(IYH); return cc_fd[0xb4]; // OR IYH
        case 0xb5: or8(IYL); return cc_fd[0xb5]; // OR IYL
        case 0xb6: { uint16_t a=WZ=IY+mem.rs8(PC++); or8(mem.r8(a)); } return cc_fd[0xb6]; // OR (IY+d)
        case 0xb7: or8(A); return cc_fd[0xb7]; // OR A
        case 0xb8: cp8(B); return cc_fd[0xb8]; // CP B
        case 0xb9: cp8(C); return cc_fd[0xb9]; // CP C
        case 0xba: cp8(D); return cc_fd[0xba]; // CP D
        case 0xbb: cp8(E); return cc_fd[0xbb]; // CP E
        case 0xbc: cp8(IYH); return cc_fd[0xbc]; // CP IYH
        case 0xbd: cp8(IYL); return cc_fd[0xbd]; // CP IYL
        case 0xbe: { uint16_t a=WZ=IY+mem.rs8(PC++); cp8(mem.r8(a)); } return cc_fd[0xbe]; // CP (IY+d)
        case 0xbf: cp8(A); return cc_fd[0xbf]; // CP A
        case 0xc0: if (!(F&ZF)) { WZ=PC=mem.r16(SP); SP+=2; return cc_ex[0xc0]+cc_fd[0xc0]; } else return cc_fd[0xc0]; // RET NZ
        case 0xc1: BC=mem.r16(SP); SP+=2; return cc_fd[0xc1]; // POP BC
        case 0xc2: WZ=mem.r16(PC); if (!(F&ZF)) { PC=WZ; } else { PC+=2; }; return cc_fd[0xc2]; // JP NZ,nn
        case 0xc3: WZ=PC=mem.r16(PC); return cc_fd[0xc3]; // JP nn
        case 0xc4: WZ=mem.r16(PC); PC+=2; if (!(F&ZF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return cc_ex[0xc4]+cc_fd[0xc4]; } else { return cc_fd[0xc4]; } // CALL NZ,nn
        case 0xc5: SP-=2; mem.w16(SP,BC); return cc_fd[0xc5]; // PUSH BC
        case 0xc6: add8(mem.r8(PC++)); return cc_fd[0xc6]; // ADD n
        case 0xc7: rst(0x0); return cc_fd[0xc7]; // RST 0x0
        case 0xc8: if ((F&ZF)) { WZ=PC=mem.r16(SP); SP+=2; return cc_ex[0xc8]+cc_fd[0xc8]; } else return cc_fd[0xc8]; // RET Z
        case 0xc9: WZ=PC=mem.r16(SP); SP+=2; return cc_fd[0xc9]; // RET
        case 0xca: WZ=mem.r16(PC); if ((F&ZF)) { PC=WZ; } else { PC+=2; }; return cc_fd[0xca]; // JP Z,nn
        case 0xcb:
          { const int d = mem.rs8(PC++);
          switch (fetch_op()) {
            case 0x0: { uint16_t a=WZ=IY+d;; B=rlc8(mem.r8(a)); mem.w8(a,B); } return cc_fdcb[0x0]; // RLC (IY+d),B
            case 0x1: { uint16_t a=WZ=IY+d;; C=rlc8(mem.r8(a)); mem.w8(a,C); } return cc_fdcb[0x1]; // RLC (IY+d),C
            case 0x2: { uint16_t a=WZ=IY+d;; D=rlc8(mem.r8(a)); mem.w8(a,D); } return cc_fdcb[0x2]; // RLC (IY+d),D
            case 0x3: { uint16_t a=WZ=IY+d;; E=rlc8(mem.r8(a)); mem.w8(a,E); } return cc_fdcb[0x3]; // RLC (IY+d),E
            case 0x4: { uint16_t a=WZ=IY+d;; H=rlc8(mem.r8(a)); mem.w8(a,H); } return cc_fdcb[0x4]; // RLC (IY+d),H
            case 0x5: { uint16_t a=WZ=IY+d;; L=rlc8(mem.r8(a)); mem.w8(a,L); } return cc_fdcb[0x5]; // RLC (IY+d),L
            case 0x6: { uint16_t a=WZ=IY+d;; mem.w8(a,rlc8(mem.r8(a))); } return cc_fdcb[0x6]; // RLC (IY+d)
            case 0x7: { uint16_t a=WZ=IY+d;; A=rlc8(mem.r8(a)); mem.w8(a,A); } return cc_fdcb[0x7]; // RLC (IY+d),A
            case 0x8: { uint16_t a=WZ=IY+d;; B=rrc8(mem.r8(a)); mem.w8(a,B); } return cc_fdcb[0x8]; // RRC (IY+d),B
            case 0x9: { uint16_t a=WZ=IY+d;; C=rrc8(mem.r8(a)); mem.w8(a,C); } return cc_fdcb[0x9]; // RRC (IY+d),C
            case 0xa: { uint16_t a=WZ=IY+d;; D=rrc8(mem.r8(a)); mem.w8(a,D); } return cc_fdcb[0xa]; // RRC (IY+d),D
            case 0xb: { uint16_t a=WZ=IY+d;; E=rrc8(mem.r8(a)); mem.w8(a,E); } return cc_fdcb[0xb]; // RRC (IY+d),E
            case 0xc: { uint16_t a=WZ=IY+d;; H=rrc8(mem.r8(a)); mem.w8(a,H); } return cc_fdcb[0xc]; // RRC (IY+d),H
            case 0xd: { uint16_t a=WZ=IY+d;; L=rrc8(mem.r8(a)); mem.w8(a,L); } return cc_fdcb[0xd]; // RRC (IY+d),L
            case 0xe: { uint16_t a=WZ=IY+d;; mem.w8(a,rrc8(mem.r8(a))); } return cc_fdcb[0xe]; // RRC (IY+d)
            case 0xf: { uint16_t a=WZ=IY+d;; A=rrc8(mem.r8(a)); mem.w8(a,A); } return cc_fdcb[0xf]; // RRC (IY+d),A
            case 0x10: { uint16_t a=WZ=IY+d;; B=rl8(mem.r8(a)); mem.w8(a,B); } return cc_fdcb[0x10]; // RL (IY+d),B
            case 0x11: { uint16_t a=WZ=IY+d;; C=rl8(mem.r8(a)); mem.w8(a,C); } return cc_fdcb[0x11]; // RL (IY+d),C
            case 0x12: { uint16_t a=WZ=IY+d;; D=rl8(mem.r8(a)); mem.w8(a,D); } return cc_fdcb[0x12]; // RL (IY+d),D
            case 0x13: { uint16_t a=WZ=IY+d;; E=rl8(mem.r8(a)); mem.w8(a,E); } return cc_fdcb[0x13]; // RL (IY+d),E
            case 0x14: { uint16_t a=WZ=IY+d;; H=rl8(mem.r8(a)); mem.w8(a,H); } return cc_fdcb[0x14]; // RL (IY+d),H
            case 0x15: { uint16_t a=WZ=IY+d;; L=rl8(mem.r8(a)); mem.w8(a,L); } return cc_fdcb[0x15]; // RL (IY+d),L
            case 0x16: { uint16_t a=WZ=IY+d;; mem.w8(a,rl8(mem.r8(a))); } return cc_fdcb[0x16]; // RL (IY+d)
            case 0x17: { uint16_t a=WZ=IY+d;; A=rl8(mem.r8(a)); mem.w8(a,A); } return cc_fdcb[0x17]; // RL (IY+d),A
            case 0x18: { uint16_t a=WZ=IY+d;; B=rr8(mem.r8(a)); mem.w8(a,B); } return cc_fdcb[0x18]; // RR (IY+d),B
            case 0x19: { uint16_t a=WZ=IY+d;; C=rr8(mem.r8(a)); mem.w8(a,C); } return cc_fdcb[0x19]; // RR (IY+d),C
            case 0x1a: { uint16_t a=WZ=IY+d;; D=rr8(mem.r8(a)); mem.w8(a,D); } return cc_fdcb[0x1a]; // RR (IY+d),D
            case 0x1b: { uint16_t a=WZ=IY+d;; E=rr8(mem.r8(a)); mem.w8(a,E); } return cc_fdcb[0x1b]; // RR (IY+d),E
            case 0x1c: { uint16_t a=WZ=IY+d;; H=rr8(mem.r8(a)); mem.w8(a,H); } return cc_fdcb[0x1c]; // RR (IY+d),H
            case 0x1d: { uint16_t a=WZ=IY+d;; L=rr8(mem.r8(a)); mem.w8(a,L); } return cc_fdcb[0x1d]; // RR (IY+d),L
            case 0x1e: { uint16_t a=WZ=IY+d;; mem.w8(a,rr8(mem.r8(a))); } return cc_fdcb[0x1e]; // RR (IY+d)
            case 0x1f: { uint16_t a=WZ=IY+d;; A=rr8(mem.r8(a)); mem.w8(a,A); } return cc_fdcb[0x1f]; // RR (IY+d),A
            case 0x20: { uint16_t a=WZ=IY+d;; B=sla8(mem.r8(a)); mem.w8(a,B); } return cc_fdcb[0x20]; // SLA (IY+d),B
            case 0x21: { uint16_t a=WZ=IY+d;; C=sla8(mem.r8(a)); mem.w8(a,C); } return cc_fdcb[0x21]; // SLA (IY+d),C
            case 0x22: { uint16_t a=WZ=IY+d;; D=sla8(mem.r8(a)); mem.w8(a,D); } return cc_fdcb[0x22]; // SLA (IY+d),D
            case 0x23: { uint16_t a=WZ=IY+d;; E=sla8(mem.r8(a)); mem.w8(a,E); } return cc_fdcb[0x23]; // SLA (IY+d),E
            case 0x24: { uint16_t a=WZ=IY+d;; H=sla8(mem.r8(a)); mem.w8(a,H); } return cc_fdcb[0x24]; // SLA (IY+d),H
            case 0x25: { uint16_t a=WZ=IY+d;; L=sla8(mem.r8(a)); mem.w8(a,L); } return cc_fdcb[0x25]; // SLA (IY+d),L
            case 0x26: { uint16_t a=WZ=IY+d;; mem.w8(a,sla8(mem.r8(a))); } return cc_fdcb[0x26]; // SLA (IY+d)
            case 0x27: { uint16_t a=WZ=IY+d;; A=sla8(mem.r8(a)); mem.w8(a,A); } return cc_fdcb[0x27]; // SLA (IY+d),A
            case 0x28: { uint16_t a=WZ=IY+d;; B=sra8(mem.r8(a)); mem.w8(a,B); } return cc_fdcb[0x28]; // SRA (IY+d),B
            case 0x29: { uint16_t a=WZ=IY+d;; C=sra8(mem.r8(a)); mem.w8(a,C); } return cc_fdcb[0x29]; // SRA (IY+d),C
            case 0x2a: { uint16_t a=WZ=IY+d;; D=sra8(mem.r8(a)); mem.w8(a,D); } return cc_fdcb[0x2a]; // SRA (IY+d),D
            case 0x2b: { uint16_t a=WZ=IY+d;; E=sra8(mem.r8(a)); mem.w8(a,E); } return cc_fdcb[0x2b]; // SRA (IY+d),E
            case 0x2c: { uint16_t a=WZ=IY+d;; H=sra8(mem.r8(a)); mem.w8(a,H); } return cc_fdcb[0x2c]; // SRA (IY+d),H
            case 0x2d: { uint16_t a=WZ=IY+d;; L=sra8(mem.r8(a)); mem.w8(a,L); } return cc_fdcb[0x2d]; // SRA (IY+d),L
            case 0x2e: { uint16_t a=WZ=IY+d;; mem.w8(a,sra8(mem.r8(a))); } return cc_fdcb[0x2e]; // SRA (IY+d)
            case 0x2f: { uint16_t a=WZ=IY+d;; A=sra8(mem.r8(a)); mem.w8(a,A); } return cc_fdcb[0x2f]; // SRA (IY+d),A
            case 0x30: { uint16_t a=WZ=IY+d;; B=sll8(mem.r8(a)); mem.w8(a,B); } return cc_fdcb[0x30]; // SLL (IY+d),B
            case 0x31: { uint16_t a=WZ=IY+d;; C=sll8(mem.r8(a)); mem.w8(a,C); } return cc_fdcb[0x31]; // SLL (IY+d),C
            case 0x32: { uint16_t a=WZ=IY+d;; D=sll8(mem.r8(a)); mem.w8(a,D); } return cc_fdcb[0x32]; // SLL (IY+d),D
            case 0x33: { uint16_t a=WZ=IY+d;; E=sll8(mem.r8(a)); mem.w8(a,E); } return cc_fdcb[0x33]; // SLL (IY+d),E
            case 0x34: { uint16_t a=WZ=IY+d;; H=sll8(mem.r8(a)); mem.w8(a,H); } return cc_fdcb[0x34]; // SLL (IY+d),H
            case 0x35: { uint16_t a=WZ=IY+d;; L=sll8(mem.r8(a)); mem.w8(a,L); } return cc_fdcb[0x35]; // SLL (IY+d),L
            case 0x36: { uint16_t a=WZ=IY+d;; mem.w8(a,sll8(mem.r8(a))); } return cc_fdcb[0x36]; // SLL (IY+d)
            case 0x37: { uint16_t a=WZ=IY+d;; A=sll8(mem.r8(a)); mem.w8(a,A); } return cc_fdcb[0x37]; // SLL (IY+d),A
            case 0x38: { uint16_t a=WZ=IY+d;; B=srl8(mem.r8(a)); mem.w8(a,B); } return cc_fdcb[0x38]; // SRL (IY+d),B
            case 0x39: { uint16_t a=WZ=IY+d;; C=srl8(mem.r8(a)); mem.w8(a,C); } return cc_fdcb[0x39]; // SRL (IY+d),C
            case 0x3a: { uint16_t a=WZ=IY+d;; D=srl8(mem.r8(a)); mem.w8(a,D); } return cc_fdcb[0x3a]; // SRL (IY+d),D
            case 0x3b: { uint16_t a=WZ=IY+d;; E=srl8(mem.r8(a)); mem.w8(a,E); } return cc_fdcb[0x3b]; // SRL (IY+d),E
            case 0x3c: { uint16_t a=WZ=IY+d;; H=srl8(mem.r8(a)); mem.w8(a,H); } return cc_fdcb[0x3c]; // SRL (IY+d),H
            case 0x3d: { uint16_t a=WZ=IY+d;; L=srl8(mem.r8(a)); mem.w8(a,L); } return cc_fdcb[0x3d]; // SRL (IY+d),L
            case 0x3e: { uint16_t a=WZ=IY+d;; mem.w8(a,srl8(mem.r8(a))); } return cc_fdcb[0x3e]; // SRL (IY+d)
            case 0x3f: { uint16_t a=WZ=IY+d;; A=srl8(mem.r8(a)); mem.w8(a,A); } return cc_fdcb[0x3f]; // SRL (IY+d),A
            case 0x40: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x1); } return cc_fdcb[0x40]; // BIT 0,(IY+d)
            case 0x41: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x1); } return cc_fdcb[0x41]; // BIT 0,(IY+d)
            case 0x42: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x1); } return cc_fdcb[0x42]; // BIT 0,(IY+d)
            case 0x43: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x1); } return cc_fdcb[0x43]; // BIT 0,(IY+d)
            case 0x44: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x1); } return cc_fdcb[0x44]; // BIT 0,(IY+d)
            case 0x45: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x1); } return cc_fdcb[0x45]; // BIT 0,(IY+d)
            case 0x46: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x1); } return cc_fdcb[0x46]; // BIT 0,(IY+d)
            case 0x47: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x1); } return cc_fdcb[0x47]; // BIT 0,(IY+d)
            case 0x48: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x2); } return cc_fdcb[0x48]; // BIT 1,(IY+d)
            case 0x49: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x2); } return cc_fdcb[0x49]; // BIT 1,(IY+d)
            case 0x4a: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x2); } return cc_fdcb[0x4a]; // BIT 1,(IY+d)
            case 0x4b: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x2); } return cc_fdcb[0x4b]; // BIT 1,(IY+d)
            case 0x4c: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x2); } return cc_fdcb[0x4c]; // BIT 1,(IY+d)
            case 0x4d: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x2); } return cc_fdcb[0x4d]; // BIT 1,(IY+d)
            case 0x4e: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x2); } return cc_fdcb[0x4e]; // BIT 1,(IY+d)
            case 0x4f: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x2); } return cc_fdcb[0x4f]; // BIT 1,(IY+d)
            case 0x50: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x4); } return cc_fdcb[0x50]; // BIT 2,(IY+d)
            case 0x51: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x4); } return cc_fdcb[0x51]; // BIT 2,(IY+d)
            case 0x52: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x4); } return cc_fdcb[0x52]; // BIT 2,(IY+d)
            case 0x53: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x4); } return cc_fdcb[0x53]; // BIT 2,(IY+d)
            case 0x54: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x4); } return cc_fdcb[0x54]; // BIT 2,(IY+d)
            case 0x55: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x4); } return cc_fdcb[0x55]; // BIT 2,(IY+d)
            case 0x56: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x4); } return cc_fdcb[0x56]; // BIT 2,(IY+d)
            case 0x57: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x4); } return cc_fdcb[0x57]; // BIT 2,(IY+d)
            case 0x58: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x8); } return cc_fdcb[0x58]; // BIT 3,(IY+d)
            case 0x59: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x8); } return cc_fdcb[0x59]; // BIT 3,(IY+d)
            case 0x5a: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x8); } return cc_fdcb[0x5a]; // BIT 3,(IY+d)
            case 0x5b: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x8); } return cc_fdcb[0x5b]; // BIT 3,(IY+d)
            case 0x5c: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x8); } return cc_fdcb[0x5c]; // BIT 3,(IY+d)
            case 0x5d: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x8); } return cc_fdcb[0x5d]; // BIT 3,(IY+d)
            case 0x5e: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x8); } return cc_fdcb[0x5e]; // BIT 3,(IY+d)
            case 0x5f: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x8); } return cc_fdcb[0x5f]; // BIT 3,(IY+d)
            case 0x60: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x10); } return cc_fdcb[0x60]; // BIT 4,(IY+d)
            case 0x61: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x10); } return cc_fdcb[0x61]; // BIT 4,(IY+d)
            case 0x62: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x10); } return cc_fdcb[0x62]; // BIT 4,(IY+d)
            case 0x63: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x10); } return cc_fdcb[0x63]; // BIT 4,(IY+d)
            case 0x64: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x10); } return cc_fdcb[0x64]; // BIT 4,(IY+d)
            case 0x65: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x10); } return cc_fdcb[0x65]; // BIT 4,(IY+d)
            case 0x66: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x10); } return cc_fdcb[0x66]; // BIT 4,(IY+d)
            case 0x67: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x10); } return cc_fdcb[0x67]; // BIT 4,(IY+d)
            case 0x68: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x20); } return cc_fdcb[0x68]; // BIT 5,(IY+d)
            case 0x69: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x20); } return cc_fdcb[0x69]; // BIT 5,(IY+d)
            case 0x6a: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x20); } return cc_fdcb[0x6a]; // BIT 5,(IY+d)
            case 0x6b: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x20); } return cc_fdcb[0x6b]; // BIT 5,(IY+d)
            case 0x6c: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x20); } return cc_fdcb[0x6c]; // BIT 5,(IY+d)
            case 0x6d: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x20); } return cc_fdcb[0x6d]; // BIT 5,(IY+d)
            case 0x6e: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x20); } return cc_fdcb[0x6e]; // BIT 5,(IY+d)
            case 0x6f: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x20); } return cc_fdcb[0x6f]; // BIT 5,(IY+d)
            case 0x70: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x40); } return cc_fdcb[0x70]; // BIT 6,(IY+d)
            case 0x71: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x40); } return cc_fdcb[0x71]; // BIT 6,(IY+d)
            case 0x72: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x40); } return cc_fdcb[0x72]; // BIT 6,(IY+d)
            case 0x73: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x40); } return cc_fdcb[0x73]; // BIT 6,(IY+d)
            case 0x74: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x40); } return cc_fdcb[0x74]; // BIT 6,(IY+d)
            case 0x75: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x40); } return cc_fdcb[0x75]; // BIT 6,(IY+d)
            case 0x76: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x40); } return cc_fdcb[0x76]; // BIT 6,(IY+d)
            case 0x77: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x40); } return cc_fdcb[0x77]; // BIT 6,(IY+d)
            case 0x78: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x80); } return cc_fdcb[0x78]; // BIT 7,(IY+d)
            case 0x79: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x80); } return cc_fdcb[0x79]; // BIT 7,(IY+d)
            case 0x7a: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x80); } return cc_fdcb[0x7a]; // BIT 7,(IY+d)
            case 0x7b: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x80); } return cc_fdcb[0x7b]; // BIT 7,(IY+d)
            case 0x7c: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x80); } return cc_fdcb[0x7c]; // BIT 7,(IY+d)
            case 0x7d: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x80); } return cc_fdcb[0x7d]; // BIT 7,(IY+d)
            case 0x7e: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x80); } return cc_fdcb[0x7e]; // BIT 7,(IY+d)
            case 0x7f: { uint16_t a=WZ=IY+d;; ibit(mem.r8(a),0x80); } return cc_fdcb[0x7f]; // BIT 7,(IY+d)
            case 0x80: { uint16_t a=WZ=IY+d;; B=mem.r8(a)&~0x1; mem.w8(a,B); } return cc_fdcb[0x80]; // RES 0,(IY+d),B
            case 0x81: { uint16_t a=WZ=IY+d;; C=mem.r8(a)&~0x1; mem.w8(a,C); } return cc_fdcb[0x81]; // RES 0,(IY+d),C
            case 0x82: { uint16_t a=WZ=IY+d;; D=mem.r8(a)&~0x1; mem.w8(a,D); } return cc_fdcb[0x82]; // RES 0,(IY+d),D
            case 0x83: { uint16_t a=WZ=IY+d;; E=mem.r8(a)&~0x1; mem.w8(a,E); } return cc_fdcb[0x83]; // RES 0,(IY+d),E
            case 0x84: { uint16_t a=WZ=IY+d;; H=mem.r8(a)&~0x1; mem.w8(a,H); } return cc_fdcb[0x84]; // RES 0,(IY+d),H
            case 0x85: { uint16_t a=WZ=IY+d;; L=mem.r8(a)&~0x1; mem.w8(a,L); } return cc_fdcb[0x85]; // RES 0,(IY+d),L
            case 0x86: { uint16_t a=WZ=IY+d;; mem.w8(a,mem.r8(a)&~0x1); } return cc_fdcb[0x86]; // RES 0,(IY+d)
            case 0x87: { uint16_t a=WZ=IY+d;; A=mem.r8(a)&~0x1; mem.w8(a,A); } return cc_fdcb[0x87]; // RES 0,(IY+d),A
            case 0x88: { uint16_t a=WZ=IY+d;; B=mem.r8(a)&~0x2; mem.w8(a,B); } return cc_fdcb[0x88]; // RES 1,(IY+d),B
            case 0x89: { uint16_t a=WZ=IY+d;; C=mem.r8(a)&~0x2; mem.w8(a,C); } return cc_fdcb[0x89]; // RES 1,(IY+d),C
            case 0x8a: { uint16_t a=WZ=IY+d;; D=mem.r8(a)&~0x2; mem.w8(a,D); } return cc_fdcb[0x8a]; // RES 1,(IY+d),D
            case 0x8b: { uint16_t a=WZ=IY+d;; E=mem.r8(a)&~0x2; mem.w8(a,E); } return cc_fdcb[0x8b]; // RES 1,(IY+d),E
            case 0x8c: { uint16_t a=WZ=IY+d;; H=mem.r8(a)&~0x2; mem.w8(a,H); } return cc_fdcb[0x8c]; // RES 1,(IY+d),H
            case 0x8d: { uint16_t a=WZ=IY+d;; L=mem.r8(a)&~0x2; mem.w8(a,L); } return cc_fdcb[0x8d]; // RES 1,(IY+d),L
            case 0x8e: { uint16_t a=WZ=IY+d;; mem.w8(a,mem.r8(a)&~0x2); } return cc_fdcb[0x8e]; // RES 1,(IY+d)
            case 0x8f: { uint16_t a=WZ=IY+d;; A=mem.r8(a)&~0x2; mem.w8(a,A); } return cc_fdcb[0x8f]; // RES 1,(IY+d),A
            case 0x90: { uint16_t a=WZ=IY+d;; B=mem.r8(a)&~0x4; mem.w8(a,B); } return cc_fdcb[0x90]; // RES 2,(IY+d),B
            case 0x91: { uint16_t a=WZ=IY+d;; C=mem.r8(a)&~0x4; mem.w8(a,C); } return cc_fdcb[0x91]; // RES 2,(IY+d),C
            case 0x92: { uint16_t a=WZ=IY+d;; D=mem.r8(a)&~0x4; mem.w8(a,D); } return cc_fdcb[0x92]; // RES 2,(IY+d),D
            case 0x93: { uint16_t a=WZ=IY+d;; E=mem.r8(a)&~0x4; mem.w8(a,E); } return cc_fdcb[0x93]; // RES 2,(IY+d),E
            case 0x94: { uint16_t a=WZ=IY+d;; H=mem.r8(a)&~0x4; mem.w8(a,H); } return cc_fdcb[0x94]; // RES 2,(IY+d),H
            case 0x95: { uint16_t a=WZ=IY+d;; L=mem.r8(a)&~0x4; mem.w8(a,L); } return cc_fdcb[0x95]; // RES 2,(IY+d),L
            case 0x96: { uint16_t a=WZ=IY+d;; mem.w8(a,mem.r8(a)&~0x4); } return cc_fdcb[0x96]; // RES 2,(IY+d)
            case 0x97: { uint16_t a=WZ=IY+d;; A=mem.r8(a)&~0x4; mem.w8(a,A); } return cc_fdcb[0x97]; // RES 2,(IY+d),A
            case 0x98: { uint16_t a=WZ=IY+d;; B=mem.r8(a)&~0x8; mem.w8(a,B); } return cc_fdcb[0x98]; // RES 3,(IY+d),B
            case 0x99: { uint16_t a=WZ=IY+d;; C=mem.r8(a)&~0x8; mem.w8(a,C); } return cc_fdcb[0x99]; // RES 3,(IY+d),C
            case 0x9a: { uint16_t a=WZ=IY+d;; D=mem.r8(a)&~0x8; mem.w8(a,D); } return cc_fdcb[0x9a]; // RES 3,(IY+d),D
            case 0x9b: { uint16_t a=WZ=IY+d;; E=mem.r8(a)&~0x8; mem.w8(a,E); } return cc_fdcb[0x9b]; // RES 3,(IY+d),E
            case 0x9c: { uint16_t a=WZ=IY+d;; H=mem.r8(a)&~0x8; mem.w8(a,H); } return cc_fdcb[0x9c]; // RES 3,(IY+d),H
            case 0x9d: { uint16_t a=WZ=IY+d;; L=mem.r8(a)&~0x8; mem.w8(a,L); } return cc_fdcb[0x9d]; // RES 3,(IY+d),L
            case 0x9e: { uint16_t a=WZ=IY+d;; mem.w8(a,mem.r8(a)&~0x8); } return cc_fdcb[0x9e]; // RES 3,(IY+d)
            case 0x9f: { uint16_t a=WZ=IY+d;; A=mem.r8(a)&~0x8; mem.w8(a,A); } return cc_fdcb[0x9f]; // RES 3,(IY+d),A
            case 0xa0: { uint16_t a=WZ=IY+d;; B=mem.r8(a)&~0x10; mem.w8(a,B); } return cc_fdcb[0xa0]; // RES 4,(IY+d),B
            case 0xa1: { uint16_t a=WZ=IY+d;; C=mem.r8(a)&~0x10; mem.w8(a,C); } return cc_fdcb[0xa1]; // RES 4,(IY+d),C
            case 0xa2: { uint16_t a=WZ=IY+d;; D=mem.r8(a)&~0x10; mem.w8(a,D); } return cc_fdcb[0xa2]; // RES 4,(IY+d),D
            case 0xa3: { uint16_t a=WZ=IY+d;; E=mem.r8(a)&~0x10; mem.w8(a,E); } return cc_fdcb[0xa3]; // RES 4,(IY+d),E
            case 0xa4: { uint16_t a=WZ=IY+d;; H=mem.r8(a)&~0x10; mem.w8(a,H); } return cc_fdcb[0xa4]; // RES 4,(IY+d),H
            case 0xa5: { uint16_t a=WZ=IY+d;; L=mem.r8(a)&~0x10; mem.w8(a,L); } return cc_fdcb[0xa5]; // RES 4,(IY+d),L
            case 0xa6: { uint16_t a=WZ=IY+d;; mem.w8(a,mem.r8(a)&~0x10); } return cc_fdcb[0xa6]; // RES 4,(IY+d)
            case 0xa7: { uint16_t a=WZ=IY+d;; A=mem.r8(a)&~0x10; mem.w8(a,A); } return cc_fdcb[0xa7]; // RES 4,(IY+d),A
            case 0xa8: { uint16_t a=WZ=IY+d;; B=mem.r8(a)&~0x20; mem.w8(a,B); } return cc_fdcb[0xa8]; // RES 5,(IY+d),B
            case 0xa9: { uint16_t a=WZ=IY+d;; C=mem.r8(a)&~0x20; mem.w8(a,C); } return cc_fdcb[0xa9]; // RES 5,(IY+d),C
            case 0xaa: { uint16_t a=WZ=IY+d;; D=mem.r8(a)&~0x20; mem.w8(a,D); } return cc_fdcb[0xaa]; // RES 5,(IY+d),D
            case 0xab: { uint16_t a=WZ=IY+d;; E=mem.r8(a)&~0x20; mem.w8(a,E); } return cc_fdcb[0xab]; // RES 5,(IY+d),E
            case 0xac: { uint16_t a=WZ=IY+d;; H=mem.r8(a)&~0x20; mem.w8(a,H); } return cc_fdcb[0xac]; // RES 5,(IY+d),H
            case 0xad: { uint16_t a=WZ=IY+d;; L=mem.r8(a)&~0x20; mem.w8(a,L); } return cc_fdcb[0xad]; // RES 5,(IY+d),L
            case 0xae: { uint16_t a=WZ=IY+d;; mem.w8(a,mem.r8(a)&~0x20); } return cc_fdcb[0xae]; // RES 5,(IY+d)
            case 0xaf: { uint16_t a=WZ=IY+d;; A=mem.r8(a)&~0x20; mem.w8(a,A); } return cc_fdcb[0xaf]; // RES 5,(IY+d),A
            case 0xb0: { uint16_t a=WZ=IY+d;; B=mem.r8(a)&~0x40; mem.w8(a,B); } return cc_fdcb[0xb0]; // RES 6,(IY+d),B
            case 0xb1: { uint16_t a=WZ=IY+d;; C=mem.r8(a)&~0x40; mem.w8(a,C); } return cc_fdcb[0xb1]; // RES 6,(IY+d),C
            case 0xb2: { uint16_t a=WZ=IY+d;; D=mem.r8(a)&~0x40; mem.w8(a,D); } return cc_fdcb[0xb2]; // RES 6,(IY+d),D
            case 0xb3: { uint16_t a=WZ=IY+d;; E=mem.r8(a)&~0x40; mem.w8(a,E); } return cc_fdcb[0xb3]; // RES 6,(IY+d),E
            case 0xb4: { uint16_t a=WZ=IY+d;; H=mem.r8(a)&~0x40; mem.w8(a,H); } return cc_fdcb[0xb4]; // RES 6,(IY+d),H
            case 0xb5: { uint16_t a=WZ=IY+d;; L=mem.r8(a)&~0x40; mem.w8(a,L); } return cc_fdcb[0xb5]; // RES 6,(IY+d),L
            case 0xb6: { uint16_t a=WZ=IY+d;; mem.w8(a,mem.r8(a)&~0x40); } return cc_fdcb[0xb6]; // RES 6,(IY+d)
            case 0xb7: { uint16_t a=WZ=IY+d;; A=mem.r8(a)&~0x40; mem.w8(a,A); } return cc_fdcb[0xb7]; // RES 6,(IY+d),A
            case 0xb8: { uint16_t a=WZ=IY+d;; B=mem.r8(a)&~0x80; mem.w8(a,B); } return cc_fdcb[0xb8]; // RES 7,(IY+d),B
            case 0xb9: { uint16_t a=WZ=IY+d;; C=mem.r8(a)&~0x80; mem.w8(a,C); } return cc_fdcb[0xb9]; // RES 7,(IY+d),C
            case 0xba: { uint16_t a=WZ=IY+d;; D=mem.r8(a)&~0x80; mem.w8(a,D); } return cc_fdcb[0xba]; // RES 7,(IY+d),D
            case 0xbb: { uint16_t a=WZ=IY+d;; E=mem.r8(a)&~0x80; mem.w8(a,E); } return cc_fdcb[0xbb]; // RES 7,(IY+d),E
            case 0xbc: { uint16_t a=WZ=IY+d;; H=mem.r8(a)&~0x80; mem.w8(a,H); } return cc_fdcb[0xbc]; // RES 7,(IY+d),H
            case 0xbd: { uint16_t a=WZ=IY+d;; L=mem.r8(a)&~0x80; mem.w8(a,L); } return cc_fdcb[0xbd]; // RES 7,(IY+d),L
            case 0xbe: { uint16_t a=WZ=IY+d;; mem.w8(a,mem.r8(a)&~0x80); } return cc_fdcb[0xbe]; // RES 7,(IY+d)
            case 0xbf: { uint16_t a=WZ=IY+d;; A=mem.r8(a)&~0x80; mem.w8(a,A); } return cc_fdcb[0xbf]; // RES 7,(IY+d),A
            case 0xc0: { uint16_t a=WZ=IY+d;; B=mem.r8(a)|0x1; mem.w8(a,B);} return cc_fdcb[0xc0]; // SET 0,(IY+d),B
            case 0xc1: { uint16_t a=WZ=IY+d;; C=mem.r8(a)|0x1; mem.w8(a,C);} return cc_fdcb[0xc1]; // SET 0,(IY+d),C
            case 0xc2: { uint16_t a=WZ=IY+d;; D=mem.r8(a)|0x1; mem.w8(a,D);} return cc_fdcb[0xc2]; // SET 0,(IY+d),D
            case 0xc3: { uint16_t a=WZ=IY+d;; E=mem.r8(a)|0x1; mem.w8(a,E);} return cc_fdcb[0xc3]; // SET 0,(IY+d),E
            case 0xc4: { uint16_t a=WZ=IY+d;; H=mem.r8(a)|0x1; mem.w8(a,IYH);} return cc_fdcb[0xc4]; // SET 0,(IY+d),H
            case 0xc5: { uint16_t a=WZ=IY+d;; L=mem.r8(a)|0x1; mem.w8(a,IYL);} return cc_fdcb[0xc5]; // SET 0,(IY+d),L
            case 0xc6: { uint16_t a=WZ=IY+d;; mem.w8(a,mem.r8(a)|0x1);} return cc_fdcb[0xc6]; // SET 0,(IY+d)
            case 0xc7: { uint16_t a=WZ=IY+d;; A=mem.r8(a)|0x1; mem.w8(a,A);} return cc_fdcb[0xc7]; // SET 0,(IY+d),A
            case 0xc8: { uint16_t a=WZ=IY+d;; B=mem.r8(a)|0x2; mem.w8(a,B);} return cc_fdcb[0xc8]; // SET 1,(IY+d),B
            case 0xc9: { uint16_t a=WZ=IY+d;; C=mem.r8(a)|0x2; mem.w8(a,C);} return cc_fdcb[0xc9]; // SET 1,(IY+d),C
            case 0xca: { uint16_t a=WZ=IY+d;; D=mem.r8(a)|0x2; mem.w8(a,D);} return cc_fdcb[0xca]; // SET 1,(IY+d),D
            case 0xcb: { uint16_t a=WZ=IY+d;; E=mem.r8(a)|0x2; mem.w8(a,E);} return cc_fdcb[0xcb]; // SET 1,(IY+d),E
            case 0xcc: { uint16_t a=WZ=IY+d;; H=mem.r8(a)|0x2; mem.w8(a,IYH);} return cc_fdcb[0xcc]; // SET 1,(IY+d),H
            case 0xcd: { uint16_t a=WZ=IY+d;; L=mem.r8(a)|0x2; mem.w8(a,IYL);} return cc_fdcb[0xcd]; // SET 1,(IY+d),L
            case 0xce: { uint16_t a=WZ=IY+d;; mem.w8(a,mem.r8(a)|0x2);} return cc_fdcb[0xce]; // SET 1,(IY+d)
            case 0xcf: { uint16_t a=WZ=IY+d;; A=mem.r8(a)|0x2; mem.w8(a,A);} return cc_fdcb[0xcf]; // SET 1,(IY+d),A
            case 0xd0: { uint16_t a=WZ=IY+d;; B=mem.r8(a)|0x4; mem.w8(a,B);} return cc_fdcb[0xd0]; // SET 2,(IY+d),B
            case 0xd1: { uint16_t a=WZ=IY+d;; C=mem.r8(a)|0x4; mem.w8(a,C);} return cc_fdcb[0xd1]; // SET 2,(IY+d),C
            case 0xd2: { uint16_t a=WZ=IY+d;; D=mem.r8(a)|0x4; mem.w8(a,D);} return cc_fdcb[0xd2]; // SET 2,(IY+d),D
            case 0xd3: { uint16_t a=WZ=IY+d;; E=mem.r8(a)|0x4; mem.w8(a,E);} return cc_fdcb[0xd3]; // SET 2,(IY+d),E
            case 0xd4: { uint16_t a=WZ=IY+d;; H=mem.r8(a)|0x4; mem.w8(a,IYH);} return cc_fdcb[0xd4]; // SET 2,(IY+d),H
            case 0xd5: { uint16_t a=WZ=IY+d;; L=mem.r8(a)|0x4; mem.w8(a,IYL);} return cc_fdcb[0xd5]; // SET 2,(IY+d),L
            case 0xd6: { uint16_t a=WZ=IY+d;; mem.w8(a,mem.r8(a)|0x4);} return cc_fdcb[0xd6]; // SET 2,(IY+d)
            case 0xd7: { uint16_t a=WZ=IY+d;; A=mem.r8(a)|0x4; mem.w8(a,A);} return cc_fdcb[0xd7]; // SET 2,(IY+d),A
            case 0xd8: { uint16_t a=WZ=IY+d;; B=mem.r8(a)|0x8; mem.w8(a,B);} return cc_fdcb[0xd8]; // SET 3,(IY+d),B
            case 0xd9: { uint16_t a=WZ=IY+d;; C=mem.r8(a)|0x8; mem.w8(a,C);} return cc_fdcb[0xd9]; // SET 3,(IY+d),C
            case 0xda: { uint16_t a=WZ=IY+d;; D=mem.r8(a)|0x8; mem.w8(a,D);} return cc_fdcb[0xda]; // SET 3,(IY+d),D
            case 0xdb: { uint16_t a=WZ=IY+d;; E=mem.r8(a)|0x8; mem.w8(a,E);} return cc_fdcb[0xdb]; // SET 3,(IY+d),E
            case 0xdc: { uint16_t a=WZ=IY+d;; H=mem.r8(a)|0x8; mem.w8(a,IYH);} return cc_fdcb[0xdc]; // SET 3,(IY+d),H
            case 0xdd: { uint16_t a=WZ=IY+d;; L=mem.r8(a)|0x8; mem.w8(a,IYL);} return cc_fdcb[0xdd]; // SET 3,(IY+d),L
            case 0xde: { uint16_t a=WZ=IY+d;; mem.w8(a,mem.r8(a)|0x8);} return cc_fdcb[0xde]; // SET 3,(IY+d)
            case 0xdf: { uint16_t a=WZ=IY+d;; A=mem.r8(a)|0x8; mem.w8(a,A);} return cc_fdcb[0xdf]; // SET 3,(IY+d),A
            case 0xe0: { uint16_t a=WZ=IY+d;; B=mem.r8(a)|0x10; mem.w8(a,B);} return cc_fdcb[0xe0]; // SET 4,(IY+d),B
            case 0xe1: { uint16_t a=WZ=IY+d;; C=mem.r8(a)|0x10; mem.w8(a,C);} return cc_fdcb[0xe1]; // SET 4,(IY+d),C
            case 0xe2: { uint16_t a=WZ=IY+d;; D=mem.r8(a)|0x10; mem.w8(a,D);} return cc_fdcb[0xe2]; // SET 4,(IY+d),D
            case 0xe3: { uint16_t a=WZ=IY+d;; E=mem.r8(a)|0x10; mem.w8(a,E);} return cc_fdcb[0xe3]; // SET 4,(IY+d),E
            case 0xe4: { uint16_t a=WZ=IY+d;; H=mem.r8(a)|0x10; mem.w8(a,IYH);} return cc_fdcb[0xe4]; // SET 4,(IY+d),H
            case 0xe5: { uint16_t a=WZ=IY+d;; L=mem.r8(a)|0x10; mem.w8(a,IYL);} return cc_fdcb[0xe5]; // SET 4,(IY+d),L
            case 0xe6: { uint16_t a=WZ=IY+d;; mem.w8(a,mem.r8(a)|0x10);} return cc_fdcb[0xe6]; // SET 4,(IY+d)
            case 0xe7: { uint16_t a=WZ=IY+d;; A=mem.r8(a)|0x10; mem.w8(a,A);} return cc_fdcb[0xe7]; // SET 4,(IY+d),A
            case 0xe8: { uint16_t a=WZ=IY+d;; B=mem.r8(a)|0x20; mem.w8(a,B);} return cc_fdcb[0xe8]; // SET 5,(IY+d),B
            case 0xe9: { uint16_t a=WZ=IY+d;; C=mem.r8(a)|0x20; mem.w8(a,C);} return cc_fdcb[0xe9]; // SET 5,(IY+d),C
            case 0xea: { uint16_t a=WZ=IY+d;; D=mem.r8(a)|0x20; mem.w8(a,D);} return cc_fdcb[0xea]; // SET 5,(IY+d),D
            case 0xeb: { uint16_t a=WZ=IY+d;; E=mem.r8(a)|0x20; mem.w8(a,E);} return cc_fdcb[0xeb]; // SET 5,(IY+d),E
            case 0xec: { uint16_t a=WZ=IY+d;; H=mem.r8(a)|0x20; mem.w8(a,IYH);} return cc_fdcb[0xec]; // SET 5,(IY+d),H
            case 0xed: { uint16_t a=WZ=IY+d;; L=mem.r8(a)|0x20; mem.w8(a,IYL);} return cc_fdcb[0xed]; // SET 5,(IY+d),L
            case 0xee: { uint16_t a=WZ=IY+d;; mem.w8(a,mem.r8(a)|0x20);} return cc_fdcb[0xee]; // SET 5,(IY+d)
            case 0xef: { uint16_t a=WZ=IY+d;; A=mem.r8(a)|0x20; mem.w8(a,A);} return cc_fdcb[0xef]; // SET 5,(IY+d),A
            case 0xf0: { uint16_t a=WZ=IY+d;; B=mem.r8(a)|0x40; mem.w8(a,B);} return cc_fdcb[0xf0]; // SET 6,(IY+d),B
            case 0xf1: { uint16_t a=WZ=IY+d;; C=mem.r8(a)|0x40; mem.w8(a,C);} return cc_fdcb[0xf1]; // SET 6,(IY+d),C
            case 0xf2: { uint16_t a=WZ=IY+d;; D=mem.r8(a)|0x40; mem.w8(a,D);} return cc_fdcb[0xf2]; // SET 6,(IY+d),D
            case 0xf3: { uint16_t a=WZ=IY+d;; E=mem.r8(a)|0x40; mem.w8(a,E);} return cc_fdcb[0xf3]; // SET 6,(IY+d),E
            case 0xf4: { uint16_t a=WZ=IY+d;; H=mem.r8(a)|0x40; mem.w8(a,IYH);} return cc_fdcb[0xf4]; // SET 6,(IY+d),H
            case 0xf5: { uint16_t a=WZ=IY+d;; L=mem.r8(a)|0x40; mem.w8(a,IYL);} return cc_fdcb[0xf5]; // SET 6,(IY+d),L
            case 0xf6: { uint16_t a=WZ=IY+d;; mem.w8(a,mem.r8(a)|0x40);} return cc_fdcb[0xf6]; // SET 6,(IY+d)
            case 0xf7: { uint16_t a=WZ=IY+d;; A=mem.r8(a)|0x40; mem.w8(a,A);} return cc_fdcb[0xf7]; // SET 6,(IY+d),A
            case 0xf8: { uint16_t a=WZ=IY+d;; B=mem.r8(a)|0x80; mem.w8(a,B);} return cc_fdcb[0xf8]; // SET 7,(IY+d),B
            case 0xf9: { uint16_t a=WZ=IY+d;; C=mem.r8(a)|0x80; mem.w8(a,C);} return cc_fdcb[0xf9]; // SET 7,(IY+d),C
            case 0xfa: { uint16_t a=WZ=IY+d;; D=mem.r8(a)|0x80; mem.w8(a,D);} return cc_fdcb[0xfa]; // SET 7,(IY+d),D
            case 0xfb: { uint16_t a=WZ=IY+d;; E=mem.r8(a)|0x80; mem.w8(a,E);} return cc_fdcb[0xfb]; // SET 7,(IY+d),E
            case 0xfc: { uint16_t a=WZ=IY+d;; H=mem.r8(a)|0x80; mem.w8(a,IYH);} return cc_fdcb[0xfc]; // SET 7,(IY+d),H
            case 0xfd: { uint16_t a=WZ=IY+d;; L=mem.r8(a)|0x80; mem.w8(a,IYL);} return cc_fdcb[0xfd]; // SET 7,(IY+d),L
            case 0xfe: { uint16_t a=WZ=IY+d;; mem.w8(a,mem.r8(a)|0x80);} return cc_fdcb[0xfe]; // SET 7,(IY+d)
            case 0xff: { uint16_t a=WZ=IY+d;; A=mem.r8(a)|0x80; mem.w8(a,A);} return cc_fdcb[0xff]; // SET 7,(IY+d),A
            default: return invalid_opcode(4);
          }
          break;
          }
        case 0xcc: WZ=mem.r16(PC); PC+=2; if ((F&ZF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return cc_ex[0xcc]+cc_fd[0xcc]; } else { return cc_fd[0xcc]; } // CALL Z,nn
        case 0xcd: SP-=2; mem.w16(SP,PC+2); WZ=PC=mem.r16(PC); return cc_fd[0xcd]; // CALL nn
        case 0xce: adc8(mem.r8(PC++)); return cc_fd[0xce]; // ADC n
        case 0xcf: rst(0x8); return cc_fd[0xcf]; // RST 0x8
        case 0xd0: if (!(F&CF)) { WZ=PC=mem.r16(SP); SP+=2; return cc_ex[0xd0]+cc_fd[0xd0]; } else return cc_fd[0xd0]; // RET NC
        case 0xd1: DE=mem.r16(SP); SP+=2; return cc_fd[0xd1]; // POP DE
        case 0xd2: WZ=mem.r16(PC); if (!(F&CF)) { PC=WZ; } else { PC+=2; }; return cc_fd[0xd2]; // JP NC,nn
        case 0xd3: out(bus, (A<<8)|mem.r8(PC++),A); return cc_fd[0xd3]; // OUT (n),A
        case 0xd4: WZ=mem.r16(PC); PC+=2; if (!(F&CF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return cc_ex[0xd4]+cc_fd[0xd4]; } else { return cc_fd[0xd4]; } // CALL NC,nn
        case 0xd5: SP-=2; mem.w16(SP,DE); return cc_fd[0xd5]; // PUSH DE
        case 0xd6: sub8(mem.r8(PC++)); return cc_fd[0xd6]; // SUB n
        case 0xd7: rst(0x10); return cc_fd[0xd7]; // RST 0x10
        case 0xd8: if ((F&CF)) { WZ=PC=mem.r16(SP); SP+=2; return cc_ex[0xd8]+cc_fd[0xd8]; } else return cc_fd[0xd8]; // RET C
        case 0xd9: swap16(BC,BC_); swap16(DE,DE_); swap16(HL,HL_); swap16(WZ,WZ_); return cc_fd[0xd9]; // EXX
        case 0xda: WZ=mem.r16(PC); if ((F&CF)) { PC=WZ; } else { PC+=2; }; return cc_fd[0xda]; // JP C,nn
        case 0xdb: A=in(bus, (A<<8)|mem.r8(PC++)); return cc_fd[0xdb]; // IN A,(n)
        case 0xdc: WZ=mem.r16(PC); PC+=2; if ((F&CF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return cc_ex[0xdc]+cc_fd[0xdc]; } else { return cc_fd[0xdc]; } // CALL C,nn
        case 0xde: sbc8(mem.r8(PC++)); return cc_fd[0xde]; // SBC n
        case 0xdf: rst(0x18); return cc_fd[0xdf]; // RST 0x18
        case 0xe0: if (!(F&PF)) { WZ=PC=mem.r16(SP); SP+=2; return cc_ex[0xe0]+cc_fd[0xe0]; } else return cc_fd[0xe0]; // RET PO
        case 0xe1: IY=mem.r16(SP); SP+=2; return cc_fd[0xe1]; // POP IY
        case 0xe2: WZ=mem.r16(PC); if (!(F&PF)) { PC=WZ; } else { PC+=2; }; return cc_fd[0xe2]; // JP PO,nn
        case 0xe3: {uint16_t swp=mem.r16(SP); mem.w16(SP,IY); IY=WZ=swp;} return cc_fd[0xe3]; // EX (SP),IY
        case 0xe4: WZ=mem.r16(PC); PC+=2; if (!(F&PF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return cc_ex[0xe4]+cc_fd[0xe4]; } else { return cc_fd[0xe4]; } // CALL PO,nn
        case 0xe5: SP-=2; mem.w16(SP,IY); return cc_fd[0xe5]; // PUSH IY
        case 0xe6: and8(mem.r8(PC++)); return cc_fd[0xe6]; // AND n
        case 0xe7: rst(0x20); return cc_fd[0xe7]; // RST 0x20
        case 0xe8: if ((F&PF)) { WZ=PC=mem.r16(SP); SP+=2; return cc_ex[0xe8]+cc_fd[0xe8]; } else return cc_fd[0xe8]; // RET PE
        case 0xe9: PC=IY; return cc_fd[0xe9]; // JP IY
        case 0xea: WZ=mem.r16(PC); if ((F&PF)) { PC=WZ; } else { PC+=2; }; return cc_fd[0xea]; // JP PE,nn
        case 0xeb: swap16(DE,HL); return cc_fd[0xeb]; // EX DE,HL
        case 0xec: WZ=mem.r16(PC); PC+=2; if ((F&PF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return cc_ex[0xec]+cc_fd[0xec]; } else { return cc_fd[0xec]; } // CALL PE,nn
        case 0xee: xor8(mem.r8(PC++)); return cc_fd[0xee]; // XOR n
        case 0xef: rst(0x28); return cc_fd[0xef]; // RST 0x28
        case 0xf0: if (!(F&SF)) { WZ=PC=mem.r16(SP); SP+=2; return cc_ex[0xf0]+cc_fd[0xf0]; } else return cc_fd[0xf0]; // RET P
        case 0xf1: AF=mem.r16(SP); SP+=2; return cc_fd[0xf1]; // POP AF
        case 0xf2: WZ=mem.r16(PC); if (!(F&SF)) { PC=WZ; } else { PC+=2; }; return cc_fd[0xf2]; // JP P,nn
        case 0xf3: di(); return cc_fd[0xf3]; // DI
        case 0xf4: WZ=mem.r16(PC); PC+=2; if (!(F&SF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return cc_ex[0xf4]+cc_fd[0xf4]; } else { return cc_fd[0xf4]; } // CALL P,nn
        case 0xf5: SP-=2; mem.w16(SP,AF); return cc_fd[0xf5]; // PUSH AF
        case 0xf6: or8(mem.r8(PC++)); return cc_fd[0xf6]; // OR n
        case 0xf7: rst(0x30); return cc_fd[0xf7]; // RST 0x30
        case 0xf8: if ((F&SF)) { WZ=PC=mem.r16(SP); SP+=2; return cc_ex[0xf8]+cc_fd[0xf8]; } else return cc_fd[0xf8]; // RET M
        case 0xf9: SP=IY; return cc_fd[0xf9]; // LD SP,IY
        case 0xfa: WZ=mem.r16(PC); if ((F&SF)) { PC=WZ; } else { PC+=2; }; return cc_fd[0xfa]; // JP M,nn
        case 0xfb: ei(); return cc_fd[0xfb]; // EI
        case 0xfc: WZ=mem.r16(PC); PC+=2; if ((F&SF)) { SP-=2; mem.w16(SP,PC); PC=WZ; return cc_ex[0xfc]+cc_fd[0xfc]; } else { return cc_fd[0xfc]; } // CALL M,nn
        case 0xfe: cp8(mem.r8(PC++)); return cc_fd[0xfe]; // CP n
        case 0xff: rst(0x38); return cc_fd[0xff]; // RST 0x38
        default: return invalid_opcode(2);
      }
      break;
    case 0xfe: cp8(mem.r8(PC++)); return cc_op[0xfe]; // CP n
    case 0xff: rst(0x38); return cc_op[0xff]; // RST 0x38
    default: return invalid_opcode(1);
  }
}
} // namespace YAKC
