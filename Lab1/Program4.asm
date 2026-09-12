
; UT Eid: jsy558
; Records at x3500: 16-bit score, 16-bit name pointer (4 bytes).
; A score of -1 terminates the records.
; Names are zero-terminated byte strings; search name starts at x6100.
; Result byte at x60FF: score 0..100, or xFF when not found.
; Load Program4_data.out separately if using the sample records.

    .ORIG x3000
FullPass AND R0, R0, #0       ; swap flag = 0
    AND R1, R1, #0           ; record index = 0
CheckNext LEA R3, Records
    LDW R3, R3, #0
    LSHF R2, R1, #2          ; index * 4 bytes
    ADD R3, R3, R2
    LDW R4, R3, #0           ; score A
    ADD R6, R4, #1
    BRz PassEnd              ; do not read beyond an empty array
    LDW R5, R3, #2           ; score B, 4 bytes after A
    ADD R6, R5, #1
    BRz PassEnd
    NOT R6, R5
    ADD R6, R6, #1
    ADD R6, R4, R6           ; A - B
    BRzp NoSwap              ; equal scores retain original order
    JSR SwapFunc
NoSwap ADD R1, R1, #1
    BR CheckNext
PassEnd ADD R0, R0, #0
    BRz Search
    BR FullPass

SwapFunc LDW R6, R3, #3      ; name pointer B
    LDW R2, R3, #1           ; name pointer A
    STW R5, R3, #0
    STW R6, R3, #1
    STW R4, R3, #2
    STW R2, R3, #3
    AND R0, R0, #0
    ADD R0, R0, #1           ; flag that this pass changed the array
    RET

Search AND R0, R0, #0
NextName LEA R1, Records
    LDW R1, R1, #0
    LSHF R5, R0, #2
    ADD R1, R1, R5
    LDW R6, R1, #0           ; current score
    ADD R5, R6, #1
    BRz NoResult
    LDW R2, R1, #1           ; current student's name pointer
    LEA R3, QueryPtr
    LDW R3, R3, #0           ; restart query for every student
CheckName LDB R4, R2, #0
    LDB R5, R3, #0
    NOT R5, R5
    ADD R5, R5, #1
    ADD R7, R4, R5
    BRz NullTerm
    ADD R0, R0, #1
    BR NextName
NullTerm ADD R5, R4, #0
    BRz Store
    ADD R2, R2, #1
    ADD R3, R3, #1
    BR CheckName
NoResult AND R6, R6, #0
    ADD R6, R6, #-1
Store LEA R3, ResultPtr
    LDW R3, R3, #0
    STB R6, R3, #0           ; x60FF is an odd byte address
    HALT

Records .FILL x3500
ResultPtr .FILL x60FF
QueryPtr .FILL x6100
    .END
