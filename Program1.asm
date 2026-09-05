; Programming Project 1 starter file
; Student Name: Justin Yang
; UTEid: jsy558
; Modify this code to satisfy the requirements of Program 1
; Compute N^M, where N and M are non-negative inputs to your program.
; The input numbers are given to you in memory locations x3500 (N) and x3501 (M) 
; The computed result has to be placed in x3502 (N2theM). 
; If the computation of the value of NM exceeds x7FFF then you put the 
; value -1 at x3502. Assume 0^0 = 0.
; Read the complete Project Description on the Google doc linked
    .ORIG  x3000
;---- Your Solution goes here
    LDW R0, R7, #0 ; R0 = N
    LDW R1, R7, #1 ; R1 = M
    
; fringe cases
    AND R3, R3, #0
    ADD R3, R3, R1     ; R3 = M
    BRp notZero        ; if M > 0, continue normally

    ; M == 0
    ADD R3, R0, #0     ; R3 = N
    BRz zero_base      ; if N == 0, result = 0
    AND R5, R5, #0
    ADD R5, R5, #1     ; result = 1 (N^0)
    STW R5, R7, #2
    BRnzp skip_overflow

zero_base
    AND R5, R5, #0
    STW R5, R7, #2     ; result = 0 (0^0 or 0^M)
    BRnzp skip_overflow
    
notZero

    ADD R3, R0, #0
    BRz zero_base
    
    AND R5, R5, #0 
    ADD R5, R5, #1 ; R5 = 1
additional_loop
    AND R4, R4, #0 
    ADD R4, R4, R0 ; R4 = N
    AND R2, R2, #0 ; R2 = 0
loop_to_N
    ADD R2, R2, R5 ; R2 += R5
    ADD R4, R4, #-1 ; R4 -= 1
    BRp loop_to_N ; if R4 is still positive, rerun xN function
    AND R5, R5, #0 ; R5 = 0
    ADD R5, R5, R2 ; R5 = R2
    ADD R1, R1, #-1 ; R1 -= 1
    BRp additional_loop ; if R1 is still positive, rerun xN with new N
    LDW R3, LargeValue ; R3 = x7FFF
    NOT R3, R3
    ADD R3, R3, #1 ; R3 = -R3
    ADD R4, R5, R3 ; R4 = R5 - R3
    BRp overflowed; if R5 is larger than zero, overflow
    STW R5, R7, #2 ; if R5 isn't larger, store R5 as result
    BRnzp skip_overflow ; jump to end of program
overflowed
    LDW R6, Overflow ; R6 = xFFFF
    STW R6, R7, #2 ; store -1 as result
skip_overflow
;---- Done
	HALT
;---- You may declare your stuff here if needed
Offset
    .FILL x3500
LargeValue
    .FILL x7FFF
Overflow
    .FILL xFFFE
    .END
    
;---- Data: Inputs and Output go here
    .ORIG x3500
N    .FILL x0003
M    .FILL x0002
;N    .FILL x000A
;M    .FILL x000A
;N    .FILL x0002
;M    .FILL x000A
;N    .FILL x00B2
;M    .FILL x0002
;N    .FILL x7FFF
;M    .FILL x0001
N2theM  .BLKW #1
    .END