.data
# Global objects

# String literals
string_12: .asciiz " 10! = "
string_11: .asciiz "          The total is "
string_9: .asciiz " b = "
string_8: .asciiz " ---------------------------------------------------------------          "
string_6: .asciiz "  "
string_1: .asciiz " y = "
string_4: .asciiz " OVERFLOW!          "
string_10: .asciiz " "
string_7: .asciiz "          "
string_0: .asciiz " x = "
string_2: .asciiz " SWAP x = "
string_3: .asciiz " SWAP y = "
string_5: .asciiz " complete number: "
# End of data segment

.text
.globl main
jal main
li $v0, 10
syscall

# User defined functions
factorial:
# Prologue
sw $ra, 0($sp)
sw $fp, -4($sp)
move $fp, $sp
addiu $sp, $sp, -8

# Passing Arguments
lw $t0, 4 ($fp)
sw $t0, 0 ($sp)

# Make room for local objects
addiu $sp, $sp, -4

# LOAD_LOCAL
lw $t0, -8 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_CONST
li $t0, 1
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# JUMP_IF_GREATER
addiu $sp, $sp, 4
lw $t0 , 0($sp)
addiu $sp, $sp, 4
lw $t1 , 0($sp)
bgt $t1, $t0, factorial_label_5

# LOAD_CONST
li $t0, 1
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# RETURN_VALUE
addiu $sp, $sp, 4
lw $v0 , 0($sp)
j factorial_return

factorial_label_5:
# LOAD_LOCAL
lw $t0, -8 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_LOCAL
lw $t0, -8 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_CONST
li $t0, 1
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# BINARY_SUB
addiu $sp, $sp, 4
lw $t0 , 0($sp)
addiu $sp, $sp, 4
lw $t1 , 0($sp)
subu $t2, $t1, $t0
sw $t2 , 0($sp)
addiu $sp, $sp, -4

# CALL_FUNCTION
jal factorial
addiu $sp, $sp 4
sw $v0 , 0($sp)
addiu $sp, $sp, -4

# BINARY_MULTIPLY
addiu $sp, $sp, 4
lw $t0 , 0($sp)
addiu $sp, $sp, 4
lw $t1 , 0($sp)
mul $t2, $t1, $t0
sw $t2 , 0($sp)
addiu $sp, $sp, -4

# RETURN_VALUE
addiu $sp, $sp, 4
lw $v0 , 0($sp)
j factorial_return

# RETURN_NONE
j factorial_return

# Epilogue
factorial_return:
lw $ra, 0($fp)
move $sp, $fp
lw $fp, -4($fp)
jr $ra

mod:
# Prologue
sw $ra, 0($sp)
sw $fp, -4($sp)
move $fp, $sp
addiu $sp, $sp, -8

# Passing Arguments
lw $t0, 8 ($fp)
sw $t0, 0 ($sp)
lw $t0, 4 ($fp)
sw $t0, -4 ($sp)

# Make room for local objects
addiu $sp, $sp, -8

# LOAD_LOCAL
lw $t0, -8 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_LOCAL
lw $t0, -8 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_LOCAL
lw $t0, -12 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# BINARY_DIVIDE
addiu $sp, $sp, 4
lw $t0 , 0($sp)
addiu $sp, $sp, 4
lw $t1 , 0($sp)
div $t2, $t1, $t0
sw $t2 , 0($sp)
addiu $sp, $sp, -4

# LOAD_LOCAL
lw $t0, -12 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# BINARY_MULTIPLY
addiu $sp, $sp, 4
lw $t0 , 0($sp)
addiu $sp, $sp, 4
lw $t1 , 0($sp)
mul $t2, $t1, $t0
sw $t2 , 0($sp)
addiu $sp, $sp, -4

# BINARY_SUB
addiu $sp, $sp, 4
lw $t0 , 0($sp)
addiu $sp, $sp, 4
lw $t1 , 0($sp)
subu $t2, $t1, $t0
sw $t2 , 0($sp)
addiu $sp, $sp, -4

# STORE_LOCAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
sw $t0, -8 ($fp)

# LOAD_LOCAL
lw $t0, -8 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# RETURN_VALUE
addiu $sp, $sp, 4
lw $v0 , 0($sp)
j mod_return

# RETURN_NONE
j mod_return

