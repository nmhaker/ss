.global aaa, c
.global b
.text	
	jmpeq a a2
	jmp e a
	jmpaseq b
	asjmp d
	call foo
d2a:	.long d
	mov r0, b
	mov c, r1
	mov r2, e
foo:	cmp a, b
	ret
	iret
.data
.skip 8
e:	.long a-e+d
	.long c
	.long .bss
a:	.long b
.bss
c:	.skip 8
.end
