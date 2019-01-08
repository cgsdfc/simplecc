.data
# Global objects
chararray: .space 12

# String literals
string_1: .asciiz "Expect a: "
string_0: .asciiz "Dump Global Array:"
# End of data segment

.text
.globl main
jal main
li $v0, 10
syscall

# User defined functions
main:
# Prologue
sw $ra, 0($sp)
sw $fp, -4($sp)
move $fp, $sp
addiu $sp, $sp, -8

# LOAD_CONST
li $t0, 97
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_GLOBAL
la $t0, chararray
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_CONST
li $t0, 0
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

# LOAD_CONST
li $t0, 98
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_GLOBAL
la $t0, chararray
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_CONST
li $t0, 1
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

# LOAD_CONST
li $t0, 99
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_GLOBAL
la $t0, chararray
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_CONST
li $t0, 2
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

# LOAD_STRING
la $t0, string_0
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

# LOAD_STRING
la $t0, string_1
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# PRINT_STRING
li $v0, 4
addiu $sp, $sp, 4
lw $a0 , 0($sp)
syscall

# LOAD_GLOBAL
la $t0, chararray
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_CONST
li $t0, 0
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

# PRINT_CHARACTER
li $v0, 11
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

# LOAD_GLOBAL
la $t0, chararray
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_CONST
li $t0, 1
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

# PRINT_CHARACTER
li $v0, 11
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

# LOAD_GLOBAL
la $t0, chararray
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_CONST
li $t0, 2
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

# PRINT_CHARACTER
li $v0, 11
addiu $sp, $sp, 4
lw $a0 , 0($sp)
syscall

# PRINT_NEWLINE
li $a0, 10
li $v0, 11
syscall

# RETURN_NONE
j main_return

# Epilogue
main_return:
lw $ra, 0($fp)
move $sp, $fp
lw $fp, -4($fp)
jr $ra

# End of text segment