# Epilogue
mod_return:
lw $ra, 0($fp)
move $sp, $fp
lw $fp, -4($fp)
jr $ra

swap:
# Prologue
sw $ra, 0($sp)
sw $fp, -4($sp)
move $fp, $sp
addiu $sp, $sp, -8

# Passing Arguments
lw $t0, 8 ($fp)
sw $t0, 0 ($sp)
lw $t0, 4 ($fp)
sw $t0, -4 ($sp)

# Make room for local objects
addiu $sp, $sp, -12

# LOAD_STRING
la $t0, string_0
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# PRINT_STRING
li $v0, 4
addiu $sp, $sp, 4
lw $a0 , 0($sp)
syscall

# LOAD_LOCAL
lw $t0, -8 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# PRINT_INTEGER
li $v0, 1
addiu $sp, $sp, 4
lw $a0 , 0($sp)
syscall

# PRINT_NEWLINE
li $a0, 10
li $v0, 11
syscall

# LOAD_STRING
la $t0, string_1
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# PRINT_STRING
li $v0, 4
addiu $sp, $sp, 4
lw $a0 , 0($sp)
syscall

# LOAD_LOCAL
lw $t0, -12 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# PRINT_INTEGER
li $v0, 1
addiu $sp, $sp, 4
lw $a0 , 0($sp)
syscall

# PRINT_NEWLINE
li $a0, 10
li $v0, 11
syscall

# LOAD_LOCAL
lw $t0, -8 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# STORE_LOCAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
sw $t0, -16 ($fp)

# LOAD_LOCAL
lw $t0, -12 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# STORE_LOCAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
sw $t0, -8 ($fp)

# LOAD_LOCAL
lw $t0, -16 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# STORE_LOCAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
sw $t0, -12 ($fp)

# LOAD_STRING
la $t0, string_2
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# PRINT_STRING
li $v0, 4
addiu $sp, $sp, 4
lw $a0 , 0($sp)
syscall

# LOAD_LOCAL
lw $t0, -8 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# PRINT_INTEGER
li $v0, 1
addiu $sp, $sp, 4
lw $a0 , 0($sp)
syscall

# PRINT_NEWLINE
li $a0, 10
li $v0, 11
syscall

# LOAD_STRING
la $t0, string_3
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# PRINT_STRING
li $v0, 4
addiu $sp, $sp, 4
lw $a0 , 0($sp)
syscall

# LOAD_LOCAL
lw $t0, -12 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# PRINT_INTEGER
li $v0, 1
addiu $sp, $sp, 4
lw $a0 , 0($sp)
syscall

# PRINT_NEWLINE
li $a0, 10
li $v0, 11
syscall

# RETURN_NONE
j swap_return

# Epilogue
swap_return:
lw $ra, 0($fp)
move $sp, $fp
lw $fp, -4($fp)
jr $ra

complete_num:
# Prologue
sw $ra, 0($sp)
sw $fp, -4($sp)
move $fp, $sp
addiu $sp, $sp, -8

# Passing Arguments
lw $t0, 4 ($fp)
sw $t0, 0 ($sp)

# Make room for local objects
addiu $sp, $sp, -568

# LOAD_LOCAL
lw $t0, -8 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# STORE_LOCAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
sw $t0, -528 ($fp)

complete_num_label_2:
# LOAD_LOCAL
lw $t0, -528 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_CONST
li $t0, 1024
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# JUMP_IF_GREATER_EQUAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
addiu $sp, $sp, 4
lw $t1 , 0($sp)
bge $t1, $t0, complete_num_label_84

# LOAD_CONST
li $t0, -1
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# STORE_LOCAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
sw $t0, -532 ($fp)

# LOAD_LOCAL
lw $t0, -528 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# STORE_LOCAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
sw $t0, -536 ($fp)

# LOAD_CONST
li $t0, 1
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# STORE_LOCAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
sw $t0, -524 ($fp)

complete_num_label_11:
# LOAD_LOCAL
lw $t0, -524 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_LOCAL
lw $t0, -528 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# JUMP_IF_GREATER_EQUAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
addiu $sp, $sp, 4
lw $t1 , 0($sp)
bge $t1, $t0, complete_num_label_51

