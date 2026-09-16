; program to test all opcodes in LC3b for PL2. 
; values stored at memory locations A, B, C, D, E
.ORIG x3000
	LEA R0, A
	LDB R1, R0, #0 
	LDB R2, R0, #1
	JSR Test_JSR ; jumps to subroutine that tests the shifts and XOR
	LDW R1, R0, #1
	LDW R2, R0, #2
	LEA R3, Test_JSRR
	JSRR R3
Return_Pt 
	BR End
Test_JSR	LSHF R3, R1, #2
	RSHFA R3, R1, #2
	RSHFL R4, R2, #2
	XOR R5, R1, #-1
	RET

Test_JSRR LSHF R3, R1, #2
	RSHFA R3, R1, #2
	RSHFL R4, R0, #2
	XOR R5, R1, R2
	LEA R6, Return_Pt
	JMP R6
	

End HALT




A .FILL x06FF
B .FILL x0702
C .FILL x0803
D .FILL x0904
E .FILL x0A05