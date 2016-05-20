#-------------------------------------------------------------------------------
#   z80_opcodes.py
#   Generate brute-force Z80 instruction decoder.
#   See: http://www.z80.info/decoding.htm
#-------------------------------------------------------------------------------

# 8-bit register table, the 'HL' entry is for instructions that use
# (HL), (IX+d) and (IY+d), and will be patched to 'IX' or 'IY' for
# the DD/FD prefixed instructions
r = [ 'B', 'C', 'D', 'E', 'H', 'L', 'HL', 'A' ]

# the same, but the 'HL' item is never patched to IX/IY
r2 = [ 'B', 'C', 'D', 'E', 'H', 'L', 'HL', 'A' ]

# 16-bit register table, with SP
rp = [ 'BC', 'DE', 'HL', 'SP' ]

# 16-bit register table, with AF
rp2 = [ 'BC', 'DE', 'HL', 'AF' ]

# condition-code table (for conditional jumps etc)
cc = [
#   NZ         Z         NC         C         PO         PE        P          M
    '!(F&ZF)', '(F&ZF)', '!(F&CF)', '(F&CF)', '!(F&PF)', '(F&PF)', '!(F&SF)', '(F&SF)'
]

# the same, but for comments
cc_cmt = [ 'NZ', 'Z', 'NC', 'C', 'PO', 'PE', 'P', 'M' ]

# 8-bit ALU instruction table (C++ method names)
alu = [ 'add8', 'adc8', 'sub8', 'sbc8', 'and8', 'xor8', 'or8', 'cp8' ]

# the same, but for comments
alu_cmt = [ 'ADD', 'ADC', 'SUB', 'SBC', 'AND', 'XOR', 'OR', 'CP' ]

# an 'opcode' consists of the opcode-byte, the C++ code, and the number of cycles,
# ops are collected in several lookup tables, invalid ops have a src set to None
class opcode :
    def __init__(self, op) :
        self.op  = op
        self.cmt = None
        self.src = None

# op lookup tables
ops = []        # main instructions
cb_ops = []     # CB prefix instructions
dd_ops = []     # DD prefix instructions
ed_ops = []     # ED prefix instructions
fd_ops = []     # FD prefix instructions 
dd_cb_ops = []  # DD CB prefix instructions
fd_cb_ops = []  # FD CB prefix instructions

#-------------------------------------------------------------------------------
# helper function, return comment for (HL), (IX+d), (IY+d)
#
def iHLcmt(ext) :
    if (ext) :
        return '({}+d)'.format(r[6])
    else :
        return '({})'.format(r[6])

#-------------------------------------------------------------------------------
# helper function, return code for address of (HL), (IX+d), (IY+d)
#
def iHLsrc(ext) :
    if (ext) :
        # IX+d or IY+d
        return '{}+mem.rs8(PC++)'.format(r[6])
    else :
        # HL
        return '{}'.format(r[6])