# LOAD_LOCAL
lw $t0, -528 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_LOCAL
lw $t0, -524 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# BINARY_DIVIDE
addiu $sp, $sp, 4
lw $t0 , 0($sp)
addiu $sp, $sp, 4
lw $t1 , 0($sp)
div $t2, $t1, $t0
sw $t2 , 0($sp)
addiu $sp, $sp, -4

# LOAD_LOCAL
lw $t0, -524 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# BINARY_MULTIPLY
addiu $sp, $sp, 4
lw $t0 , 0($sp)
addiu $sp, $sp, 4
lw $t1 , 0($sp)
mul $t2, $t1, $t0
sw $t2 , 0($sp)
addiu $sp, $sp, -4

# STORE_LOCAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
sw $t0, -540 ($fp)

# LOAD_LOCAL
lw $t0, -528 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_LOCAL
lw $t0, -524 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# CALL_FUNCTION
jal mod
addiu $sp, $sp 8
sw $v0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_CONST
li $t0, 0
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# JUMP_IF_NOT_EQUAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
addiu $sp, $sp, 4
lw $t1 , 0($sp)
bne $t1, $t0, complete_num_label_46

# LOAD_LOCAL
lw $t0, -532 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_CONST
li $t0, 1
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# BINARY_ADD
addiu $sp, $sp, 4
lw $t0 , 0($sp)
addiu $sp, $sp, 4
lw $t1 , 0($sp)
addu $t2, $t1, $t0
sw $t2 , 0($sp)
addiu $sp, $sp, -4

# STORE_LOCAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
sw $t0, -532 ($fp)

# LOAD_LOCAL
lw $t0, -536 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_LOCAL
lw $t0, -524 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# BINARY_SUB
addiu $sp, $sp, 4
lw $t0 , 0($sp)
addiu $sp, $sp, 4
lw $t1 , 0($sp)
subu $t2, $t1, $t0
sw $t2 , 0($sp)
addiu $sp, $sp, -4

# STORE_LOCAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
sw $t0, -536 ($fp)

# LOAD_LOCAL
lw $t0, -532 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_CONST
li $t0, 128
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# JUMP_IF_LESS
addiu $sp, $sp, 4
lw $t0 , 0($sp)
addiu $sp, $sp, 4
lw $t1 , 0($sp)
blt $t1, $t0, complete_num_label_39

# LOAD_STRING
la $t0, string_4
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# PRINT_STRING
li $v0, 4
addiu $sp, $sp, 4
lw $a0 , 0($sp)
syscall

# PRINT_NEWLINE
li $a0, 10
li $v0, 11
syscall

complete_num_label_39:
# LOAD_LOCAL
lw $t0, -532 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_CONST
li $t0, 128
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# JUMP_IF_GREATER_EQUAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
addiu $sp, $sp, 4
lw $t1 , 0($sp)
bge $t1, $t0, complete_num_label_46

# LOAD_LOCAL
lw $t0, -524 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_LOCAL
addiu $t0, $fp, -520
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_LOCAL
lw $t0, -532 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# STORE_SUBSCR
addiu $sp, $sp, 4
lw $t0 , 0($sp)
addiu $sp, $sp, 4
lw $t1 , 0($sp)
addiu $sp, $sp, 4
lw $t3 , 0($sp)
sll $t0, $t0, 2
addu $t2, $t1, $t0
sw $t3, 0($t2)

complete_num_label_46:
# LOAD_LOCAL
lw $t0, -524 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_CONST
li $t0, 1
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# BINARY_ADD
addiu $sp, $sp, 4
lw $t0 , 0($sp)
addiu $sp, $sp, 4
lw $t1 , 0($sp)
addu $t2, $t1, $t0
sw $t2 , 0($sp)
addiu $sp, $sp, -4

# STORE_LOCAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
sw $t0, -524 ($fp)

# JUMP_FORWARD
j complete_num_label_11

complete_num_label_51:
# LOAD_LOCAL
lw $t0, -536 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_CONST
li $t0, 0
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# JUMP_IF_NOT_EQUAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
addiu $sp, $sp, 4
lw $t1 , 0($sp)
bne $t1, $t0, complete_num_label_79

# LOAD_STRING
la $t0, string_5
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# PRINT_STRING
li $v0, 4
addiu $sp, $sp, 4
lw $a0 , 0($sp)
syscall

