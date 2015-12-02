---
layout: page
title: CAOS Reference
---

# CAOS Programming Reference
{:.no_toc}
* table of conten
{:toc}

## Restrictions

- do not modify the IX register, this is needed as base pointer
  to system memory variables by keyboard and cassette loading
  interrupts
- the only supported interrupt mode is IM2

## Invoking System Calls

CAOS system calls are identified by a number [SYSNR] and invoked
through different entry points which differ in the way how the
system call identifier and arguments are provided.

## System Call Entry Points

These are the entry point addresses for the jump vectors, and how they work:

### F003: Entry Point 1
{:.no_toc}

The SYSNR is encoded as byte in the instruction stream following 
the CALL instruction. Parameters are directly excpected in registers
BC, DE and HL, which may be changed by the system call. The required stack 
depth is 2.

### F006: Entry Point 2
{:.no_toc}

The SYSNR must be provided in the system address ARGC (B780). Parameters
are expected in BC, DE and HL. BC, DE and HL will be saved on the stack, 
which means the system call cannot use those registers for return values.
Required stack depth is 7.

### F009: Entry Point 3
{:.no_toc}

Same as _Entry Point 2_, but the SYSNR is provided in register E. 

### F00C: Entry Point 4
{:.no_toc}

Same as _Entry Point 3_, but the video memory bank (IRM) is switched on when 
on invocation, and switched off on return.

### F015: Entry Point 5 (since CAOS 3.1)
{:.no_toc}

Same as _Entry Point 3_, but switches on the IRM and sets the stack pointer to
(SYSP) == B7AE before the syscall, and after the syscall, switches off the IRM
and restores the stack pointer. The register BC will be modified.

### F01E: Entry Point 6 (since CAOS 3.1)
{:.no_toc}

Same as _Entry Point 5_, but SYSNR is provided in address (ARGC) (B780).


## Special CAOS Entry Points

### E000: Reset Entry
{:.no_toc}

This address is called after a Reset. The system memory locations will
be initialized, user memory remains unchanged.

### F000: Power-On Entry
{:.no_toc}

This address is called on Power-On. All RAM will be cleared, all hardware
modules will be switched-off (except a 16-KByte RAM modul in the slot 08, this
will automatically be mapped to address 4000 on the KC85/2 and KC85/3).

### F00F: Relative Subroutine Call
{:.no_toc}

This doesn't invoke a system call, but implements a relative subroutine call,
since there is no Z80 instruction for this. This is useful for 
position-independent-code. The relative location is expected as 16-bit word
encoded in the instruction stream following the CALL:

~~~
        RCALL SUB
        CALL 0F00FH
        DEFW UP-NEXT
NEXT:   ...
~~~

The DE register cannot be used as subroutine argument, the required stack depth
is 1.

### F018: Switch-on IRM and set SP (since CAOS 3.1)
{:.no_toc}

This call switches on the IRM and sets the stack pointer to (SYSP) (see _Entry
Point 5_). Muste be used together with **F01B**.

### F01B: Switch-off IRM and restore SP (since CAOS 3.1)
{:.no_toc}

This is the counter-function to F018, it switches off the IRM and restores the
stack pointer.

---

## System Memory Locations at B780

