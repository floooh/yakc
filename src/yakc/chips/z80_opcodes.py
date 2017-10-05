#-------------------------------------------------------------------------------
#   z80_opcodes.py
#   Generate huge switch/case Z80 instruction decoder.
#   See: 
#       http://www.z80.info/decoding.htm
#       http://www.righto.com/2014/10/how-z80s-registers-are-implemented-down.html
#       http://www.z80.info/zip/z80-documented.pdf
#       https://www.omnimaga.org/asm-language/bit-n-(hl)-flags/5/?wap2
#-------------------------------------------------------------------------------

# fips code generator version stamp
Version = 6 

# tab-width for generated code
TabWidth = 2

# the target file handle
Out = None

# 8-bit register table, the 'HL' entry is for instructions that use
# (HL), (IX+d) and (IY+d), and will be patched to 'IX' or 'IY' for
# the DD/FD prefix instructions
r = [ 'B', 'C', 'D', 'E', 'H', 'L', 'HL', 'A' ]

# the same, but the 'HL' item is never patched to IX/IY, this is
# for indexed instructions that load into H or L (e.g. LD H,(IX+d))
r2 = [ 'B', 'C', 'D', 'E', 'H', 'L', 'HL', 'A' ]

# 16-bit register table, with SP
rp = [ 'BC', 'DE', 'HL', 'SP' ]

# 16-bit register table, with AF (only used for PUSH/POP)
rp2 = [ 'BC', 'DE', 'HL', 'AF' ]

# condition-code table (for conditional jumps etc)
cond = [
#   NZ         Z         NC         C         PO         PE        P          M
    '!(F&ZF)', '(F&ZF)', '!(F&CF)', '(F&CF)', '!(F&PF)', '(F&PF)', '!(F&SF)', '(F&SF)'
]

# the same as 'human readable' flags for comments
cond_cmt = [ 'NZ', 'Z', 'NC', 'C', 'PO', 'PE', 'P', 'M' ]

# 8-bit ALU instruction table (C++ method names)
alu = [ 'add8', 'adc8', 'sub8', 'sbc8', 'and8', 'xor8', 'or8', 'cp8' ]

# the same 'human readable' for comments
alu_cmt = [ 'ADD', 'ADC', 'SUB', 'SBC', 'AND', 'XOR', 'OR', 'CP' ]

# rot and shift instruction table (C++ method names)
rot = [ 'rlc8', 'rrc8', 'rl8', 'rr8', 'sla8', 'sra8', 'sll8', 'srl8' ]

# the same 'human readbla for comments
rot_cmt = [ 'RLC', 'RRC', 'RL', 'RR', 'SLA', 'SRA', 'SLL', 'SRL' ]

import sys
import genutil  # fips code generator helpers

# an 'opcode' wraps the instruction byte, human-readable asm mnemonics,
# and the source code which implements the instruction
class opcode :
    def __init__(self, op) :
        self.byte = op
        self.cmt = None
        self.src = None

#-------------------------------------------------------------------------------
# return comment string for (HL), (IX+d), (IY+d)
#
def iHLcmt(ext) :
    if (ext) :
        return '({}+d)'.format(r[6])
    else :
        return '({})'.format(r[6])

#-------------------------------------------------------------------------------
# Return code to setup an address variable 'a' with the address of HL
# or (IX+d), (IY+d). For the index instructions also update WZ with
# IX+d or IY+d
#
def iHLsrc(ext) :
    if (ext) :
        # IX+d or IY+d
        return 'uint16_t a=WZ={}+mem.rs8(PC++)'.format(r[6])
    else :
        # HL
        return 'uint16_t a={}'.format(r[6])

#-------------------------------------------------------------------------------
# Return code to setup an variable 'a' with the address of HL or (IX+d), (IY+d).
# For the index instructions, also update WZ with IX+d or IY+d
#
def iHLdsrc(ext) :
    if (ext) :
        # IX+d or IY+d
        return 'uint16_t a=WZ={}+d;'.format(r[6])
    else :
        # HL
        return 'uint16_t a={}'.format(r[6])

#-------------------------------------------------------------------------------
# Return a string "return cc[op]" where cc is a cycle count table, 
# and op is an opcode number.
#
def cyc(cc, op):
    return 'return {}[{}]'.format(cc, hex(op))