# LOAD_LOCAL
lw $t0, -528 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# PRINT_INTEGER
li $v0, 1
addiu $sp, $sp, 4
lw $a0 , 0($sp)
syscall

# PRINT_NEWLINE
li $a0, 10
li $v0, 11
syscall

# LOAD_CONST
li $t0, 0
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# STORE_LOCAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
sw $t0, -524 ($fp)

complete_num_label_61:
# LOAD_LOCAL
lw $t0, -524 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_LOCAL
lw $t0, -532 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# JUMP_IF_GREATER
addiu $sp, $sp, 4
lw $t0 , 0($sp)
addiu $sp, $sp, 4
lw $t1 , 0($sp)
bgt $t1, $t0, complete_num_label_76

# LOAD_STRING
la $t0, string_6
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# PRINT_STRING
li $v0, 4
addiu $sp, $sp, 4
lw $a0 , 0($sp)
syscall

# LOAD_LOCAL
addiu $t0, $fp, -520
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_LOCAL
lw $t0, -524 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# BINARY_SUBSCR
addiu $sp, $sp, 4
lw $t0 , 0($sp)
addiu $sp, $sp, 4
lw $t1 , 0($sp)
sll $t0, $t0, 2
addu $t2, $t1, $t0
lw $t3, 0($t2)
sw $t3 , 0($sp)
addiu $sp, $sp, -4

# PRINT_INTEGER
li $v0, 1
addiu $sp, $sp, 4
lw $a0 , 0($sp)
syscall

# PRINT_NEWLINE
li $a0, 10
li $v0, 11
syscall

# LOAD_LOCAL
lw $t0, -524 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_CONST
li $t0, 1
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# BINARY_ADD
addiu $sp, $sp, 4
lw $t0 , 0($sp)
addiu $sp, $sp, 4
lw $t1 , 0($sp)
addu $t2, $t1, $t0
sw $t2 , 0($sp)
addiu $sp, $sp, -4

# STORE_LOCAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
sw $t0, -524 ($fp)

# JUMP_FORWARD
j complete_num_label_61

complete_num_label_76:
# LOAD_STRING
la $t0, string_7
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# PRINT_STRING
li $v0, 4
addiu $sp, $sp, 4
lw $a0 , 0($sp)
syscall

# PRINT_NEWLINE
li $a0, 10
li $v0, 11
syscall

complete_num_label_79:
# LOAD_LOCAL
lw $t0, -528 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_CONST
li $t0, 1
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# BINARY_ADD
addiu $sp, $sp, 4
lw $t0 , 0($sp)
addiu $sp, $sp, 4
lw $t1 , 0($sp)
addu $t2, $t1, $t0
sw $t2 , 0($sp)
addiu $sp, $sp, -4

# STORE_LOCAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
sw $t0, -528 ($fp)

# JUMP_FORWARD
j complete_num_label_2

complete_num_label_84:
# LOAD_STRING
la $t0, string_8
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# PRINT_STRING
li $v0, 4
addiu $sp, $sp, 4
lw $a0 , 0($sp)
syscall

# PRINT_NEWLINE
li $a0, 10
li $v0, 11
syscall

# LOAD_CONST
li $t0, 0
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# STORE_LOCAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
sw $t0, -564 ($fp)

# LOAD_CONST
li $t0, 0
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# STORE_LOCAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
sw $t0, -524 ($fp)

complete_num_label_91:
# LOAD_LOCAL
lw $t0, -524 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_CONST
li $t0, 1024
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# JUMP_IF_GREATER_EQUAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
addiu $sp, $sp, 4
lw $t1 , 0($sp)
bge $t1, $t0, complete_num_label_117

# LOAD_LOCAL
lw $t0, -564 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_LOCAL
lw $t0, -524 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# BINARY_MULTIPLY
addiu $sp, $sp, 4
lw $t0 , 0($sp)
addiu $sp, $sp, 4
lw $t1 , 0($sp)
mul $t2, $t1, $t0
sw $t2 , 0($sp)
addiu $sp, $sp, -4

# STORE_LOCAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
sw $t0, -568 ($fp)

# LOAD_LOCAL
lw $t0, -528 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_LOCAL
lw $t0, -532 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# BINARY_MULTIPLY
addiu $sp, $sp, 4
lw $t0 , 0($sp)
addiu $sp, $sp, 4
lw $t1 , 0($sp)
mul $t2, $t1, $t0
sw $t2 , 0($sp)
addiu $sp, $sp, -4