|---
|Address    |Name   |Size   |Description
|:-|:-|:-|:-
|B780       |ARGC   |1      |syscall nr for _Entry Points 2 and 4_
|B781       |ARGN   |1      |number of command line arguments
|B782       |ARG1   |2      |1st command line argument
|B784       |ARG2   |2      |2nd command line argument
|B786       |ARG3   |2      |3rd command line argument
|B788       |ARG4-9 |12     |4th to 9th command line argument
|B794       |ARG10  |2      |10th command line argument
|B796       |NUMNX  |1      |number of character in parsed hex number
|B797       |NUMVX  |2      |value of parsed hex number
|B799       |HCADDR |2      |address of channel-selection for system I/O
|B79B       |WINNR  |1      |nr. of current window
|B79C       |WINON  |2      |current window position
|B79E       |WINLG  |2      |current window size
|B7A0       |CURSO  |2      |cursor position in current window
|B7A2       |STBT   |1      |control-byte for character rendering:
|           |       |       |- bit 0 set: don't write pixels
|           |       |       |- bit 1 set: don't write colors
|           |       |       |- bit 3 set: render control-codes as ascii 
|B7A3       |COLOR  |1      |color byte for character rendering:
|           |       |       |- bits 0..2: background color
|           |       |       |- bits 3..6: foreground color
|           |       |       |- bit 7: blink flag
|B7A4       |WEND   |2      |start address of screen-scrolling service routine
|B7A6       |CCTL0  |2      |char image table 20..5F
|B7A8       |CCTL1  |2      |char image table 00..1F and 60..7F
|B7AA       |CCTL2  |2      |char image table A0..DF
|B7AC       |CCTL3  |2      |char image table 80..9F and E0..FF
|B7AE       |SYSP   |2      |initial address of system stack pointer (default 01D4)
|           |       |       |- default KC85/2: 01D4
|           |       |       |- default KC85/3, KC85/4: 01C4
|B7B0       |SUTAB  |2      |address of system-call table
|B7B2       |CTAB   |2      |address of jump table for ASCII control code handlers
|B7B9       |OUTAB  |2      |points to SYSNR table for output channel syscalls
|B7BB       |INTAB  |2      |points to SYSNR table for input channel syscalls
|B7BD       |UOUT1  |3      |jump into USER output channel 2
|B7C0       |UIN1   |3      |jump into USER input channel 2
|B7C3       |UOUT2  |3      |jump into USER output channel 3
|B7C6       |UOUT3  |3      |jump into USER input channel 3
|B7D3       |HOR    |2      |X-coord for graphics functions (0..319)
|B7D5       |VERT   |1      |Y-coord for graphics functions (0..256)
|B7D6       |FARB   |1      |color-control byte for graphics functions:
|           |       |       |- bit 0 set: XOR
|           |       |       |- bit 1 set: clear
|           |       |       |- bits 3..7: foreground color
|B7D7       |MIXIT  |1      |high-byte of IX and interrupt-table
|B7DD       |L3TAB  |2      |**KC85/4**: address of jump table for ESC-code handlers
|B7DF       |L3SIZ  |1      |**KC85/4**: number of entries in L3TAB
|B7E0       |COUNT  |1      |**KC85/4**: wait-count before 1st kbd autorepeat 
|B7E1       |HCPZ   |1      |**KC85/4**: V24 support 
|B7E2       |INTV1  |2      |**KC85/4**: V24 support 
|B7E4       |INTV1L |1      |**KC85/4**: V24 support
|B7E5       |INTV2  |2      |**KC85/4**: V24 support 
|B7E7       |INTV2L |1      |**KC85/4**: V24 support
|B7E8       |HCPZ2  |1      |**KC85/4**: V24 support 
|===

---

## System Memory Locations below 0200

The **IX** register is set to **01F0** at Reset/Power-On.

|---
|Address    |Length |Description
|:-|:-
|IX+1       |1      |**KC85/4**: backing store for port 0x84
|IX+2       |1      |block number for cassette I/O
|IX+4       |1      |**KC85/4**: backing store for port 0x86
|IX+5       |2      |address of cassette I/O buffer (default: B700)
|IX+7       |1      |cassette load control-byte:
|           |       |- bit 0 set: read
|           |       |- bit 0 clr: verify
|IX+8       |1      |key status:
|           |       |- bit 0 set: new key code available, clear bit for ack
|           |       |- bit 7 set: shift-lock activated
|IX+9       |1      |prefix byte for menu (default: 7FH)
|IX+D       |1      |key code (ASCII)
|IX+E       |2      |address of KTAB scancode/keycode conversion table
|===

--- 

## Interrupt Table

