;
; This is a simple display module
; called by the C part of the program
;

.import popa,popax
.export _AdvancedPrint
;void AdvancedPrint( (char) (x), (char) y+1, str)

.proc _AdvancedPrint

;
; We define the adress of the TEXT screen.
;
DISPLAY_ADRESS :=$BB80




;
; The message and display position will be read from the stack.
; sp+0 => X coordinate
; sp+2 => Y coordinate
; sp+4 => Adress of the message to display
;


        ; Initialise display adress
        ; this uses self-modifying code
        ; (the $0123 is replaced by display adress)
        
        ; The idea is to get the Y position from the stack,
        ; and use it as an index in the two adress tables.
        ; We also need to add the value of the X position,
        ; also taken from the stack to the resulting value.
        
		;jsr popax
		sta read+1
		stx read+2
		
		jsr popa ; y
		sta posy
        
        tax
        
		
		jsr popa ; x
		sta posx
		
        lda ScreenAdressLow,x   ; Get the LOW part of the screen adress
        clc                                             ; Clear the carry (because we will do an addition after)
		
        ;ldy #0
        adc posx                            ; Add X coordinate
        sta write+1
        lda ScreenAdressHigh,x  ; Get the HIGH part of the screen adress
        adc #0                                  ; Eventually add the carry to complete the 16 bits addition
        sta write+2                             



        ; Initialise message adress using the stack parameter
        ; this uses self-modifying code
        ; (the $0123 is replaced by message adress)
      

        ; Start at the first character
        ldx #0
loop_char:

        ; Read the character, exit if it's a 0
read:
        lda $0123,x
        beq end_loop_char

        ; Write the character on screen
write:
        sta $0123,x

        ; Next character, and loop
        inx
        jmp loop_char  

        ; Finished !
end_loop_char:
        rts

;
; We use a table of bytes to avoid the multiplication 
; by 40. We could have used a multiplication routine
; but introducing table accessing is not a bad thing.
; In order to speed up things, we precompute the real
; adress of each start of line. Each table takes only
; 28 bytes, even if it looks impressive at first glance.
;

; This table contains lower 8 bits of the adress
ScreenAdressLow:
        .byt <(DISPLAY_ADRESS+40*0)
        .byt <(DISPLAY_ADRESS+40*1)
        .byt <(DISPLAY_ADRESS+40*2)
        .byt <(DISPLAY_ADRESS+40*3)
        .byt <(DISPLAY_ADRESS+40*4)
        .byt <(DISPLAY_ADRESS+40*5)
        .byt <(DISPLAY_ADRESS+40*6)
        .byt <(DISPLAY_ADRESS+40*7)
        .byt <(DISPLAY_ADRESS+40*8)
        .byt <(DISPLAY_ADRESS+40*9)
        .byt <(DISPLAY_ADRESS+40*10)
        .byt <(DISPLAY_ADRESS+40*11)
        .byt <(DISPLAY_ADRESS+40*12)
        .byt <(DISPLAY_ADRESS+40*13)
        .byt <(DISPLAY_ADRESS+40*14)
        .byt <(DISPLAY_ADRESS+40*15)
        .byt <(DISPLAY_ADRESS+40*16)
        .byt <(DISPLAY_ADRESS+40*17)
        .byt <(DISPLAY_ADRESS+40*18)
        .byt <(DISPLAY_ADRESS+40*19)
        .byt <(DISPLAY_ADRESS+40*20)
        .byt <(DISPLAY_ADRESS+40*21)
        .byt <(DISPLAY_ADRESS+40*22)
        .byt <(DISPLAY_ADRESS+40*23)
        .byt <(DISPLAY_ADRESS+40*24)
        .byt <(DISPLAY_ADRESS+40*25)
        .byt <(DISPLAY_ADRESS+40*26)
        .byt <(DISPLAY_ADRESS+40*27)

; This table contains hight 8 bits of the adress
ScreenAdressHigh:
        .byt >(DISPLAY_ADRESS+40*0)
        .byt >(DISPLAY_ADRESS+40*1)
        .byt >(DISPLAY_ADRESS+40*2)
        .byt >(DISPLAY_ADRESS+40*3)
        .byt >(DISPLAY_ADRESS+40*4)
        .byt >(DISPLAY_ADRESS+40*5)
        .byt >(DISPLAY_ADRESS+40*6)
        .byt >(DISPLAY_ADRESS+40*7)
        .byt >(DISPLAY_ADRESS+40*8)
        .byt >(DISPLAY_ADRESS+40*9)
        .byt >(DISPLAY_ADRESS+40*10)
        .byt >(DISPLAY_ADRESS+40*11)
        .byt >(DISPLAY_ADRESS+40*12)
        .byt >(DISPLAY_ADRESS+40*13)
        .byt >(DISPLAY_ADRESS+40*14)
        .byt >(DISPLAY_ADRESS+40*15)
        .byt >(DISPLAY_ADRESS+40*16)
        .byt >(DISPLAY_ADRESS+40*17)
        .byt >(DISPLAY_ADRESS+40*18)
        .byt >(DISPLAY_ADRESS+40*19)
        .byt >(DISPLAY_ADRESS+40*20)
        .byt >(DISPLAY_ADRESS+40*21)
        .byt >(DISPLAY_ADRESS+40*22)
        .byt >(DISPLAY_ADRESS+40*23)
        .byt >(DISPLAY_ADRESS+40*24)
        .byt >(DISPLAY_ADRESS+40*25)
        .byt >(DISPLAY_ADRESS+40*26)
        .byt >(DISPLAY_ADRESS+40*27)
posx:
.byt 0
posy:
.byt 0
		
.endproc