# STORE_LOCAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
sw $t0, -572 ($fp)

# LOAD_LOCAL
lw $t0, -564 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_LOCAL
lw $t0, -524 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# BINARY_MULTIPLY
addiu $sp, $sp, 4
lw $t0 , 0($sp)
addiu $sp, $sp, 4
lw $t1 , 0($sp)
mul $t2, $t1, $t0
sw $t2 , 0($sp)
addiu $sp, $sp, -4

# STORE_LOCAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
sw $t0, -564 ($fp)

# LOAD_LOCAL
lw $t0, -564 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_LOCAL
lw $t0, -568 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# BINARY_ADD
addiu $sp, $sp, 4
lw $t0 , 0($sp)
addiu $sp, $sp, 4
lw $t1 , 0($sp)
addu $t2, $t1, $t0
sw $t2 , 0($sp)
addiu $sp, $sp, -4

# LOAD_LOCAL
lw $t0, -572 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# BINARY_ADD
addiu $sp, $sp, 4
lw $t0 , 0($sp)
addiu $sp, $sp, 4
lw $t1 , 0($sp)
addu $t2, $t1, $t0
sw $t2 , 0($sp)
addiu $sp, $sp, -4

# STORE_LOCAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
sw $t0, -564 ($fp)

# LOAD_LOCAL
lw $t0, -524 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_CONST
li $t0, 1
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# BINARY_ADD
addiu $sp, $sp, 4
lw $t0 , 0($sp)
addiu $sp, $sp, 4
lw $t1 , 0($sp)
addu $t2, $t1, $t0
sw $t2 , 0($sp)
addiu $sp, $sp, -4

# STORE_LOCAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
sw $t0, -524 ($fp)

# JUMP_FORWARD
j complete_num_label_91

complete_num_label_117:
# LOAD_LOCAL
lw $t0, -564 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# STORE_LOCAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
sw $t0, -568 ($fp)

# LOAD_CONST
li $t0, 0
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# STORE_LOCAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
sw $t0, -524 ($fp)

complete_num_label_121:
# LOAD_LOCAL
lw $t0, -524 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_CONST
li $t0, 1024
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# JUMP_IF_GREATER_EQUAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
addiu $sp, $sp, 4
lw $t1 , 0($sp)
bge $t1, $t0, complete_num_label_133

# LOAD_LOCAL
lw $t0, -568 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_LOCAL
lw $t0, -524 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# BINARY_ADD
addiu $sp, $sp, 4
lw $t0 , 0($sp)
addiu $sp, $sp, 4
lw $t1 , 0($sp)
addu $t2, $t1, $t0
sw $t2 , 0($sp)
addiu $sp, $sp, -4

# STORE_LOCAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
sw $t0, -568 ($fp)

# LOAD_LOCAL
lw $t0, -524 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_CONST
li $t0, 1
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# BINARY_ADD
addiu $sp, $sp, 4
lw $t0 , 0($sp)
addiu $sp, $sp, 4
lw $t1 , 0($sp)
addu $t2, $t1, $t0
sw $t2 , 0($sp)
addiu $sp, $sp, -4

# STORE_LOCAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
sw $t0, -524 ($fp)

# JUMP_FORWARD
j complete_num_label_121

complete_num_label_133:
# LOAD_STRING
la $t0, string_9
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# PRINT_STRING
li $v0, 4
addiu $sp, $sp, 4
lw $a0 , 0($sp)
syscall

# LOAD_LOCAL
lw $t0, -568 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# PRINT_INTEGER
li $v0, 1
addiu $sp, $sp, 4
lw $a0 , 0($sp)
syscall

# PRINT_NEWLINE
li $a0, 10
li $v0, 11
syscall

# LOAD_STRING
la $t0, string_8
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# PRINT_STRING
li $v0, 4
addiu $sp, $sp, 4
lw $a0 , 0($sp)
syscall

# PRINT_NEWLINE
li $a0, 10
li $v0, 11
syscall

# LOAD_CONST
li $t0, 0
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# STORE_LOCAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
sw $t0, -552 ($fp)

# LOAD_CONST
li $t0, 1
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# STORE_LOCAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
sw $t0, -556 ($fp)