#-------------------------------------------------------------------------------
# Same as cyc, but add a value from the cc_ex table, this is used
# if a conditional branch is taken
def cyc_ex(cc, op):
    return 'return cc_ex[{}]+{}[{}]'.format(hex(op), cc, hex(op))

#-------------------------------------------------------------------------------
# Encode a main instruction, or an DD or FD prefix instruction.
# Takes an opcode byte and returns an opcode object, for invalid instructions
# the opcode object will be in its default state (opcode.src==None).
# cc is the name of the cycle-count table.
#
def enc_op(op, ext, cc) :

    o = opcode(op)

    # split opcode byte into bit groups, these identify groups
    # or subgroups of instructions, or serve as register indices 
    #
    #   |xx|yyy|zzz|
    #   |xx|pp|q|zzz|
    x = op>>6
    y = (op>>3)&7
    z = op&7
    p = y>>1
    q = y&1

    #---- block 1: 8-bit loads, and HALT
    if x == 1:
        if y == 6:
            if z == 6:
                # special case: LD (HL),(HL) is HALT
                o.cmt = 'HALT'
                o.src = 'halt(); {};'.format(cyc(cc,op))
            else:
                # LD (HL),r; LD (IX+d),r; LD (IX+d),r
                o.cmt = 'LD {},{}'.format(iHLcmt(ext), r2[z])
                o.src = '{{ {}; mem.w8(a,{}); }} {};'.format(iHLsrc(ext), r2[z], cyc(cc,op))
        elif z == 6:
            # LD r,(HL); LD r,(IX+d); LD r,(IY+d)
            o.cmt = 'LD {},{}'.format(r2[y], iHLcmt(ext))
            o.src = '{{ {}; {}=mem.r8(a); }} {};'.format(iHLsrc(ext), r2[y], cyc(cc,op))
        else:
            # LD r,s
            o.cmt = 'LD {},{}'.format(r[y], r[z])
            o.src = '{}={}; {};'.format(r[y], r[z], cyc(cc,op))

    #---- block 2: 8-bit ALU instructions (ADD, ADC, SUB, SBC, AND, XOR, OR, CP)
    elif x == 2:
        if z == 6:
            # ALU (HL); ALU (IX+d); ALU (IY+d)
            o.cmt = '{} {}'.format(alu_cmt[y], iHLcmt(ext))
            o.src = '{{ {}; {}(mem.r8(a)); }} {};'.format(iHLsrc(ext), alu[y], cyc(cc,op))
        else:
            # ALU r
            o.cmt = '{} {}'.format(alu_cmt[y], r[z])
            o.src = '{}({}); {};'.format(alu[y], r[z], cyc(cc,op))

    #---- block 0: misc ops
    elif x == 0:
        if z == 0:
            if y == 0:
                # NOP
                o.cmt = 'NOP'
                o.src = '{};'.format(cyc(cc,op))
            elif y == 1:
                # EX AF,AF'
                o.cmt = "EX AF,AF'"
                o.src = 'swap16(AF,AF_); {};'.format(cyc(cc,op))
            elif y == 2:
                # DJNZ d
                o.cmt = 'DJNZ'
                o.src = 'if (--B>0) {{ WZ=PC=PC+mem.rs8(PC)+1; {}; }} else {{ PC++; {}; }}'.format(cyc_ex(cc,op), cyc(cc,op))
            elif  y == 3:
                # JR d
                o.cmt = 'JR d'
                o.src = 'WZ=PC=PC+mem.rs8(PC)+1; {};'.format(cyc(cc,op))
            else:
                # JR cc,d
                o.cmt = 'JR {},d'.format(cond_cmt[y-4])
                o.src = 'if ({}) {{ WZ=PC=PC+mem.rs8(PC)+1; {}; }} else {{ PC++; {}; }}'.format(cond[y-4], cyc_ex(cc,op), cyc(cc,op))
        elif z == 1:
            if q == 0:
                # 16-bit immediate loads
                o.cmt = 'LD {},nn'.format(rp[p])
                o.src = '{}=mem.r16(PC); PC+=2; {};'.format(rp[p], cyc(cc,op))
            else :
                # ADD HL,rr; ADD IX,rr; ADD IY,rr
                o.cmt = 'ADD {},{}'.format(rp[2],rp[p])
                o.src = '{}=add16({},{}); {};'.format(rp[2],rp[2],rp[p], cyc(cc,op)) 
        elif z == 2:
            # indirect loads
            op_tbl = [
                [ 'LD (BC),A', 'mem.w8(BC,A); Z=C+1; W=A; {};'.format(cyc(cc,op)) ],
                [ 'LD A,(BC)', 'A=mem.r8(BC); WZ=BC+1; {};'.format(cyc(cc,op)) ],
                [ 'LD (DE),A', 'mem.w8(DE,A); Z=E+1; W=A; {};'.format(cyc(cc,op)) ],
                [ 'LD A,(DE)', 'A=mem.r8(DE); WZ=DE+1; {};'.format(cyc(cc,op)) ],
                [ 'LD (nn),{}'.format(rp[2]), 'WZ=mem.r16(PC); mem.w16(WZ++,{}); PC+=2; {};'.format(rp[2],cyc(cc,op)) ],
                [ 'LD {},(nn)'.format(rp[2]), 'WZ=mem.r16(PC); {}=mem.r16(WZ++); PC+=2; {};'.format(rp[2],cyc(cc,op)) ],
                [ 'LD (nn),A', 'WZ=mem.r16(PC); mem.w8(WZ++,A); W=A; PC+=2; {};'.format(cyc(cc,op)) ],
                [ 'LD A,(nn)', 'WZ=mem.r16(PC); A=mem.r8(WZ++); PC+=2; {};'.format(cyc(cc,op)) ]
            ]
            o.cmt = op_tbl[y][0]
            o.src = op_tbl[y][1]
        elif z == 3:
            # 16-bit INC/DEC 
            if q == 0:
                o.cmt = 'INC {}'.format(rp[p])
                o.src = '{}++; {};'.format(rp[p], cyc(cc,op))
            else:
                o.cmt = 'DEC {}'.format(rp[p])
                o.src = '{}--; {};'.format(rp[p], cyc(cc,op))
        elif z == 4 or z == 5:
            cmt = 'INC' if z == 4 else 'DEC'
            fn = 'inc8' if z == 4 else 'dec8'
            if y == 6:
                # INC/DEC (HL)/(IX+d)/(IY+d)
                o.cmt = '{} {}'.format(cmt, iHLcmt(ext))
                o.src = '{{ {}; mem.w8(a,{}(mem.r8(a))); }} {};'.format(iHLsrc(ext), fn, cyc(cc,op))
            else:
                # INC/DEC r
                o.cmt = '{} {}'.format(cmt, r[y])
                o.src = '{}={}({}); {};'.format(r[y], fn, r[y], cyc(cc,op))
        elif z == 6:
            if y == 6:
                # LD (HL),n; LD (IX+d),n; LD (IY+d),n
                o.cmt = 'LD {},n'.format(iHLcmt(ext))
                o.src = '{{ {}; mem.w8(a,mem.r8(PC++)); }} {};'.format(iHLsrc(ext), cyc(cc,op))
            else:
                # LD r,n
                o.cmt = 'LD {},n'.format(r[y])
                o.src = '{}=mem.r8(PC++); {};'.format(r[y], cyc(cc,op))
        elif z == 7:
            # misc ops on A and F
            op_tbl = [
                [ 'RLCA', 'rlca8();'],
                [ 'RRCA', 'rrca8();'],
                [ 'RLA',  'rla8();'],
                [ 'RRA',  'rra8();'],
                [ 'DAA',  'daa();'],
                [ 'CPL',  'A^=0xFF; F=(F&(SF|ZF|PF|CF))|HF|NF|(A&(YF|XF));'],
                [ 'SCF',  'F=(F&(SF|ZF|YF|XF|PF))|CF|(A&(YF|XF));'],
                [ 'CCF',  'F=((F&(SF|ZF|YF|XF|PF|CF))|((F&CF)<<4)|(A&(YF|XF)))^CF;']
            ]
            o.cmt = op_tbl[y][0]
            o.src = op_tbl[y][1] + ' {};'.format(cyc(cc,op))

    #--- block 3: misc and extended ops
    elif x == 3:
        if z == 0:
            # RET cc
            o.cmt = 'RET {}'.format(cond_cmt[y])
            o.src = 'if ({}) {{ WZ=PC=mem.r16(SP); SP+=2; {}; }} else {};'.format(cond[y], cyc_ex(cc,op), cyc(cc,op))
        elif z == 1:
            if q == 0:
                # POP BC,DE,HL,IX,IY,AF
                o.cmt = 'POP {}'.format(rp2[p])
                o.src = '{}=mem.r16(SP); SP+=2; {};'.format(rp2[p], cyc(cc,op))
            else:
                # misc ops
                op_tbl = [
                    [ 'RET', 'WZ=PC=mem.r16(SP); SP+=2; {};'.format(cyc(cc,op)) ],
                    [ 'EXX', 'swap16(BC,BC_); swap16(DE,DE_); swap16(HL,HL_); swap16(WZ,WZ_); {};'.format(cyc(cc,op)) ],
                    [ 'JP {}'.format(rp[2]), 'PC={}; {};'.format(rp[2], cyc(cc,op)) ],
                    [ 'LD SP,{}'.format(rp[2]), 'SP={}; {};'.format(rp[2], cyc(cc,op)) ]
                ]
                o.cmt = op_tbl[p][0]
                o.src = op_tbl[p][1]
        elif z == 2:
            # JP cc,nn
            o.cmt = 'JP {},nn'.format(cond_cmt[y])
            o.src = 'WZ=mem.r16(PC); if ({}) {{ PC=WZ; }} else {{ PC+=2; }}; {};'.format(cond[y], cyc(cc,op))
        elif z == 3:
            # misc ops
            op_tbl = [
                [ 'JP nn', 'WZ=PC=mem.r16(PC); {};'.format(cyc(cc,op)) ],
                [ None, None ], # CB prefix instructions
                [ 'OUT (n),A', 'out(bus, (A<<8)|mem.r8(PC++),A); {};'.format(cyc(cc,op)) ],
                [ 'IN A,(n)', 'A=in(bus, (A<<8)|mem.r8(PC++)); {};'.format(cyc(cc,op)) ],
                [ 
                    'EX (SP),{}'.format(rp[2]), 
                    '{{uint16_t swp=mem.r16(SP); mem.w16(SP,{}); {}=WZ=swp;}} {};'.format(rp[2], rp[2], cyc(cc,op))
                ],
                [ 'EX DE,HL', 'swap16(DE,HL); {};'.format(cyc(cc,op)) ],
                [ 'DI', 'di(); {};'.format(cyc(cc,op)) ],
                [ 'EI', 'ei(); {};'.format(cyc(cc,op)) ]
            ]
            o.cmt = op_tbl[y][0]
            o.src = op_tbl[y][1]
        elif z == 4:
            # CALL cc,nn
            o.cmt = 'CALL {},nn'.format(cond_cmt[y])
            o.src = 'WZ=mem.r16(PC); PC+=2; if ({}) {{ SP-=2; mem.w16(SP,PC); PC=WZ; {}; }} else {{ {}; }}'.format(cond[y], cyc_ex(cc,op), cyc(cc,op))
        elif z == 5:
            if q == 0:
                # PUSH BC,DE,HL,IX,IY,AF
                o.cmt = 'PUSH {}'.format(rp2[p])
                o.src = 'SP-=2; mem.w16(SP,{}); {};'.format(rp2[p], cyc(cc,op))
            else:
                op_tbl = [
                    [ 'CALL nn', 'SP-=2; mem.w16(SP,PC+2); WZ=PC=mem.r16(PC); {};'.format(cyc(cc,op)) ],
                    [ None, None ], # DD prefix instructions
                    [ None, None ], # ED prefix instructions
                    [ None, None ], # FD prefix instructions
                ]
                o.cmt = op_tbl[p][0]
                o.src = op_tbl[p][1]
        elif z == 6:
            # ALU n
            o.cmt = '{} n'.format(alu_cmt[y])
            o.src = '{}(mem.r8(PC++)); {};'.format(alu[y], cyc(cc,op))
        elif z == 7:
            # RST
            o.cmt = 'RST {}'.format(hex(y*8))
            o.src = 'rst({}); {};'.format(hex(y*8), cyc(cc,op))

    return o

