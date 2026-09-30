.syntax unified
.cpu cortex-m3
.thumb

.global Reset_Handler
.global g_pfnVectors

.section .isr_vector,"a",%progbits
.align 2

g_pfnVectors:
    .word _estack
    .word Reset_Handler
    .word Reset_Handler
    .word Reset_Handler
    .word Reset_Handler
    .word Reset_Handler
    .word Reset_Handler
    .word Reset_Handler
    .word Reset_Handler
    .word Reset_Handler
    .word Reset_Handler
    .word Reset_Handler
    .word Reset_Handler
    .word Reset_Handler
    .word Reset_Handler
    .word Reset_Handler

.section .text.Reset_Handler
.thumb_func
.type Reset_Handler, %function

Reset_Handler:

    ldr r0, =_sidata
    ldr r1, =_sdata
    ldr r2, =_edata

copy_data:

    cmp r1, r2
    bcs zero_bss

    ldr r3, [r0]
    str r3, [r1]

    adds r0, r0, #4
    adds r1, r1, #4

    b copy_data

zero_bss:

    ldr r1, =_sbss
    ldr r2, =_ebss

    movs r3, #0

clear_bss:

    cmp r1, r2
    bcs start_main

    str r3, [r1]

    adds r1, r1, #4

    b clear_bss

start_main:

    bl main

    b start_main