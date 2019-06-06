.data
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
.globl main
.text
read:
  li $v0, 4
  la $a0, _prompt
  syscall
  li $v0, 5
  syscall
  jr $ra

write:
  li $v0, 1
  syscall
  li $v0, 4
  la $a0, _ret
  syscall
  move $v0, $0
  jr $ra
fact:
  sw $a0, 0x10010020
  lw $t1, 0x10010020
  sw $t1, 0x10010024
  li $t1, 1
  sw $t1, 0x10010028
  lw $t1, 0x10010024
  lw $t2, 0x10010028
  beq $t1, $t2, label1
  j label2
label1:
  lw $t1, 0x10010020
  sw $t1, 0x1001002c
  lw $v0, 0x1001002c
  jr $ra
  j label3
label2:
  lw $t1, 0x10010020
  sw $t1, 0x10010030
  lw $t1, 0x10010020
  sw $t1, 0x10010034
  li $t1, 1
  sw $t1, 0x10010038
  lw $t1, 0x10010034
  lw $t2, 0x10010038
  sub $t1, $t1, $t2
  sw $t1, 0x1001003c
  lw $a0, 0x1001003c
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal fact
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  sw $v0, 0x10010040
  lw $t1, 0x10010030
  lw $t2, 0x10010040
  mul $t1, $t1, $t2
  sw $t1, 0x10010044
  lw $v0, 0x10010044
  jr $ra
label3:
main:
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal read
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  sw $v0, 0x10010048
  lw $t1, 0x10010048
  sw $t1, 0x1001004c
  lw $t1, 0x1001004c
  sw $t1, 0x10010050
  lw $t1, 0x1001004c
  sw $t1, 0x10010054
  li $t1, 1
  sw $t1, 0x10010058
  lw $t1, 0x10010054
  lw $t2, 0x10010058
  bgt $t1, $t2, label4
  j label5
label4:
  lw $t1, 0x1001004c
  sw $t1, 0x1001005c
  lw $a0, 0x1001005c
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal fact
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  sw $v0, 0x10010060
  lw $t1, 0x10010060
  sw $t1, 0x10010064
  lw $t1, 0x10010064
  sw $t1, 0x10010068
  j label6
label5:
  li $t1, 1
  sw $t1, 0x1001006c
  lw $t1, 0x1001006c
  sw $t1, 0x10010064
  lw $t1, 0x10010064
  sw $t1, 0x10010070
label6:
  lw $t1, 0x10010064
  sw $t1, 0x10010074
  lw $a0, 0x10010074
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal write
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  li $t1, 0
  sw $t1, 0x10010078
  lw $v0, 0x10010078
  jr $ra