#-------------------------------------------------------------------------------
#   ED prefix instructions
#
def enc_ed_op(op) :
    o = opcode(op)
    cc = 'cc_ed'

    x = op>>6
    y = (op>>3)&7
    z = op&7
    p = y>>1
    q = y&1

    if x == 2:
        # block instructions (LDIR etc)
        if y >= 4 and z < 4 :
            op_tbl = [
                [ 
                    [ 'LDI',    'ldi(); {};'.format(cyc(cc,op)) ],
                    [ 'LDD',    'ldd(); {};'.format(cyc(cc,op)) ],
                    [ 'LDIR',   'return ldir();' ],
                    [ 'LDDR',   'return lddr();' ]
                ],
                [
                    [ 'CPI',    'cpi(); {};'.format(cyc(cc,op)) ],
                    [ 'CPD',    'cpd(); {};'.format(cyc(cc,op)) ],
                    [ 'CPIR',   'return cpir();' ],
                    [ 'CPDR',   'return cpdr();' ]
                ],
                [
                    [ 'INI',    'ini(bus); {};'.format(cyc(cc,op)) ],
                    [ 'IND',    'ind(bus); {};'.format(cyc(cc,op)) ],
                    [ 'INIR',   'return inir(bus);' ],
                    [ 'INDR',   'return indr(bus);' ]
                ],
                [
                    [ 'OUTI',   'outi(bus); {};'.format(cyc(cc,op)) ],
                    [ 'OUTD',   'outd(bus); {};'.format(cyc(cc,op)) ],
                    [ 'OTID',   'return otir(bus);' ],
                    [ 'OTDR',   'return otdr(bus);' ]
                ]
            ]
            o.cmt = op_tbl[z][y-4][0]
            o.src = op_tbl[z][y-4][1]

    elif x == 1:
        # misc ops
        if z == 0:
            # IN r,(C)
            if y == 6:
                # undocumented special case 'IN F,(C)', only alter flags, don't store result
                o.cmt = 'IN (C)';
                o.src = 'F=szp[in(bus, BC)]|(F&CF); {};'.format(cyc(cc,op))
            else:
                o.cmt = 'IN {},(C)'.format(r[y])
                o.src = '{}=in(bus, BC); F=szp[{}]|(F&CF); {};'.format(r[y],r[y], cyc(cc,op))
        elif z == 1:
            # OUT (C),r
            if y == 6:
                # undocumented special case 'OUT (C),F', always output 0
                o.cmd = 'OUT (C)';
                o.src = 'out(bus, BC,0); {};'.format(cyc(cc,op));
            else:
                o.cmt = 'OUT (C),{}'.format(r[y])
                o.src = 'out(bus, BC,{}); {};'.format(r[y], cyc(cc,op))
        elif z == 2:
            # SBC/ADC HL,rr
            cmt = 'SBC' if q == 0 else 'ADC'
            src = 'sbc16' if q == 0 else 'adc16'
            o.cmt = '{} HL,{}'.format(cmt, rp[p])
            o.src = 'HL={}(HL,{}); {};'.format(src, rp[p], cyc(cc,op))
        elif z == 3:
            # 16-bit immediate address load/store
            if q == 0:
                o.cmt = 'LD (nn),{}'.format(rp[p])
                o.src = 'WZ=mem.r16(PC); mem.w16(WZ++,{}); PC+=2; {};'.format(rp[p], cyc(cc,op))
            else:
                o.cmt = 'LD {},(nn)'.format(rp[p])
                o.src = 'WZ=mem.r16(PC); {}=mem.r16(WZ++); PC+=2; {};'.format(rp[p], cyc(cc,op))
        elif z == 4:
            # NEG
            o.cmt = 'NEG'
            o.src = 'neg8(); {};'.format(cyc(cc,op))
        elif z == 5:
            # RETN, RETI (only RETI implemented!)
            if y == 1:
                o.cmt = 'RETI'
                o.src = 'reti(); {};'.format(cyc(cc,op))
        elif z == 6:
            # IM m
            im_mode = [ 0, 0, 1, 2, 0, 0, 1, 2 ]
            o.cmt = 'IM {}'.format(im_mode[y])
            o.src = 'IM={}; {};'.format(im_mode[y], cyc(cc,op))
        elif z == 7:
            # misc ops on I,R and A
            op_tbl = [
                [ 'LD I,A', 'I=A; {};'.format(cyc(cc,op)) ],
                [ 'LD R,A', 'R=A; {};'.format(cyc(cc,op)) ],
                [ 'LD A,I', 'A=I; F=sziff2(I,IFF2)|(F&CF); {};'.format(cyc(cc,op)) ],
                [ 'LD A,R', 'A=R; F=sziff2(R,IFF2)|(F&CF); {};'.format(cyc(cc,op)) ],
                [ 'RRD',    'rrd(); {};'.format(cyc(cc,op)) ],
                [ 'RLD',    'rld(); {};'.format(cyc(cc,op)) ],
                [ 'NOP (ED)', '{};'.format(cyc(cc,op)) ],
                [ 'NOP (ED)', '{};'.format(cyc(cc,op)) ],
            ]
            o.cmt = op_tbl[y][0]
            o.src = op_tbl[y][1]
    return o

