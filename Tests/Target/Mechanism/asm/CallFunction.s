.data
# Global objects

# String literals
string_3: .asciiz "Expect 10: "
string_2: .asciiz "Expect 6: "
string_0: .asciiz "Expect 1: "
string_1: .asciiz "Expect 3: "
# End of data segment

.text
.globl main
jal main
li $v0, 10
syscall

# User defined functions
function_1:
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

# RETURN_VALUE
addiu $sp, $sp, 4
lw $v0 , 0($sp)
j function_1_return

# RETURN_NONE
j function_1_return

# Epilogue
function_1_return:
lw $ra, 0($fp)
move $sp, $fp
lw $fp, -4($fp)
jr $ra

function_2:
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

# CALL_FUNCTION
jal function_1
addiu $sp, $sp 4
sw $v0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_LOCAL
lw $t0, -12 ($fp)
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

# RETURN_VALUE
addiu $sp, $sp, 4
lw $v0 , 0($sp)
j function_2_return

# RETURN_NONE
j function_2_return

# Epilogue
function_2_return:
lw $ra, 0($fp)
move $sp, $fp
lw $fp, -4($fp)
jr $ra

function_3:
# Prologue
sw $ra, 0($sp)
sw $fp, -4($sp)
move $fp, $sp
addiu $sp, $sp, -8

# Passing Arguments
lw $t0, 12 ($fp)
sw $t0, 0 ($sp)
lw $t0, 8 ($fp)
sw $t0, -4 ($sp)
lw $t0, 4 ($fp)
sw $t0, -8 ($sp)

# Make room for local objects
addiu $sp, $sp, -12

# LOAD_LOCAL
lw $t0, -8 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_LOCAL
lw $t0, -12 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# CALL_FUNCTION
jal function_2
addiu $sp, $sp 8
sw $v0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_LOCAL
lw $t0, -16 ($fp)
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

# RETURN_VALUE
addiu $sp, $sp, 4
lw $v0 , 0($sp)
j function_3_return

# RETURN_NONE
j function_3_return

# Epilogue
function_3_return:
lw $ra, 0($fp)
move $sp, $fp
lw $fp, -4($fp)
jr $ra

function_4:
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

# LOAD_LOCAL
lw $t0, -8 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_LOCAL
lw $t0, -12 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_LOCAL
lw $t0, -16 ($fp)
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# CALL_FUNCTION
jal function_3
addiu $sp, $sp 12
sw $v0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_LOCAL
lw $t0, -20 ($fp)
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

# RETURN_VALUE
addiu $sp, $sp, 4
lw $v0 , 0($sp)
j function_4_return

# RETURN_NONE
j function_4_return

# Epilogue
function_4_return:
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

# LOAD_STRING
la $t0, string_0
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# PRINT_STRING
li $v0, 4
addiu $sp, $sp, 4
lw $a0 , 0($sp)
syscall

# LOAD_CONST
li $t0, 1
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# CALL_FUNCTION
jal function_1
addiu $sp, $sp 4
sw $v0 , 0($sp)
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

# LOAD_CONST
li $t0, 1
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_CONST
li $t0, 2
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# CALL_FUNCTION
jal function_2
addiu $sp, $sp 8
sw $v0 , 0($sp)
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

# LOAD_CONST
li $t0, 1
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_CONST
li $t0, 2
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_CONST
li $t0, 3
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# CALL_FUNCTION
jal function_3
addiu $sp, $sp 12
sw $v0 , 0($sp)
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

# LOAD_CONST
li $t0, 1
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_CONST
li $t0, 2
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_CONST
li $t0, 3
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# LOAD_CONST
li $t0, 4
sw $t0 , 0($sp)
addiu $sp, $sp, -4

# CALL_FUNCTION
jal function_4
addiu $sp, $sp 16
sw $v0 , 0($sp)
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
j main_return

# Epilogue
main_return:
lw $ra, 0($fp)
move $sp, $fp
lw $fp, -4($fp)
jr $ra

# End of text segment