# LOAD_CONST
li $t0, 2
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# STORE_LOCAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
sw $t0, -544 ($fp)

complete_num_label_147:
# LOAD_LOCAL
lw $t0, -544 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_CONST
li $t0, 1024
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# JUMP_IF_GREATER
addiu $sp, $sp, 4
lw $t0 , 0($sp)
addiu $sp, $sp, 4
lw $t1 , 0($sp)
bgt $t1, $t0, complete_num_label_208

# LOAD_LOCAL
lw $t0, -544 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_CONST
li $t0, 2
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# BINARY_DIVIDE
addiu $sp, $sp, 4
lw $t0 , 0($sp)
addiu $sp, $sp, 4
lw $t1 , 0($sp)
div $t2, $t1, $t0
sw $t2 , 0($sp)
addiu $sp, $sp, -4

# STORE_LOCAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
sw $t0, -548 ($fp)

# LOAD_CONST
li $t0, 2
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# STORE_LOCAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
sw $t0, -524 ($fp)

complete_num_label_156:
# LOAD_LOCAL
lw $t0, -524 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_LOCAL
lw $t0, -548 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# JUMP_IF_GREATER
addiu $sp, $sp, 4
lw $t0 , 0($sp)
addiu $sp, $sp, 4
lw $t1 , 0($sp)
bgt $t1, $t0, complete_num_label_177

# LOAD_LOCAL
lw $t0, -544 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_LOCAL
lw $t0, -524 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# BINARY_DIVIDE
addiu $sp, $sp, 4
lw $t0 , 0($sp)
addiu $sp, $sp, 4
lw $t1 , 0($sp)
div $t2, $t1, $t0
sw $t2 , 0($sp)
addiu $sp, $sp, -4

# LOAD_LOCAL
lw $t0, -524 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# BINARY_MULTIPLY
addiu $sp, $sp, 4
lw $t0 , 0($sp)
addiu $sp, $sp, 4
lw $t1 , 0($sp)
mul $t2, $t1, $t0
sw $t2 , 0($sp)
addiu $sp, $sp, -4

# STORE_LOCAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
sw $t0, -560 ($fp)

# LOAD_LOCAL
lw $t0, -544 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_LOCAL
lw $t0, -524 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# CALL_FUNCTION
jal mod
addiu $sp, $sp 8
sw $v0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_CONST
li $t0, 0
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# JUMP_IF_NOT_EQUAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
addiu $sp, $sp, 4
lw $t1 , 0($sp)
bne $t1, $t0, complete_num_label_172

# LOAD_CONST
li $t0, 0
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# STORE_LOCAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
sw $t0, -556 ($fp)

complete_num_label_172:
# LOAD_LOCAL
lw $t0, -524 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_CONST
li $t0, 1
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# BINARY_ADD
addiu $sp, $sp, 4
lw $t0 , 0($sp)
addiu $sp, $sp, 4
lw $t1 , 0($sp)
addu $t2, $t1, $t0
sw $t2 , 0($sp)
addiu $sp, $sp, -4

# STORE_LOCAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
sw $t0, -524 ($fp)

# JUMP_FORWARD
j complete_num_label_156

complete_num_label_177:
# LOAD_LOCAL
lw $t0, -556 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_CONST
li $t0, 1
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# JUMP_IF_NOT_EQUAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
addiu $sp, $sp, 4
lw $t1 , 0($sp)
bne $t1, $t0, complete_num_label_201

# LOAD_STRING
la $t0, string_10
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# PRINT_STRING
li $v0, 4
addiu $sp, $sp, 4
lw $a0 , 0($sp)
syscall

# LOAD_LOCAL
lw $t0, -544 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# PRINT_INTEGER
li $v0, 1
addiu $sp, $sp, 4
lw $a0 , 0($sp)
syscall

# PRINT_NEWLINE
li $a0, 10
li $v0, 11
syscall

# LOAD_LOCAL
lw $t0, -552 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_CONST
li $t0, 1
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# BINARY_ADD
addiu $sp, $sp, 4
lw $t0 , 0($sp)
addiu $sp, $sp, 4
lw $t1 , 0($sp)
addu $t2, $t1, $t0
sw $t2 , 0($sp)
addiu $sp, $sp, -4

# STORE_LOCAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
sw $t0, -552 ($fp)