#-------------------------------------------------------------------------------
# encode a main instruction, this may also be an DD or FD prefix instruction
# takes an opcode byte and returns an opcode object with code
#
def enc_op(op, cyc, ext) :

    o = opcode(op)

    # split opcode byte into bit groups 
    #   |xx|yyy|zzz|
    #   |xx|pp|q|zzz|
    x = op>>6
    y = (op>>3)&7
    z = op&7
    p = y>>1
    q = y&1
    ext_cyc = 8 if ext else 0

    #---- block 1: 8-bit loads, and special case HALT
    if x == 1:
        if y == 6:
            if z == 6:
                # special case LD (HL),(HL) is HALT
                o.cmt = 'HALT'
                o.src = 'halt(); return {};'.format(4+cyc)
            else:
                # LD (HL),r; LD (IX+d),r; LD (IX+d),r
                o.cmt = 'LD {},{}'.format(iHLcmt(ext), r2[z])
                o.src = 'mem.w8({},{}); return {};'.format(iHLsrc(ext), r2[z], 7+cyc+ext_cyc)
        elif z == 6:
            # LD r,(HL); LD r,(IX+d); LD r,(IY+d)
            o.cmt = 'LD {},{}'.format(r2[y], iHLcmt(ext))
            o.src = '{}=mem.r8({}); return {};'.format(r2[y], iHLsrc(ext), 7+cyc+ext_cyc)
        else:
            # LD r,s
            o.cmt = 'LD {},{}'.format(r2[y], r2[z])
            o.src = '{}={}; return {};'.format(r2[y], r2[z], 4+cyc)

    #---- block 2: 8-bit ALU instructions (ADD, ADC, SUB, SBC, AND, XOR, OR, CP)
    elif x == 2:
        if z == 6:
            # ALU (HL); ALU (IX+d); ALU (IY+d)
            o.cmt = '{} {}'.format(alu_cmt[y], iHLcmt(ext))
            o.src = '{}(mem.r8({})); return {};'.format(alu[y], iHLsrc(ext), 7+cyc+ext_cyc)
        else:
            # ALU r
            o.cmt = '{} {}'.format(alu_cmt[y], r[z])
            o.src = '{}({}); return {};'.format(alu[y], r[z], 4+cyc)

    #---- block 0: misc ops
    elif x == 0:
        if z == 0:
            if y == 0:
                # NOP
                o.cmt = 'NOP'
                o.src = 'return {};'.format(4+cyc)
            elif y == 1:
                # EX AF,AF'
                o.cmt = "EX AF,AF'"
                o.src = 'swap16(AF,AF_); return {};'.format(4+cyc)
            elif y == 2:
                # DJNZ d
                o.cmt = 'DJNZ'
                o.src = 'if (--B>0) {{ PC+=mem.rs8(PC)+1; return {}; }} else {{ PC++; return {}; }}'.format(13+cyc, 8+cyc)
            elif  y == 3:
                # JR d
                o.cmt = 'JR d'
                o.src = 'PC+=mem.rs8(PC)+1; return {};'.format(12+cyc)
            else:
                # JR cc,d
                o.cmt = 'JR {},d'.format(cc_cmt[y-4])
                o.src = 'if ({}) {{ PC+=mem.rs8(PC)+1; return {}; }} else {{ PC++; return {}; }}'.format(cc[y-4], 12+cyc, 7+cyc)
        elif z == 1:
            if q == 0:
                # 16-bit immediate loads
                o.cmt = 'LD {},nn'.format(rp[p])
                o.src = '{}=mem.r16(PC); return {};'.format(rp[p], 10+cyc)
            else :
                # ADD HL,rr; ADD IX,rr; ADD IY,rr
                o.cmt = 'ADD {},{}'.format(rp[2],rp[p])
                o.src = '{}=add16({},{}); return {};'.format(rp[2],rp[2],rp[p], 11+cyc) 
        elif z == 2:
            # indirect loads
            op_tbl = [
                [ 'LD (BC),A', 'mem.w8(BC,A); return {};'.format(7+cyc) ],
                [ 'LD A,(BC)', 'A=mem.r8(BC); return {};'.format(7+cyc) ],
                [ 'LD (DE),A', 'mem.w8(DE,A); return {};'.format(7+cyc) ],
                [ 'LD A,(DE)', 'A=mem.r8(DE); return {};'.format(7+cyc) ],
                [ 'LD (nn),{}'.format(rp[2]), 'mem.w16(mem.r16(PC),{}; PC+=2; return {};'.format(rp[2],16+cyc) ],
                [ 'LD {},(nn)'.format(rp[2]), '{}=mem.r16(mem.r16(PC); PC+=2; return {};'.format(rp[2],16+cyc) ],
                [ 'LD (nn),A', 'mem.w8(mem.r16(PC),A); PC+=2; return {};'.format(13+cyc) ],
                [ 'LD A,(nn)', 'A=mem.r8(mem.r16(PC)); PC+=2; return {};'.format(13+cyc) ]
            ]
            o.cmt = op_tbl[y][0]
            o.src = op_tbl[y][1]
        elif z == 3:
            # 16-bit INC/DEC 
            if q == 0:
                o.cmt = 'INC {}'.format(rp[p])
                o.src = '{}++; return {};'.format(rp[p], 6+cyc)
            else:
                o.cmt = 'DEC {}'.format(rp[p])
                o.src = '{}--; return {};'.format(rp[p], 6+cyc)
        elif z == 4 or z == 5:
            cmt = 'INC' if z == 4 else 'DEC'
            fn = 'inc8' if z == 4 else 'dec8'
            if y == 6:
                # INC/DEC (HL)/(IX+d)/(IY+d)
                o.cmt = '{} {}'.format(cmt, iHLcmt(ext))
                o.src = '{{ const uword a={}; mem.w8(a,{}(mem.r8(a))); }} return {};'.format(iHLsrc(ext), fn, 11+cyc+ext_cyc)
            else:
                # INC/DEC r
                o.cmt = '{} {}'.format(cmt, r[y])
                o.src = '{}={}({}); return {}'.format(r[y], fn, r[y], 4+cyc)
        elif z == 6:
            if y == 6:
                # LD (HL),n; LD (IX+d),n; LD (IY+d),n
                o.cmt = 'LD {},n'.format(iHLcmt(ext))
                o.src = 'mem.w8({},mem.r8(PC++)); return {};'.format(iHLsrc(ext), (15 if ext else 10)+cyc)
            else:
                # LD r,n
                o.cmt = 'LD {},n'.format(r[y])
                o.src = '{}=mem.r8(PC++); return {};'.format(r[y], 7+cyc)
        elif z == 7:
            # misc ops on A and F
            op_tbl = [
                [ 'RLCA', 'A=rlc8(A,false);'],
                [ 'RRCA', 'A=rrc8(A,false);'],
                [ 'RLA', 'A=rl8(A,false);'],
                [ 'RRA', 'A=rr8(A,false);'],
                [ 'DAA', 'daa();'],
                [ 'CPL', 'A^=0xFF; F=(F&(SF|ZF|PF|CF))|HF|NF|(A&(YF|XF));'],
                [ 'SCF', 'F=(F&(SF|ZF|YF|XF|PF))|CF|(A&(YF|XF));'],
                [ 'CCF', 'F=((F&(SF|ZF|YF|XF|PF|CF))|((F&CF)<<4)|(A&(YF|XF)))^CF;']
            ]
            o.cmt = op_tbl[y][0]
            o.src = op_tbl[y][1] + ' return {};'.format(4+cyc)

    #--- block 3: misc and extended ops
    elif x == 3:
        if z == 0:
            # RET cc
            o.cmt = 'RET {}'.format(cc_cmt[y])
            o.src = 'if ({}) {{ PC=mem.r16(SP); SP+=2; return {}; }} else return {};'.format(cc[y], 11+cyc, 5+cyc)
        elif z == 1:
            if q == 0:
                # POP BC,DE,HL,IX,IY,AF
                o.cmt = 'POP {}'.format(rp2[p])
                o.src = '{}=mem.r16(SP); SP+=2; return {};'.format(rp2[p], 10+cyc)
            else:
                # misc ops
                op_tbl = [
                    [ 'RET', 'PC=mem.r16(SP); SP+=2; return {};'.format(10+cyc) ],
                    [ 'EXX', 'swap16(BC,BC_); swap16(DE,DE_); swap16(HL,HL_); return {};'.format(4+cyc) ],
                    [ 'JP {}'.format(rp[2]), 'PC={}; return {};'.format(rp[2], 4+cyc) ],
                    [ 'LD SP,{}'.format(rp[2]), 'SP={}; return {};'.format(rp[2], 6+cyc) ]
                ]
                o.cmt = op_tbl[p][0]
                o.src = op_tbl[p][1]
        elif z == 2:
            # JP cc,nn
            o.cmt = 'JP {},nn'.format(cc_cmt[y])
            o.src = 'if ({}) {{ PC=mem.r16(PC); }} else {{ PC+=2; }}; return {};'.format(cc[y], 10+cyc)
        elif z == 3:
            # misc ops
            op_tbl = [
                [ 'JP nn', 'PC=mem.r16(PC); return {};'.format(10+cyc) ],
                [ None, None ], # CB prefix instructions
                [ 'OUT (n),A', 'out((A<<8)|mem.r8(PC++),A); return {};'.format(11+cyc) ],
                [ 'IN A,(n)', 'A=in((A<<8)|mem.r8(PC++)); return {};'.format(11+cyc) ],
                [ 
                    'EX (SP),{}'.format(rp[2]), 
                    '{{ const uword swp=mem.r16(SP); mem.w16(SP,{}); {}=swp; }} return {}'.format(rp[2], rp[2], 19+cyc)
                ],
                [ 'EX DE,HL', 'swap16(DE,HL); return {}'.format(4+cyc) ],
                [ 'DI', 'di(); return {};'.format(4+cyc) ],
                [ 'EI', 'ei(); return {};'.format(4+cyc) ]
            ]
            o.cmt = op_tbl[y][0]
            o.src = op_tbl[y][1]
        elif z == 4:
            # CALL cc,nn
            o.cmt = 'CALL {},nn'.format(cc_cmt[y])
            o.src = 'if ({}) {{ SP-=2; mem.w16(SP,PC+2); PC=mem.r16(SP); return {}; }} else {{ PC+=2; return {}; }}'.format(cc[y], 17+cyc, 10+cyc)
        elif z == 5:
            if q == 0:
                # PUSH BC,DE,HL,IX,IY,AF
                o.cmt = 'PUSH {}'.format(rp2[p])
                o.src = 'SP-=2; mem.w16(SP,{}); return {};'.format(rp2[p], 11+cyc)
            else:
                op_tbl = [
                    [ 'CALL nn', 'SP-=2; mem.w16(SP,PC+2); PC=mem.r16(PC); return {}'.format(17+cyc) ],
                    [ None, None ], # DD prefix instructions
                    [ None, None ], # ED prefix instructions
                    [ None, None ], # FD prefix instructions
                ]
                o.cmt = op_tbl[p][0]
                o.src = op_tbl[p][1]
        elif z == 6:
            # ALU n
            o.cmt = '{} n'.format(alu_cmt[y])
            o.src = '{}(mem.r8(PC++)); return {};'.format(alu[y], 7+cyc)
        elif z == 7:
            # RST
            o.cmt = 'RST {}'.format(y*8)
            o.src = 'rst({}); return {};'.format(y*8, 11+cyc)            

    return o

#-------------------------------------------------------------------------------
# main encoder function, this populates all the opcode tables
#
def encode_all() :

    # main instructions
    for i in range(0, 256) :
        o = enc_op(i, 0, False)
        if o.src :
            print('{} =>  {}:\n  {}\n'.format(hex(o.op), o.cmt, o.src))

    # DD instructions
    r[4] = 'IXH'; r[5] = 'IXL'; r[6] = 'IX'
    rp[2] = rp2[2] = 'IX'
    for i in range(0, 256) :
        o = enc_op(i, 4, True)
        if o.src :
            print('0xDD {} => {}:\n  {}\n'.format(hex(o.op), o.cmt, o.src))

    # FD instructions
    r[4] = 'IYH'; r[5] = 'IYL'; r[6] = 'IY'
    rp[2] = rp2[2] = 'IY'
    for i in range(0, 256) :
        o = enc_op(i, 4, True)
        if o.src :
            print('0xFD {} => {}:\n  {}\n'.format(hex(o.op), o.cmt, o.src))

# __main__
if __name__ == "__main__" :
    encode_all()