#-------------------------------------------------------------------------------
#   CB prefix instructions
#
def enc_cb_op(op, ext, cc) :
    o = opcode(op)

    x = op>>6
    y = (op>>3)&7
    z = op&7

    if x == 0:
        # rotates and shifts
        if z == 6:
            # ROT (HL); ROT (IX+d); ROT (IY+d)
            o.cmt = '{} {}'.format(rot_cmt[y],iHLcmt(ext))
            o.src = '{{ {}; mem.w8(a,{}(mem.r8(a))); }} {};'.format(iHLdsrc(ext), rot[y], cyc(cc,op))
        elif ext:
            # undocumented: ROT (IX+d),(IY+d),r (also stores result in a register)
            o.cmt = '{} {},{}'.format(rot_cmt[y],iHLcmt(ext),r2[z])
            o.src = '{{ {}; {}={}(mem.r8(a)); mem.w8(a,{}); }} {};'.format(iHLdsrc(ext), r2[z], rot[y], r2[z], cyc(cc,op))
        else:
            # ROT r
            o.cmt = '{} {}'.format(rot_cmt[y],r2[z])
            o.src = '{}={}({}); {};'.format(r2[z], rot[y], r2[z], cyc(cc,op))
    elif x == 1:
        # BIT n
        if z == 6 or ext:
            # BIT n,(HL); BIT n,(IX+d); BIT n,(IY+d)
            o.cmt = 'BIT {},{}'.format(y,iHLcmt(ext))
            o.src = '{{ {}; ibit(mem.r8(a),{}); }} {};'.format(iHLdsrc(ext), hex(1<<y), cyc(cc,op))
        else:
            # BIT n,r
            o.cmt = 'BIT {},{}'.format(y,r2[z])
            o.src = 'bit({},{}); {};'.format(r2[z], hex(1<<y), cyc(cc,op))
    elif x == 2:
        # RES n
        if z == 6:
            # RES n,(HL); RES n,(IX+d); RES n,(IY+d)
            o.cmt = 'RES {},{}'.format(y,iHLcmt(ext))
            o.src = '{{ {}; mem.w8(a,mem.r8(a)&~{}); }} {};'.format(iHLdsrc(ext), hex(1<<y), cyc(cc,op))
        elif ext:
            # undocumented: RES n,(IX+d),r; RES n,(IY+d),r
            o.cmt = 'RES {},{},{}'.format(y,iHLcmt(ext),r2[z])
            o.src = '{{ {}; {}=mem.r8(a)&~{}; mem.w8(a,{}); }} {};'.format(iHLdsrc(ext), r2[z], hex(1<<y), r2[z], cyc(cc,op))
        else:
            # RES n,r
            o.cmt = 'RES {},{}'.format(y,r2[z])
            o.src = '{}&=~{}; {};'.format(r2[z], hex(1<<y), cyc(cc,op))
    elif x == 3:
        # SET n
        if z == 6:
            # SET n,(HL); RES n,(IX+d); RES n,(IY+d)
            o.cmt = 'SET {},{}'.format(y,iHLcmt(ext))
            o.src = '{{ {}; mem.w8(a,mem.r8(a)|{});}} {};'.format(iHLdsrc(ext), hex(1<<y), cyc(cc,op))
        elif ext:
            # undocumented: SET n,(IX+d),r; SET n,(IY+d),r
            o.cmt = 'SET {},{},{}'.format(y,iHLcmt(ext),r2[z])
            o.src = '{{ {}; {}=mem.r8(a)|{}; mem.w8(a,{});}} {};'.format(iHLdsrc(ext), r2[z], hex(1<<y), r[z], cyc(cc,op))
        else:
            # SET n,r
            o.cmt = 'SET {},{}'.format(y,r2[z])
            o.src = '{}|={}; {};'.format(r2[z], hex(1<<y), cyc(cc,op))

    return o

