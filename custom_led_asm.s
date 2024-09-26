.code   16              @ This directive selects the instruction set being generated. 
                            @ The value 16 selects Thumb, with the value 32 selecting ARM.

    .text                   @ Tell the assembler that the upcoming section is to be considered
                            @ assembly language instructions - Code section (text -> ROM)

@@ Function Header Block
    .align  2               @ Code alignment - 2^n alignment (n=2)
                            @ This causes the assembler to use 4 byte alignment

    .syntax unified         @ Sets the instruction set to the new unified ARM + THUMB
                            @ instructions. The default is divided (separate instruction sets)

    .global asmtest        @ Make the symbol name for the function visible to the linker

    .code   16              @ 16bit THUMB code (BOTH .code and .thumb_func are required)
    .thumb_func             @ Specifies that the following symbol is the name of a THUMB
                            @ encoded function. Necessary for interlinking between ARM and THUMB code.

    .type   asmtest, %function   @ Declares that the symbol is a function (not strictly required)

asmtest:
bkpt
@ This instruction can also be mov r0, #5
    push {lr}
    @push {r1}
    @push {r0-r7}
    ldr  r0, =0
    bl   BSP_LED_Toggle           @@ call BSP function
    @pop  {r0-r7}
    @ldr  r1, =myTickCount
    @ldr  r0, [r1]
    @pop  {r1} 
    @pop  {pc}
    pop {lr}
    bx lr
.size   asmtest, .-asmtest    @@ - symbol size (not strictly required, but makes the debugger happy)

.code   16
    .text

@@ Function Header Block
    .align  2
    .syntax unified
    .global armfunct
    .code   16
    .thumb_func
    .type   armfunct, %function

armfunct:
    ldr r0, =0
    bx  lr

.size   armfunct, .-armfunct    @@ - symbol size (not strictly required, but makes the debugger happy)

@ Stack testing!
    .code   16
    .text

@@ Function Header Block
    .align  2
    .syntax unified
    .global stackfunct
    .code   16
    .thumb_func
    .type   stackfunct, %function

stackfunct:
bkpt
    ldr     r0, =66
    ldr     r1, =67
    ldr     r2, =68
    push    {r0}
    push    {r1}
    push    {r2}
    
    pop     {r2}
    pop     {r1}
    pop     {r0}
    bx      lr

.size   stackfunct, .-stackfunct    @@ - symbol size (not strictly required, but makes the debugger happy)



.end