## KC85/3 keyboard input

### Useful links:

* Service manual: http://www.sax.de/~zander/kc85/doku/kc853serv.pdf
* Circuit diagrams: http://www.mpm-kc85.de/schaltbild/KC85_3_Grundgeraet_sp.pdf
* System manual: http://www.mpm-kc85.de/dokupack/KC85_3_SystemHB.pdf

### Overview

The serial keyboard chip sends timed impulses to pin BSTB of the PIO which 
which invokes the PIO-B interrupt handler at address $E199. The interrupt
handler decodes the serial keyboard signal by measuring the time
between pulses using CTC-3 channel as timer and writes temporary data
into the following memory locations:

* **(IX+C)**: temporary keyboard scancode
* **(IX+A)**: pause-counter for key-repeat
* **bit 4 in (IX+8)**: cleared for first key-repeat, set for followup key-repeat
* **bit 3 in (IX+8)**: time-out bit from CTC3 interrupt service routine

If the CTC-3 timer reaches 0 without being pre-empted by the PIO-B
interrupt, a timeout bit (bit 3 in IX+8) is set, and the keycode
in IX+D is reset to 0.

The final keycode is written to IX+D, and bit 0 in IX+8 is set as signal
that a new keycode is set. In case of a key-repeat, the keycode remains in
IX+D, and bit 0 of IX+8 is triggered per key-repeat.

The keyboard interrupt service routine starts by reading the CTC-3 channel
which measures the time between keyboard controller pulses. From the measured
time, either a 0 or 1 is rotated into the temporary scancode at IX+$0C. 

Once the scancode is complete, the caps-lock bit 7 in IX+8 is
merged with the scancode and the scancode is converted to a
keycode using the KTAB lookup table at IX+E and IX+F.

The keycode is compared to the last keycode in IX+D to check whether
key-repeat must be handled. If no, the keycode is stored in IX+D and
bit 0 in IX+8 is set.

If keyrepeat needs to happen, a key-repeat counter in IX+A is maintained,
and bit 4 in IX+8 defines whether this is the first key-repeat (bit 4 cleared),
or a followup keycode (bit 4 set). The first key-repeat kicks in when the
key-repeat counter reaches the value 8, and followup key-repeats happen
every time the key-repeat counter reaches 2.

Now the question is, how often is the keyboard interrupt called to get
the correct key-repeat timing. Hmm...

### Disassembly of OS keyboard handling functions:

**KBDS (get key status without ack):**

Carry flag is set if new keycode available in A.

```z80asm
E2A8:   OR A            ; clear CF
E2A9:   BIT 0,(IX+$08)  ; new key available?
E2AD:   RET Z           ; no
E2AE:   LD A,(IX+$0D)   ; load keycode into A...
E2B1:   SCF             ; ...and set carry flag
E2B2:   RET
```

**KBDZ (get key status with acknowledge, proper key-repeat):**

Carry flag is set if new keycode available in A.

```z80asm
E29F:   CALL $E2A8      ; KBDS
E2A2:   RET NC          ; return if no key
E2A3:   RES 0,(IX+$08)  ; acknowledge key
E2A7:   RET
```

### The CTC-3 interrupt service routine:
This is called when the CTC-3 timer programmed in the PIO-B 
interrupt reaches zero without being pre-empted by the
PIO-B keyboard interrupt, so this is a timeout and deletes the current
keycode in IX+D:

```z80asm
E189:   EI
E18A:   PUSH AF
                            ; this stops the timer until the next keyboard interrupt:
E18B:   LD A,$23            ; write 00100011 CTC3 control word
                            ;   0: disable interrupt
                            ;   0: timer mode
                            ;   1: prescaler 256
                            ;   0: falling edge
                            ;   0: start timer when time constant is written
                            ;   0: no time constant follows
                            ;   1: reset
                            ;   1: this is a control word
E18D:   OUT ($8F),A
E18F:   SET 3,(IX+$08)      ; set timeout bit
E193:   LD (IX+$0D),$00     ; clear current keycode
E197:   JR $E206            ; done (just a POP AF, RETI) 
```

### The PIO-B interrupt service routine:

This is called when a new pulse from the keyboard chip comes in.

