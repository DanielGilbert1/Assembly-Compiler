.entry Length
.extern ext
Label: mov 5,@r3
;not enough operands
jmp 
; , is after last operand
add 3, X,
;label Length already defined as .entry
Length: .data 17
.entry Length
;paramter for .entry cant be an external
.entry ext
;first operand must be a label
lea 10, X
;destination opernd can't be an immediate number
sub @r5, 12
;empty src operand
cmp , @r4
;invalid paramter (not register, immediate or label)
not A
;too many operdans
stop @r5
;no such command
move 5, X
;Label alreday exist
Label: inc X
X: .data 5
;no such directive
Y: .some 7
;can't attach label to empty line
emp: 
;label too long
thisLabelIsMoreThan31CharsSoWeWillGetAnError: .data5
;line too long
JustVeryLongLabelToFillLine: mov LongLabaelForToolongLine1, LongLabaelForToolongLine2
LongLabaelForToolongLine1: .data 5
LongLabaelForToolongLine2: .data 1