#-------------------------------------------------------------------------------
# patch register lookup tables for IX or IY instructions
#
def patch_reg_tables(reg) :
    r[4] = reg+'H'; r[5] = reg+'L'; r[6] = reg;
    rp[2] = rp2[2] = reg

#-------------------------------------------------------------------------------
# 'unpatch' the register lookup tables
#
def unpatch_reg_tables() :
    r[4] = 'H'; r[5] = 'L'; r[6] = 'HL'
    rp[2] = rp2[2] = 'HL'

#-------------------------------------------------------------------------------
# return a tab-string for given indent level
#
def tab(indent) :
    return ' '*TabWidth*indent

#-------------------------------------------------------------------------------
# output a src line
def l(s) :
    Out.write(s+'\n')

#-------------------------------------------------------------------------------
# write source header
#
def write_header() :
    l('// #version:{}#'.format(Version))
    l('// machine generated, do not edit!')
    l('#include "z80.h"')
    l('namespace YAKC {')
    l('uint32_t z80::do_op(system_bus* bus) {')

#-------------------------------------------------------------------------------
# begin a new instruction group (begins a switch statement)
#
def write_begin_group(indent, ext_byte=None, read_offset=False) :
    if ext_byte :
        # this is a prefix instruction, need to write a case
        l('{}case {}:'.format(tab(indent), hex(ext_byte)))
    indent += 1
    # special case for DD/FD CB 'double extended' instructions,
    # these have the d offset after the CB byte and before
    # the actual instruction byte
    if read_offset :
        l('{}{{ const int d = mem.rs8(PC++);'.format(tab(indent)))
    l('{}switch (fetch_op()) {{'.format(tab(indent)))
    indent += 1
    return indent