The **I** register is set to **01** at Reset/Power-On.

|---
|Address    |Description
|:-|:-
|01D4-01E1  |free for user interrupt-tables
|01E2       |interrupt SIO channel B (if V24 modul in system)
|01E4       |interrupt PIO channel A (cassette input)
|01E6       |interrupt PIO channel B (keyboard input)
|01E8       |interrupt CTC channel 0 (free)
|01EA       |interrupt CTC channel 1 (cassette output)
|01EC       |interrupt CTC channel 2 (sound duration)
|01EE       |interrupt CTC channel 3 (keyboard input)
|===


---

## System Calls

### 00 CRT -- print character

~~~
Desc:       Print a character on screen
In:         A - character code (ASCII)
Out,Change: -
Stack:      11
~~~

### 01 MBO -- tape output

~~~
Desc:      Output a data block to cassette tape
In:        BC     - length of leading sound
           (IX+1) - block number - 1
           (IX+5) - L (buffer address)
           (IX+6) - H (buffer address)
Out:       DE     - buffer end + 1
           (IX+2) - block number
Change:    AF, BC, DE, HL
Stack:     4
~~~

### 02 UOT1 -- output user channel 1

~~~
Desc:       call user function for output to user channel 1
In:         A   - character code 
Out:        depends on function
Change:     depends on function
Note:       the address of the user-provided function must be set
            in UOUT1 (B7BE)
~~~

### 03 UOT2 -- output user channel 2

~~~
Desc:       see UOT1
Note:       the address of the user-provided function must be set
            in UOUT2 (B7C4)
~~~

### 04 KBD -- keyboard input with cursor

~~~
Desc:       show cursor and wait for keyboard input or return
            code sequence from previously pressed function key
In:         -
Out:        A   - character code
Change:     AF, HL
Stack:      9
~~~

### 05 MBI -- tape input

~~~
Desc:       read a data block from cassette tape
In:         (IX+5)  - L (buffer address)
            (IX+6)  - H (buffer address)
Out:        CY = 1  - read error
            (IX+2)  - block number
Change:     AF, BC
Stack:      4
~~~

### 06 USIN1 -- input user channel 1

~~~
Desc:       call user function for input from user channel 1
In:         depends on function
Out:        depends on function
Change:     depends on function
Note:       the address of the user-provided function must be set
            in UIN1 (B7C1)
~~~

### 07 USIN2 -- input user channel 2

~~~
Desc:       see USIN1
Note:       the address of the user-provided function must be set
            in UIN2 (B7C7)
~~~

### 08 ISRO -- initialize tape output

~~~
Desc:           initialize cassette tape output, writes the 1st data block
In,Out,Change:  see 01 MBO
Stack:          4
~~~

### 09 CSRO -- finalize tape output

~~~
Desc:           finalize cassette tape output, writes the last data block
In,Out,Change:  see 01 MBO
Stack:          10
~~~

### 0A ISRI -- initialize tape input

~~~
Desc:           initialize cassette tape input, reads the 1st data block
In,Out,Change:  see 05 MBI
Stack:          5
~~~

### 0B CSRI -- finalize tape input

~~~
Desc:       finalize cassette tape input.
In,Out:     -
Change:     AF,HL
Stack:      1
~~~

### 0C KBDS -- get pressed key without ack

