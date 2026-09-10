.ORIG x3000 
    LEA R0, Access
    LDB R1, R0, #0 ; x3100
    LDB R1, R0, #1 ; x3101
    AND R4, R4, #0 ; R4 will track overflow status
Loop
    ADD R2, R2, R1 ; R2 has product
    BRn Overflow ; if the result suddenly becomes negative, overflow
Resume_Loop
    ADD R0, R0, #-1
    BRz Loop_Over
    BR Loop




Overflow
    ADD R4, R4, #1
    BR Resume_Loop ; go back to loop and continue where left off

Loop_Over
    STB R4, R0, #3 ; 1 -> MEM[x3103] 
    STB R2, R0, #2 ; Result -> MEM[x3102]

HALT

Access
    .FILL x3100


.END