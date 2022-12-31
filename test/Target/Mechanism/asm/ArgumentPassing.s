.data
# Global objects

# String literals
string_4: .asciiz "Expect b: "
string_3: .asciiz "Expect a: "
string_2: .asciiz "Expect 2: "
string_0: .asciiz "Dump Arguments:"
string_1: .asciiz "Expect 1: "
# End of data segment

.text
.globl main
jal main
li $v0, 10
syscall

# User defined functions
testargumentpassing:
# Prologue
sw $ra, 0($sp)
sw $fp, -4($sp)
move $fp, $sp
addiu $sp, $sp, -8

# Passing Arguments
lw $t0, 16 ($fp)
sw $t0, 0 ($sp)
lw $t0, 12 ($fp)
sw $t0, -4 ($sp)
lw $t0, 8 ($fp)
sw $t0, -8 ($sp)
lw $t0, 4 ($fp)
sw $t0, -12 ($sp)

# Make room for local objects
addiu $sp, $sp, -16

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
la $t0, string_2
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
lw $t0, -16 ($fp)
sw $t0 , 0($sp)
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
la $t0, string_4
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# PRINT_STRING
li $v0, 4
addiu $sp, $sp, 4
lw $a0 , 0($sp)
syscall

# LOAD_LOCAL
lw $t0, -20 ($fp)
sw $t0 , 0($sp)
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
j testargumentpassing_return

# Epilogue
testargumentpassing_return:
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

# LOAD_CONST
li $t0, 1
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_CONST
li $t0, 2
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_CONST
li $t0, 97
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_CONST
li $t0, 98
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# CALL_FUNCTION
jal testargumentpassing
addiu $sp, $sp 16
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
