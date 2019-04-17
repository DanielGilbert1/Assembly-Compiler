mov x, @r4
Res: clr x
inc x
dec @r5
jmp Res
red @r1
cmp 1, @r1
lea Label, @r3
Label: .string "this"
x: .data 10