```z80asm
keyboard interrupt start:
E199:   EI
E19A:   PUSH AF

read measured time from CTC3:
E19B:   IN A,($8F)      ; read CTC3 (measured time between keyboard pulses)
E19D:   PUSH AF

reset CTC3 for next measurement:
E19E:   LD A,$A7        ; write 10100111 CTC3 control word
                        ;   1: enable interrupt, 
                        ;   0: timer mode, 
                        ;   1: prescaler256, 
                        ;   0: falling-edge
                        ;   0: start timer when constant is written
                        ;   1: constant follows
                        ;   1: reset channel
                        ;   1: this is a control word
E1A0:   OUT ($8F),A     
E1A2:   LD A,$8F        ; time-constant CTC3, start timer
E1A4:   OUT ($8F),A
E1A6:   POP AF

E1A7:   BIT 3,(IX+$08)  ; timeout from CTC-3 interrupt?
E1AB:   RES 3,(IX+$08)  ; clear bit 3 IX+8 (undocumented)
E1AF:   JR NZ,$E206     ; bit 3 in IX+8 was set, RETI

look at measured time to decide whether next scancode bit is 0 or 1
E1B1:   CP $65
E1B3:   JR NC,$E209     ; measured time > $65 (0 -> bit7)
E1B5:   CP $44
E1B7:   JR NC $E20C     ; measured time > $44 (1 -> bit7)

scancode byte is complete
E1B9:   PUSH HL         ; IX+C is now the scancode<<1?
E1BA:   PUSH DE

integrate capslock bit into scancode byte
D1BB:   SRL (IX+$0C)    ; scancode>>1
E1BF:   LD A,(IX+$08)   
E1C2:   AND $80         ; isolate caps-lock bit
E1C4:   RLCA            ; rotate caps-lock bit into carry and bit 0 of A
E1C5:   XOR (IX+$0C)    ; xor caps-lock with shift-key-bit(?)

convert scancode to keycode using KTAB lookup table
E1C8:   LD H,$00
E1CA:   LD E,(IX+$0E)   ; IX+F,IX+E is address of KTAB
E1CD:   LD D,(IX+$0F)
E1D0:   LD L,A
E1D1:   ADD HL,DE
E1D2:   LD A,(HL)       ; convert scancode to keycode, A is now ASCII keycode
E1D3:   POP DE
E1D4:   POP HL

A is now keycode, check for keyrepeat (if keycode hasn't changed)
E1D5:   CP (IX+$0D)     ; compare keycode with last keycode
E1D8:   JR Z,$E1EB      ; key hasn't changed

No key-repeat, write new keycode to IX+D
E1DA:   LD (IX+$0D),A   ; key has changed, store new keycode in IX+D
E1DD:   RES 4,(IX+$08)  ; key has not changed, clear bit 4 IX+8 key-repeat flag

E1E1:   SET 0,(IX+$08)  ; set bit 0 (key-ready) in IX+8 
E1E5:   LD (IX+$0A),$00 ; reset key repeat counter in IX+A
E1E9:   JR $E206        ; and we're done

Handle key-repeat:
E1EB:   INC (IX+$0A)    ; increment key-repeat counter
E1EE:   BIT 4,(IX+$08)  ; first-key-repeat?
E1F2:   JR NZ,$E200     ; no, it's a followup key-repeat

E1F4:   BIT 4,(IX+$0A)  ; first (long) key-repeat: is key-counter >= 8?
E1F8:   JR Z,$E206      ; nope, return without setting 'key-ready' bit

E1FA:   SET 4,(IX+$08)  ; first pause key repeat pause over, set 'followup key-repeat' bit
E1FE:   JR $E1E1        ; set 'key-ready' bit and done

E200:   BIT 1,(IX+$0A)  ; followup (short) key-repeat: is key-counter >= 2? 
E204:   JR NZ,$E1E1     ; yes, set 'key-ready' and done

Done:
E206:   POP AF          
E207:   RETI

Scancode construction, new bit is 0
E209:   OR A            ; clear carry flag, called when measured CTC3 time was >$64
E20A:   JR E20D

Scancode construction, new bit is 1
E20C:   SCF             ; C=1, called when measured CTC3 time was >$44 
E20D:   RR (IX+$0C)     ; build scancode in IX+$0C by rotating from carry flag
E211:   JR $E206
```