~~~
Desc:       gets pressed key without acknowledge
In:         -
Out:        CY = 1  - key pressed
            A       - key code (ASCII, function keys are F1..FC
Change:     AF
Stack:      0
~~~

### 0D BYE -- warmstart

~~~
Desc:           jumps to RESET address E000
In,Out,Change:  -
~~~

### 0E KBDZ -- get pressed key with ack

~~~
Desc:       get pressed key with acknowledge and autorepeat
In:         -
Out:        CY = 1  - key pressed
            A       - key code (ASCII, function keys are F1..FC)
Change:     AF
Stack:      1
~~~

### 0F COLOR -- set color

~~~
Desc:       sets the current color
In:         E - background color (0..7)
            L - foreground color (0..1FH)
            (ARGN) = 1  - only change foreground color
                   = 2  - change fore- and background color
Out:        -
Change:     AF,L
Stack:      1
~~~

### 10 LOAD -- load file

~~~
Desc:       loads a file from cassette tape
In:         (ARGN)  = 0 - load without offset
                    = 1 - load with offset
            (ARG1)  = loading offset
Out:        -
Change:     AF,BC,DE,HL
Stack:      14
~~~

### 11 VERIF -- verify a file on tape

~~~
Desc:       verifies a file on cassette tape by checking
            the data block checksums
In,Out:     -
Change:     AF,BC,DE,HL
Stack:      14
~~~

### 12 LOOP -- return control to CAOS

~~~
Desc:           returns control to CAOS without memory initialization,
                can be used by menu programs if a RET instruction is
                not possible
In,Out,Change:  -
~~~

### 13 NORM -- reset I/O channels

~~~
Desc:       sets the input and output channel handler functions to 
            CRT and KBD
In:         -
Out:        HL - old output pointer
Change:     HL
Stack:      2
~~~

### 14 WAIT -- waiting loop

~~~
Desc:       implements a simple waiting loop without interrupts
In:         A - t=(A) * 6ms
Out:        -
Change:     AF,B
Stack:      2
~~~

### 15 LARG -- load arguments

~~~
Desc:       load arguments into registers
In:         -
Out:        HL - (ARG1)
            DE - (ARG2)
            BC - (ARG3)
            A  - (ARGN)
Change:     A,BC,DE,HL
Stack:      0
~~~

### 16 INTB -- character from input channel

~~~
Desc:       returns character from current input channel
            (defined by INTAB)
In:         -
Out:        A - character (ASCII)
Change:     -
Stack:      12 (if input channel is set to keyboard)
~~~

### 17 INLIN -- line input

~~~
Desc:       input a complete line until [ENTER]
In:         -
Out:        DE - address to start of line in video ram ascii buffer
Change:     AF,DE
Stack:      12
~~~

### 18 RHEX -- parse hex number string

~~~
Desc:       converts a hex number string into a number
In:         DE - points to number string
Out:        DE - points to end of number string
            (NUMNX) - length of string
            (NUMVX) - converted number
            CY = 1  - error, invalid characters or number string too long
Change:     AF,DE,HL
Stack:      0
~~~

### 19 ERRM -- print ERROR

~~~
Desc:           outputs the text "ERROR"
In,Out,Change:  -
Stack:          13
~~~

### 1A HLHX -- print HL

~~~
Desc:       outputs the register HL as hex number
In:         HL
Out:        -
Change:     AF
Stack:      17
~~~


### 1B HLDE -- print HL DE

~~~
Desc:       outputs the registers HL and DE as hex numbers
In:         HL,DE
Out:        -
Change:     AF
Stack:      19
~~~

### 1C AHEX -- print A

~~~
Desc:       outputs the register A as hex number
In:         A
Out:        -
Change:     AF
Stack:      16
~~~

### 1D ZSUCH -- find command

~~~
Desc:       find a menu command by name
In:         A   - prolog byte (e.g. for CAOS menu: 7F)
            BC  - length of search area
            DE  - start of search string
            HL  - start of search area
Out:        DE      - end+1 of search string
            HL      - end+1 of found string
            CY=1    - if search successful
Change:     AF,BC,DE,HL
Stack:      2
~~~

### 1E SOUT -- set output table

~~~
Desc:       updates the OUTAB pointer to HL
In:         HL - new OUTAB pointer
Out:        HL - old OUTAB pointer
Change:     HL
Stack:      1
~~~

### 1F SIN -- set input table

~~~
Desc:       updates the INTAB pointer to HL
In:         HL - new INTAB pointer
Out:        HL - old INTAB pointer
Change:     HL
Stack:      1
~~~

