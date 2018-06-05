.global aaa, c
.global b
.text	
	jmpeq 0xab2
	jmp e
	jmpaeq b
	jmp d
d2a:	.long d
	mov r0 , b
	mov c, r1
	mov r2, e
.data
.skip 8
e:	.long a-e+d
	.long c
	.long .bss
a:	.long b
.bss
c:	.skip 8
.end