# LOAD_LOCAL
lw $t0, -552 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_CONST
li $t0, 10
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# BINARY_DIVIDE
addiu $sp, $sp, 4
lw $t0 , 0($sp)
addiu $sp, $sp, 4
lw $t1 , 0($sp)
div $t2, $t1, $t0
sw $t2 , 0($sp)
addiu $sp, $sp, -4

# LOAD_CONST
li $t0, 10
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# BINARY_MULTIPLY
addiu $sp, $sp, 4
lw $t0 , 0($sp)
addiu $sp, $sp, 4
lw $t1 , 0($sp)
mul $t2, $t1, $t0
sw $t2 , 0($sp)
addiu $sp, $sp, -4

# STORE_LOCAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
sw $t0, -560 ($fp)

# LOAD_LOCAL
lw $t0, -560 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_LOCAL
lw $t0, -552 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# JUMP_IF_NOT_EQUAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
addiu $sp, $sp, 4
lw $t1 , 0($sp)
bne $t1, $t0, complete_num_label_201

# LOAD_STRING
la $t0, string_7
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# PRINT_STRING
li $v0, 4
addiu $sp, $sp, 4
lw $a0 , 0($sp)
syscall

# PRINT_NEWLINE
li $a0, 10
li $v0, 11
syscall

complete_num_label_201:
# LOAD_CONST
li $t0, 1
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# STORE_LOCAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
sw $t0, -556 ($fp)

# LOAD_LOCAL
lw $t0, -544 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_CONST
li $t0, 1
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# BINARY_ADD
addiu $sp, $sp, 4
lw $t0 , 0($sp)
addiu $sp, $sp, 4
lw $t1 , 0($sp)
addu $t2, $t1, $t0
sw $t2 , 0($sp)
addiu $sp, $sp, -4

# STORE_LOCAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
sw $t0, -544 ($fp)

# JUMP_FORWARD
j complete_num_label_147

complete_num_label_208:
# LOAD_STRING
la $t0, string_11
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# PRINT_STRING
li $v0, 4
addiu $sp, $sp, 4
lw $a0 , 0($sp)
syscall

# LOAD_LOCAL
lw $t0, -552 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# PRINT_INTEGER
li $v0, 1
addiu $sp, $sp, 4
lw $a0 , 0($sp)
syscall

# PRINT_NEWLINE
li $a0, 10
li $v0, 11
syscall

# RETURN_NONE
j complete_num_return

# Epilogue
complete_num_return:
lw $ra, 0($fp)
move $sp, $fp
lw $fp, -4($fp)
jr $ra

main:
# Prologue
sw $ra, 0($sp)
sw $fp, -4($sp)
move $fp, $sp
addiu $sp, $sp, -8

# Make room for local objects
addiu $sp, $sp, -4

# LOAD_CONST
li $t0, 10
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# CALL_FUNCTION
jal factorial
addiu $sp, $sp 4
sw $v0 , 0($sp)
addiu $sp, $sp, -4

# STORE_LOCAL
addiu $sp, $sp, 4
lw $t0 , 0($sp)
sw $t0, -8 ($fp)

# LOAD_STRING
la $t0, string_12
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# PRINT_STRING
li $v0, 4
addiu $sp, $sp, 4
lw $a0 , 0($sp)
syscall

# LOAD_LOCAL
lw $t0, -8 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# PRINT_INTEGER
li $v0, 1
addiu $sp, $sp, 4
lw $a0 , 0($sp)
syscall

# PRINT_NEWLINE
li $a0, 10
li $v0, 11
syscall

# LOAD_CONST
li $t0, 5
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_CONST
li $t0, 10
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# CALL_FUNCTION
jal swap
addiu $sp, $sp 8
sw $v0 , 0($sp)
addiu $sp, $sp, -4

# POP_TOP
addiu $sp, $sp, 4

# LOAD_CONST
li $t0, 2
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# CALL_FUNCTION
jal complete_num
addiu $sp, $sp 4
sw $v0 , 0($sp)
addiu $sp, $sp, -4

# POP_TOP
addiu $sp, $sp, 4

# RETURN_NONE
j main_return

# Epilogue
main_return:
lw $ra, 0($fp)
move $sp, $fp
lw $fp, -4($fp)
jr $ra

# End of text segment
