.entry r8
.entry str1

main:	add 12, @r3
	cmp X, -123
data:	.data 6, 75
label1:	stop
	red vAr
	bne str3
	.data 0, -999, 123, 45
	prn 124
	bne @r4
.entry Rst
str1:	.string "mmn14"

.extern ext

label2:	dec X
	sub @r1, str1
r8:	.data -32

.entry label2
	jsr @r6
uselessLabel: .entry label1
	lea r8, ext
	inc main
.extern X

; This is one string.
vAr:	.string "hello" "world" 
Rst:	rts
	.data 887, -0
str2: 	.string ".data"
.extern str3
.entry str2
