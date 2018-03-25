         *= $0801

         .byte $4c,$16,$08,$00,$97,$32
         .byte $2c,$30,$3a,$9e,$32,$30
         .byte $37,$30,$00,$00,$00,$a9
         .byte $01,$85,$02

         jsr print
         .byte 13
         .text "(up)bmir"
         .byte 0

         lda #%00011011
         sta db
         lda #%11000110
         sta ab
         lda #%10110001
         sta xb
         lda #%01101100
         sta yb
         lda #%10000000
         sta pb
         tsx
         stx sb

         lda #0
         sta db

         lda #<break
         sta $0316
         lda #>break
         sta $0317

         ldx #0
         txa
fill     sta $1082,x
         sta $1182,x
         inx
         bne fill

next     lda db
         sta da
         sta dr

         lda ab
         sta ar

         lda xb
         sta xr

         lda yb
         sta yr

         lda pb
         ora #%00110000
         sta pr

         ldx cmd+1
         lda branch
         sta $1100,x
         lda db
         sta $1101,x

         ldx sb
         stx sr
         txs
         lda pb
         pha
         lda ab
         ldx xb
         ldy yb
         plp

cmd      jmp $1100

break    pla
         sta ya
         pla
         sta xa
         pla
         sta aa
         pla
         sta pa
         pla
         sta al+1
         pla
         sta ah+1
         tsx
         stx sa

         clc
         lda cmd+1
         ldy cmd+2
         adc #4
         bcc noinc
         iny
noinc    bit db
         bpl pos
         dey
pos      clc
         adc db
         bcc al
         iny
al       cmp #0
         bne err
ah       cpy #0
         beq noerr
err      jsr print
         .byte 13
         .text "wrong jump address"
         .byte 13,0
         jsr wait
noerr    jsr check

         inc db
         lda db
         cmp #$fe
         bne jmpnext
         lda #0
         sta db
         ldx cmd+1
         sta $1100,x
         inc cmd+1
         beq nonext
jmpnext  jmp next
nonext
         lda #$7f
branch   bmi berr

ookk     jsr print
         .text " - ok"
         .byte 13,0
         lda 2
         beq load
wt       jsr $ffe4
         beq wt
         jmp $8000

berr     jsr print
         .byte 13
         .text "no jump expected"
         .byte 13,0
         jsr wait
         jmp ookk

load     jsr print
name     .text "bplr"
namelen  = *-name
         .byte 0
         lda #0
         sta $0a
         sta $b9
         lda #namelen
         sta $b7
         lda #<name
         sta $bb
         lda #>name
         sta $bc
         pla
         pla
         jmp $e16f

db       .byte 0
ab       .byte 0
xb       .byte 0
yb       .byte 0
pb       .byte 0
sb       .byte 0
da       .byte 0
aa       .byte 0
xa       .byte 0
ya       .byte 0
pa       .byte 0
sa       .byte 0
dr       .byte 0
ar       .byte 0
xr       .byte 0
yr       .byte 0
pr       .byte 0
sr       .byte 0

check    lda da
         cmp dr
         bne error
         lda aa
         cmp ar
         bne error
         lda xa
         cmp xr
         bne error
         lda ya
         cmp yr
         bne error
         lda pa
         cmp pr
         bne error
         lda sa
         cmp sr
         bne error
         rts

error    jsr print
         .byte 13
         .null "before  "
         ldx #<db
         ldy #>db
         jsr showregs
         jsr print
         .byte 13
         .null "after   "
         ldx #<da
         ldy #>da
         jsr showregs
         jsr print
         .byte 13
         .null "right   "
         ldx #<dr
         ldy #>dr
         jsr showregs
         lda #13
         jsr $ffd2
wait     jsr $ffe4
         beq wait
         cmp #3
         beq stop
         rts
stop
         lda 2
         beq basic
         jmp $8000
basic    jmp ($a002)

showregs stx 172
         sty 173
         ldy #0
         lda (172),y
         jsr hexb
         lda #32
         jsr $ffd2
         lda #32
         jsr $ffd2
         iny
         lda (172),y
         jsr hexb
         lda #32
         jsr $ffd2
         iny
         lda (172),y
         jsr hexb
         lda #32
         jsr $ffd2
         iny
         lda (172),y
         jsr hexb
         lda #32
         jsr $ffd2
         iny
         lda (172),y
         ldx #"n"
         asl a
         bcc ok7
         ldx #"N"
ok7      pha
         txa
         jsr $ffd2
         pla
         ldx #"v"
         asl a
         bcc ok6
         ldx #"V"
ok6      pha
         txa
         jsr $ffd2
         pla
         ldx #"0"
         asl a
         bcc ok5
         ldx #"1"
ok5      pha
         txa
         jsr $ffd2
         pla
         ldx #"b"
         asl a
         bcc ok4
         ldx #"B"
ok4      pha
         txa
         jsr $ffd2
         pla
         ldx #"d"
         asl a
         bcc ok3
         ldx #"D"
ok3      pha
         txa
         jsr $ffd2
         pla
         ldx #"i"
         asl a
         bcc ok2
         ldx #"I"
ok2      pha
         txa
         jsr $ffd2
         pla
         ldx #"z"
         asl a
         bcc ok1
         ldx #"Z"
ok1      pha
         txa
         jsr $ffd2
         pla
         ldx #"c"
         asl a
         bcc ok0
         ldx #"C"
ok0      pha
         txa
         jsr $ffd2
         pla
         lda #32
         jsr $ffd2
         iny
         lda (172),y
hexb     pha
         lsr a
         lsr a
         lsr a
         lsr a
         jsr hexn
         pla
         and #$0f
hexn     ora #$30
         cmp #$3a
         bcc hexn0
         adc #6
hexn0    jmp $ffd2

print    pla
         .block
         sta print0+1
         pla
         sta print0+2
         ldx #1
print0   lda !*,x
         beq print1
         jsr $ffd2
         inx
         bne print0
print1   sec
         txa
         adc print0+1
         sta print2+1
         lda #0
         adc print0+2
         sta print2+2
print2   jmp !*
         .bend


