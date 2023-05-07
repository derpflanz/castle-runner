; Blink LED on PB5(Arduino Uno pin 13)
; http://forum.arduino.cc/index.php?topic=159572#msg1194604


; sbi     Set bit in IO register
; ldi     Load immediate   reg <- value
; sbiw    Subtract immediate from word
; brne    Branch if not equal

#define __SFR_OFFSET 0

#include "avr/io.h"

.global myAssemblyMain

myAssemblyMain:
  sbi   DDRA, 0     ; Set PA0 as output
  sbi   DDRA, 1
  sbi   DDRA, 2
  sbi   DDRA, 3
  sbi   DDRA, 4
  sbi   DDRA, 5
  sbi   DDRA, 6
  sbi   DDRA, 7

  clr 16
blink:
  ldi r16, 0          ; 1c
  out  PORTA, r16     ; 1c

  call delay_us_2
  call delay_us_2
  call delay_us_2
  call delay_us_2

  ldi r16, 1
  out   PORTA, r16

  call delay_us_2
  call delay_us
  call delay_us
  call delay_us
  call delay_us
  call delay_us
  call delay_us

  ldi r16, 2
  out PORTA, r16

  call delay_52us

  rjmp   blink

delay_us:             ; 4
  ; us = 16 cycles
  ; call = 4
  ; ret = 4
  ; 8nops = 8

  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  
  ret                 ; 4 cycles

delay_us_2:             ; 4
  ; us = 16 cycles
  ; call = 4
  ; ret = 4
  ; 8nops = 8

  nop
  nop
  nop
  nop
  nop
  nop
  
  ret                 ; 4 cycles  

delay_52us:
  ldi r30, 44

_inner:
  dec r30
  call delay_us_2
  brne _inner

  ret