#-------------------------------------------------------------------------------
# write a single (writes a case inside the current switch)
#
def write_op(indent, op) :
    if op.src :
        l('{}case {}: {} // {}'.format(tab(indent), hex(op.byte), op.src, op.cmt))

#-------------------------------------------------------------------------------
# finish an instruction group (ends current statement)
#
def write_end_group(indent, inv_op_bytes, ext_byte=None, read_offset=False) :
    l('{}default: return invalid_opcode({});'.format(tab(indent), inv_op_bytes))
    indent -= 1
    l('{}}}'.format(tab(indent)))
    # if this was a prefix instruction, need to write a final break
    if ext_byte:
        l('{}break;'.format(tab(indent)))
    if read_offset :
        l('{}}}'.format(tab(indent)))
    indent -= 1
    return indent

#-------------------------------------------------------------------------------
# write source footer
#
def write_footer() :
    l('}')
    l('} // namespace YAKC');

#-------------------------------------------------------------------------------
# main encoder function, this populates all the opcode tables and
# generates the C++ source code into the file f
#
def do_it(f) :

    global Out
    Out = f

    write_header()
    
    # loop over all instruction bytes
    indent = write_begin_group(0)
    for i in range(0, 256) :
        # DD or FD prefix instruction?
        if i == 0xDD or i == 0xFD:
            indent = write_begin_group(indent, i)
            patch_reg_tables('IX' if i==0xDD else 'IY')
            for ii in range(0, 256) :
                if ii == 0xCB:
                    # DD/FD CB prefix
                    indent = write_begin_group(indent, ii, True)
                    for iii in range(0, 256) :
                        write_op(indent, enc_cb_op(iii, True, 'cc_xycb'))
                    indent = write_end_group(indent, 4, True, True)
                else:
                    write_op(indent, enc_op(ii, True, 'cc_xy'))
            unpatch_reg_tables()
            indent = write_end_group(indent, 2, True)
        # ED prefix instructions
        elif i == 0xED:
            indent = write_begin_group(indent, i)
            for ii in range(0, 256) :
                write_op(indent, enc_ed_op(ii))
            indent = write_end_group(indent, 2, True)
        # CB prefix instructions
        elif i == 0xCB:
            indent = write_begin_group(indent, i, False)
            for ii in range(0, 256) :
                write_op(indent, enc_cb_op(ii, False, 'cc_cb'))
            indent = write_end_group(indent, 2, True)
        # non-prefixed instruction
        else:
            write_op(indent, enc_op(i, False, 'cc_op'))
    write_end_group(indent, 1)
    write_footer()

#-------------------------------------------------------------------------------
# fips code generator entry 
#
def generate(input, out_src, out_hdr) :
    if genutil.isDirty(Version, [input], [out_src]) :
        with open(out_src, 'w') as f:
            do_it(f)

